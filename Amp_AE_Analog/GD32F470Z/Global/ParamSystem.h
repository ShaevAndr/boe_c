/*=============================================================================
  Project: 
  Platform: 
  Filename: ParamSystem.h
  Description:
  Programmer:
  Version: 1.0
  Created: 2024.05.30
  Last modified: 2024.05.30
=============================================================================*/
#ifndef __ParamSystem_H__
#define __ParamSystem_H__
//-----------------------------------------------------------------------------
#include <stdint.h>
#include "lfs_storage.h"
#include "../Global/LockUnlock.h"
//-----------------------------------------------------------------------------

#define PARAMSAVED_MAC_ADDR_LEN          (6)
#define PARAMSAVED_RS485_MODBUS_COUNT    (4)

#define PROTOCOL_UNICORN                 (0)
#define PROTOCOL_MODBUS                  (1)

#pragma pack(1)
typedef struct
{
	uint8_t version;
  uint8_t reserve1[3];

  struct {
    uint8_t UARTSpeed;
    uint8_t ModbusAddress;
		uint8_t ProtocolMode;
		uint8_t reserved;
  } rs485Modbus[PARAMSAVED_RS485_MODBUS_COUNT];
  
  struct {
    uint8_t DHCPEnable;
    uint8_t reserve2;
    uint8_t MACAddr[PARAMSAVED_MAC_ADDR_LEN];
    uint32_t ipaddr;
    uint32_t netmask;
    uint32_t gateway;
    uint32_t dns;
  } eth;
  struct {
    uint64_t DateOfManufacture;
    uint16_t SerialNumber;
  } device;
  uint32_t UnlockKey[luktCount];
	
	uint16_t	CRC16;
} TParamSystem;
#pragma pack()
//-----------------------------------------------------------------------------
extern TParamSystem gParamSystem;
//-----------------------------------------------------------------------------
int macAddrIsValid(const uint8_t* addr);
void SaveParamSystem (void);
void LoadParamSystem (lfs_t *storage);
void RoutineSaveParamSystem (void);
//-----------------------------------------------------------------------------
#endif /*__ParamSystem_H__ */
