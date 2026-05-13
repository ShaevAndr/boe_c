/*=============================================================================
     Project:
     Platform: GD32F470
     Filename: WriteSingleRegister.c
     Description: FC 0x06 - Write Single Register / Parameter
                  (custom 4-byte parameter protocol).
     Version: 2.0
     Created: 2026.05.04
============================================================================*/
#include <string.h>
#include "WriteSingleRegister.h"
#include "ModbusUtils.h"
#include "ErrorHandler.h"
#include "CommandParcerModbus.h"
#include "../Unicorn2/AccessIntParam.h"
#include "../Unicorn2/AccessFloatParam.h"
//--------------------------------------------------------------------------//
// Request PDU:  FC(1) | RegAddr(2) | Value(4)                            = 7 bytes
// Response PDU: echo of request                                          = 7 bytes
//   RegAddr = parameter index. Value is the full 4-byte parameter
//   (big-endian: byte 0 = MSB).
//--------------------------------------------------------------------------//
uint8_t WriteSingleRegister(uint8_t NumUART, uint8_t Command, uint8_t *B, uint32_t *pSize)
{
	uint16_t regAddr = (uint16_t)(B[1] << 8) | B[2];
	uint32_t raw     = ((uint32_t)B[3] << 24)
	                 | ((uint32_t)B[4] << 16)
	                 | ((uint32_t)B[5] << 8)
	                 |  (uint32_t)B[6];

	uint16_t paramIndex;
	ParamType type = GetHoldingRegisterMapping(regAddr, &paramIndex);

	if (type == PARAM_NONE)
		return _IllegalDataAddress;

	int8_t err;

	if (type == PARAM_INT)
	{
		int32_t newVal = (int32_t)raw;
		err = AccessIntParam((IntParam_t)paramIndex, &newVal, _PAM_WO);
		if (err != (int8_t)_NoError)
			return ConvertUnicornErrorIntoModbusError((uint8_t)err);
	}
	else // PARAM_FLOAT
	{
		float fval;
		memcpy(&fval, &raw, sizeof(fval));
		err = AccessFloatParam((FloatParam_t)paramIndex, &fval, _PAM_WO);
		if (err != (int8_t)_NoError)
			return ConvertUnicornErrorIntoModbusError((uint8_t)err);
	}

	// Response = echo of request (B[0..6] unchanged).
	*pSize = 7;
	return _NoError;
}
