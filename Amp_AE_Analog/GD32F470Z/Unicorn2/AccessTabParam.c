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

#define FACTORY_CALIBRATION_ROWS 4
#define FACTORY_CALIBRATION_COLUMNS 6
#define FACTORY_CALIBRATION_PREPARATION_STEPS 4
#define FACTORY_CALIBRATION_TABLE_SIZE \
	((uint32_t)sizeof(FactoryCalibrationParameters))

static float MockFactoryCalibrationParameters
	[FACTORY_CALIBRATION_ROWS][FACTORY_CALIBRATION_COLUMNS] = {
	{1.001F, 1.002F, 1.003F, 1.004F, 1.005F, 1.006F},
	{-0.01001F, -0.01002F, -0.01003F, -0.01004F, -0.01005F, -0.01006F},
	{0.991F, 0.992F, 0.993F, 0.994F, 0.995F, 0.996F},
	{0.101F, 0.102F, 0.103F, 0.104F, 0.105F, 0.106F}
};

static float FactoryCalibrationParameters
	[FACTORY_CALIBRATION_ROWS][FACTORY_CALIBRATION_COLUMNS];
static int32_t FactoryCalibrationPreparationStep;
static uint8_t FactoryCalibrationPreparationActive;

uint8_t GetCountTabParam (uint32_t * count)
{
	if (count == 0)
		return _ErrorSize;

	*count = _TabPCount;
	return _NoError;
}
//--------------------------------------------------------------------------//
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
		case _TabFactoryCalibrationParameters:
			if ((FactoryCalibrationPreparationActive == 0U) ||
				(FactoryCalibrationPreparationStep <
					FACTORY_CALIBRATION_PREPARATION_STEPS))
			{
				ErrorNum = _ErrorDataNotReady;
				break;
			}
			ErrorNum = CopyTableData(B,
				(const uint8_t *)FactoryCalibrationParameters,
				FACTORY_CALIBRATION_TABLE_SIZE,
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
		case _TabFactoryCalibrationParameters:
			if ((B == 0) || (stride == 0U))
			{
				ErrorNum = _ErrorSize;
				break;
			}
			if ((offset > FACTORY_CALIBRATION_TABLE_SIZE) ||
				(size > (FACTORY_CALIBRATION_TABLE_SIZE - offset)))
			{
				ErrorNum = _ErrorUnCorrParam;
				break;
			}
			for (i = 0U; i < size; i++)
			{
				uint32_t dstOffset = offset + (i / stride) * stride + (i % stride);

				((uint8_t *)MockFactoryCalibrationParameters)[dstOffset] = B[i];
			}
			FactoryCalibrationPreparationActive = 0U;
			FactoryCalibrationPreparationStep = 0;
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
		case _TabFactoryCalibrationParameters:
			(void)column;
			memset(FactoryCalibrationParameters, 0,
				sizeof(FactoryCalibrationParameters));
			FactoryCalibrationPreparationStep = 0;
			FactoryCalibrationPreparationActive = 1U;
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
		case _TabFactoryCalibrationParameters:
			if (FactoryCalibrationPreparationActive == 0U)
			{
				ErrorNum = _ErrorDataNotReady;
				break;
			}
			if (FactoryCalibrationPreparationStep <
				FACTORY_CALIBRATION_PREPARATION_STEPS)
			{
				memcpy(
					FactoryCalibrationParameters[FactoryCalibrationPreparationStep],
					MockFactoryCalibrationParameters[FactoryCalibrationPreparationStep],
					sizeof(FactoryCalibrationParameters[0]));
				FactoryCalibrationPreparationStep++;
			}
			*rows = FACTORY_CALIBRATION_ROWS;
			*column = FACTORY_CALIBRATION_COLUMNS;
			*curStep = FactoryCalibrationPreparationStep;
			*stepsCount = FACTORY_CALIBRATION_PREPARATION_STEPS;
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
		case _TabFactoryCalibrationParameters:
			memset(FactoryCalibrationParameters, 0,
				sizeof(FactoryCalibrationParameters));
			FactoryCalibrationPreparationStep = 0;
			FactoryCalibrationPreparationActive = 0U;
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
	static const char Description[] =
		"FactoryCalibrationParameters; file=100; rows=4; columns=6; "
		"data=float32; bytes=96";

	(void)NumUART;
	if (pSize == 0)
		return _ErrorSize;

	*pSize = 0;
	switch (NumParam)
	{
		case _TabFactoryCalibrationParameters:
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
