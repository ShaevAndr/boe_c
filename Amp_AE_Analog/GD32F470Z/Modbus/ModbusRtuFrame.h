/*=============================================================================
     Project:
     Platform: GD32F470
     Filename: ModbusRtuFrame.h
     Description: Modbus RTU frame processing (address check, CRC, response)
     Version: 1.0
     Created: 2026.05.04
============================================================================*/
#ifndef ModbusRtuFrame_H
#define ModbusRtuFrame_H

#include <stdint.h>

//------------------------------------------------------------------------------
// Process a complete Modbus RTU frame
// Buff: full frame [Addr(1)][PDU(N)][CRC_Lo(1)][CRC_Hi(1)]
// pSize: in = frame length, out = response length (0 = no response)
// deviceAddress: this device's Modbus address
void ModbusRtuFrame_Process(uint8_t NumUART, uint8_t *Buff, uint32_t *pSize, uint8_t deviceAddress);
//------------------------------------------------------------------------------

#endif
