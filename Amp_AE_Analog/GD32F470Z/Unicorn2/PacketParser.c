/*=============================================================================
2     Project: 
3     Platform: GD32F407
4     Filename: PacketParser.c
5     Description:
6     Version: 0.0
7     Created: 2017.11.23
8     Last modified: 2023.06.28
9===========================================================================*/
#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
//--------------------------------------------------------------------------//
#include "drv_time.h"
#include "../drv_LEDs.h"
#include "CommandParser.h"
#include "ParamSystem.h"
#include "PacketParser.h"
//--------------------------------------------------------------------------//
PacketParserParam_t PacketParserParam [_PacketParsersCount];
//--------------------------------------------------------------------------//
static uint8_t CheckSumm (uint8_t * Buff, uint16_t Size)
{
  uint32_t s = 0;
  
  for (int i = 0; i < Size; i++)
    s += *Buff++;
  return ((uint8_t) (s & 0x000000ff));
}
//--------------------------------------------------------------------------//
static const uint8_t HexDigit [16] =
{'0', '1', '2', '3', '4', '5', '6', '7',
 '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
//--------------------------------------------------------------------------//
static uint8_t Char2HEX (uint8_t c, int8_t * err)
{
  switch (toupper (c))
  {
		case '0': return (0x00);
		case '1': return (0x01);
		case '2': return (0x02);
		case '3': return (0x03);
		case '4': return (0x04);
		case '5': return (0x05);
		case '6': return (0x06);
		case '7': return (0x07);
		case '8': return (0x08);
		case '9': return (0x09);
		case 'A': return (0x0A);
		case 'B': return (0x0B);
		case 'C': return (0x0C);
		case 'D': return (0x0D);
		case 'E': return (0x0E);
		case 'F': return (0x0F);
		default: *err |= 1; return (0xff);
  }
}
//--------------------------------------------------------------------------//
static int8_t Str2Char (uint8_t * p, uint8_t * d)
{
  int8_t err = 0;
  *d = (uint8_t)(Char2HEX (p [0], &err) << 4);
  *d |= Char2HEX (p [1], &err);
  return (err);
}
//--------------------------------------------------------------------------//
static void Char2Str (uint8_t * p, uint8_t d)
{
  p [0] = HexDigit [(d >> 4) & 0x0f];
  p [1] = HexDigit [d & 0x0f];
}
//--------------------------------------------------------------------------//
#define _UnPackOKBroadcast  (1)
#define _UnPackOK           (0)
#define _UnPackErrorCharKC  (-2)
#define _UnPackErrorKC      (-3)
#define _UnPackErrorChar    (-4)
#define _UnPackAddrNoMatch  (-5)
//--------------------------------------------------------------------------//
static int8_t UnpacRxPacet (uint8_t NumUART, uint8_t * Buff, uint32_t * pSize)
{
  uint8_t c;
  uint32_t i;

  //проверка КС
  if (Str2Char (&(Buff [(*pSize) - 2]), &c) != 0)
  {
    *pSize = 0;
    PacketParserParam [NumUART].stat.ErrorCharKC++;
    return (_UnPackErrorCharKC);
  }

  if (CheckSumm (Buff, (uint16_t)(*pSize) - 2) != c)
  {
    *pSize = 0;
		PacketParserParam [NumUART].stat.ErrorKC++;
    return (_UnPackErrorKC);
  }
  *pSize -= 2;

  //корректировка размера пакета для перевода в HEX (бинарный) формат
  *pSize -= 1;
  *pSize /= 2;

  //преобразование пакета из ASCII в HEX (бинарный) формат
  for (i = 0; i < *pSize; i++)
  {
    if (Str2Char (&(Buff [i * 2 + 1]), &c) != 0)
    {
      *pSize = 0;
		 	PacketParserParam [NumUART].stat.ErrorChar++;
      return (_UnPackErrorChar);
    }
    Buff [i] = c;
  }
  
  //проверка адреса
  if ((Buff [0] != PacketParserParam [NumUART].Address) &&
    ((Buff [0] != 0xFF) || (Buff [1] != 0xFF)))
  {
    *pSize = 0;
		PacketParserParam [NumUART].stat.AddressNoMach++;
    return (_UnPackAddrNoMatch);
  }
  if (Buff [0] == PacketParserParam [NumUART].Address)
  {
		PacketParserParam [NumUART].stat.AddressMach++;
  	return (_UnPackOK);
  }
  Buff [0] = PacketParserParam [NumUART].Address;
	PacketParserParam [NumUART].stat.AddressBroadcast++;
  return (_UnPackOKBroadcast);
}
//--------------------------------------------------------------------------//
static void PackRxPacet (uint8_t * Buff, uint32_t * pSize, char Flag)
{
  uint8_t c;
  int32_t i;

  if ((*pSize * 2 + 1) > _SizeBuffRS485)
  {
    *pSize = 0;
    return;
  }
  //преобразование пакета из ASCII в HEX (бинарный) формат
  for (i = (int32_t)(*pSize - 1); i >= 0; i--)
    Char2Str (&(Buff [i * 2 + 1]), Buff [i]);
//  {
//    Char2Str (&(Buff [(uint32_t)i * 2 + 1]), Buff [(uint32_t)i]);
//  }
  *pSize *= 2;
  *pSize += 1;
  //флаг ответа
  Buff [0] = Flag;
  //расчет КС
  c = CheckSumm (Buff, (uint16_t)(*pSize));
  Char2Str (Buff + *pSize, c);
  *pSize += 2;
  Buff [*pSize] = _MODBUS_DELIMITER;
  *pSize += 1;
}
//--------------------------------------------------------------------------//
static void RetError (uint8_t * Buff, uint32_t * pSize, uint8_t ErrorNum)
{
  uint8_t c;
  int32_t i;

  Buff [2] = ErrorNum;
    *pSize = 3;
  //преобразование пакета из ASCII в HEX (бинарный) формат
  for (i = (int32_t)(*pSize - 1); i >= 0; i--)
    Char2Str (&(Buff [(uint16_t)i * 2 + 1]), Buff [(uint16_t)i]);
  *pSize *= 2;
  *pSize += 1;
  //флаг ответа
  Buff [0] = _MODBUS_ERROR_PACKET_HEADER;
  //расчет КС
  c = CheckSumm (Buff, (uint16_t)(*pSize));
  Char2Str (Buff + *pSize, c);
  *pSize += 2;
  Buff [*pSize] = _MODBUS_DELIMITER;
  *pSize += 1;
}
//--------------------------------------------------------------------------//
void RecieveCompl (uint8_t NumUART, uint8_t * Buff, uint32_t * pSize)
{
  uint8_t ErrorNumCom = _NoError;
  int8_t ErrorNumPack;

	PacketParserParam [NumUART].stat.TotalRxPacket++;
  
  if (Buff [0] != _MODBUS_REQUEST_PACKET_HEADER)
  {
    *pSize = 0;
     return;
  }

  //if (NumUART > 2) NumUART = 2;
  ErrorNumPack = UnpacRxPacet (NumUART, Buff, pSize);
  
  if ((ErrorNumPack != _UnPackOK) && (ErrorNumPack != _UnPackOKBroadcast))
  {
    *pSize = 0;
  	return;
  }
	PacketParserParam [NumUART].stat.TotalTxPacket++;
  
  ErrorNumCom = _NoError;
  ErrorNumCom = CommandProcess (NumUART, Buff, pSize);
	PingActivitiLED ();
  
  if (ErrorNumCom != _NoError)
    RetError (Buff, pSize, ErrorNumCom);
  else
    PackRxPacet (Buff, pSize, _MODBUS_ANSWER_PACKET_HEADER);
}
//--------------------------------------------------------------------------//
void PacketParser_Init(void)
{
  for (int i = 0; i < _RS485_MODBUS_COUNT; i++)
  {
    PacketParser_SetDeviceAddress (i, gParamSystem.rs485Modbus[i].ModbusAddress);
  }
  for (int i = _RS485_MODBUS_COUNT; i < _PacketParsersCount; i++)
  {
    PacketParserParam [i].Address = 1;
  }
}

void PacketParser_SetDeviceAddress(uint8_t NumUART, uint8_t address)
{
  PacketParserParam [NumUART].Address = address;
  printf("RS485:%d address %d\n", NumUART, address);
}
