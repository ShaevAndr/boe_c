/*=============================================================================
2	  Project: 
3	  Platform: GD32F407
4	  Filename: drv_time.c
5	  Description:
6	  Version: 0.0
7	  Created: 2022.08.20
8   Last modified: 2023.06.28
9	=============================================================================*/
#include <stdint.h>
#include <stdlib.h>
//#include <stm32h7xx_hal.h>
#include "gd32f4xx.h"
#include "math.h"
//------------------------------------------------------------------------------
#include "drv_time.h"
//#include "../drv_TestPinKT.h"
//------------------------------------------------------------------------------
static volatile uint64_t TimeCounter = 0;
static volatile int64_t CorrTimeCounter = 0;
static int32_t PeriodCorrTime = 0;
//------------------------------------------------------------------------------
void TimeCounter_Handler(void);
//------------------------------------------------------------------------------
void InitTime (void)
{
    //Ининциализация счетчика времени
	TimeCounter = 0;
	
	
	
//  SystemCoreClockUpdate ();                      /* Get Core Clock Frequency   */
//  SysTick_Config (0x00ffffff);
}
//------------------------------------------------------------------------------
static uint32_t SystickClockPerSec (void)
{
	uint32_t u = rcu_clock_freq_get (CK_AHB);
	if (0 == (SysTick->CTRL & SYSTICK_CLKSOURCE_HCLK)) u >>= 3;
//  uint32_t u = HAL_RCC_GetSysClockFreq();
	return (u);
}
//------------------------------------------------------------------------------
static uint32_t SystickClockMaxValue (void)
{
	return (SysTick->LOAD + 1UL);
}
//------------------------------------------------------------------------------
//процедура обработки прерываения
//void TimeCounter_Handler (void)
void SysTick_Handler (void)
{
  TimeCounter++;
}
//------------------------------------------------------------------------------
static TTime GetTime (void)/* __critical*/ //возвращает текущее значение счетчика
{
	TTime t;
	uint32_t ui;
	uint64_t tt;
	static TTime NextCorrTime = 0;
	do {
		tt = TimeCounter;
		ui = SysTick->VAL;
	} while (tt != TimeCounter);
	t = tt;
	t *= SystickClockMaxValue ();
	t += SystickClockMaxValue () - ui;
	if (0 != PeriodCorrTime && NextCorrTime < t)
	{
		NextCorrTime = t + abs (PeriodCorrTime);
		if (PeriodCorrTime > 0) CorrTimeCounter++;
		else CorrTimeCounter--;
	}
	t += CorrTimeCounter;
	return (t);
}
//------------------------------------------------------------------------------
void SetCorrTimeInPPM (float Corr)
{
	if (Corr != 0) Corr = 1.0e6 / Corr;
	else Corr = 2147483647.0f;
	if (Corr > 0) Corr += 0.5f;
	else  Corr -= 0.5f;
	if (Corr > 2147483647.0f) Corr = 2147483647.0f;
	if (Corr < -2147483647.0f) Corr = -2147483647.0f;
	PeriodCorrTime = Corr;
}
//------------------------------------------------------------------------------
float GetCorrTimeInPPM (void)
{
	float f = PeriodCorrTime;
	f = 1.0e6 / f;
	return (f);
}
//------------------------------------------------------------------------------
int32_t GetPeriodCorrTime (void)
{
	return (PeriodCorrTime);
}
//------------------------------------------------------------------------------
uint32_t GetTimeSec (void) //возвращает текущее значение времени в секундах
{
	TTime t;
	uint64_t ui;

	t = GetTime ();
	ui = t / SystickClockPerSec ();
	
	return ((uint32_t)ui);
}

uint32_t GetTimeMSec (void) //возвращает текущее значение времени в миллисекундах
{
  TTime t;
	uint64_t ui;

	t = GetTime();
  t *= 1000;
	ui = t / SystickClockPerSec ();
	
	return ((uint32_t)ui);
}

uint64_t GetTimeUSec (void) //возвращает текущее значение времени в микросекундах
{
  TTime t;
	uint64_t ui;

	t = GetTime();
  t *= 1000;
	ui = t / (SystickClockPerSec()/1000);
	
	return ui;
}
//------------------------------------------------------------------------------
float fGetTime (void) //возвращает текущее значение времени в секундах
{
	TTime t = GetTime ();
	float f = (float)t;
	f /= (float)(SystickClockPerSec ());
	return (f);
}
//------------------------------------------------------------------------------
TTime SetTime_ms (uint32_t Time)//Преобразовывает мс в отсчеты счетчика
{
	TTime t = GetTime ();
	uint64_t i = SystickClockPerSec ();
	i *= Time;
	i /= 1000;
	t += i;
	return (t);
}
//------------------------------------------------------------------------------
TTime SetTime_us (uint32_t Time)
{
	TTime t = GetTime ();
	uint64_t i = SystickClockPerSec ();
	i *= Time;
	i /= 1000;
	i /= 1000;
	t += i;
	return (t);
}
//------------------------------------------------------------------------------
int32_t EndTime (TTime Time)//наступил ли момент времени Time (в отсчетах)
{
	TTime t = GetTime();
	if (t > Time)
		return (1);
	else
		return (0);
}
//------------------------------------------------------------------------------
/*
TTime ClrTime (void)
{
	idata TTime t;
	t = GetTime();
	return (t);
}
//------------------------------------------------------------------------------
*/
void Delay_ms (uint32_t ms)
{
	TTime t = SetTime_ms (ms);
	while (!EndTime (t)) __asm volatile("");
}
//------------------------------------------------------------------------------
void Delay_us (uint32_t us)
{
	TTime t = SetTime_us (us);
	while (!EndTime (t)) __asm volatile("");
}
//------------------------------------------------------------------------------
static int64_t GlobalTimestampOffset = 1000000LL*60ULL*60LL*24LL*365LL*(2022ULL-1970LL);
//------------------------------------------------------------------------------
/*
static long long uptimeMSec()
{
	TTime uptime = GetTime ();
	return (uptime / SystickClockPerSec ());
}
*/
//--------------------------------------------------------------------------//
static uint64_t MomentOfLastTimeAdjustment = 0;
static float LastTimeAdjustmentSizeSec = 0.0;
//------------------------------------------------------------------------------
uint64_t globalTimestampUSec (void)
{
	int64_t ll = GlobalTimestampOffset;
	ll += GetTimeUSec();
	return ((uint64_t)ll);
}
//------------------------------------------------------------------------------
int64_t globalTimestampOffset (void)
{
	return GlobalTimestampOffset;
}
//------------------------------------------------------------------------------
void correctGlobalTimestampOffset (int64_t correction)
{
	GlobalTimestampOffset += correction;
	LastTimeAdjustmentSizeSec = correction;
	LastTimeAdjustmentSizeSec *= 1e-6;
	MomentOfLastTimeAdjustment = GetTimeUSec ();
}

uint64_t timeSinceLastSntpTimeSyncUSec()
{
  return GetTimeUSec() - MomentOfLastTimeAdjustment;
}
//--------------------------------------------------------------------------//
float GetSNTPLastTimeAdjustmentSize (void)
{
	return (LastTimeAdjustmentSizeSec);
}
//--------------------------------------------------------------------------//
uint32_t GetSNTPTimeOfAdjustment (void)
{
	uint64_t ui64 = GetTimeUSec ();
	ui64 -= MomentOfLastTimeAdjustment;
	ui64 /= 1000000LL;
	return (ui64);
}
//--------------------------------------------------------------------------//
