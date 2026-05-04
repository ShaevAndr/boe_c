/*=============================================================================
     Project:
     Platform: GD32F470
     Filename: ModbusRtuRoutine.h
     Description: Modbus RTU frame reception and transmission routine
     Version: 1.0
     Created: 2026.05.04
============================================================================*/
#ifndef ModbusRtuRoutine_H
#define ModbusRtuRoutine_H

#include <stdint.h>

//------------------------------------------------------------------------------
#define PROTOCOL_UNICORN  0
#define PROTOCOL_MODBUS   1
//------------------------------------------------------------------------------
void ModbusRtuRoutine(void);
void ModbusRtuRoutine_Init(void);
//------------------------------------------------------------------------------

#endif
