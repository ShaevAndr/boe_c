/*=============================================================================
     Project:
     Platform: GD32F470
     Filename: ReadInputRegisters.h
     Description: Modbus FC 0x04 - Read Input Registers (Telemetry)
     Version: 1.0
     Created: 2026.05.04
============================================================================*/
#ifndef ReadInputRegisters_H
#define ReadInputRegisters_H

#include <stdint.h>

uint8_t ReadInputRegisters(uint8_t NumUART, uint8_t Command, uint8_t *B, uint32_t *pSize);

#endif
