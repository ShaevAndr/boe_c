/*=============================================================================
     Project:
     Platform: GD32F470
     Filename: TabParamFiles.c
     Description: Test table backend for Modbus file-record access.
============================================================================*/
#include <string.h>
#include "TabParamFiles.h"
#include "CommandParcerModbus.h"

#define TEST_TABLE_SIZE 16U
#define TEST_TABLE_ROWS 4
#define TEST_TABLE_COLUMNS 4

static uint8_t TestTable[TEST_TABLE_SIZE] = {
	0x10U, 0x11U, 0x12U, 0x13U,
	0x20U, 0x21U, 0x22U, 0x23U,
	0x30U, 0x31U, 0x32U, 0x33U,
	0x40U, 0x41U, 0x42U, 0x43U
};

static int IsKnownTableFile(uint16_t fileNumber)
{
	return fileNumber == (uint16_t)MODBUS_TABLE_TEST_FILE;
}

static int IsKnownTableIndex(uint32_t tableIndex)
{
	return tableIndex == MODBUS_TABLE_TEST_INDEX;
}

uint8_t ModbusTabParam_GetCount(uint32_t *count)
{
	if (count == 0)
		return _ErrorSize;

	*count = 1U;
	return _NoError;
}

uint8_t ModbusTabParam_GetDescription(uint32_t tableIndex,
	const char **description,
	uint32_t *descriptionSize)
{
	static const char TestDescription[] = "Test table parameter; file=100; bytes=16";

	if ((description == 0) || (descriptionSize == 0))
		return _ErrorSize;
	if (!IsKnownTableIndex(tableIndex))
		return _ErrorUnCorrParam;

	*description = TestDescription;
	*descriptionSize = sizeof(TestDescription) - 1U;
	return _NoError;
}

uint8_t ModbusTabParam_ReadFile(uint16_t fileNumber,
	uint32_t offset,
	uint32_t requestedSize,
	uint8_t *destination,
	uint32_t *actualSize)
{
	uint32_t available;

	if ((destination == 0) || (actualSize == 0))
		return _ErrorSize;
	if (!IsKnownTableFile(fileNumber))
		return _ErrorUnCorrParam;

	if (offset >= TEST_TABLE_SIZE)
	{
		*actualSize = 0U;
		return _NoError;
	}

	available = TEST_TABLE_SIZE - offset;
	*actualSize = requestedSize < available ? requestedSize : available;
	memcpy(destination, &TestTable[offset], *actualSize);
	return _NoError;
}

uint8_t ModbusTabParam_WriteFile(uint16_t fileNumber,
	uint32_t offset,
	const uint8_t *source,
	uint32_t size)
{
	if (source == 0)
		return _ErrorSize;
	if (!IsKnownTableFile(fileNumber))
		return _ErrorUnCorrParam;
	if ((offset > TEST_TABLE_SIZE) || (size > (TEST_TABLE_SIZE - offset)))
		return _ErrorUnCorrParam;

	memcpy(&TestTable[offset], source, size);
	return _NoError;
}

uint8_t ModbusTabParam_Prepare(uint32_t tableIndex, int32_t column)
{
	(void)column;

	if (!IsKnownTableIndex(tableIndex))
		return _ErrorUnCorrParam;
	return _NoError;
}

uint8_t ModbusTabParam_GetPrepareProgress(uint32_t tableIndex,
	int32_t *rows,
	int32_t *columns,
	int32_t *currentStep,
	int32_t *stepsCount)
{
	if ((rows == 0) || (columns == 0) || (currentStep == 0) || (stepsCount == 0))
		return _ErrorSize;
	if (!IsKnownTableIndex(tableIndex))
		return _ErrorUnCorrParam;

	*rows = TEST_TABLE_ROWS;
	*columns = TEST_TABLE_COLUMNS;
	*currentStep = 1;
	*stepsCount = 1;
	return _NoError;
}

uint8_t ModbusTabParam_Release(uint32_t tableIndex)
{
	if (!IsKnownTableIndex(tableIndex))
		return _ErrorUnCorrParam;
	return _NoError;
}
