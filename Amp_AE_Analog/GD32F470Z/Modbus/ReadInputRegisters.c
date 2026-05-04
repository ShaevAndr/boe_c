/*=============================================================================
     Project:
     Platform: GD32F470
     Filename: ReadInputRegisters.c
     Description: Modbus FC 0x04 - Read Input Registers (Telemetry)
     Version: 1.0
     Created: 2026.05.04
============================================================================*/
#include <string.h>
#include "ReadInputRegisters.h"
#include "ModbusUtils.h"
#include "ErrorHandler.h"
#include "CommandParcerModbus.h"
#include "../Unicorn2/AccessTelemParam.h"
//--------------------------------------------------------------------------//
// Request PDU:  FC(1) | StartAddr(2) | Quantity(2)
// Response PDU: FC(1) | ByteCount(1) | Data(N*2)
//--------------------------------------------------------------------------//
uint8_t ReadInputRegisters(uint8_t NumUART, uint8_t Command, uint8_t *B, uint32_t *pSize)
{
	uint16_t startAddr = (uint16_t)(B[1] << 8) | B[2];
	uint16_t quantity  = (uint16_t)(B[3] << 8) | B[4];

	if (quantity < 1 || quantity > 125)
		return _IllegalDataValue;

	if (startAddr + quantity > GetInputRegisterCount())
		return _IllegalDataAddress;

	uint8_t byteCount = (uint8_t)(quantity * 2);
	B[0] = Command;
	B[1] = byteCount;

	uint16_t lastParamIndex = 0xFFFF;
	uint32_t cachedRaw = 0;

	for (uint16_t i = 0; i < quantity; i++)
	{
		uint16_t regAddr = startAddr + i;
		uint16_t paramIndex;
		uint8_t  wordOffset;
		ParamType type = GetInputRegisterMapping(regAddr, &paramIndex, &wordOffset);

		if (type == PARAM_NONE)
			return _IllegalDataAddress;

		// Read each parameter only once to avoid torn reads
		if (paramIndex != lastParamIndex)
		{
			float fval;
			int8_t err = AccessTelemParam((TelimParam_t)paramIndex, &fval);
			if (err != (int8_t)_NoError)
				return ConvertUnicornErrorIntoModbusError((uint8_t)err);
			memcpy(&cachedRaw, &fval, sizeof(cachedRaw));
			lastParamIndex = paramIndex;
		}

		uint16_t regValue = (wordOffset == 0) ? (uint16_t)(cachedRaw >> 16) : (uint16_t)(cachedRaw & 0xFFFF);

		B[2 + i * 2]     = (uint8_t)(regValue >> 8);
		B[2 + i * 2 + 1] = (uint8_t)(regValue & 0xFF);
	}

	*pSize = 2 + byteCount;
	return _NoError;
}
