/*=============================================================================
     Project:
     Platform: GD32F470
     Filename: Diagnostics.c
     Description: FC 0x08 - Diagnostics extensions for table parameters.
============================================================================*/
#include <string.h>
#include "Diagnostics.h"
#include "ErrorHandler.h"
#include "CommandParcerModbus.h"
#include "TabParamFiles.h"

#define DIAG_MAX_PDU_SIZE 253U

static uint32_t GetU32Be(const uint8_t *source)
{
	return ((uint32_t)source[0] << 24) |
	       ((uint32_t)source[1] << 16) |
	       ((uint32_t)source[2] << 8) |
	       (uint32_t)source[3];
}

static int32_t GetI32Be(const uint8_t *source)
{
	return (int32_t)GetU32Be(source);
}

static void PutU32Be(uint8_t *destination, uint32_t value)
{
	destination[0] = (uint8_t)(value >> 24);
	destination[1] = (uint8_t)(value >> 16);
	destination[2] = (uint8_t)(value >> 8);
	destination[3] = (uint8_t)value;
}

static void PutI32Be(uint8_t *destination, int32_t value)
{
	PutU32Be(destination, (uint32_t)value);
}

uint8_t Diagnostics(uint8_t NumUART, uint8_t Command, uint8_t *B, uint32_t *pSize)
{
	uint32_t reqSize = *pSize;
	uint16_t subFunction = (uint16_t)(((uint16_t)B[1] << 8) | B[2]);
	uint8_t err;

	(void)NumUART;
	B[0] = Command;

	switch (subFunction)
	{
		case MODBUS_DIAG_TABLE_COUNT:
		{
			uint32_t count;

			if (reqSize != 3U)
				return _IllegalDataValue;
			err = ModbusTabParam_GetCount(&count);
			if (err != _NoError)
				return ConvertUnicornErrorIntoModbusError(err);

			PutU32Be(&B[3], count);
			*pSize = 7U;
			return _NoError;
		}

		case MODBUS_DIAG_TABLE_DESCRIPTION:
		{
			uint32_t tableIndex;
			const char *description;
			uint32_t descriptionSize;

			if (reqSize != 7U)
				return _IllegalDataValue;
			tableIndex = GetU32Be(&B[3]);
			err = ModbusTabParam_GetDescription(tableIndex, &description, &descriptionSize);
			if (err != _NoError)
				return ConvertUnicornErrorIntoModbusError(err);
			if ((11U + descriptionSize) > DIAG_MAX_PDU_SIZE)
				return _IllegalDataValue;

			PutU32Be(&B[3], tableIndex);
			PutU32Be(&B[7], descriptionSize);
			memcpy(&B[11], description, descriptionSize);
			*pSize = 11U + descriptionSize;
			return _NoError;
		}

		case MODBUS_DIAG_TABLE_PREPARE:
		{
			uint32_t tableIndex;
			int32_t column;

			if (reqSize != 11U)
				return _IllegalDataValue;
			tableIndex = GetU32Be(&B[3]);
			column = GetI32Be(&B[7]);
			err = ModbusTabParam_Prepare(tableIndex, column);
			if (err != _NoError)
				return ConvertUnicornErrorIntoModbusError(err);

			PutU32Be(&B[3], tableIndex);
			PutI32Be(&B[7], column);
			*pSize = 11U;
			return _NoError;
		}

		case MODBUS_DIAG_TABLE_PREPARE_PROGRESS:
		{
			uint32_t tableIndex;
			int32_t rows;
			int32_t columns;
			int32_t currentStep;
			int32_t stepsCount;

			if (reqSize != 7U)
				return _IllegalDataValue;
			tableIndex = GetU32Be(&B[3]);
			err = ModbusTabParam_GetPrepareProgress(tableIndex, &rows, &columns,
				&currentStep, &stepsCount);
			if (err != _NoError)
				return ConvertUnicornErrorIntoModbusError(err);

			PutU32Be(&B[3], tableIndex);
			PutI32Be(&B[7], currentStep);
			PutI32Be(&B[11], stepsCount);
			PutI32Be(&B[15], rows);
			PutI32Be(&B[19], columns);
			*pSize = 23U;
			return _NoError;
		}

		case MODBUS_DIAG_TABLE_RELEASE:
		{
			uint32_t tableIndex;

			if (reqSize != 7U)
				return _IllegalDataValue;
			tableIndex = GetU32Be(&B[3]);
			err = ModbusTabParam_Release(tableIndex);
			if (err != _NoError)
				return ConvertUnicornErrorIntoModbusError(err);

			PutU32Be(&B[3], tableIndex);
			*pSize = 7U;
			return _NoError;
		}

		default:
			return _IllegalDataValue;
	}
}
