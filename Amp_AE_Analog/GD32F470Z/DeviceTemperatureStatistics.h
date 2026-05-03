/*=============================================================================
  Project: 
  Platform: 
  Filename: DeviceTemperatureStatistics.h
  Description:
  Programmer:
  Version: 1.0
  Created: 2025.07.25
  Last modified: 2025.07.25
=============================================================================*/
#ifndef __DeviceTemperatureStatistics_H__
	#define __DeviceTemperatureStatistics_H__
	//-----------------------------------------------------------------------------
	#include <stdint.h>
	#include <stdbool.h>
	#include "lfs_storage.h"
	//-----------------------------------------------------------------------------
	float GetHourMaxTempValue (void);
	float GetDayMaxTempValue (void);
	float GetMountMaxTempValue (void);
	float GetYearMaxTempValue (void);
	float GetAbsoluteMaxTempValue (void);
	uint32_t GetAbsoluteMaxTempTimestamp (void);
	uint32_t GetOperatingTimeAtHighTemp (void);
	uint32_t GetNumberErrorWriteDeviceTempStat (void);
	bool GetUsingDefaultTempSettings (void);
	void InitDeviceTempStatistics (lfs_t *storage);
	void RoutineDeviceTempStatistics (void);
	//-----------------------------------------------------------------------------
#endif /*__DeviceTemperatureStatistics_H__ */
