/*=============================================================================
2     Project: 
3     Platform: 
4     Filename: Unicorn2Routine.h
5     Description:
6     Version: 0.0
7     Created: 2017.11.23
8     Last modified: 2023.06.28
9============================================================================*/
#ifndef Unicorn2Routine_H
#define Unicorn2Routine_H

#include <stdint.h>
#include "drv_RS485x.h"
//#include "unicorn2_tcp.h"
#define _MAX_UNICORN2_TCP_SESSIONS_COUNT (0)

//#define _RS485_MODBUS_COUNT  (_RS485_Count)
#define _RS485_MODBUS_COUNT  (3)
#define _MaxUARTNumber (_RS485_MODBUS_COUNT + _MAX_UNICORN2_TCP_SESSIONS_COUNT)
//-----------------------------------------------------------------------------
typedef struct
{
  uint32_t TotalRxByte;
  uint32_t TotalTxByte;
  uint32_t BeginFlags;
  uint32_t EndFlags;
} Unicorn2RoutineParam_t;
//-----------------------------------------------------------------------------
extern Unicorn2RoutineParam_t Unicorn2RoutineParam [_MaxUARTNumber + 1];
extern uint8_t ResetFlag;
//-----------------------------------------------------------------------------
void Unicorn2Routine (void);
//-----------------------------------------------------------------------------
uint32_t GetCurrUARTNumber (void);
void NeedToReset (uint8_t NumUART);
void SetUARTSpeed (uint8_t NumUART, uint8_t Speed);
void SetDeviceAddress(uint8_t NumUART, uint8_t address);
//-----------------------------------------------------------------------------
#endif 
