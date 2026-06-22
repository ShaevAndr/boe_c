/*=============================================================================
     Project:
     Platform: GD32F470
     Filename: ReadDeviceIdentification.c
     Description: FC 0x08 - Read Device Identification
     Version: 1.0
     Created: 2026.06.22
============================================================================*/
#include <stdint.h>
#include <string.h>

#include "ReadDeviceIdentification.h"
#include "CommandParcerModbus.h"
#include "ErrorHandler.h"
#include "../Unicorn2/CommandParser.h"

/*
 * Response PDU:
 *   FC(1) | ByteCount(1) | DeviceType(2) | DeviceVersion(2) |
 *   ShortDescription(N)
 *
 * DeviceType and DeviceVersion are big-endian. ShortDescription does not
 * include the terminating zero.
 *
 * Maximum RTU frame size is 256 bytes:
 *   Address(1) + PDU(253) + CRC(2)
 */
#define MAX_RESPONSE_PDU_SIZE          253U
#define FIXED_IDENTIFICATION_DATA_SIZE   4U
#define RESPONSE_HEADER_SIZE             2U
#define MAX_SHORT_DESCRIPTION_SIZE \
	(MAX_RESPONSE_PDU_SIZE - RESPONSE_HEADER_SIZE - FIXED_IDENTIFICATION_DATA_SIZE)
#define SHORT_DESCRIPTION_SCAN_LIMIT \
	(((UNIT_DESCRIPTION_BUFFER_SIZE - 1U) < MAX_SHORT_DESCRIPTION_SIZE) ? \
	 (UNIT_DESCRIPTION_BUFFER_SIZE - 1U) : MAX_SHORT_DESCRIPTION_SIZE)

static uint32_t BoundedStringLength(const char *text, uint32_t limit)
{
	uint32_t length = 0U;

	if (text == NULL)
		return limit + 1U;

	while ((length <= limit) && (text[length] != '\0'))
		length++;

	return length;
}

uint8_t ReadDeviceIdentification(
	uint8_t NumUART,
	uint8_t Command,
	uint8_t *B,
	uint32_t *pSize)
{
	uint32_t descriptionLength;
	uint32_t dataLength;

	(void)NumUART;

	descriptionLength =
		BoundedStringLength(UnitDescription, SHORT_DESCRIPTION_SCAN_LIMIT);
	if (descriptionLength > SHORT_DESCRIPTION_SCAN_LIMIT)
		return _IllegalDataValue;

	dataLength = FIXED_IDENTIFICATION_DATA_SIZE + descriptionLength;

	B[0] = Command;
	B[1] = (uint8_t)dataLength;
	B[2] = (uint8_t)(_TypeDev >> 8);
	B[3] = (uint8_t)(_TypeDev & 0xFFU);
	B[4] = (uint8_t)(_VerDev >> 8);
	B[5] = (uint8_t)(_VerDev & 0xFFU);

	if (descriptionLength > 0U)
		memcpy(&B[6], UnitDescription, descriptionLength);

	*pSize = RESPONSE_HEADER_SIZE + dataLength;
	return _NoError;
}
