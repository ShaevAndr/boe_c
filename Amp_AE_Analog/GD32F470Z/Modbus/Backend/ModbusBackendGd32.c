#include <stdint.h>
#include <stddef.h>

#include "ModbusBackendGd32.h"
#include "../CommandParcerModbus.h"
#include "../../drv_LEDs.h"

static uint8_t ProcessPdu(void *context, uint8_t *pdu, uint32_t *pduSize)
{
	return ModbusCommandProcess((uint8_t)(uintptr_t)context, pdu, pduSize);
}

static void FrameActivity(void *context)
{
	(void)context;
	PingActivitiLED();
}

void ModbusBackendGd32_Init(ModbusBackend *backend, uint8_t uartNumber)
{
	backend->context = (void *)(uintptr_t)uartNumber;
	backend->processPdu = ProcessPdu;
	backend->frameActivity = FrameActivity;
}
