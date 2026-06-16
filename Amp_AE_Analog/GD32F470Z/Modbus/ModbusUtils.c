/*=============================================================================
     Project:
     Platform: GD32F470
     Filename: ModbusUtils.c
     Description: Register-to-parameter mapping. One register address = one
                  4-byte parameter.
     Version: 2.0
     Created: 2026.05.04
============================================================================*/
#include "ModbusUtils.h"
#include "../Unicorn2/CommandList.h"
//--------------------------------------------------------------------------//
ParamType GetHoldingRegisterMapping(uint16_t regAddr, uint16_t *paramIndex)
{
	if (regAddr < (uint16_t)_IPCount)
	{
		*paramIndex = regAddr;
		return PARAM_INT;
	}

	uint16_t floatOffset = regAddr - (uint16_t)_IPCount;
	if (floatOffset < (uint16_t)_FPCount)
	{
		*paramIndex = floatOffset;
		return PARAM_FLOAT;
	}

	return PARAM_NONE;
}
//--------------------------------------------------------------------------//
ParamType GetInputRegisterMapping(uint16_t regAddr, uint16_t *paramIndex)
{
	if (regAddr < (uint16_t)_TelPCount)
	{
		*paramIndex = regAddr;
		return PARAM_TELEMETRY;
	}

	return PARAM_NONE;
}
//--------------------------------------------------------------------------//
uint16_t GetHoldingRegisterCount(void)
{
	return (uint16_t)(_IPCount + _FPCount);
}
//--------------------------------------------------------------------------//
uint16_t GetInputRegisterCount(void)
{
	return (uint16_t)_TelPCount;
}
