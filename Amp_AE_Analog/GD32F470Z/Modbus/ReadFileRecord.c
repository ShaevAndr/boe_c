/*=============================================================================
     Project:
     Platform: GD32F470
     Filename: ReadFileRecord.c
     Description: FC 0x14 - Read File Record.
     Version: 2.0
     Created: 2026.06.05
============================================================================*/
#include <string.h>
#include "ReadFileRecord.h"
#include "../deviceInfo/deviceDescription.h"
#include "../Unicorn2/AccessTabParam.h"
#include "ErrorHandler.h"
#include "CommandParcerModbus.h"
//--------------------------------------------------------------------------//FC + ByteCount + RefType + FileNumber(2) + RecordNumber(2) + RecordLength(2)
// Request PDU:  FC(1) | ByteCount(1) | RefType(1) | FileNumber(2) | RecordNumber(2) | RecordLength(2) = 9 bytes
// Response PDU: FC(1) | ResponseDataLength(1) | FileResponseLength(1) | RefType(1) | RecordData(n*2bytes) = 4 + 2*Q bytes
//   Each parameter is serialized big-endian (MSB first).
//--------------------------------------------------------------------------//

#define CHUNK_SIZE                    245U
#define FILE_REF_TYPE                 6U
#define FILE_RECORD_SIZE              2U
#define READ_FILE_REQUEST_SIZE        9U
#define READ_FILE_REQUEST_BYTE_COUNT  7U
#define MODBUS_MAX_PDU_SIZE           253U
#define READ_FILE_RESPONSE_OVERHEAD   4U
#define MAX_TABLE_READ_SIZE \
	(MODBUS_MAX_PDU_SIZE - READ_FILE_RESPONSE_OVERHEAD)

static const char *GetFileData(uint16_t fileNumber, uint32_t *size)
{
    switch(fileNumber)
    {
		case _DeviceDescription:
			*size = descriptionLength;
			return DeviceDescription;

		default:
			return NULL;
    }
}

uint8_t ReadFileRecord(uint8_t NumUART, uint8_t Command, uint8_t *B, uint32_t *pSize)
{
	uint32_t reqSize = *pSize;
	uint8_t byteCount = B[1];
	uint8_t refType = B[2];
	uint16_t fileNumber = (uint16_t)(B[3] << 8) | B[4];
	uint16_t recordNumber  = (uint16_t)(B[5] << 8) | B[6];
	uint16_t recordLength = (uint16_t)(B[7] << 8) | B[8];

	uint32_t fileLength = 0;
	const char *fileData = GetFileData(fileNumber, &fileLength);
	uint32_t tableSize = 0;
	uint8_t tableError;

	if ((reqSize != READ_FILE_REQUEST_SIZE) ||
		(byteCount != READ_FILE_REQUEST_BYTE_COUNT) ||
		(refType != FILE_REF_TYPE))
		return _IllegalDataValue;

	if(fileData == NULL) {
	    tableSize = MAX_TABLE_READ_SIZE;
	    tableError = ReadTabParam(NumUART,
	        (uint32_t)fileNumber - MODBUS_TABLE_FILE_BASE,
	        &B[4], (uint32_t)recordNumber * FILE_RECORD_SIZE,
	        FILE_RECORD_SIZE, FILE_RECORD_SIZE, &tableSize);
	    if (tableError != _NoError) {
	        return ConvertUnicornErrorIntoModbusError(tableError);
	    }

	    B[0] = Command;
	    B[2] = (uint8_t)(1U + tableSize);
	    B[3] = FILE_REF_TYPE;
	    B[1] = (uint8_t)(2U + tableSize);
	    *pSize = 4U + tableSize;
	    return _NoError;
	}

	if (recordLength == 0U)
	    return _IllegalDataValue;

	uint32_t offset = (uint32_t)recordNumber * CHUNK_SIZE;
	uint8_t chunkLength = 0;

	if(offset < fileLength)
    {
        uint32_t remain = fileLength - offset;

        chunkLength =
            (remain > CHUNK_SIZE)
                ? CHUNK_SIZE
                : (uint8_t)remain;
    }

    /*
        Response:

        FC(1)
        ResponseDataLength(1)
        FileResponseLength(1)
        RefType(1)
        RecordData(chunkLength bytes)
    */

    B[0] = Command;
    B[2] = (uint8_t)(1 + chunkLength);
    B[3] = FILE_REF_TYPE;

    if(chunkLength > 0)
    {
        memcpy(
            &B[4],
            &fileData[offset],
            chunkLength);
    }

    B[1] = (uint8_t)(2 + chunkLength);

    *pSize = 4 + chunkLength;

    return _NoError;
}
