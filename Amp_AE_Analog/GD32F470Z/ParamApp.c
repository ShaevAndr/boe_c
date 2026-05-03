/*=============================================================================
  Project:
  Platform: STM32G743
  Filename: ParamApp.c
  Description:
  Programmer:
  Version: 2.0
  Created: 2006.01.16
  Last modified: 2022.10.11
=============================================================================*/
//#include <stm32h7xx_hal.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
//-----------------------------------------------------------------------------
#include "drv_EEPROM.h"
#include "drv_time.h"
#include "crc16.h"
//#include "netconf.h"
//#include "LockUnlock.h"
//#include "unicorn_uart_speed.h"
//-----------------------------------------------------------------------------
#define STRUCT_TO_STR
#include "ParamApp.h"
//-----------------------------------------------------------------------------
#define _FreeByte (0xFF)
static int32_t Init = 0;
//static TMTDDevice * pSPmtdDev;
//static uint32_t SizeOfFlash;
static lfs_t * pStorage = NULL;
//--------------------------------------------------------------------------//
#define PARAM_FILENAME  "app_param.bin"

TParamApp gParamApp;

static bool UsingDefaultSettings = true;
static TTime timeToWrite = 0;
static uint8_t needToSave = 0;
//--------------------------------------------------------------------------//
static void WriteToFlashParam (void);
static void WriteToFlashParamNow (void);
//--------------------------------------------------------------------------//
/*#define _FlashMaxCount (1)
//--------------------------------------------------------------------------//
int32_t GetFlashCount (void)
{
	if (!Init) return (0);
	return (_FlashMaxCount);
}
//--------------------------------------------------------------------------//
int32_t GetFlashDescription (uint32_t FlashNum, uint8_t * p)
{
	if (!Init) return (0);
	switch (FlashNum)
	{
//		case 0: return (sprintf ((char *)p, "Code flash"));
		case 0: return (sprintf ((char *)p, "Env flash"));
		default: return (0);
	}
}*/
//--------------------------------------------------------------------------//
#define FNV_32_PRIME ((unsigned int)0x01000193)
uint32_t FNV1Hash (uint8_t *buf)
{
	uint32_t hval = 0x811c9dc5; // FNV0 hval = 0
	while (*buf)
	{
		hval ^= (uint32_t)*buf++;
		hval *= FNV_32_PRIME;
	}
 	return hval;
}
//--------------------------------------------------------------------------//
static void InitParamApp (void)
{
	int32_t i;
  
//	gParamApp.version = 1;
	gParamApp.hashFNV = FNV1Hash (structTParamApp);
	
	gParamApp.DSLoc [0] = _DSLocIP;
	gParamApp.DSLoc [1] = _DSLocCore;

	gParamApp.ICPMinVoltage = 3.5f;
	for (i = 0; i < 8; i++) gParamApp.Ku_OP [i] = 0;

	gParamApp.CRC16 = 0;
  UsingDefaultSettings = true;
}
//--------------------------------------------------------------------------//
static uint16_t CalcCRC16 (void)
{
	return (CRC16_CCITT16 (	&gParamApp,
													sizeof (gParamApp) - sizeof (uint16_t))); 
}
//--------------------------------------------------------------------------//
static int32_t ValidityCheck (void)
{
	uint16_t ui;
	if (gParamApp.hashFNV != FNV1Hash (structTParamApp)) return (0);
	ui = CalcCRC16 ();
	if (ui != gParamApp.CRC16) return (0);
	return (1);
}
//--------------------------------------------------------------------------//
void LoadParamApp (lfs_t *storage)
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
    InitParamApp ();
    return;
  }
  resf = lfs_file_read(pStorage, &paramFile, &gParamApp, sizeof(gParamApp));
  res = lfs_file_close(pStorage, &paramFile);
  if (resf < 0)
  {
    InitParamApp ();
    return;
  }
  if (resf != sizeof(gParamApp))
  { // data size mismatch
    InitParamApp ();
    return;
  }
  if (res < 0)
  {
    InitParamApp ();
    return;
  }
  if (!ValidityCheck ())
  { // Validation failed
    InitParamApp ();
    return;
  }

  UsingDefaultSettings = false;
}
//-----------------------------------------------------------------------------
static void WriteToFlashParam (void)
{
	if (!Init) return;
	WriteToFlashParamNow ();
}
//-----------------------------------------------------------------------------
void SaveParamApp (void)
{
	if (!Init) return;
	timeToWrite = SetTime_ms (1000);
	needToSave = 1;
}
//-----------------------------------------------------------------------------
static int32_t TryWrite (int32_t AddrInc)
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
  resf = lfs_file_write(pStorage, &paramFile, &gParamApp, sizeof(gParamApp));
  res = lfs_file_close(pStorage, &paramFile);
  if (resf < 0)
  { // write error
    return -2;
  }
  if (resf != sizeof(gParamApp))
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
static void WriteToFlashParamNow (void)
{
  gParamApp.hashFNV = FNV1Hash (structTParamApp);
	gParamApp.CRC16 = CalcCRC16 ();
}
//-----------------------------------------------------------------------------
void RoutineSaveParamApp (void)
{
	int32_t res;
	if (!Init)
	{
		needToSave = 0;
		return;
	}
	if ((needToSave == 1) && (EndTime(timeToWrite)))
	{
		res = TryWrite (1);
//		if (0 == res)
//		{
//			EraseNow ();
//			TryWrite (0);
//		}
		needToSave = 0;
	}
}
//--------------------------------------------------------------------------//
void SetStateSaveParamApp (int32_t val)
{
	if (1 == val) SaveParamApp ();
}
//--------------------------------------------------------------------------//
int32_t GetStateSaveParamApp (void)
{
	if (needToSave) return (1);
	else return (0);
}
//--------------------------------------------------------------------------//
