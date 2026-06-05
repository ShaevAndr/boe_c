/*=============================================================================
2     Project: 
3     Platform: 
4     Filename: ReadFileRecord.h
5     Description:
6     Version: 0.0
7     Created: 05.06.26
8     Last modified: 2026.06.05
9============================================================================*/

#ifndef ReadFileRecord_H
#define ReadFileRecord_H
#include <stdint.h>

typedef enum {
	_DeviceDescription = 1,
	_ModbusFileCounts
} modbusFiles;

uint8_t ReadFileRecord(uint8_t NumUART, uint8_t Command,	uint8_t * B, uint32_t * pSize);
#endif