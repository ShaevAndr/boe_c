/*=============================================================================
     Project:
     Platform: GD32F470
     Filename: ModbusRtuFrame.c
     Description: Modbus RTU frame processing
     Version: 1.0
     Created: 2026.05.04
============================================================================*/
#include "ModbusRtuFrame.h"
#include "Core/ModbusCore.h"
#include "Backend/ModbusBackendGd32.h"

void ModbusRtuFrame_Process(uint8_t NumUART, uint8_t *Buff, uint32_t *pSize, uint8_t deviceAddress)
{
	ModbusBackend backend;
	ModbusBackendGd32_Init(&backend, NumUART);
	ModbusCore_ProcessRtuFrame(&backend, Buff, pSize, deviceAddress);
}
