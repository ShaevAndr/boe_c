/*=============================================================================
2     Project: 
3     Platform: 
4     Filename: ModbusUtils.h
5     Description:
6     Version: 0.0
7     Created: 03.05.26
8     Last modified: 2017.11.23
9============================================================================*/

#ifndef ModbusUtils_H
#define ModbusUtils_H
#include <stdint.h>

typedef enum {
	NONE,
	INT,
	FLOAT,
	TELEMETRY	
} ParamType;

ParamType GetParamNumAndType (uint32_t RegisterNumver, uint8_t * paramNum);
#endif