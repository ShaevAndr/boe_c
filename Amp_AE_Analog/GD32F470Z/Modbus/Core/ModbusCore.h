/* Platform-independent Modbus RTU frame and CRC processing. */
#ifndef MODBUS_CORE_H
#define MODBUS_CORE_H

#include <stdint.h>
#include "../Backend/ModbusBackend.h"

uint16_t ModbusCore_Crc16(const uint8_t *data, uint32_t length);
void ModbusCore_ProcessRtuFrame(const ModbusBackend *backend, uint8_t *frame,
	uint32_t *frameSize, uint8_t deviceAddress);

#endif
