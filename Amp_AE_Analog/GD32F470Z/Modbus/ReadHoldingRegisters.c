/*=============================================================================
     Project:
     Platform: GD32F470
     Filename: ReadHoldingRegisters.c
     Description: Modbus FC 0x03 - Read Holding Registers
     Version: 1.0
     Created: 2026.05.04
============================================================================*/
#include <string.h>
#include "ReadHoldingRegisters.h"
#include "ModbusUtils.h"
#include "ErrorHandler.h"
#include "CommandParcerModbus.h"
#include "../Unicorn2/AccessIntParam.h"
#include "../Unicorn2/AccessFloatParam.h"
//--------------------------------------------------------------------------//
// Request PDU:  FC(1) | StartAddr(2) | Quantity(2)
// Response PDU: FC(1) | ByteCount(1) | Data(N*2)
//--------------------------------------------------------------------------//
uint8_t ReadHoldingsRegisters(uint8_t NumUART, uint8_t Command, uint8_t *B, uint32_t *pSize)
{
	uint16_t startAddr = (uint16_t)(B[1] << 8) | B[2];
	uint16_t quantity  = (uint16_t)(B[3] << 8) | B[4];

	if (quantity < 1 || quantity > 125)
		return _IllegalDataValue;

	if (startAddr + quantity > GetHoldingRegisterCount())
		return _IllegalDataAddress;

	uint8_t byteCount = (uint8_t)(quantity * 2);
	B[0] = Command;
	B[1] = byteCount;

	uint16_t lastParamIndex = 0xFFFF;
	ParamType lastType = PARAM_NONE;
	uint32_t cachedRaw = 0;

	for (uint16_t i = 0; i < quantity; i++)
	{
		uint16_t regAddr = startAddr + i;
		uint16_t paramIndex;
		uint8_t  wordOffset;
		ParamType type = GetHoldingRegisterMapping(regAddr, &paramIndex, &wordOffset);

		if (type == PARAM_NONE)
			return _IllegalDataAddress;

		// Read each parameter only once to avoid torn reads
		if (paramIndex != lastParamIndex || type != lastType)
		{
			int8_t err;
			if (type == PARAM_INT)
			{
				int32_t val;
				err = AccessIntParam((IntParam_t)paramIndex, &val, _PAM_RO);
				if (err != (int8_t)_NoError)
					return ConvertUnicornErrorIntoModbusError((uint8_t)err);
				cachedRaw = (uint32_t)val;
			}
			else if (type == PARAM_FLOAT)
			{
				float fval;
				err = AccessFloatParam((FloatParam_t)paramIndex, &fval, _PAM_RO);
				if (err != (int8_t)_NoError)
					return ConvertUnicornErrorIntoModbusError((uint8_t)err);
				memcpy(&cachedRaw, &fval, sizeof(cachedRaw));
			}
			lastParamIndex = paramIndex;
			lastType = type;
		}

		uint16_t regValue = (wordOffset == 0) ? (uint16_t)(cachedRaw >> 16) : (uint16_t)(cachedRaw & 0xFFFF);
		B[2 + i * 2]     = (uint8_t)(regValue >> 8);
		B[2 + i * 2 + 1] = (uint8_t)(regValue & 0xFF);
	}

	*pSize = 2 + byteCount;
	return _NoError;
}
