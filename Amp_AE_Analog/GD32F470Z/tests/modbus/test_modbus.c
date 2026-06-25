#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../../Modbus/CommandParcerModbus.h"
#include "../../Modbus/Diagnostics.h"
#include "../../Modbus/ErrorHandler.h"
#include "../../Modbus/ModbusRtuFrame.h"
#include "../../Modbus/ModbusUtils.h"
#include "../../Modbus/ReadDeviceIdentification.h"
#include "../../Modbus/ReadFileRecord.h"
#include "../../Modbus/TabParamFiles.h"
#include "../../Modbus/WriteFileRecord.h"
#include "../../Modbus/ReadHoldingRegisters.h"
#include "../../Modbus/ReadInputRegisters.h"
#include "../../Modbus/WriteMultipleRegisters.h"
#include "../../Modbus/WriteSingleRegister.h"
#include "../../Unicorn2/AccessFloatParam.h"
#include "../../Unicorn2/AccessIntParam.h"
#include "../../Unicorn2/AccessTelemParam.h"
#include "../../Unicorn2/crc16.h"
#include "../../deviceInfo/deviceDescription.h"

#define ARRAY_SIZE(array) (sizeof(array) / sizeof((array)[0]))

static unsigned testsRun;
static unsigned testsFailed;

#define CHECK(condition) \
	do \
	{ \
		if (!(condition)) \
		{ \
			printf("  FAIL %s:%d: %s\n", __FILE__, __LINE__, #condition); \
			return 0; \
		} \
	} while (0)

#define CHECK_EQ_U32(expected, actual) \
	CHECK((uint32_t)(expected) == (uint32_t)(actual))

#define RUN_TEST(testFunction) \
	do \
	{ \
		testsRun++; \
		printf("[TEST] %s\n", #testFunction); \
		if (!(testFunction())) \
		{ \
			testsFailed++; \
		} \
	} while (0)

static int32_t intValues[_IPCount];
static float floatValues[_FPCount];
static float telemetryValues[_TelPCount];
static int8_t intAccessError;
static int8_t floatAccessError;
static int8_t telemetryAccessError;
static unsigned intReadCount;
static unsigned intWriteCount;
static unsigned floatReadCount;
static unsigned floatWriteCount;
static unsigned telemetryReadCount;

const char *UnitDescription = "BUE-8 test device";

static void ResetBackend(void)
{
	uint16_t index;

	memset(intValues, 0, sizeof(intValues));
	memset(floatValues, 0, sizeof(floatValues));
	memset(telemetryValues, 0, sizeof(telemetryValues));
	intAccessError = (int8_t)_NoError;
	floatAccessError = (int8_t)_NoError;
	telemetryAccessError = (int8_t)_NoError;
	intReadCount = 0U;
	intWriteCount = 0U;
	floatReadCount = 0U;
	floatWriteCount = 0U;
	telemetryReadCount = 0U;
	UnitDescription = "BUE-8 test device";

	for (index = 0U; index < (uint16_t)_IPCount; index++)
		intValues[index] = (int32_t)(0x10000000UL + index);

	for (index = 0U; index < (uint16_t)_FPCount; index++)
		floatValues[index] = (float)index + 0.5F;

	for (index = 0U; index < (uint16_t)_TelPCount; index++)
		telemetryValues[index] = (float)index + 10.25F;
}

int8_t AccessIntParam(IntParam_t parameter, int32_t *value, ParamAccessMode_t mode)
{
	if (intAccessError != (int8_t)_NoError)
		return intAccessError;

	if ((parameter < 0) || (parameter >= _IPCount))
		return (int8_t)_ErrorNoSuchParam;

	if (mode == _PAM_RO)
	{
		*value = intValues[parameter];
		intReadCount++;
	}
	else
	{
		intValues[parameter] = *value;
		intWriteCount++;
	}

	return (int8_t)_NoError;
}

int8_t AccessFloatParam(FloatParam_t parameter, float *value, ParamAccessMode_t mode)
{
	if (floatAccessError != (int8_t)_NoError)
		return floatAccessError;

	if ((parameter < 0) || (parameter >= _FPCount))
		return (int8_t)_ErrorNoSuchParam;

	if (mode == _PAM_RO)
	{
		*value = floatValues[parameter];
		floatReadCount++;
	}
	else
	{
		floatValues[parameter] = *value;
		floatWriteCount++;
	}

	return (int8_t)_NoError;
}

int8_t AccessTelemParam(TelimParam_t parameter, float *value)
{
	if (telemetryAccessError != (int8_t)_NoError)
		return telemetryAccessError;

	if ((parameter < 0) || (parameter >= _TelPCount))
		return (int8_t)_ErrorNoSuchParam;

	*value = telemetryValues[parameter];
	telemetryReadCount++;
	return (int8_t)_NoError;
}

static uint32_t FloatBits(float value)
{
	uint32_t bits;
	memcpy(&bits, &value, sizeof(bits));
	return bits;
}

static void PutU32Be(uint8_t *destination, uint32_t value)
{
	destination[0] = (uint8_t)(value >> 24);
	destination[1] = (uint8_t)(value >> 16);
	destination[2] = (uint8_t)(value >> 8);
	destination[3] = (uint8_t)value;
}

static uint32_t GetU32Be(const uint8_t *source)
{
	return ((uint32_t)source[0] << 24) |
	       ((uint32_t)source[1] << 16) |
	       ((uint32_t)source[2] << 8) |
	       (uint32_t)source[3];
}

static uint32_t BuildRtuRequest(
	uint8_t *frame,
	uint8_t address,
	const uint8_t *pdu,
	uint32_t pduSize)
{
	uint16_t crc;

	frame[0] = address;
	memcpy(&frame[1], pdu, pduSize);
	crc = CRC16(frame, 1U + pduSize);
	frame[1U + pduSize] = (uint8_t)crc;
	frame[2U + pduSize] = (uint8_t)(crc >> 8);
	return pduSize + 3U;
}

static int CheckRtuCrc(const uint8_t *frame, uint32_t size)
{
	uint16_t expected;
	uint16_t actual;

	if (size < 4U)
		return 0;

	expected = CRC16(frame, size - 2U);
	actual = (uint16_t)frame[size - 2U] |
	         ((uint16_t)frame[size - 1U] << 8);
	return expected == actual;
}

static const uint8_t *FindDeviceIdObject(
	const uint8_t *pdu,
	uint32_t pduSize,
	uint8_t objectId,
	uint8_t *objectLength)
{
	uint32_t offset = 7U;
	uint8_t index;

	if (pduSize < 7U)
		return NULL;

	for (index = 0U; index < pdu[6]; index++)
	{
		uint8_t currentId;
		uint8_t currentLength;

		if ((offset + 2U) > pduSize)
			return NULL;

		currentId = pdu[offset++];
		currentLength = pdu[offset++];
		if ((offset + currentLength) > pduSize)
			return NULL;

		if (currentId == objectId)
		{
			*objectLength = currentLength;
			return &pdu[offset];
		}

		offset += currentLength;
	}

	return NULL;
}

static int TestModbusUtilsMappings(void)
{
	uint16_t index = 0xFFFFU;

	CHECK_EQ_U32(PARAM_INT, GetHoldingRegisterMapping(0U, &index));
	CHECK_EQ_U32(0U, index);
	CHECK_EQ_U32(PARAM_INT, GetHoldingRegisterMapping((uint16_t)_IPCount - 1U, &index));
	CHECK_EQ_U32((uint16_t)_IPCount - 1U, index);
	CHECK_EQ_U32(PARAM_FLOAT, GetHoldingRegisterMapping((uint16_t)_IPCount, &index));
	CHECK_EQ_U32(0U, index);
	CHECK_EQ_U32(PARAM_FLOAT, GetHoldingRegisterMapping(
		(uint16_t)(_IPCount + _FPCount - 1U), &index));
	CHECK_EQ_U32((uint16_t)_FPCount - 1U, index);
	CHECK_EQ_U32(PARAM_NONE, GetHoldingRegisterMapping(
		(uint16_t)(_IPCount + _FPCount), &index));
	CHECK_EQ_U32(PARAM_TELEMETRY, GetInputRegisterMapping(0U, &index));
	CHECK_EQ_U32(PARAM_NONE, GetInputRegisterMapping((uint16_t)_TelPCount, &index));
	CHECK_EQ_U32((uint16_t)(_IPCount + _FPCount), GetHoldingRegisterCount());
	CHECK_EQ_U32((uint16_t)_TelPCount, GetInputRegisterCount());
	return 1;
}

static int TestErrorConversion(void)
{
	CHECK_EQ_U32(_NoError, ConvertUnicornErrorIntoModbusError(_NoError));
	CHECK_EQ_U32(_IllegalFunction, ConvertUnicornErrorIntoModbusError(_ErrorUnKnowComand));
	CHECK_EQ_U32(_IllegalDataAddress, ConvertUnicornErrorIntoModbusError(_ErrorNoSuchParam));
	CHECK_EQ_U32(_IllegalDataValue, ConvertUnicornErrorIntoModbusError(_ErrorWriteROParam));
	CHECK_EQ_U32(_SlaveDeviceBusy, ConvertUnicornErrorIntoModbusError(_ErrorTimeout));
	CHECK_EQ_U32(_SlaveDeviceFailure, ConvertUnicornErrorIntoModbusError(0x55U));
	return 1;
}

static int TestReadHoldingIntAndFloat(void)
{
	uint8_t pdu[16] = {0x03U};
	uint32_t size = 5U;
	uint16_t start = (uint16_t)_IPCount - 1U;

	ResetBackend();
	intValues[_IPCount - 1] = (int32_t)0x89ABCDEFUL;
	floatValues[0] = 1.5F;
	pdu[1] = (uint8_t)(start >> 8);
	pdu[2] = (uint8_t)start;
	pdu[3] = 0U;
	pdu[4] = 2U;

	CHECK_EQ_U32(_NoError, ReadHoldingsRegisters(0U, 0x03U, pdu, &size));
	CHECK_EQ_U32(10U, size);
	CHECK_EQ_U32(8U, pdu[1]);
	CHECK_EQ_U32(0x89ABCDEFUL, GetU32Be(&pdu[2]));
	CHECK_EQ_U32(FloatBits(1.5F), GetU32Be(&pdu[6]));
	CHECK_EQ_U32(1U, intReadCount);
	CHECK_EQ_U32(1U, floatReadCount);
	return 1;
}

static int TestReadHoldingValidationAndBackendError(void)
{
	uint8_t pdu[8] = {0x03U, 0U, 0U, 0U, 0U};
	uint32_t size = 5U;

	ResetBackend();
	CHECK_EQ_U32(_IllegalDataValue, ReadHoldingsRegisters(0U, 0x03U, pdu, &size));

	pdu[1] = (uint8_t)(GetHoldingRegisterCount() >> 8);
	pdu[2] = (uint8_t)GetHoldingRegisterCount();
	pdu[4] = 1U;
	CHECK_EQ_U32(_IllegalDataAddress, ReadHoldingsRegisters(0U, 0x03U, pdu, &size));

	pdu[1] = 0U;
	pdu[2] = 0U;
	intAccessError = (int8_t)_ErrorDataNotReady;
	CHECK_EQ_U32(_SlaveDeviceBusy, ReadHoldingsRegisters(0U, 0x03U, pdu, &size));
	return 1;
}

static int TestReadInputRegisters(void)
{
	uint8_t pdu[16] = {0x04U, 0U, 1U, 0U, 2U};
	uint32_t size = 5U;

	ResetBackend();
	telemetryValues[1] = -2.5F;
	telemetryValues[2] = 3.25F;
	CHECK_EQ_U32(_NoError, ReadInputRegisters(0U, 0x04U, pdu, &size));
	CHECK_EQ_U32(10U, size);
	CHECK_EQ_U32(8U, pdu[1]);
	CHECK_EQ_U32(FloatBits(-2.5F), GetU32Be(&pdu[2]));
	CHECK_EQ_U32(FloatBits(3.25F), GetU32Be(&pdu[6]));
	CHECK_EQ_U32(2U, telemetryReadCount);

	pdu[1] = (uint8_t)(GetInputRegisterCount() >> 8);
	pdu[2] = (uint8_t)GetInputRegisterCount();
	pdu[3] = 0U;
	pdu[4] = 1U;
	size = 5U;
	CHECK_EQ_U32(_IllegalDataAddress, ReadInputRegisters(0U, 0x04U, pdu, &size));

	pdu[1] = 0U;
	pdu[2] = 0U;
	telemetryAccessError = (int8_t)_ErrorInternalError;
	CHECK_EQ_U32(_SlaveDeviceFailure, ReadInputRegisters(0U, 0x04U, pdu, &size));
	return 1;
}

static int TestWriteSingleIntAndFloat(void)
{
	uint8_t pdu[8] = {0x06U};
	uint32_t size = 7U;
	uint16_t floatAddress = (uint16_t)_IPCount;

	ResetBackend();
	pdu[1] = 0U;
	pdu[2] = 2U;
	PutU32Be(&pdu[3], 0xFEDCBA98UL);
	CHECK_EQ_U32(_NoError, WriteSingleRegister(0U, 0x06U, pdu, &size));
	CHECK_EQ_U32(7U, size);
	CHECK_EQ_U32(0xFEDCBA98UL, (uint32_t)intValues[2]);
	CHECK_EQ_U32(1U, intWriteCount);

	pdu[1] = (uint8_t)(floatAddress >> 8);
	pdu[2] = (uint8_t)floatAddress;
	PutU32Be(&pdu[3], FloatBits(-7.75F));
	size = 7U;
	CHECK_EQ_U32(_NoError, WriteSingleRegister(0U, 0x06U, pdu, &size));
	CHECK_EQ_U32(FloatBits(-7.75F), FloatBits(floatValues[0]));
	CHECK_EQ_U32(1U, floatWriteCount);
	return 1;
}

static int TestWriteSingleValidationAndBackendError(void)
{
	uint8_t pdu[8] = {0x06U};
	uint32_t size = 7U;
	uint16_t invalidAddress = GetHoldingRegisterCount();

	ResetBackend();
	pdu[1] = (uint8_t)(invalidAddress >> 8);
	pdu[2] = (uint8_t)invalidAddress;
	CHECK_EQ_U32(_IllegalDataAddress, WriteSingleRegister(0U, 0x06U, pdu, &size));

	pdu[1] = 0U;
	pdu[2] = 0U;
	intAccessError = (int8_t)_ErrorWriteROParam;
	CHECK_EQ_U32(_IllegalDataValue, WriteSingleRegister(0U, 0x06U, pdu, &size));
	return 1;
}

static int TestWriteMultipleAcrossTypeBoundary(void)
{
	uint8_t pdu[20] = {0x10U};
	uint32_t size = 14U;
	uint16_t start = (uint16_t)_IPCount - 1U;

	ResetBackend();
	pdu[1] = (uint8_t)(start >> 8);
	pdu[2] = (uint8_t)start;
	pdu[3] = 0U;
	pdu[4] = 2U;
	pdu[5] = 8U;
	PutU32Be(&pdu[6], 0x12345678UL);
	PutU32Be(&pdu[10], FloatBits(9.5F));

	CHECK_EQ_U32(_NoError, WriteMultipleRegisters(0U, 0x10U, pdu, &size));
	CHECK_EQ_U32(5U, size);
	CHECK_EQ_U32(0x12345678UL, (uint32_t)intValues[_IPCount - 1]);
	CHECK_EQ_U32(FloatBits(9.5F), FloatBits(floatValues[0]));
	CHECK_EQ_U32(1U, intWriteCount);
	CHECK_EQ_U32(1U, floatWriteCount);
	return 1;
}

static int TestWriteMultipleValidation(void)
{
	uint8_t pdu[16] = {0x10U, 0U, 0U, 0U, 2U, 7U};
	uint32_t size = 13U;

	ResetBackend();
	CHECK_EQ_U32(_IllegalDataValue, WriteMultipleRegisters(0U, 0x10U, pdu, &size));

	pdu[5] = 8U;
	size = 10U;
	CHECK_EQ_U32(_IllegalDataValue, WriteMultipleRegisters(0U, 0x10U, pdu, &size));

	pdu[1] = (uint8_t)(GetHoldingRegisterCount() >> 8);
	pdu[2] = (uint8_t)GetHoldingRegisterCount();
	pdu[3] = 0U;
	pdu[4] = 1U;
	pdu[5] = 4U;
	size = 10U;
	CHECK_EQ_U32(_IllegalDataAddress, WriteMultipleRegisters(0U, 0x10U, pdu, &size));
	return 1;
}

static int TestReadFileRecord(void)
{
	uint8_t pdu[256] = {0x14U, 7U, 6U, 0U, 1U, 0U, 0U, 0U, 1U};
	uint32_t size = 9U;
	uint32_t expectedLength = (uint32_t)strlen(DeviceDescription);

	CHECK_EQ_U32(_NoError, ReadFileRecord(0U, 0x14U, pdu, &size));
	CHECK_EQ_U32(4U + expectedLength, size);
	CHECK_EQ_U32(2U + expectedLength, pdu[1]);
	CHECK_EQ_U32(1U + expectedLength, pdu[2]);
	CHECK_EQ_U32(6U, pdu[3]);
	CHECK(memcmp(&pdu[4], DeviceDescription, expectedLength) == 0);

	pdu[3] = 0U;
	pdu[4] = 2U;
	size = 9U;
	CHECK_EQ_U32(_IllegalDataAddress, ReadFileRecord(0U, 0x14U, pdu, &size));

	pdu[3] = 0U;
	pdu[4] = 1U;
	pdu[5] = 0U;
	pdu[6] = 1U;
	size = 9U;
	CHECK_EQ_U32(_NoError, ReadFileRecord(0U, 0x14U, pdu, &size));
	CHECK_EQ_U32(4U, size);
	CHECK_EQ_U32(2U, pdu[1]);
	return 1;
}

static int TestTableFileRecords(void)
{
	uint8_t pdu[32] = {0x14U, 7U, 6U, 0U, MODBUS_TABLE_TEST_FILE, 0U, 0U, 0U, 4U};
	uint32_t size = 9U;

	CHECK_EQ_U32(_NoError, ReadFileRecord(0U, 0x14U, pdu, &size));
	CHECK_EQ_U32(12U, size);
	CHECK_EQ_U32(10U, pdu[1]);
	CHECK_EQ_U32(9U, pdu[2]);
	CHECK_EQ_U32(6U, pdu[3]);
	CHECK_EQ_U32(0x10U, pdu[4]);
	CHECK_EQ_U32(0x11U, pdu[5]);
	CHECK_EQ_U32(0x12U, pdu[6]);
	CHECK_EQ_U32(0x13U, pdu[7]);

	pdu[0] = 0x15U;
	pdu[1] = 11U;
	pdu[2] = 6U;
	pdu[3] = 0U;
	pdu[4] = MODBUS_TABLE_TEST_FILE;
	pdu[5] = 0U;
	pdu[6] = 2U;
	pdu[7] = 0U;
	pdu[8] = 2U;
	pdu[9] = 0xAAU;
	pdu[10] = 0xBBU;
	pdu[11] = 0xCCU;
	pdu[12] = 0xDDU;
	size = 13U;
	CHECK_EQ_U32(_NoError, ModbusCommandProcess(0U, pdu, &size));
	CHECK_EQ_U32(13U, size);

	pdu[0] = 0x14U;
	pdu[1] = 7U;
	pdu[2] = 6U;
	pdu[3] = 0U;
	pdu[4] = MODBUS_TABLE_TEST_FILE;
	pdu[5] = 0U;
	pdu[6] = 2U;
	pdu[7] = 0U;
	pdu[8] = 2U;
	size = 9U;
	CHECK_EQ_U32(_NoError, ModbusCommandProcess(0U, pdu, &size));
	CHECK_EQ_U32(8U, size);
	CHECK_EQ_U32(0xAAU, pdu[4]);
	CHECK_EQ_U32(0xBBU, pdu[5]);
	CHECK_EQ_U32(0xCCU, pdu[6]);
	CHECK_EQ_U32(0xDDU, pdu[7]);

	pdu[0] = 0x15U;
	pdu[1] = 9U;
	pdu[2] = 6U;
	pdu[3] = 0U;
	pdu[4] = 0xFFU;
	pdu[5] = 0U;
	pdu[6] = 0U;
	pdu[7] = 0U;
	pdu[8] = 1U;
	pdu[9] = 0U;
	pdu[10] = 0U;
	size = 11U;
	CHECK_EQ_U32(_IllegalDataAddress, WriteFileRecord(0U, 0x15U, pdu, &size));
	return 1;
}

static int TestDiagnosticsTableFunctions(void)
{
	uint8_t pdu[128] = {0x08U, 0U, MODBUS_DIAG_TABLE_COUNT};
	uint32_t size = 3U;
	const char expectedDescription[] = "Test table parameter; file=100; bytes=16";

	CHECK_EQ_U32(_NoError, Diagnostics(0U, 0x08U, pdu, &size));
	CHECK_EQ_U32(7U, size);
	CHECK_EQ_U32(1U, GetU32Be(&pdu[3]));

	pdu[0] = 0x08U;
	pdu[1] = 0U;
	pdu[2] = MODBUS_DIAG_TABLE_DESCRIPTION;
	PutU32Be(&pdu[3], MODBUS_TABLE_TEST_INDEX);
	size = 7U;
	CHECK_EQ_U32(_NoError, ModbusCommandProcess(0U, pdu, &size));
	CHECK_EQ_U32(11U + strlen(expectedDescription), size);
	CHECK_EQ_U32(MODBUS_TABLE_TEST_INDEX, GetU32Be(&pdu[3]));
	CHECK_EQ_U32(strlen(expectedDescription), GetU32Be(&pdu[7]));
	CHECK(memcmp(&pdu[11], expectedDescription, strlen(expectedDescription)) == 0);

	pdu[0] = 0x08U;
	pdu[1] = 0U;
	pdu[2] = MODBUS_DIAG_TABLE_PREPARE;
	PutU32Be(&pdu[3], MODBUS_TABLE_TEST_INDEX);
	PutU32Be(&pdu[7], 3U);
	size = 11U;
	CHECK_EQ_U32(_NoError, Diagnostics(0U, 0x08U, pdu, &size));
	CHECK_EQ_U32(11U, size);
	CHECK_EQ_U32(3U, GetU32Be(&pdu[7]));

	pdu[0] = 0x08U;
	pdu[1] = 0U;
	pdu[2] = MODBUS_DIAG_TABLE_PREPARE_PROGRESS;
	PutU32Be(&pdu[3], MODBUS_TABLE_TEST_INDEX);
	size = 7U;
	CHECK_EQ_U32(_NoError, Diagnostics(0U, 0x08U, pdu, &size));
	CHECK_EQ_U32(23U, size);
	CHECK_EQ_U32(1U, GetU32Be(&pdu[7]));
	CHECK_EQ_U32(1U, GetU32Be(&pdu[11]));
	CHECK_EQ_U32(4U, GetU32Be(&pdu[15]));
	CHECK_EQ_U32(4U, GetU32Be(&pdu[19]));

	pdu[0] = 0x08U;
	pdu[1] = 0U;
	pdu[2] = MODBUS_DIAG_TABLE_RELEASE;
	PutU32Be(&pdu[3], MODBUS_TABLE_TEST_INDEX);
	size = 7U;
	CHECK_EQ_U32(_NoError, Diagnostics(0U, 0x08U, pdu, &size));
	CHECK_EQ_U32(7U, size);

	PutU32Be(&pdu[3], 0x12345678UL);
	size = 7U;
	CHECK_EQ_U32(_IllegalDataAddress, Diagnostics(0U, 0x08U, pdu, &size));
	return 1;
}

static int TestReadDeviceIdentificationRegular(void)
{
	uint8_t pdu[256] = {0x2BU, 0x0EU, 0x02U, 0x00U};
	uint32_t size = 4U;
	uint8_t length;
	const uint8_t *value;

	ResetBackend();
	CHECK_EQ_U32(_NoError, ReadDeviceIdentification(0U, 0x2BU, pdu, &size));
	CHECK_EQ_U32(0x2BU, pdu[0]);
	CHECK_EQ_U32(0x0EU, pdu[1]);
	CHECK_EQ_U32(0x02U, pdu[2]);
	CHECK_EQ_U32(0x82U, pdu[3]);
	CHECK_EQ_U32(0U, pdu[4]);
	CHECK_EQ_U32(0U, pdu[5]);
	CHECK_EQ_U32(4U, pdu[6]);

	value = FindDeviceIdObject(pdu, size, 0x00U, &length);
	CHECK(value != NULL && length == 7U && memcmp(value, "Unicorn", 7U) == 0);
	value = FindDeviceIdObject(pdu, size, 0x01U, &length);
	CHECK(value != NULL && length == 6U && memcmp(value, "0x0320", 6U) == 0);
	value = FindDeviceIdObject(pdu, size, 0x02U, &length);
	CHECK(value != NULL && length == 3U && memcmp(value, "1.0", 3U) == 0);
	value = FindDeviceIdObject(pdu, size, 0x04U, &length);
	CHECK(value != NULL && length == strlen(UnitDescription));
	CHECK(memcmp(value, UnitDescription, length) == 0);
	return 1;
}

static int TestReadDeviceIdentificationModesAndErrors(void)
{
	uint8_t pdu[256] = {0x2BU, 0x0EU, 0x01U, 0x00U};
	uint32_t size = 4U;
	uint8_t length;
	static char unterminated[UNIT_DESCRIPTION_BUFFER_SIZE];

	ResetBackend();
	CHECK_EQ_U32(_NoError, ReadDeviceIdentification(0U, 0x2BU, pdu, &size));
	CHECK_EQ_U32(3U, pdu[6]);
	CHECK(FindDeviceIdObject(pdu, size, 0x04U, &length) == NULL);

	pdu[1] = 0x0EU;
	pdu[2] = 0x04U;
	pdu[3] = 0x04U;
	size = 4U;
	CHECK_EQ_U32(_NoError, ReadDeviceIdentification(0U, 0x2BU, pdu, &size));
	CHECK_EQ_U32(1U, pdu[6]);
	CHECK(FindDeviceIdObject(pdu, size, 0x04U, &length) != NULL);

	pdu[1] = 0x0EU;
	pdu[2] = 0x04U;
	pdu[3] = 0x03U;
	size = 4U;
	CHECK_EQ_U32(_IllegalDataAddress,
		ReadDeviceIdentification(0U, 0x2BU, pdu, &size));

	pdu[1] = 0x0DU;
	pdu[2] = 0x02U;
	pdu[3] = 0U;
	CHECK_EQ_U32(_IllegalDataValue,
		ReadDeviceIdentification(0U, 0x2BU, pdu, &size));

	memset(unterminated, 'X', sizeof(unterminated));
	UnitDescription = unterminated;
	pdu[1] = 0x0EU;
	pdu[2] = 0x02U;
	pdu[3] = 0U;
	CHECK_EQ_U32(_IllegalDataValue,
		ReadDeviceIdentification(0U, 0x2BU, pdu, &size));
	return 1;
}

static int TestCommandDispatcher(void)
{
	uint8_t pdu[16] = {0x03U, 0U, 0U, 0U};
	uint32_t size = 4U;

	ResetBackend();
	CHECK_EQ_U32(_IllegalDataValue, ModbusCommandProcess(0U, pdu, &size));

	pdu[0] = 0x2BU;
	pdu[1] = 0x0EU;
	pdu[2] = 0x02U;
	size = 3U;
	CHECK_EQ_U32(_IllegalDataValue, ModbusCommandProcess(0U, pdu, &size));

	pdu[0] = 0x7FU;
	size = 1U;
	CHECK_EQ_U32(_IllegalFunction, ModbusCommandProcess(0U, pdu, &size));
	return 1;
}

static int TestRtuNormalResponse(void)
{
	uint8_t frame[256] = {0};
	const uint8_t pdu[] = {0x03U, 0U, 0U, 0U, 1U};
	uint32_t size;

	ResetBackend();
	intValues[0] = (int32_t)0x11223344UL;
	size = BuildRtuRequest(frame, 1U, pdu, sizeof(pdu));
	ModbusRtuFrame_Process(0U, frame, &size, 1U);

	CHECK_EQ_U32(9U, size);
	CHECK_EQ_U32(1U, frame[0]);
	CHECK_EQ_U32(0x03U, frame[1]);
	CHECK_EQ_U32(4U, frame[2]);
	CHECK_EQ_U32(0x11223344UL, GetU32Be(&frame[3]));
	CHECK(CheckRtuCrc(frame, size));
	return 1;
}

static int TestRtuDropsInvalidFrames(void)
{
	uint8_t frame[32] = {0};
	const uint8_t pdu[] = {0x03U, 0U, 0U, 0U, 1U};
	uint32_t size;

	ResetBackend();
	size = BuildRtuRequest(frame, 1U, pdu, sizeof(pdu));
	frame[size - 1U] ^= 0xFFU;
	ModbusRtuFrame_Process(0U, frame, &size, 1U);
	CHECK_EQ_U32(0U, size);

	size = BuildRtuRequest(frame, 2U, pdu, sizeof(pdu));
	ModbusRtuFrame_Process(0U, frame, &size, 1U);
	CHECK_EQ_U32(0U, size);
	return 1;
}

static int TestRtuExceptionAndBroadcast(void)
{
	uint8_t frame[32] = {0};
	const uint8_t invalidPdu[] = {0x7FU};
	uint8_t writePdu[] = {0x06U, 0U, 0U, 0U, 0U, 0U, 5U};
	uint32_t size;

	ResetBackend();
	size = BuildRtuRequest(frame, 1U, invalidPdu, sizeof(invalidPdu));
	ModbusRtuFrame_Process(0U, frame, &size, 1U);
	CHECK_EQ_U32(5U, size);
	CHECK_EQ_U32(0xFFU, frame[1]);
	CHECK_EQ_U32(_IllegalFunction, frame[2]);
	CHECK(CheckRtuCrc(frame, size));

	size = BuildRtuRequest(frame, 0U, writePdu, sizeof(writePdu));
	ModbusRtuFrame_Process(0U, frame, &size, 1U);
	CHECK_EQ_U32(0U, size);
	CHECK_EQ_U32(5U, intValues[0]);
	CHECK_EQ_U32(1U, intWriteCount);
	return 1;
}

int main(void)
{
	RUN_TEST(TestModbusUtilsMappings);
	RUN_TEST(TestErrorConversion);
	RUN_TEST(TestReadHoldingIntAndFloat);
	RUN_TEST(TestReadHoldingValidationAndBackendError);
	RUN_TEST(TestReadInputRegisters);
	RUN_TEST(TestWriteSingleIntAndFloat);
	RUN_TEST(TestWriteSingleValidationAndBackendError);
	RUN_TEST(TestWriteMultipleAcrossTypeBoundary);
	RUN_TEST(TestWriteMultipleValidation);
	RUN_TEST(TestReadFileRecord);
	RUN_TEST(TestTableFileRecords);
	RUN_TEST(TestDiagnosticsTableFunctions);
	RUN_TEST(TestReadDeviceIdentificationRegular);
	RUN_TEST(TestReadDeviceIdentificationModesAndErrors);
	RUN_TEST(TestCommandDispatcher);
	RUN_TEST(TestRtuNormalResponse);
	RUN_TEST(TestRtuDropsInvalidFrames);
	RUN_TEST(TestRtuExceptionAndBroadcast);

	printf("\n%u tests, %u failures\n", testsRun, testsFailed);
	return testsFailed == 0U ? 0 : 1;
}
