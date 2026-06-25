/*=============================================================================
     Project:
     Platform: GD32F470
     Filename: TabParamFiles.h
     Description: Test table backend for Modbus file-record access.
============================================================================*/
#ifndef TAB_PARAM_FILES_H
#define TAB_PARAM_FILES_H

#include <stdint.h>

#define MODBUS_TABLE_TEST_FILE 100U
#define MODBUS_TABLE_TEST_INDEX 0U

uint8_t ModbusTabParam_GetCount(uint32_t *count);
uint8_t ModbusTabParam_GetDescription(uint32_t tableIndex,
	const char **description,
	uint32_t *descriptionSize);
uint8_t ModbusTabParam_ReadFile(uint16_t fileNumber,
	uint32_t offset,
	uint32_t requestedSize,
	uint8_t *destination,
	uint32_t *actualSize);
uint8_t ModbusTabParam_WriteFile(uint16_t fileNumber,
	uint32_t offset,
	const uint8_t *source,
	uint32_t size);
uint8_t ModbusTabParam_Prepare(uint32_t tableIndex, int32_t column);
uint8_t ModbusTabParam_GetPrepareProgress(uint32_t tableIndex,
	int32_t *rows,
	int32_t *columns,
	int32_t *currentStep,
	int32_t *stepsCount);
uint8_t ModbusTabParam_Release(uint32_t tableIndex);

#endif
