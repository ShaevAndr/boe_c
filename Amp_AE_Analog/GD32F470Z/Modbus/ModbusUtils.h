/*=============================================================================
     Project:
     Platform: GD32F470
     Filename: ModbusUtils.h
     Description: Modbus register to parameter type/index mapping
     Version: 1.0
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

// Map holding register address to parameter type, index, and word offset (0=hi, 1=lo)
ParamType GetHoldingRegisterMapping(uint16_t regAddr, uint16_t *paramIndex, uint8_t *wordOffset);

// Map input register address to telemetry parameter index and word offset
ParamType GetInputRegisterMapping(uint16_t regAddr, uint16_t *paramIndex, uint8_t *wordOffset);

// Total register counts
uint16_t GetHoldingRegisterCount(void);
uint16_t GetInputRegisterCount(void);

#endif
