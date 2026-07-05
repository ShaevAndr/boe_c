/*=============================================================================
     Project:
     Platform: GD32F470
     Filename: ModbusRtuRoutine.c
     Description: Modbus RTU frame reception/transmission with hardware-timestamped
                  byte arrival (Timer12, 1us tick). End-of-frame detection (t3.5)
                  and inter-character violation (t1.5) are measured against the
                  physical RX moment captured in IRQ, not against the polling
                  moment in main loop.
     Version: 2.0
     Created: 2026.05.04
============================================================================*/
#include <stdint.h>
#include <stdbool.h>
//--------------------------------------------------------------------------//
#include "../Unicorn2/drv_time.h"
#include "../Unicorn2/drv_RS485x.h"
#include "../Unicorn2/Unicorn2Routine.h"
#include "../Global/ParamSystem.h"
#include "../Global/logger.h"
#include "../Unicorn2/unicorn_uart_speed.h"
#include "ModbusRtuRoutine.h"
#include "ModbusRtuFrame.h"
//--------------------------------------------------------------------------//
// Дополнительная пауза сверх расчётных t3.5 / t1.5 (мкс) — компенсирует
// задержку планирования main loop и джиттер захвата таймстампа в IRQ.
#define MODBUS_RTU_SILENCE_EXTRA_US     200
#define MODBUS_RTU_INTERCHAR_EXTRA_US   100
//--------------------------------------------------------------------------//
// Минимальные значения для baud > 19200 (по спецификации Modbus RTU).
#define MODBUS_RTU_T35_FIXED_US     1750
#define MODBUS_RTU_T15_FIXED_US     750
//--------------------------------------------------------------------------//
#define MODBUS_RTU_MAX_FRAME_SIZE   256
//--------------------------------------------------------------------------//
static uint8_t  BuffRtu [_RS485_MODBUS_COUNT][MODBUS_RTU_MAX_FRAME_SIZE];
static uint16_t RtuT35Us [_RS485_MODBUS_COUNT];      // порог конца кадра
static uint16_t RtuT15Us [_RS485_MODBUS_COUNT];      // порог inter-char gap
//--------------------------------------------------------------------------//
// t3.5 = 3.5 * 11 / baud * 1e6 = 38500000 / baud (мкс).
// При baud > 19200 спецификация требует фиксировать на 1750 мкс.
static uint16_t CalcT35Us (uint32_t baudrate)
{
	uint32_t t35;

	if (baudrate == 0)
		return MODBUS_RTU_T35_FIXED_US + MODBUS_RTU_SILENCE_EXTRA_US;

	if (baudrate > 19200)
		t35 = MODBUS_RTU_T35_FIXED_US;
	else
		t35 = 38500000UL / baudrate;

	t35 += MODBUS_RTU_SILENCE_EXTRA_US;
	if (t35 > 0xFFFFU) t35 = 0xFFFFU;
	return (uint16_t)t35;
}
//--------------------------------------------------------------------------//
// t1.5 = 1.5 * 11 / baud * 1e6 = 16500000 / baud (мкс).
// При baud > 19200 спецификация требует фиксировать на 750 мкс.
static uint16_t CalcT15Us (uint32_t baudrate)
{
	uint32_t t15;

	if (baudrate == 0)
		return MODBUS_RTU_T15_FIXED_US + MODBUS_RTU_INTERCHAR_EXTRA_US;

	if (baudrate > 19200)
		t15 = MODBUS_RTU_T15_FIXED_US;
	else
		t15 = 16500000UL / baudrate;

	t15 += MODBUS_RTU_INTERCHAR_EXTRA_US;
	if (t15 > 0xFFFFU) t15 = 0xFFFFU;
	return (uint16_t)t15;
}
//--------------------------------------------------------------------------//
typedef enum { RtuIdle, RtuReceive, RtuTransmit } TRtuState;
//--------------------------------------------------------------------------//
static TRtuState RtuMode [_RS485_MODBUS_COUNT];
static uint32_t  RtuIndex [_RS485_MODBUS_COUNT];
static uint32_t  RtuSize [_RS485_MODBUS_COUNT];
static uint16_t  RtuLastTick [_RS485_MODBUS_COUNT];   // ts последнего принятого байта
static uint32_t  RtuFrameErrors [_RS485_MODBUS_COUNT];// счётчик нарушений t1.5
static TTime     RtuTxTO [_RS485_MODBUS_COUNT];
//--------------------------------------------------------------------------//
static TRxByte RtuPopRx (uint8_t N)
{
	switch (N)
	{
		case 0: return RS485_PopRxFIFO (_RS485_1);
		case 1: return RS485_PopRxFIFO (_RS485_2);
		case 2: return RS485_PopRxFIFO (_RS485_3);
		default: { TRxByte z = {0, 0}; return z; }
	}
}
//--------------------------------------------------------------------------//
static uint32_t RtuGetRxCount (uint8_t N)
{
	switch (N)
	{
		case 0: return RS485_GetNumberOfByteRxFIFO (_RS485_1);
		case 1: return RS485_GetNumberOfByteRxFIFO (_RS485_2);
		case 2: return RS485_GetNumberOfByteRxFIFO (_RS485_3);
		default: return 0;
	}
}
//--------------------------------------------------------------------------//
static uint32_t RtuGetTxFree (uint8_t N)
{
	switch (N)
	{
		case 0: return RS485_GetNumberOfFreeByteTxFIFO (_RS485_1);
		case 1: return RS485_GetNumberOfFreeByteTxFIFO (_RS485_2);
		case 2: return RS485_GetNumberOfFreeByteTxFIFO (_RS485_3);
		default: return 0;
	}
}
//--------------------------------------------------------------------------//
static uint32_t RtuPushTxBuf (uint8_t N, const uint8_t *buf, uint32_t size)
{
	switch (N)
	{
		case 0: return RS485_PushTxFIFOBuf (_RS485_1, buf, size);
		case 1: return RS485_PushTxFIFOBuf (_RS485_2, buf, size);
		case 2: return RS485_PushTxFIFOBuf (_RS485_3, buf, size);
		default: return 0;
	}
}
//--------------------------------------------------------------------------//
static uint32_t min_u32 (uint32_t a, uint32_t b)
{
	return (a < b) ? a : b;
}
//--------------------------------------------------------------------------//
void ModbusRtuRoutine_Init (void)
{
	for (int i = 0; i < _RS485_MODBUS_COUNT; i++)
	{
		uint32_t baudrate = unicorn_uart_speed_to_baudrate (gParamSystem.rs485Modbus[i].UARTSpeed);
		RtuT35Us[i]       = CalcT35Us (baudrate);
		RtuT15Us[i]       = CalcT15Us (baudrate);
		RtuMode[i]        = RtuIdle;
		RtuIndex[i]       = 0;
		RtuSize[i]        = 0;
		RtuLastTick[i]    = 0;
		RtuFrameErrors[i] = 0;
		RtuTxTO[i]        = 0;
	}
}
//--------------------------------------------------------------------------//
uint32_t ModbusRtuGetFrameErrors (uint8_t uartNum)
{
	if (uartNum >= _RS485_MODBUS_COUNT) return 0;
	return RtuFrameErrors[uartNum];
}
//--------------------------------------------------------------------------//
void ModbusRtuRoutine (void)
{
	uint32_t rxCount;
	TRxByte  rx;

	for (uint8_t uartNum = 0; uartNum < _RS485_MODBUS_COUNT; uartNum++)
	{
		if (gParamSystem.rs485Modbus[uartNum].ProtocolMode != PROTOCOL_MODBUS)
		{
			RtuMode[uartNum] = RtuIdle;
			RtuIndex[uartNum] = 0;
			RtuSize[uartNum] = 0;
			continue;
		}

		rxCount = RtuGetRxCount(uartNum);
		
		switch (RtuMode[uartNum])
{
    case RtuIdle:
			if (rxCount > 0)
      {
				rx = RtuPopRx(uartNum);

        RtuMode[uartNum] = RtuReceive;
        RtuIndex[uartNum] = 0;
        BuffRtu[uartNum][RtuIndex[uartNum]++] = rx.byte;
        RtuLastTick[uartNum] = rx.timestamp;
			}
			break;

    case RtuReceive:
			while (rxCount > 0)
				{
					rx = RtuPopRx(uartNum);
					rxCount--;

          uint16_t gap = (uint16_t)(rx.timestamp - RtuLastTick[uartNum]);

          if (gap >= RtuT35Us[uartNum])
          {
						RtuIndex[uartNum] = 0; // новый кадр
          }
          else if (gap > RtuT15Us[uartNum])
          {
						RtuFrameErrors[uartNum]++;
            RtuIndex[uartNum] = 0;
					}

          if (RtuIndex[uartNum] < MODBUS_RTU_MAX_FRAME_SIZE)
          {
						BuffRtu[uartNum][RtuIndex[uartNum]++] = rx.byte;
					}

            RtuLastTick[uartNum] = rx.timestamp;
        }

        if (RtuIndex[uartNum] > 0)
        {
					uint16_t silence = (uint16_t)(RS485_GetTick() - RtuLastTick[uartNum]);

          if (silence >= RtuT35Us[uartNum])
          {
						RtuSize[uartNum] = RtuIndex[uartNum];

						uint8_t deviceAddr = gParamSystem.rs485Modbus[uartNum].ModbusAddress;

            ModbusRtuFrame_Process(
							uartNum,
              BuffRtu[uartNum],
              &RtuSize[uartNum],
              deviceAddr
            );

            if (RtuSize[uartNum] > 0)
            {
							RtuMode[uartNum] = RtuTransmit;
              RtuIndex[uartNum] = 0;
              RtuTxTO[uartNum] = SetTime_ms(0);
            }
            else
            {
							RtuMode[uartNum] = RtuIdle;
              RtuIndex[uartNum] = 0;
            }
          }
        }
        break;

    case RtuTransmit:
        if (EndTime(RtuTxTO[uartNum]))
        {
					uint32_t txFree = RtuGetTxFree(uartNum);
          if (txFree > 0) txFree--;

          uint32_t toSend = min_u32(txFree, RtuSize[uartNum]);

          if (toSend > 0)
          {
						toSend = RtuPushTxBuf(
							uartNum,
              &BuffRtu[uartNum][RtuIndex[uartNum]],
              toSend
            );

            RtuIndex[uartNum] += toSend;
            RtuSize[uartNum] -= toSend;

            if (RtuSize[uartNum] == 0)
            {
							RtuMode[uartNum] = RtuIdle;
              RtuIndex[uartNum] = 0;
            }
          }
        }
        break;
		}
	}
}
