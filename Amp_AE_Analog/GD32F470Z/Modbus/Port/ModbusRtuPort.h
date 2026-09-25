/* Hardware-independent RTU byte-stream state machine. */
#ifndef MODBUS_RTU_PORT_H
#define MODBUS_RTU_PORT_H

#include <stdbool.h>
#include <stdint.h>
#include "../Backend/ModbusBackend.h"

#define MODBUS_RTU_MAX_FRAME_SIZE 256U

typedef struct { uint8_t byte; uint16_t timestamp; } ModbusRtuRxByte;

typedef struct
{
	void *context;
	uint32_t (*rxCount)(void *context);
	bool (*rxPop)(void *context, ModbusRtuRxByte *byte);
	uint16_t (*tickUs)(void *context);
	uint32_t (*txFree)(void *context);
	uint32_t (*txWrite)(void *context, const uint8_t *data, uint32_t size);
} ModbusRtuPort;

typedef struct
{
	ModbusRtuPort port;
	const ModbusBackend *backend;
	uint8_t deviceAddress;
	uint8_t frame[MODBUS_RTU_MAX_FRAME_SIZE];
	uint16_t t15Us;
	uint16_t t35Us;
	uint16_t lastTick;
	uint32_t index;
	uint32_t size;
	uint32_t frameErrors;
	uint8_t state;
} ModbusRtu;

void ModbusRtu_Init(ModbusRtu *rtu, const ModbusRtuPort *port,
	const ModbusBackend *backend, uint8_t deviceAddress, uint32_t baudrate);
void ModbusRtu_Reset(ModbusRtu *rtu);
void ModbusRtu_Poll(ModbusRtu *rtu);
uint32_t ModbusRtu_FrameErrors(const ModbusRtu *rtu);

#endif
