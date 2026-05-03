/*=============================================================================
  Project: 
  Platform: GD32F470Z
  Filename: ChecEnviron.c
  Description:
  Programmer: 
  Version: 0.0
  Created: 2024.06.28
  Last modified: 2024.06.28
=============================================================================*/
//#include <gd32f4xx.h>
//#include <stdint.h>
#include <math.h>
//------------------------------------------------------------------------------
#include "drv_time.h"
#include "drv_Monp.h"
#include "ParamApp.h"
#include "drv_ICP.h"
#include "CheckEnviron.h"
//--------------------------------------------------------------------------//
static Env_Status_t Env_Status = {.ui32 = 0};
//--------------------------------------------------------------------------//
Env_Status_t GetStatusEnvNoCLR (void)
{
	return (Env_Status);
}
//--------------------------------------------------------------------------//
Env_Status_t GetStatusEnv (void)
{
	Env_Status_t ret = Env_Status;
	
	Env_Status.st.ErrorICP = 0;
	Env_Status.st.ErrorPowUin = 0;
	Env_Status.st.ErrorTemp = 0;
	Env_Status.st.Reserv0 = 0;
	Env_Status.st.Reserv1 = 0;
	Env_Status.st.Reserv2 = 0;

	return (ret);
}
//--------------------------------------------------------------------------//
#define _MaxValuePowerUIn (29.0)
#define _MinValuePowerUIn (19.0)
//--------------------------------------------------------------------------//
static bool GoodPowerUIn (void)
{
	float f;
	f = GetMonpParam (_Monp_Up_AVG);
	if (f > _MaxValuePowerUIn) return (false);
	if (f < _MinValuePowerUIn) return (false);
	f = GetMonpParam (_Monp_Up_Min);
	if (f > _MaxValuePowerUIn) return (false);
	if (f < _MinValuePowerUIn) return (false);
	f = GetMonpParam (_Monp_Up_Max);
	if (f > _MaxValuePowerUIn) return (false);
	if (f < _MinValuePowerUIn) return (false);
	return (true);
}
//--------------------------------------------------------------------------//
#define _MaxValueBOCvTemp (90.0)
#define _MinValueBOCvTemp (0.0)
//--------------------------------------------------------------------------//
static bool GoodTemp (void)
{
	float f;
	int i;
	for (i = 0; i < _NumDS18B20; i++)
	{
		f = DS18B20_GetTemperature (i);
		if (!isfinite (f)) continue;
//		if (f == NAN) continue;
//		if (!(f < +INFINITY)) continue;
//		if (!(f > -INFINITY)) continue;
		if (f > _MaxValueBOCvTemp) return (false);
		if (f < _MinValueBOCvTemp) return (false);
	}
	return (true);
}
//--------------------------------------------------------------------------//
static bool GoodStatusICP (void)
{
	if (GetOPPowerState () == GetICPDisablePower ()) return (true);
	else return (false);
}
//--------------------------------------------------------------------------//
static void RoutineEnv (void)
{
	if (!GoodStatusICP ()) Env_Status.st.ErrorICP = 1;
	if (!GoodPowerUIn ()) Env_Status.st.ErrorPowUin = 1;
	if (!GoodTemp ()) Env_Status.st.ErrorTemp = 1;

	Env_Status.st.Reserv0 = 0;
	Env_Status.st.Reserv1 = 0;
	Env_Status.st.Reserv2 = 0;
}
//--------------------------------------------------------------------------//
void CheckEnviron_Routine (void)
{
	static TTime EnvTO = 0;
	static bool Init = false;
	
	if (!Init)
	{
		EnvTO = SetTime_ms (15000);
		Init = true;
		return;
	}

	if (EndTime (EnvTO))
	{
		RoutineEnv ();
		EnvTO = SetTime_ms (520);
	}
}
//--------------------------------------------------------------------------//
