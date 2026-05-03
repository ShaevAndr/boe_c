/*=============================================================================
  Project: 
  Platform: STM32G743
  Filename: DeviceUsageStatistics.c
  Description:
  Programmer: 
  Version: 2.0
  Created: 2025.04.02
  Last modified: 2025.04.02
=============================================================================*/
#include <stdint.h>
#include <math.h>
//#include <stdbool.h>
//-----------------------------------------------------------------------------
#include "drv_time.h"
#include "crc16.h"
//-----------------------------------------------------------------------------
#include "DeviceUsageStatistics.h"
//-----------------------------------------------------------------------------
#define DEVUSAGE_FILENAME  "device_usage_stat.bin"
#define _DeviceUsStatStVersion (5)
//#define _FreeByte (0xFF)
#define WRITE_PERIOD_MSEC (60000)
//#define WRITE_PERIOD_MSEC (1000)
#define HIGHTEMP_VALUE  (85.0f)
//-----------------------------------------------------------------------------
#pragma pack(1)
typedef struct
{
	uint8_t version;
  uint8_t reserve1[1];

	uint16_t NumberOfBootloaderLaunches;
	uint32_t TotalOperatingTime;
	uint16_t NumberOfApplicationLaunches;

	uint16_t	CRC16;
} TDeviceUsStat;
#pragma pack()
//--------------------------------------------------------------------------//
static int32_t Init = 0;
static lfs_t * pStorage = NULL;
static TDeviceUsStat DeviceUsStat;
static uint32_t ErrorWriteDeviceUsStat = 0;
static TTime OneMinTO;

static bool UsingDefaultSettings = true;
//--------------------------------------------------------------------------//
uint32_t GetTotalOperatingTime (void)
{
	return (DeviceUsStat.TotalOperatingTime / 60);//hours = min / 60
}
//--------------------------------------------------------------------------//
uint32_t GetTotalOperatingTimeImMinutes (void)
{
	return (DeviceUsStat.TotalOperatingTime);
}
//--------------------------------------------------------------------------//
uint32_t GetNumberOfBootloaderLaunches (void)
{
	return (DeviceUsStat.NumberOfBootloaderLaunches);
}
//--------------------------------------------------------------------------//
uint32_t GetNumberOfApplicationLaunches (void)
{
	return (DeviceUsStat.NumberOfApplicationLaunches);
}
//--------------------------------------------------------------------------//
uint32_t GetNumberErrorWriteDeviceUsStat (void)
{
	return (ErrorWriteDeviceUsStat);
}
//--------------------------------------------------------------------------//
static void InitDeviceUsStatParams (void)
{
	int32_t i;
  
	DeviceUsStat.version = _DeviceUsStatStVersion;
	DeviceUsStat.reserve1 [0] = 0U;
	DeviceUsStat.TotalOperatingTime = 1UL;
	DeviceUsStat.NumberOfBootloaderLaunches = 1UL;
	DeviceUsStat.NumberOfApplicationLaunches = 1UL;

  UsingDefaultSettings = true;
}
//--------------------------------------------------------------------------//
static uint16_t CalcCRC16 (void)
{
	return (CRC16_CCITT16 (	&DeviceUsStat,
													sizeof (DeviceUsStat) - sizeof (uint16_t))); 
}
//--------------------------------------------------------------------------//
static int32_t ValidityCheck (void)
{
	uint16_t ui;
	ui = CalcCRC16 ();
	if (ui != DeviceUsStat.CRC16) return (0);
	if (DeviceUsStat.version != _DeviceUsStatStVersion) return (0);
	return (1);
}
//--------------------------------------------------------------------------//
static int32_t TryWrite ()
{
	static bool Error = false;
  uint8_t lfs_read_file_buffer [LFS_CACHE_SIZE];
  struct lfs_file_config lfs_file_cfg;
  lfs_file_t paramFile;
	int res, resC;

	if (Error) return (-3);
	memset (&lfs_file_cfg, 0, sizeof (lfs_file_cfg));
	lfs_file_cfg.buffer = lfs_read_file_buffer;  // use the static buffer
	lfs_file_cfg.attr_count = 0;

	res = lfs_file_opencfg (pStorage, &paramFile, DEVUSAGE_FILENAME, LFS_O_RDWR | LFS_O_CREAT, &lfs_file_cfg);
  if (res < 0)
  { // open error
    return -1;
  }
  DeviceUsStat.version = _DeviceUsStatStVersion;
	DeviceUsStat.CRC16 = CalcCRC16 ();
  res = lfs_file_write (pStorage, &paramFile, &DeviceUsStat, sizeof (DeviceUsStat));
	resC = lfs_file_close (pStorage, &paramFile);
	if ((res == sizeof (DeviceUsStat)) && resC >= 0)
		return (0);
	else
	{
		Error = true;
		return (-2);
	}
}
//--------------------------------------------------------------------------//
void InitDeviceUsageStatistics (lfs_t *storage)
{
  uint8_t lfs_read_file_buffer [LFS_CACHE_SIZE];
  struct lfs_file_config lfs_file_cfg;
  lfs_file_t paramFile;
	int res, resC;
  
	OneMinTO = SetTime_ms (WRITE_PERIOD_MSEC);
	pStorage = storage;
  
	memset (&lfs_file_cfg, 0, sizeof (lfs_file_cfg));
	lfs_file_cfg.buffer = lfs_read_file_buffer;  // use the static buffer
	lfs_file_cfg.attr_count = 0;

	res = lfs_file_opencfg (pStorage, &paramFile, DEVUSAGE_FILENAME, LFS_O_RDONLY, &lfs_file_cfg);
  if (res == 0)
  { // open error
		res = lfs_file_read (pStorage, &paramFile, &DeviceUsStat, sizeof(DeviceUsStat));
		resC = lfs_file_close (pStorage, &paramFile);
  }
	Init = 1;
		
  if (!ValidityCheck ()) InitDeviceUsStatParams ();
	else UsingDefaultSettings = false;

	#ifdef __BootLoader__
		if (DeviceUsStat.NumberOfBootloaderLaunches < UINT16_MAX)
			DeviceUsStat.NumberOfBootloaderLaunches++;
	#endif
	#ifdef __Aplication__
		if (DeviceUsStat.NumberOfApplicationLaunches < UINT16_MAX)
			DeviceUsStat.NumberOfApplicationLaunches++;
	#endif
	DeviceUsStat.TotalOperatingTime++;
	
	if (TryWrite () != 0) ErrorWriteDeviceUsStat++;
}
//--------------------------------------------------------------------------//
void RoutineDeviceUsageStatistics (void)
{
	if (!Init) return;
	if (!EndTime (OneMinTO)) return;
	OneMinTO = SetTime_ms (WRITE_PERIOD_MSEC);
	DeviceUsStat.TotalOperatingTime++;

	if (TryWrite () != 0) ErrorWriteDeviceUsStat++;
}
//--------------------------------------------------------------------------//
