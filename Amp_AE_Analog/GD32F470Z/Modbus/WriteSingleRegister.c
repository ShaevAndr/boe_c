/*=============================================================================
     Project:
     Platform: GD32F470
     Filename: WriteSingleRegister.c
     Description: Modbus FC 0x06 - Write Single Register
     Version: 1.0
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
// Request PDU:  FC(1) | RegAddr(2) | Value(2)
// Response PDU: echo of request
//
// Since parameters are 32-bit (2 registers each),
// writing a single register performs read-modify-write on the affected half.
//--------------------------------------------------------------------------//
uint8_t WriteSingleRegister(uint8_t NumUART, uint8_t Command, uint8_t *B, uint32_t *pSize)
{
	uint16_t regAddr = (uint16_t)(B[1] << 8) | B[2];
	uint16_t value   = (uint16_t)(B[3] << 8) | B[4];

	uint16_t paramIndex;
	uint8_t  wordOffset;
	ParamType type = GetHoldingRegisterMapping(regAddr, &paramIndex, &wordOffset);

	if (type == PARAM_NONE)
		return _IllegalDataAddress;

	int8_t err;

	if (type == PARAM_INT)
	{
		int32_t currentVal;
		err = AccessIntParam((IntParam_t)paramIndex, &currentVal, _PAM_RO);
		if (err != (int8_t)_NoError)
			return ConvertUnicornErrorIntoModbusError((uint8_t)err);

		uint32_t u = (uint32_t)currentVal;
		if (wordOffset == 0)
			u = (u & 0x0000FFFF) | ((uint32_t)value << 16);
		else
			u = (u & 0xFFFF0000) | value;

		int32_t newVal = (int32_t)u;
		err = AccessIntParam((IntParam_t)paramIndex, &newVal, _PAM_WO);
		if (err != (int8_t)_NoError)
			return ConvertUnicornErrorIntoModbusError((uint8_t)err);
	}
	else if (type == PARAM_FLOAT)
	{
		float fval;
		err = AccessFloatParam((FloatParam_t)paramIndex, &fval, _PAM_RO);
		if (err != (int8_t)_NoError)
			return ConvertUnicornErrorIntoModbusError((uint8_t)err);

		uint32_t u;
		memcpy(&u, &fval, sizeof(u));
		if (wordOffset == 0)
			u = (u & 0x0000FFFF) | ((uint32_t)value << 16);
		else
			u = (u & 0xFFFF0000) | value;
		memcpy(&fval, &u, sizeof(fval));

		err = AccessFloatParam((FloatParam_t)paramIndex, &fval, _PAM_WO);
		if (err != (int8_t)_NoError)
			return ConvertUnicornErrorIntoModbusError((uint8_t)err);
	}

	// Response = echo of request (B unchanged)
	*pSize = 5;
	return _NoError;
}
