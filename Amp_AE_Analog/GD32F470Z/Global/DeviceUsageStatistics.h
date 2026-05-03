/*=============================================================================
  Project: 
  Platform: 
  Filename: DeviceUsageStatistics.h
  Description:
  Programmer:
  Version: 1.0
  Created: 2024.05.30
  Last modified: 2024.05.30
=============================================================================*/
#ifndef __DeviceUsageStatistics_H__
#define __DeviceUsageStatistics_H__
//-----------------------------------------------------------------------------
#include <stdint.h>
#include "lfs_storage.h"
//-----------------------------------------------------------------------------
uint32_t GetTotalOperatingTime (void);
uint32_t GetTotalOperatingTimeImMinutes (void);
uint32_t GetNumberOfBootloaderLaunches (void);
uint32_t GetNumberOfApplicationLaunches (void);
uint32_t GetNumberErrorWriteDeviceUsStat (void);
//float GetAbsoluteMaxTempValueDeviceUsStat (void);
//uint32_t GetAbsoluteMaxTempTimestampDeviceUsStat (void);
//uint32_t GetOperatingTimeAtHighTempDeviceUsStat (void);
void InitDeviceUsageStatistics (lfs_t *storage);
void RoutineDeviceUsageStatistics (void);
//-----------------------------------------------------------------------------
#endif /*__DeviceUsageStatistics_H__ */
