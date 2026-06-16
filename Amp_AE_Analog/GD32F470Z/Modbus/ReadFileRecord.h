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
#include <string.h>
#include "../deviceInfo/deviceDescription.h"

typedef enum {
	_DeviceDescription = 1,
	_DeviceTypeVersion,
	_ModbusFileCounts
} modbusFiles;

static const char* GetFileData(uint16_t fileNumber, uint32_t *size);


uint8_t ReadFileRecord(uint8_t NumUART, uint8_t Command,	uint8_t * B, uint32_t * pSize);
#endif
