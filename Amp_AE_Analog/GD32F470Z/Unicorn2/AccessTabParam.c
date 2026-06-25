/*=============================================================================
2     Project:
3     Platform: GD32F470
4     Filename: AccessTabParam.c
5     Description:
6     Version: 0.0
7     Created: 2022.09.15
8     Last modified: 2026.02.08
============================================================================*/

#include <stdint.h>
#include <string.h>

#include "CommandList.h"
#include "CommandParser.h"
#include "AccessTabParam.h"

#define TEST_TABLE_SIZE 16U
#define TEST_TABLE_ROWS 4
#define TEST_TABLE_COLUMNS 4

static uint8_t TestTable[TEST_TABLE_SIZE] = {
	0x10U, 0x11U, 0x12U, 0x13U,
	0x20U, 0x21U, 0x22U, 0x23U,
	0x30U, 0x31U, 0x32U, 0x33U,
	0x40U, 0x41U, 0x42U, 0x43U
};

static uint8_t CopyTableData(uint8_t *destination, const uint8_t *source,
	uint32_t tableSize, uint32_t offset, uint32_t wordSize, uint32_t stride,
	uint32_t *size)
{
	uint32_t requested;
	uint32_t copied = 0U;

	if ((destination == 0) || (size == 0) || (wordSize == 0U) || (stride == 0U))
		return _ErrorSize;

	requested = *size;
	while (copied < requested)
	{
		uint32_t srcOffset = offset + (copied / wordSize) * stride + (copied % wordSize);

		if (srcOffset >= tableSize)
			break;

		destination[copied] = source[srcOffset];
		copied++;
	}

	*size = copied;
	return _NoError;
}

uint8_t ReadTabParam (uint8_t NumUART, uint32_t NumParam, uint8_t * B,
	uint32_t offset, uint32_t wordSize, uint32_t stride, uint32_t * size)
{
	uint8_t ErrorNum = _NoError;
	(void)NumUART;
	switch (NumParam)
	{
		case _TabTestTable:
			ErrorNum = CopyTableData(B, TestTable, TEST_TABLE_SIZE,
				offset, wordSize, stride, size);
			break;

		default: ErrorNum = _ErrorUnCorrParam; break;
	}
	return (ErrorNum);
}
//--------------------------------------------------------------------------//
uint8_t WriteTabParam (uint8_t NumUART, uint32_t NumParam, uint8_t * B,
	uint32_t offset, uint32_t stride, uint32_t size)
{
	uint8_t ErrorNum = _NoError;
	uint32_t i;

	(void)NumUART;
	switch (NumParam)
	{
		case _TabTestTable:
			if ((B == 0) || (stride == 0U))
			{
				ErrorNum = _ErrorSize;
				break;
			}
			for (i = 0U; i < size; i++)
			{
				uint32_t dstOffset = offset + (i / stride) * stride + (i % stride);

				if (dstOffset >= TEST_TABLE_SIZE)
					break;
				TestTable[dstOffset] = B[i];
			}
			break;

		default: ErrorNum = _ErrorUnCorrParam; break;
	}
	return (ErrorNum);
}
//--------------------------------------------------------------------------//
uint8_t PreparTabParam (uint8_t NumUART, uint32_t NumParam, int32_t column)
{
	uint8_t ErrorNum = _NoError;

	(void)NumUART;
	switch (NumParam)
	{
		case _TabTestTable:
			(void)column;
			ErrorNum = _NoError;
			break;

		default: ErrorNum = _ErrorUnCorrParam; break;
	}
	return (ErrorNum);
}
//--------------------------------------------------------------------------//
uint8_t ProgrPreparTabParam (uint8_t NumUART, uint32_t NumParam,
	int32_t * rows, int32_t * column, int32_t * curStep, int32_t * stepsCount)
{
	uint8_t ErrorNum = _NoError;

	(void)NumUART;
	if ((rows == 0) || (column == 0) || (curStep == 0) || (stepsCount == 0))
		return _ErrorSize;

	*rows = *column = *curStep = *stepsCount = 0;
	switch (NumParam)
	{
		case _TabTestTable:
			*rows = TEST_TABLE_ROWS;
			*column = TEST_TABLE_COLUMNS;
			*curStep = 1;
			*stepsCount = 1;
			break;

		default: ErrorNum = _ErrorUnCorrParam; break;
	}
	return (ErrorNum);
}
//--------------------------------------------------------------------------//
uint8_t ReleaseTabParam (uint8_t NumUART, uint32_t NumParam)
{
	uint8_t ErrorNum = _NoError;
	(void)NumUART;
	switch (NumParam)
	{
		case _TabTestTable:
			ErrorNum = _NoError;
			break;

		default: ErrorNum = _ErrorUnCorrParam; break;
	}
	return (ErrorNum);
}
//--------------------------------------------------------------------------//
uint8_t ReadDescrTabParam (uint8_t NumUART, uint32_t NumParam,
	uint8_t * B, uint32_t * pSize)
{
	static const char Description[] = "Test table parameter";

	(void)NumUART;
	if (pSize == 0)
		return _ErrorSize;

	*pSize = 0;
	switch (NumParam)
	{
		case _TabTestTable:
			if (B == 0)
				return _ErrorSize;
			memcpy(B, Description, sizeof(Description) - 1U);
			*pSize = sizeof(Description) - 1U;
			return _NoError;

		default:
			return _ErrorUnCorrParam;
	}
}
//--------------------------------------------------------------------------//
