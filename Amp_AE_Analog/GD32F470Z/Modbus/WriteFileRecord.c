/*=============================================================================
     Project:
     Platform: GD32F470
     Filename: WriteFileRecord.c
     Description: FC 0x15 - Write File Record.
============================================================================*/
#include "WriteFileRecord.h"
#include "ErrorHandler.h"
#include "CommandParcerModbus.h"
#include "TabParamFiles.h"

#define FILE_REF_TYPE 6U

uint8_t WriteFileRecord(uint8_t NumUART, uint8_t Command, uint8_t *B, uint32_t *pSize)
{
	uint32_t reqSize = *pSize;
	uint8_t byteCount = B[1];
	uint8_t refType = B[2];
	uint16_t fileNumber = (uint16_t)(((uint16_t)B[3] << 8) | B[4]);
	uint16_t recordNumber = (uint16_t)(((uint16_t)B[5] << 8) | B[6]);
	uint16_t recordLength = (uint16_t)(((uint16_t)B[7] << 8) | B[8]);
	uint32_t dataSize = (uint32_t)recordLength * 2U;
	uint8_t err;

	(void)NumUART;
	if ((byteCount != (uint8_t)(7U + dataSize)) || (reqSize != (uint32_t)(2U + byteCount)))
		return _IllegalDataValue;
	if ((refType != FILE_REF_TYPE) || (recordLength == 0U))
		return _IllegalDataValue;

	err = ModbusTabParam_WriteFile(fileNumber, (uint32_t)recordNumber * 2U,
		&B[9], dataSize);
	if (err != _NoError)
		return ConvertUnicornErrorIntoModbusError(err);

	B[0] = Command;
	*pSize = reqSize;
	return _NoError;
}
