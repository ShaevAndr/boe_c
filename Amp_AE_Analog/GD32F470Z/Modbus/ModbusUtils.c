/*=============================================================================
     Project:
     Platform: GD32F470
     Filename: ModbusUtils.c
     Description: Modbus register to parameter mapping
     Version: 1.0
     Created: 2026.05.04
============================================================================*/
#include "ModbusUtils.h"
#include "../Unicorn2/CommandList.h"
//--------------------------------------------------------------------------//

// Holding register map:
//   [0                .. _IPCount*2-1]                    -> INT params
//   [_IPCount*2       .. _IPCount*2 + _FPCount*2 - 1]    -> FLOAT params
// Input register map:
//   [0                .. _TelPCount*2-1]                  -> TELEMETRY params

//--------------------------------------------------------------------------//
ParamType GetHoldingRegisterMapping(uint16_t regAddr, uint16_t *paramIndex, uint8_t *wordOffset)
{
	uint16_t intRegCount = _IPCount * 2;
	uint16_t floatRegCount = _FPCount * 2;

	if (regAddr < intRegCount)
	{
		*paramIndex = regAddr / 2;
		*wordOffset = regAddr % 2;
		return PARAM_INT;
	}

	uint16_t floatRegOffset = regAddr - intRegCount;
	if (floatRegOffset < floatRegCount)
	{
		*paramIndex = floatRegOffset / 2;
		*wordOffset = floatRegOffset % 2;
		return PARAM_FLOAT;
	}

	return PARAM_NONE;
}
//--------------------------------------------------------------------------//
ParamType GetInputRegisterMapping(uint16_t regAddr, uint16_t *paramIndex, uint8_t *wordOffset)
{
	uint16_t telemRegCount = _TelPCount * 2;

	if (regAddr < telemRegCount)
	{
		*paramIndex = regAddr / 2;
		*wordOffset = regAddr % 2;
		return PARAM_TELEMETRY;
	}

	return PARAM_NONE;
}
//--------------------------------------------------------------------------//
uint16_t GetHoldingRegisterCount(void)
{
	return (_IPCount + _FPCount) * 2;
}
//--------------------------------------------------------------------------//
uint16_t GetInputRegisterCount(void)
{
	return _TelPCount * 2;
}
