/*=============================================================================
2     Project:
3     Platform: GD32F470
4     Filename: CommandParcerModbus.c
5     Description: Modbus RTU command dispatcher
6     Version: 1.0
7     Created: 2017.11.23
9============================================================================*/
#include <stdint.h>
#include <stdbool.h>
//--------------------------------------------------------------------------//
#include "ReadHoldingRegisters.h"
#include "ReadInputRegisters.h"
#include "WriteSingleRegister.h"
#include "WriteMultipleRegisters.h"
#include "ErrorHandler.h"
#include "CommandParcerModbus.h"
//--------------------------------------------------------------------------//
uint8_t ModbusCommandProcess (uint8_t NumUART, uint8_t * Buff, uint32_t * pSize)
{
  uint8_t ErrorNum = _NoError;
  uint8_t Command = Buff [0];

  switch (Command)
  {
		case _ReadHoldingRegisters:
			ErrorNum = ReadHoldingsRegisters(NumUART, Command, Buff, pSize);
			break;
		case _ReadInputRegisters:
			ErrorNum = ReadInputRegisters(NumUART, Command, Buff, pSize);
			break;
		case _WriteSingleRegister:
			ErrorNum = WriteSingleRegister(NumUART, Command, Buff, pSize);
			break;
		case _WriteMultipleRegisters:
			ErrorNum = WriteMultipleRegisters(NumUART, Command, Buff, pSize);
			break;
		default:
			ErrorNum = _IllegalFunction;
			break;
  }
  return (ErrorNum);
}
