/*=============================================================================
     Project:
     Platform: GD32F470
     Filename: WriteMultipleRegisters.c
     Description: FC 0x10 - Write Multiple Registers / Parameters
                  (custom 4-byte parameter protocol).
     Version: 2.0
     Created: 2026.05.04
============================================================================*/
#include <string.h>
#include "WriteMultipleRegisters.h"
#include "ModbusUtils.h"
#include "ErrorHandler.h"
#include "CommandParcerModbus.h"
#include "../Unicorn2/AccessIntParam.h"
#include "../Unicorn2/AccessFloatParam.h"
//--------------------------------------------------------------------------//
// Request PDU:  FC(1) | StartAddr(2) | Quantity(2) | ByteCount(1) | Data(4*Q)
//                                                                          = 6 + 4*Q bytes
// Response PDU: FC(1) | StartAddr(2) | Quantity(2)                         = 5 bytes
//   ByteCount = Quantity * 4 (8-bit; quantity capped so it fits in 1 byte).
//   Each parameter is 4 bytes big-endian.
//--------------------------------------------------------------------------//
#define WRITE_MAX_QUANTITY  60
//--------------------------------------------------------------------------//
uint8_t WriteMultipleRegisters(uint8_t NumUART, uint8_t Command, uint8_t *B, uint32_t *pSize)
{
	uint32_t reqSize   = *pSize;
	uint16_t startAddr = (uint16_t)(B[1] << 8) | B[2];
	uint16_t quantity  = (uint16_t)(B[3] << 8) | B[4];
	uint8_t  byteCount = B[5];
	uint8_t *data      = &B[6];

	if (quantity < 1 || quantity > WRITE_MAX_QUANTITY || byteCount != (uint8_t)(quantity * 4))
		return _IllegalDataValue;

	// Ensure the request actually carries all declared data bytes.
	if (reqSize < (uint32_t)(6 + byteCount))
		return _IllegalDataValue;

	if (startAddr + quantity > GetHoldingRegisterCount())
		return _IllegalDataAddress;

	for (uint16_t i = 0; i < quantity; i++)
	{
		uint16_t paramIndex;
		ParamType type = GetHoldingRegisterMapping(startAddr + i, &paramIndex);

		if (type == PARAM_NONE)
			return _IllegalDataAddress;

		uint8_t *src = &data[i * 4];
		uint32_t raw = ((uint32_t)src[0] << 24)
		             | ((uint32_t)src[1] << 16)
		             | ((uint32_t)src[2] << 8)
		             |  (uint32_t)src[3];

		int8_t err;
		if (type == PARAM_INT)
		{
			int32_t newVal = (int32_t)raw;
			err = AccessIntParam((IntParam_t)paramIndex, &newVal, _PAM_WO);
		}
		else // PARAM_FLOAT
		{
			float fval;
			memcpy(&fval, &raw, sizeof(fval));
			err = AccessFloatParam((FloatParam_t)paramIndex, &fval, _PAM_WO);
		}

		if (err != (int8_t)_NoError)
			return ConvertUnicornErrorIntoModbusError((uint8_t)err);
	}

	// Response: FC | StartAddr | Quantity (B[0..4] already correct).
	B[0] = Command;
	*pSize = 5;
	return _NoError;
}
