/*=============================================================================
     Project:
     Platform: GD32F470
     Filename: ModbusUtils.h
     Description: Register-to-parameter mapping. One Modbus "register" address
                  = one 4-byte parameter (custom protocol, not standard Modbus).
     Version: 2.0
     Created: 2026.05.04
============================================================================*/
#ifndef ModbusUtils_H
#define ModbusUtils_H

#include <stdint.h>

typedef enum {
	PARAM_NONE,
	PARAM_INT,
	PARAM_FLOAT,
	PARAM_TELEMETRY
} ParamType;

// Map holding register address to parameter type and index.
// Each register address corresponds to one 4-byte parameter:
//   [0 .. _IPCount-1]                       -> INT params    (index = regAddr)
//   [_IPCount .. _IPCount+_FPCount-1]       -> FLOAT params  (index = regAddr - _IPCount)
ParamType GetHoldingRegisterMapping(uint16_t regAddr, uint16_t *paramIndex);

// Map input register address to telemetry parameter index.
//   [0 .. _TelPCount-1] -> TELEMETRY params (index = regAddr)
ParamType GetInputRegisterMapping(uint16_t regAddr, uint16_t *paramIndex);

// Total parameter counts (one address per parameter, no x2).
uint16_t GetHoldingRegisterCount(void);
uint16_t GetInputRegisterCount(void);

#endif
