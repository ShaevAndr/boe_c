/*=============================================================================
     Project:
     Platform: GD32F470
     Filename: ModbusRtuRoutine.c
     Description: Modbus RTU frame reception/transmission (silence-based framing)
     Version: 1.0
     Created: 2026.05.04
============================================================================*/
#include <stdint.h>
#include <stdbool.h>
//--------------------------------------------------------------------------//
#include "../Unicorn2/drv_time.h"
#include "../Unicorn2/drv_RS485x.h"
#include "../Unicorn2/Unicorn2Routine.h"
#include "../Global/ParamSystem.h"
#include "../Unicorn2/unicorn_uart_speed.h"
#include "../drv_LEDs.h"
#include "ModbusRtuRoutine.h"
#include "ModbusRtuFrame.h"
//--------------------------------------------------------------------------//
// Extra silence added to calculated t3.5 timeout (microseconds).
// Increase if frames are being split, decrease for faster response.
#define MODBUS_RTU_SILENCE_EXTRA_US  200
//--------------------------------------------------------------------------//
// Minimum silence timeout (microseconds).
// Modbus spec recommends fixed 1750us for baud > 19200.
#define MODBUS_RTU_SILENCE_MIN_US  1750
//--------------------------------------------------------------------------//
#define MODBUS_RTU_MAX_FRAME_SIZE  256
//--------------------------------------------------------------------------//
static uint8_t  BuffRtu [_MaxUARTNumber + 1][MODBUS_RTU_MAX_FRAME_SIZE];
static uint32_t RtuSilenceTimeoutUs [_MaxUARTNumber + 1];
//--------------------------------------------------------------------------//
// Calculate inter-frame silence timeout from baudrate.
// Modbus RTU: 1 char = 11 bits (start + 8 data + parity + stop)
// t3.5 = 3.5 * 11 / baudrate * 1000000 = 38500000 / baudrate (us)
// For baud > 19200, spec says use fixed 1750us.
static uint32_t CalcSilenceTimeoutUs(uint32_t baudrate)
{
	uint32_t t35;

	if (baudrate == 0)
		return MODBUS_RTU_SILENCE_MIN_US + MODBUS_RTU_SILENCE_EXTRA_US;

	if (baudrate > 19200)
		t35 = MODBUS_RTU_SILENCE_MIN_US;
	else
		t35 = 38500000UL / baudrate;  // 3.5 chars * 11 bits * 1e6

	return t35 + MODBUS_RTU_SILENCE_EXTRA_US;
}
//--------------------------------------------------------------------------//
typedef enum { RtuIdle, RtuReceive, RtuTransmit } TRtuState;
//--------------------------------------------------------------------------//
static TRtuState RtuMode [_MaxUARTNumber + 1];
static uint32_t  RtuIndex [_MaxUARTNumber + 1];
static uint32_t  RtuSize [_MaxUARTNumber + 1];
static TTime     RtuSilenceTimer [_MaxUARTNumber + 1];
static TTime     RtuTxTO [_MaxUARTNumber + 1];
static uint8_t   RtuInitDone = 0;
//--------------------------------------------------------------------------//
static uint32_t RtuGetRxCount(uint8_t N)
{
	switch (N)
	{
		case 0: return RS485_GetNumberOfByteRxFIFO(_RS485_1);
		case 1: return RS485_GetNumberOfByteRxFIFO(_RS485_2);
		case 2: return RS485_GetNumberOfByteRxFIFO(_RS485_3);
		default: return 0;
	}
}
//--------------------------------------------------------------------------//
static uint8_t RtuPopRx(uint8_t N)
{
	switch (N)
	{
		case 0: return RS485_PopRxFIFO(_RS485_1);
		case 1: return RS485_PopRxFIFO(_RS485_2);
		case 2: return RS485_PopRxFIFO(_RS485_3);
		default: return 0;
	}
}
//--------------------------------------------------------------------------//
static uint32_t RtuGetTxFree(uint8_t N)
{
	switch (N)
	{
		case 0: return RS485_GetNumberOfFreeByteTxFIFO(_RS485_1);
		case 1: return RS485_GetNumberOfFreeByteTxFIFO(_RS485_2);
		case 2: return RS485_GetNumberOfFreeByteTxFIFO(_RS485_3);
		default: return 0;
	}
}
//--------------------------------------------------------------------------//
static uint32_t RtuGetTxCount(uint8_t N)
{
	switch (N)
	{
		case 0: return RS485_GetNumberOfByteTxFIFO(_RS485_1);
		case 1: return RS485_GetNumberOfByteTxFIFO(_RS485_2);
		case 2: return RS485_GetNumberOfByteTxFIFO(_RS485_3);
		default: return 0;
	}
}
//--------------------------------------------------------------------------//
static uint32_t RtuPushTxBuf(uint8_t N, const uint8_t *buf, uint32_t size)
{
	switch (N)
	{
		case 0: return RS485_PushTxFIFOBuf(_RS485_1, buf, size);
		case 1: return RS485_PushTxFIFOBuf(_RS485_2, buf, size);
		case 2: return RS485_PushTxFIFOBuf(_RS485_3, buf, size);
		default: return 0;
	}
}
//--------------------------------------------------------------------------//
static uint32_t min_u32(uint32_t a, uint32_t b)
{
	return (a < b) ? a : b;
}
//--------------------------------------------------------------------------//
void ModbusRtuRoutine_Init(void)
{
	for (int i = 0; i < _RS485_MODBUS_COUNT; i++)
	{
		uint32_t baudrate = unicorn_uart_speed_to_baudrate(gParamSystem.rs485Modbus[i].UARTSpeed);
		RS485_Init(i, baudrate);
		RtuSilenceTimeoutUs[i] = CalcSilenceTimeoutUs(baudrate);
	}
}
//--------------------------------------------------------------------------//
void ModbusRtuRoutine(void)
{
	uint32_t rxCount;
	uint8_t  rxByte;

	if (!RtuInitDone)
	{
		RtuInitDone = 1;
		for (uint8_t i = 0; i <= _MaxUARTNumber; i++)
		{
			RtuMode[i] = RtuIdle;
			RtuIndex[i] = 0;
			RtuSize[i] = 0;
		}
	}

	for (uint8_t uartNum = 0; uartNum < _RS485_MODBUS_COUNT; uartNum++)
	{
		rxCount = RtuGetRxCount(uartNum);

		if (rxCount > 0 && RtuMode[uartNum] != RtuTransmit)
		{
			do
			{
				rxByte = RtuPopRx(uartNum);

				switch (RtuMode[uartNum])
				{
					case RtuIdle:
						// First byte — start receiving
						RtuMode[uartNum] = RtuReceive;
						RtuIndex[uartNum] = 0;
						BuffRtu[uartNum][RtuIndex[uartNum]++] = rxByte;
						RtuSilenceTimer[uartNum] = SetTime_us(RtuSilenceTimeoutUs[uartNum]);
						break;

					case RtuReceive:
						if (RtuIndex[uartNum] < MODBUS_RTU_MAX_FRAME_SIZE)
						{
							BuffRtu[uartNum][RtuIndex[uartNum]++] = rxByte;
						}
						// Reset silence timer on each byte
						RtuSilenceTimer[uartNum] = SetTime_us(RtuSilenceTimeoutUs[uartNum]);
						break;

					case RtuTransmit:
						break;
				}
			} while (--rxCount);
		}

		// Check silence timeout — frame complete
		if (RtuMode[uartNum] == RtuReceive && EndTime(RtuSilenceTimer[uartNum]))
		{
			RtuSize[uartNum] = RtuIndex[uartNum];

			// Process complete RTU frame
			uint8_t deviceAddr = gParamSystem.rs485Modbus[uartNum].ModbusAddress;
			ModbusRtuFrame_Process(uartNum, BuffRtu[uartNum], &RtuSize[uartNum], deviceAddr);
			PingActivitiLED();

			if (RtuSize[uartNum] > 0)
			{
				RtuMode[uartNum] = RtuTransmit;
				RtuIndex[uartNum] = 0;
				RtuTxTO[uartNum] = SetTime_ms(0);
			}
			else
			{
				RtuMode[uartNum] = RtuIdle;
			}
		}

		// Transmit response
		if (RtuMode[uartNum] == RtuTransmit && EndTime(RtuTxTO[uartNum]))
		{
			uint32_t txFree = RtuGetTxFree(uartNum);
			if (txFree > 0) txFree--;
			uint32_t toSend = min_u32(txFree, RtuSize[uartNum]);
			if (toSend > 0)
			{
				toSend = RtuPushTxBuf(uartNum, &BuffRtu[uartNum][RtuIndex[uartNum]], toSend);
				RtuIndex[uartNum] += toSend;
				RtuSize[uartNum] -= toSend;
				if (RtuSize[uartNum] == 0)
				{
					RtuMode[uartNum] = RtuIdle;
				}
			}
		}
	}
}
