#include <stdint.h>
#include <string.h>

#include "ModbusRtuPort.h"
#include "../Core/ModbusCore.h"

#define RTU_IDLE 0U
#define RTU_RECEIVE 1U
#define RTU_TRANSMIT 2U
#define RTU_T35_FIXED_US 1750U
#define RTU_T15_FIXED_US 750U
#define RTU_SILENCE_EXTRA_US 200U
#define RTU_INTERCHAR_EXTRA_US 100U

static uint16_t CalcT35Us(uint32_t baudrate)
{
	uint32_t value = (baudrate == 0U || baudrate > 19200U) ? RTU_T35_FIXED_US : 38500000UL / baudrate;
	value += RTU_SILENCE_EXTRA_US;
	return (value > 0xFFFFU) ? 0xFFFFU : (uint16_t)value;
}

static uint16_t CalcT15Us(uint32_t baudrate)
{
	uint32_t value = (baudrate == 0U || baudrate > 19200U) ? RTU_T15_FIXED_US : 16500000UL / baudrate;
	value += RTU_INTERCHAR_EXTRA_US;
	return (value > 0xFFFFU) ? 0xFFFFU : (uint16_t)value;
}

void ModbusRtu_Init(ModbusRtu *rtu, const ModbusRtuPort *port,
	const ModbusBackend *backend, uint8_t deviceAddress, uint32_t baudrate)
{
	memset(rtu, 0, sizeof(*rtu));
	rtu->port = *port;
	rtu->backend = backend;
	rtu->deviceAddress = deviceAddress;
	rtu->t35Us = CalcT35Us(baudrate);
	rtu->t15Us = CalcT15Us(baudrate);
}

void ModbusRtu_Reset(ModbusRtu *rtu)
{
	rtu->state = RTU_IDLE;
	rtu->index = 0U;
	rtu->size = 0U;
}

uint32_t ModbusRtu_FrameErrors(const ModbusRtu *rtu)
{
	return rtu->frameErrors;
}

void ModbusRtu_Poll(ModbusRtu *rtu)
{
	ModbusRtuRxByte byte;
	uint16_t gap;
	uint16_t silence;
	uint32_t toSend;

	if ((rtu->port.rxCount == 0) || (rtu->port.rxPop == 0) ||
		(rtu->port.tickUs == 0) || (rtu->port.txFree == 0) || (rtu->port.txWrite == 0))
		return;
	if (rtu->state == RTU_IDLE)
	{
		if ((rtu->port.rxCount(rtu->port.context) == 0U) ||
			!rtu->port.rxPop(rtu->port.context, &byte)) return;
		rtu->frame[0] = byte.byte;
		rtu->index = 1U;
		rtu->lastTick = byte.timestamp;
		rtu->state = RTU_RECEIVE;
		return;
	}
	if (rtu->state == RTU_RECEIVE)
	{
		while ((rtu->port.rxCount(rtu->port.context) > 0U) &&
			rtu->port.rxPop(rtu->port.context, &byte))
		{
			gap = (uint16_t)(byte.timestamp - rtu->lastTick);
			if (gap >= rtu->t35Us) rtu->index = 0U;
			else if (gap > rtu->t15Us) { rtu->frameErrors++; rtu->index = 0U; }
			if (rtu->index < MODBUS_RTU_MAX_FRAME_SIZE) rtu->frame[rtu->index++] = byte.byte;
			rtu->lastTick = byte.timestamp;
		}
		if (rtu->index == 0U) return;
		silence = (uint16_t)(rtu->port.tickUs(rtu->port.context) - rtu->lastTick);
		if (silence < rtu->t35Us) return;
		rtu->size = rtu->index;
		ModbusCore_ProcessRtuFrame(rtu->backend, rtu->frame, &rtu->size, rtu->deviceAddress);
		rtu->index = 0U;
		rtu->state = (rtu->size == 0U) ? RTU_IDLE : RTU_TRANSMIT;
		return;
	}
	toSend = rtu->port.txFree(rtu->port.context);
	if (toSend > 0U) toSend--; /* keep one FIFO byte free, matching existing driver policy */
	if (toSend > rtu->size) toSend = rtu->size;
	if (toSend == 0U) return;
	toSend = rtu->port.txWrite(rtu->port.context, &rtu->frame[rtu->index], toSend);
	rtu->index += toSend;
	rtu->size -= toSend;
	if (rtu->size == 0U) { rtu->index = 0U; rtu->state = RTU_IDLE; }
}
