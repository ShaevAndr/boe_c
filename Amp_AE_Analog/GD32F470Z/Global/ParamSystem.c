/*=============================================================================
  Project: 
  Platform: STM32G743
  Filename: ParamSaved.c
  Description:
  Programmer: 
  Version: 2.0
  Created: 2006.01.16
  Last modified: 2022.10.11
=============================================================================*/
#include <stdint.h>
#include <stdbool.h>
//-----------------------------------------------------------------------------
#include "drv_time.h"
#include "crc16.h"
#include "unicorn_uart_speed.h"
//-----------------------------------------------------------------------------
#include "ParamSystem.h"
//-----------------------------------------------------------------------------
#define _FreeByte (0xFF)
static int32_t Init = 0;
static lfs_t * pStorage = NULL;
//--------------------------------------------------------------------------//

#define PARAM_FILENAME  "system_param.bin"

TParamSystem gParamSystem;

static bool UsingDefaultSettings = true;
static TTime timeToWrite = 0;
static uint8_t needToSave = 0;
//--------------------------------------------------------------------------//
static void WriteToFlashParam (void);
static void WriteToFlashParamNow (void);
//--------------------------------------------------------------------------//
/*int32_t GetFlashDescription (uint32_t FlashNum, uint8_t * p)
{
	if (!Init) return (0);
	switch (FlashNum)
	{
//		case 0: return (sprintf ((char *)p, "Code flash"));
		case 0: return (sprintf ((char *)p, "Env flash"));
		default: return (0);
	}
}*/

int macAddrIsValid(const uint8_t* addr)
{
  for (int i = 0; i < PARAMSAVED_MAC_ADDR_LEN; i++)
  {
    if (addr[i] != _FreeByte)
      return (1);
  }
  return (0);
}

static void InitParamSystem (void)
{
	int32_t i;
  
  memset(&gParamSystem, 0, sizeof(gParamSystem));
	gParamSystem.version = 1;
  for (i = 0; i < luktCount; i++)
    gParamSystem.UnlockKey[i] = CalcKey(i);
  for (i = 0; i < PARAMSAVED_RS485_MODBUS_COUNT; i++)
  {
    gParamSystem.rs485Modbus[i].UARTSpeed = _speed115200;
    gParamSystem.rs485Modbus[i].ModbusAddress = 1;
  }
  for (i = 0; i < PARAMSAVED_MAC_ADDR_LEN; i++)
    gParamSystem.eth.MACAddr[i] = _FreeByte;
  gParamSystem.eth.DHCPEnable = 1;
  //gParamSystem.eth.ipaddr = 0;
  //gParamSystem.eth.netmask = 0;
  //gParamSystem.eth.gateway = 0;
  //gParamSystem.eth.dns = 0;

	gParamSystem.CRC16 = 0;
  UsingDefaultSettings = true;
}
//--------------------------------------------------------------------------//
static uint16_t CalcCRC16 (void)
{
	return (CRC16_CCITT16 (	&gParamSystem,
													sizeof (gParamSystem) - sizeof (uint16_t))); 
}
//--------------------------------------------------------------------------//
static int32_t ValidityCheck (void)
{
	uint16_t ui;
	ui = CalcCRC16 ();
	if (ui != gParamSystem.CRC16) return (0);
	return (1);
}
//--------------------------------------------------------------------------//
void LoadParamSystem (lfs_t *storage)
{
  uint8_t lfs_read_file_buffer[LFS_CACHE_SIZE];
  struct lfs_file_config lfs_file_cfg;
  lfs_file_t paramFile;
	int res, resf;
  
	pStorage = storage;
	Init = 1;
  
	memset(&lfs_file_cfg, 0, sizeof(lfs_file_cfg));
	lfs_file_cfg.buffer = lfs_read_file_buffer;  // use the static buffer
	lfs_file_cfg.attr_count = 0;

	res = lfs_file_opencfg(pStorage, &paramFile, PARAM_FILENAME, LFS_O_RDONLY, &lfs_file_cfg);
  if (res < 0)
  { // open error
    InitParamSystem ();
    return;
  }
  resf = lfs_file_read(pStorage, &paramFile, &gParamSystem, sizeof(gParamSystem));
  res = lfs_file_close(pStorage, &paramFile);
  if (resf < 0)
  {
    InitParamSystem ();
    return;
  }
  if (resf != sizeof(gParamSystem))
  { // data size mismatch
    InitParamSystem ();
    return;
  }
  if (res < 0)
  {
    InitParamSystem ();
    return;
  }
  if (!ValidityCheck ())
  { // Validation failed
    InitParamSystem ();
    return;
  }
  UsingDefaultSettings = false;
}
//-----------------------------------------------------------------------------
static void WriteToFlashParamNow (void)
{
  gParamSystem.version = 1;
	gParamSystem.CRC16 = CalcCRC16 ();
}
//-----------------------------------------------------------------------------
static void WriteToFlashParam (void)
{
	if (!Init) return;
	WriteToFlashParamNow ();
}
//-----------------------------------------------------------------------------
static int32_t TryWrite ()
{
  uint8_t lfs_read_file_buffer[LFS_CACHE_SIZE];
  struct lfs_file_config lfs_file_cfg;
  lfs_file_t paramFile;
	int res, resf;
  
	memset(&lfs_file_cfg, 0, sizeof(lfs_file_cfg));
	lfs_file_cfg.buffer = lfs_read_file_buffer;  // use the static buffer
	lfs_file_cfg.attr_count = 0;

	res = lfs_file_opencfg(pStorage, &paramFile, PARAM_FILENAME, LFS_O_RDWR | LFS_O_CREAT, &lfs_file_cfg);
  if (res < 0)
  { // open error
    return -1;
  }
  WriteToFlashParamNow();
  resf = lfs_file_write(pStorage, &paramFile, &gParamSystem, sizeof(gParamSystem));
  res = lfs_file_close(pStorage, &paramFile); 
  if (resf < 0)
  { // write error
    return -2;
  }
  if (resf != sizeof(gParamSystem))
  { // data size mismatch
    return -3;
  }
  if (res < 0)
  {
    return -4;
  }
  return 0;
}
//-----------------------------------------------------------------------------
void SaveParamSystem (void)
{
	if (!Init) return;
	timeToWrite = SetTime_ms (1000);
	needToSave = 1;
  //int res = TryWrite();
}
//-----------------------------------------------------------------------------
void RoutineSaveParamSystem (void)
{
	int32_t res;
	if (!Init)
	{
		needToSave = 0;
		return;
	}
	if ((needToSave == 1) && (EndTime(timeToWrite)))
	{
		res = TryWrite ();
//		if (0 == res)
//		{
//			EraseNow ();
//			TryWrite (0);
//		}
		needToSave = 0;
	}
}
//--------------------------------------------------------------------------//
void SetStateSaveParamSystem (int32_t val)
{
	if (1 == val) SaveParamSystem ();
}
//--------------------------------------------------------------------------//
int32_t GetStateSaveParamSystem (void)
{
	if (needToSave) return (1);
	else return (0);
}
//--------------------------------------------------------------------------//
