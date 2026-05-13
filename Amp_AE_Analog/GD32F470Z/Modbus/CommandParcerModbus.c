/*=============================================================================
     Project:
     Platform: GD32F470
     Filename: CommandParcerModbus.c
     Description: Modbus RTU command dispatcher
     Version: 1.0
     Created: 2017.11.23
============================================================================*/
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
// Minimum request PDU sizes (bytes), enough to read fixed-length headers.
// Variable-length payload (FC 0x10) is additionally validated by the handler.
#define MIN_PDU_READ                  5  // FC + StartAddr(2) + Qty(2)
#define MIN_PDU_WRITE_SINGLE          7  // FC + RegAddr(2) + Value(4)
#define MIN_PDU_WRITE_MULTIPLE_HDR   10  // FC + StartAddr(2) + Qty(2) + BC(1) + Data(>=4)
//--------------------------------------------------------------------------//
uint8_t ModbusCommandProcess (uint8_t NumUART, uint8_t * Buff, uint32_t * pSize)
{
	uint8_t  Command = Buff[0];
	uint32_t reqSize = *pSize;

	switch (Command)
	{
		case _ReadHoldingRegisters:
			if (reqSize < MIN_PDU_READ) return _IllegalDataValue;
			return ReadHoldingsRegisters(NumUART, Command, Buff, pSize);

		case _ReadInputRegisters:
			if (reqSize < MIN_PDU_READ) return _IllegalDataValue;
			return ReadInputRegisters(NumUART, Command, Buff, pSize);

		case _WriteSingleRegister:
			if (reqSize < MIN_PDU_WRITE_SINGLE) return _IllegalDataValue;
			return WriteSingleRegister(NumUART, Command, Buff, pSize);

		case _WriteMultipleRegisters:
			if (reqSize < MIN_PDU_WRITE_MULTIPLE_HDR) return _IllegalDataValue;
			return WriteMultipleRegisters(NumUART, Command, Buff, pSize);

		default:
			return _IllegalFunction;
	}
}
