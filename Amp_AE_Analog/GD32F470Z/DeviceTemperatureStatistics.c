/*=============================================================================
  Project: 
  Platform: STM32G743
  Filename: DeviceTemperatureStatistics.c
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
#include "drv_DS18B20.h"
#include "DeviceUsageStatistics.h"
//-----------------------------------------------------------------------------
#include "DeviceTemperatureStatistics.h"
//-----------------------------------------------------------------------------
#define _DEVUSAGE_FILENAME  "device_temper_stat.bin"
#define _DeviceTempStVersion (1)
#define _WRITE_PERIOD_MSEC (60000)
#define _HIGHTEMP_VALUE  (85.0f)
#define _DayTempStatSize (24)
#define _MountTempStatSize (29)
#define _YearTempStatSize (11)
//-----------------------------------------------------------------------------
#pragma pack(1)
typedef struct
{
	uint8_t version;
	uint8_t reserv1 [3];
	uint32_t NumberOfFileRecords;
  struct {
    float value;
    uint32_t timestamp;
  } AbsoluteMaxTemp;
	uint32_t OperatingTimeAtHighTemp;
	float DayTemp [_DayTempStatSize];
	float MountTemp [_MountTempStatSize];
	float YearTemp [_YearTempStatSize];
	uint16_t CRC16;
} TDeviceTempStat;
#pragma pack()
//--------------------------------------------------------------------------//
static int32_t Init = 0;
static lfs_t * pStorage = NULL;
static TDeviceTempStat DeviceTempStat __attribute__((__aligned__(4)));
static uint32_t ErrorWriteDeviceTempStat = 0;
static TTime OneMinTO;

static bool UsingDefaultTempSettings = true;
//--------------------------------------------------------------------------//
bool GetUsingDefaultTempSettings (void)
{
	return (UsingDefaultTempSettings);
}
//--------------------------------------------------------------------------//
static void ShiftMass (float * p, uint32_t size, float value)
{
	for (int i = size - 1; i > 0; i--) p [i] = p [i - 1];
	p [0] = value;
}
//--------------------------------------------------------------------------//
static float MaxInMass (float * p, uint32_t size)
{
	int i;
	float ret = -INFINITY;
	for (i = 0; i < size; i++) if (p [i] > ret) ret = p [i];
	return (ret);
}
//--------------------------------------------------------------------------//
static void InitMass (float * p, uint32_t size, float value)
{
	for (int i = 0; i < size; i++) p [i] = value;
}
//--------------------------------------------------------------------------//
uint32_t GetNumberErrorWriteDeviceTempStat (void)
{
	return (ErrorWriteDeviceTempStat);
}
//--------------------------------------------------------------------------//
float GetAbsoluteMaxTempValue (void)
{
  return (DeviceTempStat.AbsoluteMaxTemp.value);
}
//--------------------------------------------------------------------------//
uint32_t GetAbsoluteMaxTempTimestamp (void)
{
  return (DeviceTempStat.AbsoluteMaxTemp.timestamp / 60);//hours = min / 60
}
//--------------------------------------------------------------------------//
uint32_t GetOperatingTimeAtHighTemp (void)
{
  return (DeviceTempStat.OperatingTimeAtHighTemp / 60);//hours = min / 60
}
//--------------------------------------------------------------------------//
float GetHourMaxTempValue (void)
{
  return (DeviceTempStat.DayTemp [0]);
}
//--------------------------------------------------------------------------//
float GetDayMaxTempValue (void)
{
  return (MaxInMass (DeviceTempStat.DayTemp, _DayTempStatSize));
}
//--------------------------------------------------------------------------//
float GetMountMaxTempValue (void)
{
	float DayMaxTemp = GetDayMaxTempValue ();
	float ret = MaxInMass (DeviceTempStat.MountTemp, _MountTempStatSize);
	if (ret < DayMaxTemp) ret = DayMaxTemp;
  return (ret);
}
//--------------------------------------------------------------------------//
float GetYearMaxTempValue (void)
{
	float MountMaxTemp = GetMountMaxTempValue ();
	float ret = MaxInMass (DeviceTempStat.YearTemp, _YearTempStatSize);
	if (ret < MountMaxTemp) ret = MountMaxTemp;
  return (ret);
}
//--------------------------------------------------------------------------//
static void InitDeviceUsStatParams (void)
{
	int32_t i;
  
	DeviceTempStat.version = _DeviceTempStVersion;
	DeviceTempStat.reserv1 [0] = DeviceTempStat.reserv1 [1]
		= DeviceTempStat.reserv1 [2] = 0U;
	DeviceTempStat.NumberOfFileRecords = 0UL;
  
	DeviceTempStat.AbsoluteMaxTemp.value = 0.0f;
	DeviceTempStat.AbsoluteMaxTemp.timestamp = 0UL;
	DeviceTempStat.OperatingTimeAtHighTemp = 0UL;

  UsingDefaultTempSettings = true;
}
//--------------------------------------------------------------------------//
static uint16_t CalcCRC16 (void)
{
	return (CRC16_CCITT16 (	&DeviceTempStat,
													sizeof (DeviceTempStat) - sizeof (uint16_t))); 
}
//--------------------------------------------------------------------------//
static int32_t ValidityCheck (void)
{
	uint16_t ui;
	ui = CalcCRC16 ();
	if (ui != DeviceTempStat.CRC16) return (0);
	if (DeviceTempStat.version != _DeviceTempStVersion) return (0);
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

	res = lfs_file_opencfg (pStorage, &paramFile, _DEVUSAGE_FILENAME, LFS_O_RDWR | LFS_O_CREAT, &lfs_file_cfg);
  if (res < 0)
  { // open error
    return -1;
  }
//  DeviceTempStat.version = _DeviceTempStVersion;
	DeviceTempStat.NumberOfFileRecords++;
	DeviceTempStat.CRC16 = CalcCRC16 ();
  res = lfs_file_write (pStorage, &paramFile, &DeviceTempStat, sizeof (DeviceTempStat));
	resC = lfs_file_close (pStorage, &paramFile);
	if ((res == sizeof (DeviceTempStat)) && resC >= 0)
		return (0);
	else
	{
		Error = true;
		return (-2);
	}
}
//--------------------------------------------------------------------------//
void InitDeviceTempStatistics (lfs_t *storage)
{
  uint8_t lfs_read_file_buffer [LFS_CACHE_SIZE];
  struct lfs_file_config lfs_file_cfg;
  lfs_file_t paramFile;
	int res, resC;
  
	OneMinTO = SetTime_ms (_WRITE_PERIOD_MSEC);
	pStorage = storage;
  
	memset (&lfs_file_cfg, 0, sizeof (lfs_file_cfg));
	lfs_file_cfg.buffer = lfs_read_file_buffer;  // use the static buffer
	lfs_file_cfg.attr_count = 0;

	res = lfs_file_opencfg (pStorage, &paramFile, _DEVUSAGE_FILENAME, LFS_O_RDONLY, &lfs_file_cfg);
  if (res == 0)
  { // open error
		res = lfs_file_read (pStorage, &paramFile, &DeviceTempStat, sizeof(DeviceTempStat));
		resC = lfs_file_close (pStorage, &paramFile);
  }
	Init = 1;
	
  if (!ValidityCheck ())
	{
		InitDeviceUsStatParams ();
		if (TryWrite () != 0) ErrorWriteDeviceTempStat++;
	}
	else UsingDefaultTempSettings = false;
}
//--------------------------------------------------------------------------//
void RoutineDeviceTempStatistics (void)
{
	float maxTemp = DS18B20_GetMaxTemperature();
	uint32_t TotalOperatingTime = GetTotalOperatingTimeImMinutes ();
	bool NeedToSave = false;
	if (!Init) return;
	if (!EndTime (OneMinTO)) return;
	OneMinTO = SetTime_ms (_WRITE_PERIOD_MSEC);
	if (isfinite (maxTemp))
	{
		if (maxTemp > DeviceTempStat.AbsoluteMaxTemp.value)
		{
			DeviceTempStat.AbsoluteMaxTemp.value = maxTemp;
			DeviceTempStat.AbsoluteMaxTemp.timestamp = TotalOperatingTime;
			NeedToSave = true;
		}
		if (maxTemp >= _HIGHTEMP_VALUE)
		{
			DeviceTempStat.OperatingTimeAtHighTemp++;
			NeedToSave = true;
		}
		if (DeviceTempStat.DayTemp [0] < maxTemp)
		{
			DeviceTempStat.DayTemp [0] = maxTemp;
			NeedToSave = true;
		}
		if ((TotalOperatingTime % 60) == 0)
		{
			if ((TotalOperatingTime % (60 * _DayTempStatSize)) == 0)
			{
				if ((TotalOperatingTime % (60 * _DayTempStatSize * (_MountTempStatSize + 1))) == 0)
					ShiftMass (DeviceTempStat.YearTemp, _YearTempStatSize, MaxInMass (DeviceTempStat.MountTemp, _MountTempStatSize));
				ShiftMass (DeviceTempStat.MountTemp, _MountTempStatSize, MaxInMass (DeviceTempStat.DayTemp, _DayTempStatSize));
			}
			ShiftMass (DeviceTempStat.DayTemp, _DayTempStatSize, maxTemp);
			NeedToSave = true;
		}
		if (NeedToSave)
		{
			if (TryWrite () != 0) ErrorWriteDeviceTempStat++;
		}
	}
}
//--------------------------------------------------------------------------//
