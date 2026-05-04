/*=============================================================================
     Project:
     Platform: GD32F470
     Filename: WriteMultipleRegisters.c
     Description: Modbus FC 0x10 - Write Multiple Registers
     Version: 1.0
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
// Request PDU:  FC(1) | StartAddr(2) | Quantity(2) | ByteCount(1) | Data(N*2)
// Response PDU: FC(1) | StartAddr(2) | Quantity(2)
//--------------------------------------------------------------------------//
static uint8_t WriteParamInt(uint16_t paramIndex, uint32_t fullValue)
{
	int32_t val = (int32_t)fullValue;
	int8_t err = AccessIntParam((IntParam_t)paramIndex, &val, _PAM_WO);
	return (err != (int8_t)_NoError) ? ConvertUnicornErrorIntoModbusError((uint8_t)err) : _NoError;
}
//--------------------------------------------------------------------------//
static uint8_t WriteParamFloat(uint16_t paramIndex, uint32_t fullValue)
{
	float fval;
	memcpy(&fval, &fullValue, sizeof(fval));
	int8_t err = AccessFloatParam((FloatParam_t)paramIndex, &fval, _PAM_WO);
	return (err != (int8_t)_NoError) ? ConvertUnicornErrorIntoModbusError((uint8_t)err) : _NoError;
}
//--------------------------------------------------------------------------//
static uint8_t ReadModifyWriteRegister(ParamType type, uint16_t paramIndex, uint8_t wordOffset, uint16_t regValue)
{
	uint32_t u;
	int8_t err;

	if (type == PARAM_INT)
	{
		int32_t currentVal;
		err = AccessIntParam((IntParam_t)paramIndex, &currentVal, _PAM_RO);
		if (err != (int8_t)_NoError)
			return ConvertUnicornErrorIntoModbusError((uint8_t)err);
		u = (uint32_t)currentVal;
	}
	else
	{
		float fval;
		err = AccessFloatParam((FloatParam_t)paramIndex, &fval, _PAM_RO);
		if (err != (int8_t)_NoError)
			return ConvertUnicornErrorIntoModbusError((uint8_t)err);
		memcpy(&u, &fval, sizeof(u));
	}

	if (wordOffset == 0)
		u = (u & 0x0000FFFF) | ((uint32_t)regValue << 16);
	else
		u = (u & 0xFFFF0000) | regValue;

	if (type == PARAM_INT)
		return WriteParamInt(paramIndex, u);
	else
		return WriteParamFloat(paramIndex, u);
}
//--------------------------------------------------------------------------//
uint8_t WriteMultipleRegisters(uint8_t NumUART, uint8_t Command, uint8_t *B, uint32_t *pSize)
{
	uint16_t startAddr = (uint16_t)(B[1] << 8) | B[2];
	uint16_t quantity  = (uint16_t)(B[3] << 8) | B[4];
	uint8_t  byteCount = B[5];
	uint8_t *data = &B[6];

	if (quantity < 1 || quantity > 123 || byteCount != quantity * 2)
		return _IllegalDataValue;

	if (startAddr + quantity > GetHoldingRegisterCount())
		return _IllegalDataAddress;

	for (uint16_t i = 0; i < quantity; )
	{
		uint16_t regAddr = startAddr + i;
		uint16_t paramIndex;
		uint8_t  wordOffset;
		ParamType type = GetHoldingRegisterMapping(regAddr, &paramIndex, &wordOffset);

		if (type == PARAM_NONE)
			return _IllegalDataAddress;

		// Check if we have both halves of this param (aligned pair)
		uint8_t hasPair = 0;
		if (wordOffset == 0 && (i + 1) < quantity)
		{
			uint16_t nextParamIndex;
			uint8_t  nextWordOffset;
			ParamType nextType = GetHoldingRegisterMapping(regAddr + 1, &nextParamIndex, &nextWordOffset);
			if (nextType == type && nextParamIndex == paramIndex && nextWordOffset == 1)
				hasPair = 1;
		}

		uint8_t err;

		if (hasPair)
		{
			// Full 32-bit write — no read needed
			uint16_t hi = (uint16_t)(data[i * 2] << 8) | data[i * 2 + 1];
			uint16_t lo = (uint16_t)(data[(i + 1) * 2] << 8) | data[(i + 1) * 2 + 1];
			uint32_t fullValue = ((uint32_t)hi << 16) | lo;

			if (type == PARAM_INT)
				err = WriteParamInt(paramIndex, fullValue);
			else
				err = WriteParamFloat(paramIndex, fullValue);

			if (err != _NoError)
				return err;
			i += 2;
		}
		else
		{
			// Single register — read-modify-write
			uint16_t regValue = (uint16_t)(data[i * 2] << 8) | data[i * 2 + 1];
			err = ReadModifyWriteRegister(type, paramIndex, wordOffset, regValue);
			if (err != _NoError)
				return err;
			i += 1;
		}
	}

	// Response: FC | StartAddr | Quantity (B[0..4] already correct)
	B[0] = Command;
	*pSize = 5;
	return _NoError;
}
