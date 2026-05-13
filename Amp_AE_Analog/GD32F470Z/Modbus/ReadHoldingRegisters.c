/*=============================================================================
     Project:
     Platform: GD32F470
     Filename: ReadHoldingRegisters.c
     Description: FC 0x03 - Read Holding Registers (custom 4-byte parameter
                  protocol).
     Version: 2.0
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
// Request PDU:  FC(1) | StartAddr(2) | Quantity(2)                       = 5 bytes
// Response PDU: FC(1) | ByteCount(1) | Data(Quantity * 4)                = 2 + 4*Q bytes
//   ByteCount = Quantity * 4 (8-bit; quantity capped so it fits in 1 byte)
//   Each parameter is serialized big-endian (MSB first).
//--------------------------------------------------------------------------//
#define READ_MAX_QUANTITY   60
//--------------------------------------------------------------------------//
uint8_t ReadHoldingsRegisters(uint8_t NumUART, uint8_t Command, uint8_t *B, uint32_t *pSize)
{
	uint16_t startAddr = (uint16_t)(B[1] << 8) | B[2];
	uint16_t quantity  = (uint16_t)(B[3] << 8) | B[4];

	if (quantity < 1 || quantity > READ_MAX_QUANTITY)
		return _IllegalDataValue;

	if (startAddr + quantity > GetHoldingRegisterCount())
		return _IllegalDataAddress;

	uint8_t byteCount = (uint8_t)(quantity * 4);

	B[0] = Command;
	B[1] = byteCount;

	for (uint16_t i = 0; i < quantity; i++)
	{
		uint16_t paramIndex;
		ParamType type = GetHoldingRegisterMapping(startAddr + i, &paramIndex);

		if (type == PARAM_NONE)
			return _IllegalDataAddress;

		uint32_t raw = 0;
		int8_t   err;

		if (type == PARAM_INT)
		{
			int32_t val;
			err = AccessIntParam((IntParam_t)paramIndex, &val, _PAM_RO);
			if (err != (int8_t)_NoError)
				return ConvertUnicornErrorIntoModbusError((uint8_t)err);
			raw = (uint32_t)val;
		}
		else // PARAM_FLOAT
		{
			float fval;
			err = AccessFloatParam((FloatParam_t)paramIndex, &fval, _PAM_RO);
			if (err != (int8_t)_NoError)
				return ConvertUnicornErrorIntoModbusError((uint8_t)err);
			memcpy(&raw, &fval, sizeof(raw));
		}

		uint8_t *dst = &B[2 + i * 4];
		dst[0] = (uint8_t)(raw >> 24);
		dst[1] = (uint8_t)(raw >> 16);
		dst[2] = (uint8_t)(raw >> 8);
		dst[3] = (uint8_t)(raw & 0xFF);
	}

	*pSize = 2 + byteCount;
	return _NoError;
}
