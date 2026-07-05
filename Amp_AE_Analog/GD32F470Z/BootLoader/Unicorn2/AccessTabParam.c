/*=============================================================================
2     Project:
3     Platform: GD32F470
4     Filename: AccessTabParam.c
5     Description: Access to bootloader table parameters
6     Version: 1.0
7     Created: 2023.08.31
8     Last modified: 2026.07.05
============================================================================*/

#include <stdint.h>
#include <string.h>

#include "ParamSystem.h"
#include "CommandList.h"
#include "CommandParser.h"
#include "Unicorn2Routine.h"
#include "unicorn_uart_speed.h"
#include "AccessTabParam.h"

#define RS485_TABLE_ROWS       3U
#define RS485_TABLE_COLUMNS    3U
#define RS485_TABLE_SIZE \
	(RS485_TABLE_ROWS * RS485_TABLE_COLUMNS * (uint32_t)sizeof(int32_t))

#define RS485_ROW_ADDRESS      0U
#define RS485_ROW_SPEED        1U
#define RS485_ROW_PROTOCOL     2U

#define APP_PROTOCOL_UNICORN_VALUE 0
#define APP_PROTOCOL_MODBUS_VALUE  1

static void MakeRS485Table(int32_t table[RS485_TABLE_ROWS][RS485_TABLE_COLUMNS])
{
	uint32_t column;

	for (column = 0U; column < RS485_TABLE_COLUMNS; column++)
	{
		table[RS485_ROW_ADDRESS][column] =
			(int32_t)gParamSystem.rs485Modbus[column].ModbusAddress;
		table[RS485_ROW_SPEED][column] =
			(int32_t)gParamSystem.rs485Modbus[column].UARTSpeed;
		table[RS485_ROW_PROTOCOL][column] =
			(int32_t)gParamSystem.rs485Modbus[column].ProtocolMode;
	}
}

static uint8_t CopyTableData(uint8_t *destination, const uint8_t *source,
	uint32_t tableSize, uint32_t offset, uint32_t wordSize, uint32_t stride,
	uint32_t *size)
{
	uint32_t requested;
	uint32_t copied = 0U;

	if ((destination == 0) || (source == 0) || (size == 0) ||
		(wordSize == 0U) || (stride == 0U) || (wordSize > stride))
	{
		return _ErrorSize;
	}

	requested = *size;
	while (copied < requested)
	{
		uint32_t word = copied / wordSize;
		uint32_t byteInWord = copied % wordSize;
		uint32_t sourceOffset;

		if ((word != 0U) && (stride > ((UINT32_MAX - offset) / word)))
			break;

		sourceOffset = offset + word * stride;
		if (byteInWord > (UINT32_MAX - sourceOffset))
			break;
		sourceOffset += byteInWord;

		if (sourceOffset >= tableSize)
			break;

		destination[copied++] = source[sourceOffset];
	}

	*size = copied;
	return _NoError;
}

uint8_t ReadTabParam(uint8_t NumUART, uint32_t NumParam, uint8_t *B,
	uint32_t offset, uint32_t wordSize, uint32_t stride, uint32_t *size)
{
	uint8_t ErrorNum = _NoError;
	int32_t table[RS485_TABLE_ROWS][RS485_TABLE_COLUMNS];

	(void)NumUART;
	switch (NumParam)
	{
		case _tblpRS485Params:
			MakeRS485Table(table);
			ErrorNum = CopyTableData(B, (const uint8_t *)table,
				RS485_TABLE_SIZE, offset, wordSize, stride, size);
			break;

		default:
			ErrorNum = _ErrorUnCorrParam;
			break;
	}
	return ErrorNum;
}

static uint8_t ValidateRS485Table(
	const int32_t table[RS485_TABLE_ROWS][RS485_TABLE_COLUMNS])
{
	uint32_t column;

	for (column = 0U; column < RS485_TABLE_COLUMNS; column++)
	{
		if ((table[RS485_ROW_ADDRESS][column] < 1) ||
			(table[RS485_ROW_ADDRESS][column] > 254))
		{
			return _ErrorUnCorrParam;
		}
		if ((table[RS485_ROW_SPEED][column] < 0) ||
			(table[RS485_ROW_SPEED][column] > _speedMaxNum))
		{
			return _ErrorUnCorrParam;
		}
		if ((table[RS485_ROW_PROTOCOL][column] !=
				APP_PROTOCOL_UNICORN_VALUE) &&
			(table[RS485_ROW_PROTOCOL][column] !=
				APP_PROTOCOL_MODBUS_VALUE))
		{
			return _ErrorUnCorrParam;
		}
	}

	return _NoError;
}

