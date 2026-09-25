/* GD32 RS-485 adapter for the portable Modbus RTU port state machine. */
#include <stdbool.h>
#include <stdint.h>

#include "../Unicorn2/drv_RS485x.h"
#include "../Unicorn2/Unicorn2Routine.h"
#include "../Unicorn2/unicorn_uart_speed.h"
#include "../Global/ParamSystem.h"
#include "Backend/ModbusBackendGd32.h"
#include "Port/ModbusRtuPort.h"
#include "ModbusRtuRoutine.h"

static ModbusRtu Rtu[_RS485_MODBUS_COUNT];
static ModbusBackend Backend[_RS485_MODBUS_COUNT];
static uint8_t RtuInitialized[_RS485_MODBUS_COUNT];

static uint32_t Gd32RxCount(void *context)
{
	switch ((uint8_t)(uintptr_t)context)
	{
		case 0U: return RS485_GetNumberOfByteRxFIFO(_RS485_1);
		case 1U: return RS485_GetNumberOfByteRxFIFO(_RS485_2);
		case 2U: return RS485_GetNumberOfByteRxFIFO(_RS485_3);
		default: return 0U;
	}
}

static bool Gd32RxPop(void *context, ModbusRtuRxByte *byte)
{
	TRxByte received;
	switch ((uint8_t)(uintptr_t)context)
	{
		case 0U: received = RS485_PopRxFIFO(_RS485_1); break;
		case 1U: received = RS485_PopRxFIFO(_RS485_2); break;
		case 2U: received = RS485_PopRxFIFO(_RS485_3); break;
		default: return false;
	}
	byte->byte = received.byte;
	byte->timestamp = received.timestamp;
	return true;
}

static uint16_t Gd32TickUs(void *context)
{
	(void)context;
	return RS485_GetTick();
}

static uint32_t Gd32TxFree(void *context)
{
	switch ((uint8_t)(uintptr_t)context)
	{
		case 0U: return RS485_GetNumberOfFreeByteTxFIFO(_RS485_1);
		case 1U: return RS485_GetNumberOfFreeByteTxFIFO(_RS485_2);
		case 2U: return RS485_GetNumberOfFreeByteTxFIFO(_RS485_3);
		default: return 0U;
	}
}

static uint32_t Gd32TxWrite(void *context, const uint8_t *data, uint32_t size)
{
	switch ((uint8_t)(uintptr_t)context)
	{
		case 0U: return RS485_PushTxFIFOBuf(_RS485_1, data, size);
		case 1U: return RS485_PushTxFIFOBuf(_RS485_2, data, size);
		case 2U: return RS485_PushTxFIFOBuf(_RS485_3, data, size);
		default: return 0U;
	}
}

static void InitPort(uint8_t uartNumber)
{
	ModbusRtuPort port;
	uint32_t baudrate = unicorn_uart_speed_to_baudrate(
		gParamSystem.rs485Modbus[uartNumber].UARTSpeed);

	port.context = (void *)(uintptr_t)uartNumber;
	port.rxCount = Gd32RxCount;
	port.rxPop = Gd32RxPop;
	port.tickUs = Gd32TickUs;
	port.txFree = Gd32TxFree;
	port.txWrite = Gd32TxWrite;
	ModbusBackendGd32_Init(&Backend[uartNumber], uartNumber);
	ModbusRtu_Init(&Rtu[uartNumber], &port, &Backend[uartNumber],
		gParamSystem.rs485Modbus[uartNumber].ModbusAddress, baudrate);
	RtuInitialized[uartNumber] = 1U;
}

void ModbusRtuRoutine_Init(void)
{
	uint8_t uartNumber;
	for (uartNumber = 0U; uartNumber < _RS485_MODBUS_COUNT; uartNumber++)
		InitPort(uartNumber);
}

uint32_t ModbusRtuGetFrameErrors(uint8_t uartNumber)
{
	if (uartNumber >= _RS485_MODBUS_COUNT) return 0U;
	return ModbusRtu_FrameErrors(&Rtu[uartNumber]);
}

void ModbusRtuRoutine(void)
{
	uint8_t uartNumber;
	for (uartNumber = 0U; uartNumber < _RS485_MODBUS_COUNT; uartNumber++)
	{
		if (gParamSystem.rs485Modbus[uartNumber].ProtocolMode != PROTOCOL_MODBUS)
		{
			ModbusRtu_Reset(&Rtu[uartNumber]);
			RtuInitialized[uartNumber] = 0U;
			continue;
		}
		if (RtuInitialized[uartNumber] == 0U)
			InitPort(uartNumber);
		/* Address changes are applied without resetting an in-flight RTU port. */
		Rtu[uartNumber].deviceAddress =
			gParamSystem.rs485Modbus[uartNumber].ModbusAddress;
		ModbusRtu_Poll(&Rtu[uartNumber]);
	}
}
