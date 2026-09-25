/* GD32/Unicorn2 adapter. This is the only backend used by the legacy API. */
#ifndef MODBUS_BACKEND_GD32_H
#define MODBUS_BACKEND_GD32_H

#include <stdint.h>
#include "ModbusBackend.h"

void ModbusBackendGd32_Init(ModbusBackend *backend, uint8_t uartNumber);

#endif
