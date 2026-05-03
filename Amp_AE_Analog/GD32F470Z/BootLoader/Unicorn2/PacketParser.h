/*=============================================================================
2     Project: 
3     Platform: 
4     Filename: PacketParser.h
5     Description:
6     Version: 0.0
7     Created: 2017.11.23
8     Last modified: 2023.06.28
9============================================================================*/
#ifndef PacketParser_H
#define PacketParser_H

#include <stdint.h>
//-----------------------------------------------------------------------------
#include "Unicorn2Routine.h"
//-----------------------------------------------------------------------------
#define _SizeBuffRS485 (8500)
//#define _SizeBuffRS485 (1400)
#define _PacketParsersCount (_MaxUARTNumber + 1)

#define _kMinDeviceAddress (1)
#define _kMaxDeviceAddress (254)
//-----------------------------------------------------------------------------
typedef struct
{
  uint32_t TotalRxPacket;
  uint32_t TotalTxPacket;
  uint32_t ErrorChar;
  uint32_t ErrorCharKC;
  uint32_t ErrorKC;
  uint32_t AddressNoMach;
  uint32_t AddressMach;
  uint32_t AddressBroadcast;
} PacketParserStat_t;
typedef struct
{
  uint8_t Address;
  PacketParserStat_t stat;
} PacketParserParam_t;

#define _MODBUS_DELIMITER             (0x0D)
#define _MODBUS_REQUEST_PACKET_HEADER ':'
#define _MODBUS_ANSWER_PACKET_HEADER  '!'
#define _MODBUS_ERROR_PACKET_HEADER   '?'

//-----------------------------------------------------------------------------
//extern PacketParserParam_t PacketParserParam [_PacketParsersCount];
//------------------------------------------------------------------------------
void RecieveCompl (uint8_t NumUART, uint8_t * Buff, uint32_t * pSize);
//-----------------------------------------------------------------------------
void PacketParser_Init(void);
void PacketParser_SetDeviceAddress(uint8_t NumUART, uint8_t address);

#endif
