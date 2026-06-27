/*=============================================================================
     Project:
     Platform: GD32F470
     Filename: WriteFileRecord.c
     Description: FC 0x15 - Write File Record.
============================================================================*/
#include "WriteFileRecord.h"
#include "../Unicorn2/AccessTabParam.h"
#include "ErrorHandler.h"
#include "CommandParcerModbus.h"

#define FILE_REF_TYPE                6U
#define FILE_RECORD_SIZE             2U
#define WRITE_FILE_REQUEST_OVERHEAD  9U
#define WRITE_FILE_REQUEST_DATA_MAX  0xFBU
#define MAX_TABLE_WRITE_RECORDS \
	((WRITE_FILE_REQUEST_DATA_MAX - 7U) / FILE_RECORD_SIZE)

uint8_t WriteFileRecord(uint8_t NumUART, uint8_t Command, uint8_t *B, uint32_t *pSize)
{
	uint32_t reqSize = *pSize;
	uint8_t byteCount = B[1];
	uint8_t refType = B[2];
	uint16_t fileNumber = (uint16_t)(((uint16_t)B[3] << 8) | B[4]);
	uint16_t recordNumber = (uint16_t)(((uint16_t)B[5] << 8) | B[6]);
	uint16_t recordLength = (uint16_t)(((uint16_t)B[7] << 8) | B[8]);
	uint32_t dataSize;
	uint8_t err;

	if ((refType != FILE_REF_TYPE) || (recordLength == 0U) ||
		(recordLength > MAX_TABLE_WRITE_RECORDS))
		return _IllegalDataValue;

	dataSize = (uint32_t)recordLength * FILE_RECORD_SIZE;
	if (((uint32_t)byteCount != (7U + dataSize)) ||
		(reqSize != (WRITE_FILE_REQUEST_OVERHEAD + dataSize)))
		return _IllegalDataValue;

	err = WriteTabParam(NumUART,
		(uint32_t)fileNumber - MODBUS_TABLE_FILE_BASE,
		&B[9], (uint32_t)recordNumber * FILE_RECORD_SIZE,
		FILE_RECORD_SIZE, dataSize);
	if (err != _NoError)
		return ConvertUnicornErrorIntoModbusError(err);

	B[0] = Command;
	*pSize = reqSize;
	return _NoError;
}
