#include <stdbool.h>
#include <stdint.h>

#include "ModbusCore.h"
#include "../ErrorHandler.h"
#include "../CommandParcerModbus.h"

uint16_t ModbusCore_Crc16(const uint8_t *data, uint32_t length)
{
	uint16_t crc = 0xFFFFU;
	uint32_t i;
	uint8_t bit;

	for (i = 0U; i < length; i++)
	{
		crc ^= data[i];
		for (bit = 0U; bit < 8U; bit++)
			crc = (crc & 1U) ? (uint16_t)((crc >> 1) ^ 0xA001U) : (uint16_t)(crc >> 1);
	}
	return crc;
}

void ModbusCore_ProcessRtuFrame(const ModbusBackend *backend, uint8_t *frame,
	uint32_t *frameSize, uint8_t deviceAddress)
{
	uint32_t pduSize;
	uint32_t size = *frameSize;
	uint8_t *pdu;
	uint8_t functionCode;
	uint8_t error;
	uint16_t crc;
	bool broadcast;

	if ((backend == 0) || (backend->processPdu == 0) || (size < 4U))
	{
		*frameSize = 0U;
		return;
	}
	crc = (uint16_t)frame[size - 2U] | ((uint16_t)frame[size - 1U] << 8);
	if (crc != ModbusCore_Crc16(frame, size - 2U))
	{
		*frameSize = 0U;
		return;
	}
	broadcast = (frame[0] == 0U);
	if ((frame[0] != deviceAddress) && !broadcast)
	{
		*frameSize = 0U;
		return;
	}
	if (backend->frameActivity != 0)
		backend->frameActivity(backend->context);
	pdu = &frame[1];
	pduSize = size - 3U;
	functionCode = pdu[0];
	error = backend->processPdu(backend->context, pdu, &pduSize);
	if (broadcast)
	{
		*frameSize = 0U;
		return;
	}
	if (error != _NoError)
	{
		frame[0] = deviceAddress;
		frame[1] = functionCode | 0x80U;
		frame[2] = error;
		crc = ModbusCore_Crc16(frame, 3U);
		frame[3] = (uint8_t)crc;
		frame[4] = (uint8_t)(crc >> 8);
		*frameSize = 5U;
		return;
	}
	frame[0] = deviceAddress;
	crc = ModbusCore_Crc16(frame, 1U + pduSize);
	frame[1U + pduSize] = (uint8_t)crc;
	frame[2U + pduSize] = (uint8_t)(crc >> 8);
	*frameSize = 3U + pduSize;
}
