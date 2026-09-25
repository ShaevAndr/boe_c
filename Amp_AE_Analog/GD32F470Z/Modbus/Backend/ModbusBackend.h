/* Device-facing boundary for the portable Modbus core. */
#ifndef MODBUS_BACKEND_H
#define MODBUS_BACKEND_H

#include <stdint.h>

typedef uint8_t (*ModbusBackendProcessPdu)(void *context, uint8_t *pdu,
	uint32_t *pduSize);
typedef void (*ModbusBackendFrameActivity)(void *context);

typedef struct
{
	void *context;
	ModbusBackendProcessPdu processPdu;
	ModbusBackendFrameActivity frameActivity;
} ModbusBackend;

#endif
