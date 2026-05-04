/*=============================================================================
     Project:
     Platform: GD32F470
     Filename: ModbusRtuFrame.c
     Description: Modbus RTU frame processing
     Version: 1.0
     Created: 2026.05.04
============================================================================*/
#include <stdint.h>
#include <stdbool.h>
//--------------------------------------------------------------------------//
#include "ModbusRtuFrame.h"
#include "CommandParcerModbus.h"
#include "ErrorHandler.h"
#include "../Unicorn2/crc16.h"
//--------------------------------------------------------------------------//

void ModbusRtuFrame_Process(uint8_t NumUART, uint8_t *Buff, uint32_t *pSize, uint8_t deviceAddress)
{
	uint32_t frameSize = *pSize;

	// Minimum frame: Addr(1) + FC(1) + CRC(2) = 4 bytes
	if (frameSize < 4)
	{
		*pSize = 0;
		return;
	}

	// Verify CRC16
	uint16_t receivedCrc = (uint16_t)Buff[frameSize - 2] | ((uint16_t)Buff[frameSize - 1] << 8);
	uint16_t calculatedCrc = CRC16(Buff, frameSize - 2);
	if (receivedCrc != calculatedCrc)
	{
		*pSize = 0;
		return;
	}

	// Check address
	uint8_t address = Buff[0];
	bool isBroadcast = (address == 0);

	if (address != deviceAddress && !isBroadcast)
	{
		*pSize = 0;
		return;
	}

	// PDU starts after address, ends before CRC
	uint8_t *pdu = &Buff[1];
	uint32_t pduSize = frameSize - 3;

	// Minimum PDU is 5 bytes (FC + 2 addr + 2 data) for most commands
	if (pduSize < 5)
	{
		*pSize = 0;
		return;
	}

	uint8_t fc = pdu[0];
	uint8_t error = ModbusCommandProcess(NumUART, pdu, &pduSize);

	// No response to broadcast
	if (isBroadcast)
	{
		*pSize = 0;
		return;
	}

	if (error != _NoError)
	{
		// Exception response: [Addr][FC|0x80][ExceptionCode][CRC]
		Buff[0] = deviceAddress;
		Buff[1] = fc | 0x80;
		Buff[2] = error;
		uint16_t crc = CRC16(Buff, 3);
		Buff[3] = (uint8_t)(crc & 0xFF);
		Buff[4] = (uint8_t)(crc >> 8);
		*pSize = 5;
		return;
	}

	// Normal response: [Addr][ResponsePDU][CRC]
	Buff[0] = deviceAddress;
	// ResponsePDU already at Buff[1], length = pduSize
	uint16_t crc = CRC16(Buff, 1 + pduSize);
	Buff[1 + pduSize]     = (uint8_t)(crc & 0xFF);
	Buff[1 + pduSize + 1] = (uint8_t)(crc >> 8);
	*pSize = 1 + pduSize + 2;
}
