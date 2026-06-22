/*=============================================================================
     Project:
     Platform: GD32F470
     Filename: ReadDeviceIdentification.c
     Description: FC 0x2B / MEI 0x0E - Read Device Identification
     Version: 2.0
     Created: 2026.06.22
============================================================================*/
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "ReadDeviceIdentification.h"
#include "CommandParcerModbus.h"
#include "ErrorHandler.h"
#include "../Unicorn2/CommandParser.h"
#include "../deviceInfo/deviceDescription.h"

/* MODBUS Application Protocol Specification V1.1b3, section 6.21. */
#define MEI_TYPE_READ_DEVICE_IDENTIFICATION  0x0EU

#define READ_DEVICE_ID_BASIC                 0x01U
#define READ_DEVICE_ID_REGULAR               0x02U
#define READ_DEVICE_ID_EXTENDED              0x03U
#define READ_DEVICE_ID_INDIVIDUAL            0x04U

#define DEVICE_ID_CONFORMITY_REGULAR         0x82U
#define DEVICE_ID_MORE_FOLLOWS_NONE          0x00U
#define DEVICE_ID_NEXT_OBJECT_NONE           0x00U

#define DEVICE_ID_VENDOR_NAME                0x00U
#define DEVICE_ID_PRODUCT_CODE               0x01U
#define DEVICE_ID_MAJOR_MINOR_REVISION       0x02U
#define DEVICE_ID_PRODUCT_NAME               0x04U

#define MODBUS_MAX_PDU_SIZE                  253U
#define RESPONSE_HEADER_SIZE                   7U

#define PRODUCT_CODE_BUFFER_SIZE               7U  /* "0xFFFF" + NUL */
#define REVISION_BUFFER_SIZE                   8U  /* "255.255" + NUL */

typedef struct
{
	uint8_t id;
	const char *value;
	uint8_t length;
} DeviceIdentificationObject;

static uint32_t BoundedStringLength(const char *text, uint32_t limit)
{
	uint32_t length = 0U;

	if (text == NULL)
		return limit + 1U;

	while ((length <= limit) && (text[length] != '\0'))
		length++;

	return length;
}

static uint8_t GetObjectStartIndex(
	const DeviceIdentificationObject *objects,
	uint8_t objectCount,
	uint8_t objectId)
{
	uint8_t index;

	for (index = 0U; index < objectCount; index++)
	{
		if (objects[index].id == objectId)
			return index;
	}

	/* Section 6.21: unknown Object Id in stream access restarts at object 0. */
	return 0U;
}

static uint8_t AppendObject(
	uint8_t *buffer,
	uint32_t *responseSize,
	const DeviceIdentificationObject *object)
{
	uint32_t objectSize = 2U + object->length;

	if ((*responseSize + objectSize) > MODBUS_MAX_PDU_SIZE)
		return _IllegalDataValue;

	buffer[(*responseSize)++] = object->id;
	buffer[(*responseSize)++] = object->length;

	if (object->length > 0U)
	{
		memcpy(&buffer[*responseSize], object->value, object->length);
		*responseSize += object->length;
	}

	return _NoError;
}

uint8_t ReadDeviceIdentification(
	uint8_t NumUART,
	uint8_t Command,
	uint8_t *B,
	uint32_t *pSize)
{
	char productCode[PRODUCT_CODE_BUFFER_SIZE];
	char revision[REVISION_BUFFER_SIZE];
	DeviceIdentificationObject objects[4];
	uint32_t productNameLength;
	uint32_t responseSize = RESPONSE_HEADER_SIZE;
	uint8_t readDeviceIdCode = B[2];
	uint8_t requestedObjectId = B[3];
	uint8_t availableObjectCount;
	uint8_t firstObjectIndex;
	uint8_t returnedObjectCount;
	uint8_t index;
	int formatResult;

	(void)NumUART;

	if (B[1] != MEI_TYPE_READ_DEVICE_IDENTIFICATION)
		return _IllegalDataValue;

	if ((readDeviceIdCode < READ_DEVICE_ID_BASIC) ||
		(readDeviceIdCode > READ_DEVICE_ID_INDIVIDUAL))
	{
		return _IllegalDataValue;
	}

	formatResult = snprintf(productCode, sizeof(productCode), "0x%04X", (unsigned)_TypeDev);
	if ((formatResult < 0) || ((uint32_t)formatResult >= sizeof(productCode)))
		return _SlaveDeviceFailure;

	formatResult = snprintf(
		revision,
		sizeof(revision),
		"%u.%u",
		(unsigned)((_VerDev >> 8) & 0xFFU),
		(unsigned)(_VerDev & 0xFFU));
	if ((formatResult < 0) || ((uint32_t)formatResult >= sizeof(revision)))
		return _SlaveDeviceFailure;

	productNameLength =
		BoundedStringLength(UnitDescription, UNIT_DESCRIPTION_BUFFER_SIZE - 1U);
	if ((productNameLength >= UNIT_DESCRIPTION_BUFFER_SIZE) ||
		(productNameLength > 0xFFU))
	{
		return _IllegalDataValue;
	}

	objects[0].id = DEVICE_ID_VENDOR_NAME;
	objects[0].value = DEVICE_VENDOR_NAME;
	objects[0].length = (uint8_t)(sizeof(DEVICE_VENDOR_NAME) - 1U);

	objects[1].id = DEVICE_ID_PRODUCT_CODE;
	objects[1].value = productCode;
	objects[1].length = (uint8_t)strlen(productCode);

	objects[2].id = DEVICE_ID_MAJOR_MINOR_REVISION;
	objects[2].value = revision;
	objects[2].length = (uint8_t)strlen(revision);

	objects[3].id = DEVICE_ID_PRODUCT_NAME;
	objects[3].value = UnitDescription;
	objects[3].length = (uint8_t)productNameLength;

	availableObjectCount =
		(readDeviceIdCode == READ_DEVICE_ID_BASIC) ? 3U : 4U;

	if (readDeviceIdCode == READ_DEVICE_ID_INDIVIDUAL)
	{
		availableObjectCount = 4U;
		firstObjectIndex = 0xFFU;

		for (index = 0U; index < availableObjectCount; index++)
		{
			if (objects[index].id == requestedObjectId)
			{
				firstObjectIndex = index;
				break;
			}
		}

		if (firstObjectIndex == 0xFFU)
			return _IllegalDataAddress;

		returnedObjectCount = 1U;
	}
	else
	{
		firstObjectIndex =
			GetObjectStartIndex(objects, availableObjectCount, requestedObjectId);
		returnedObjectCount = availableObjectCount - firstObjectIndex;
	}

	B[0] = Command;
	B[1] = MEI_TYPE_READ_DEVICE_IDENTIFICATION;
	B[2] = readDeviceIdCode;
	B[3] = DEVICE_ID_CONFORMITY_REGULAR;
	B[4] = DEVICE_ID_MORE_FOLLOWS_NONE;
	B[5] = DEVICE_ID_NEXT_OBJECT_NONE;
	B[6] = returnedObjectCount;

	for (index = 0U; index < returnedObjectCount; index++)
	{
		uint8_t error = AppendObject(
			B,
			&responseSize,
			&objects[firstObjectIndex + index]);

		if (error != _NoError)
			return error;
	}

	*pSize = responseSize;
	return _NoError;
}
