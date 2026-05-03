/*=============================================================================
2	  Project:
3	  Platform: GD32F407
4	  Filename: drv_time.h
5	  Description:
6	  Version: 0.0
7	  Created: 2022.08.20
8   Last modified: 2023.06.28
9	=============================================================================*/
#ifndef drv_time_H
#define drv_time_H

#include <stdint.h>

	typedef uint64_t TTime;

	void SysTick_Handler (void);
	//void TimeCounter_Handler (void);

	void InitTime (void);
	uint32_t GetTimeSec (void);
	uint32_t GetTimeMSec (void);
	uint64_t GetTimeUSec (void);
	float fGetTime (void);
	TTime SetTime_ms (uint32_t Time);
	TTime SetTime_us (uint32_t Time);
	int32_t EndTime (TTime Time);
	//TTime ClrTime (void);
	void Delay_ms (uint32_t ms);
	void Delay_us (uint32_t us);

	void SetCorrTimeInPPM (float Corr);
	float GetCorrTimeInPPM (void);
	int32_t GetPeriodCorrTime (void);

	uint64_t globalTimestampUSec (void);
	int64_t globalTimestampOffset (void);
	void correctGlobalTimestampOffset (int64_t correction);
  uint64_t timeSinceLastSntpTimeSyncUSec();

	uint32_t GetSNTPTimeOfAdjustment (void);
	float GetSNTPLastTimeAdjustmentSize (void);

#endif 