uint8_t WriteTabParam(uint8_t NumUART, uint32_t NumParam, uint8_t *B,
	uint32_t offset, uint32_t stride, uint32_t size)
{
	uint8_t ErrorNum = _NoError;
	int32_t table[RS485_TABLE_ROWS][RS485_TABLE_COLUMNS];
	uint32_t column;
	uint8_t protocolChanged = 0U;
	uint8_t error;

	(void)NumUART;
	switch (NumParam)
	{
		case _tblpRS485Params:
			if ((B == 0) || (stride == 0U))
				return _ErrorSize;
			if ((offset > RS485_TABLE_SIZE) ||
				(size > (RS485_TABLE_SIZE - offset)))
			{
				return _ErrorUnCorrParam;
			}

			/*
			 * The write command carries no wordSize field. Its payload is a
			 * contiguous byte range; stride is retained for protocol
			 * compatibility.
			 */
			MakeRS485Table(table);
			memcpy((uint8_t *)table + offset, B, size);

			error = ValidateRS485Table(table);
			if (error != _NoError)
				return error;

			for (column = 0U; column < RS485_TABLE_COLUMNS; column++)
			{
				if (table[RS485_ROW_SPEED][column] !=
					(int32_t)gParamSystem.rs485Modbus[column].UARTSpeed)
				{
					SetUARTSpeed((uint8_t)column,
						(uint8_t)table[RS485_ROW_SPEED][column]);
				}
				if (table[RS485_ROW_ADDRESS][column] !=
					(int32_t)gParamSystem.rs485Modbus[column].ModbusAddress)
				{
					SetDeviceAddress((uint8_t)column,
						(uint8_t)table[RS485_ROW_ADDRESS][column]);
				}
				if (table[RS485_ROW_PROTOCOL][column] !=
					(int32_t)gParamSystem.rs485Modbus[column].ProtocolMode)
				{
					/*
					 * ProtocolMode is consumed only by the main application.
					 * The bootloader always uses Unicorn protocol (mode 0).
					 */
					gParamSystem.rs485Modbus[column].ProtocolMode =
						(uint8_t)table[RS485_ROW_PROTOCOL][column];
					protocolChanged = 1U;
				}
			}

			if (protocolChanged != 0U)
				SaveParamSystem();
			break;

		default:
			ErrorNum = _ErrorUnCorrParam;
			break;
	}

	return ErrorNum;
}

uint8_t PreparTabParam(uint8_t NumUART, uint32_t NumParam, int32_t column)
{
	uint8_t ErrorNum = _NoError;

	(void)NumUART;
	(void)column;
	switch (NumParam)
	{
		default:
			ErrorNum = _ErrorUnCorrParam;
			break;
	}
	return ErrorNum;
}

uint8_t ProgrPreparTabParam(uint8_t NumUART, uint32_t NumParam,
	int32_t *rows, int32_t *column, int32_t *curStep, int32_t *stepsCount)
{
	uint8_t ErrorNum = _NoError;

	(void)NumUART;
	if ((rows == 0) || (column == 0) || (curStep == 0) || (stepsCount == 0))
		return _ErrorSize;

	*rows = *column = *curStep = *stepsCount = 0;
	switch (NumParam)
	{
		default:
			ErrorNum = _ErrorUnCorrParam;
			break;
	}
	return ErrorNum;
}

uint8_t ReleaseTabParam(uint8_t NumUART, uint32_t NumParam)
{
	uint8_t ErrorNum = _NoError;

	(void)NumUART;
	switch (NumParam)
	{
		default:
			ErrorNum = _ErrorUnCorrParam;
			break;
	}
	return ErrorNum;
}

uint8_t ReadDescrTabParam(uint8_t NumUART, uint32_t NumParam,
	uint8_t *B, uint32_t *pSize)
{
	(void)NumUART;
	(void)NumParam;
	(void)B;
	if (pSize == 0)
		return _ErrorSize;

	*pSize = 0U;
	return _ErrorUnCorrParam;
}
