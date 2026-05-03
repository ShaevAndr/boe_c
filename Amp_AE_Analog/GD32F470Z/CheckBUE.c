/*=============================================================================
  Project: 
  Platform: GD32F470Z
  Filename: ChecBOCv.c
  Description:
  Programmer: 
  Version: 0.0
  Created: 2024.06.28
  Last modified: 2024.06.28
=============================================================================*/
//#include <gd32f4xx.h>
//#include <stdint.h>
//#include <math.h>
#include <stdbool.h>
//------------------------------------------------------------------------------
#include "drv_time.h"
#include "drv_Monp.h"
//#include "drv_MonADC0.h"
#include "drv_MonADC2.h"
#include "drv_DS18B20.h"
#include "CheckBUE.h"
//--------------------------------------------------------------------------//
static StatusBUE_t StatusBUE = {.ui32 = 0};
//--------------------------------------------------------------------------//
StatusBUE_t GetStatusBUE (void)
{
	StatusBUE_t ret = StatusBUE;

	StatusBUE.st.Reserv0 = 0;
	StatusBUE.st.Reserv1= 0;
	StatusBUE.st.Reserv2 = 0;
	StatusBUE.st.Reserv3 = 0;
	StatusBUE.st.MonpRxError = 0;
	StatusBUE.st.IntVoltError = 0;
	StatusBUE.st.OneWireSensorError = 0;

	return (ret);
}
//--------------------------------------------------------------------------//
StatusBUE_t GetStatusBUENoCLR (void)
{
  return (StatusBUE);
}
//--------------------------------------------------------------------------//
static bool InRange (float Val, float Min, float Max)
{
	if (Val > Max) return (false);
	if (Val < Min) return (false);
	return (true);
}
//--------------------------------------------------------------------------//
static ErrorInterVolt_t ErrorInterVolt = {.ui32 = 0};
//--------------------------------------------------------------------------//
ErrorInterVolt_t GetStatusInternalVoltage (void)
{
	ErrorInterVolt_t ret = ErrorInterVolt;
	ErrorInterVolt.ui32 = 0;
	return (ret);
}
//--------------------------------------------------------------------------//
ErrorInterVolt_t GetStatusInternalVoltageNoCLR (void)
{
  return (ErrorInterVolt);
}
//--------------------------------------------------------------------------//
static bool AllInternalVoltageNormal (void)
{
	ErrorInterVolt_t TempErrorIV = {.ui32 = 0};
	if (!InRange (ResultDataMonADC2Min (_ADC2_p3_3Vd), 3.1f, 3.5f)) TempErrorIV.st.ADC2_p3_3Vd_Min = 1;
	if (!InRange (ResultDataMonADC2AVG (_ADC2_p3_3Vd), 3.1f, 3.5f)) TempErrorIV.st.ADC2_p3_3Vd_AVG = 1;
	if (!InRange (ResultDataMonADC2Max (_ADC2_p3_3Vd), 3.1f, 3.5f)) TempErrorIV.st.ADC2_p3_3Vd_Max = 1;
	if (!InRange (ResultDataMonADC2Min (_ADC2_p5Va), 4.8f, 5.3f)) 		TempErrorIV.st.ADC2_p5Va_Min = 1;
	if (!InRange (ResultDataMonADC2AVG (_ADC2_p5Va), 4.8f, 5.3f))			TempErrorIV.st.ADC2_p5Va_AVG = 1;
	if (!InRange (ResultDataMonADC2Max (_ADC2_p5Va), 4.8f, 5.3f))			TempErrorIV.st.ADC2_p5Va_Max = 1;
	if (!InRange (ResultDataMonADC2Min (_ADC2_p7V), 6.0f, 9.0f))			TempErrorIV.st.ADC2_p7V_Min = 1;
	if (!InRange (ResultDataMonADC2AVG (_ADC2_p7V), 6.0f, 9.0f))			TempErrorIV.st.ADC2_p7V_AVG = 1;
	if (!InRange (ResultDataMonADC2Max (_ADC2_p7V), 6.0f, 9.0f))			TempErrorIV.st.ADC2_p7V_Max = 1;
	if (!InRange (ResultDataMonADC2Min (_ADC2_p24V), 23.0f, 25.0f))		TempErrorIV.st.ADC2_p24V_Min = 1;
	if (!InRange (ResultDataMonADC2AVG (_ADC2_p24V), 23.5f, 25.0f))		TempErrorIV.st.ADC2_p24V_AVG = 1;
	if (!InRange (ResultDataMonADC2Max (_ADC2_p24V), 23.5f, 25.5f))		TempErrorIV.st.ADC2_p24V_Max = 1;
	if (!InRange (ResultDataMonADC2Min (_ADC2_p27V), 25.0f, 30.0f))		TempErrorIV.st.ADC2_p27V_Min = 1;
	if (!InRange (ResultDataMonADC2AVG (_ADC2_p27V), 26.0f, 30.0f))		TempErrorIV.st.ADC2_p27V_AVG = 1;
	if (!InRange (ResultDataMonADC2Max (_ADC2_p27V), 26.0f, 30.5f))		TempErrorIV.st.ADC2_p27V_Max = 1;
	if (!InRange (ResultDataMonADC2Min (_ADC2_Vocm), 2.1f, 2.9f))			TempErrorIV.st.ADC2_Vocm_Min = 1;
	if (!InRange (ResultDataMonADC2AVG (_ADC2_Vocm), 2.1f, 2.9f))			TempErrorIV.st.ADC2_Vocm_AVG = 1;
	if (!InRange (ResultDataMonADC2Max (_ADC2_Vocm), 2.1f, 2.9f))			TempErrorIV.st.ADC2_Vocm_Max = 1;
	if (!InRange (GetMonpParam (_Monp_Vcc_Min), 10.0f, 13.5f))				TempErrorIV.st.Monp_Vcc_Min = 1;
	if (!InRange (GetMonpParam (_Monp_Vcc_AVG), 10.0f, 13.5f))				TempErrorIV.st.Monp_Vcc_AVG = 1;
	if (!InRange (GetMonpParam (_Monp_Vcc_Max), 10.0f, 13.5f))				TempErrorIV.st.Monp_Vcc_Max = 1;
	if (!InRange (GetMonpParam (_Monp_3_3Vp_Min), 2.95f, 3.05f))			TempErrorIV.st.Monp_3_3Vp_Min = 1;
	if (!InRange (GetMonpParam (_Monp_3_3Vp_AVG), 2.95f, 3.05f))			TempErrorIV.st.Monp_3_3Vp_AVG = 1;
	if (!InRange (GetMonpParam (_Monp_3_3Vp_Max), 2.95f, 3.05f))			TempErrorIV.st.Monp_3_3Vp_Max = 1;
	TempErrorIV.st.Reserv0 = 0;
	
	ErrorInterVolt.ui32 |= TempErrorIV.ui32;
	
	if (TempErrorIV.ui32 == 0) return (true);
	else return (false);
}
//--------------------------------------------------------------------------//
static bool AllDS18B20Normal (void)
{
	int i;
	for (i = 0; i < _NumDS18B20; i++)
		if (DS18B20NormalWork (i) == false) return (false);
	return (true);
}
//--------------------------------------------------------------------------//
void RoutineCheckBUE (void)
{
	static bool InitFisrt = false;
	static bool Init = false;
	static bool InitVoltage = false;
	static TTime TOStatus = 0;
	static TTime TOVoltage = 0;
	static uint32_t MonpPassCount = 0;
	static uint32_t MonpErrorCount = 0;
	
	if (EndTime (TOStatus))
	{
		if (Init)
		{
			TOStatus = SetTime_ms (20000);
			if ((MonpPassPacketCount () - MonpPassCount) < 10) StatusBUE.st.MonpRxError = 1;
			if ((MonpErrorPacketCount () - MonpErrorCount) > 3) StatusBUE.st.MonpRxError = 1;
		}
		else
		{
			TOStatus = SetTime_ms (20100);
			StatusBUE.st.Reserv0 = 0;
			StatusBUE.st.Reserv1= 0;
			StatusBUE.st.Reserv2 = 0;
			StatusBUE.st.Reserv3 = 0;
			StatusBUE.st.MonpRxError = 0;
			StatusBUE.st.IntVoltError = 0;
			StatusBUE.st.OneWireSensorError = 0;
		}
		if (InitFisrt) Init = true;
		MonpPassCount = MonpPassPacketCount ();
		MonpErrorCount = MonpErrorPacketCount ();
	}
	if (EndTime (TOVoltage))
	{
		if (InitVoltage)
		{
			TOVoltage = SetTime_ms (930);
			if (!AllInternalVoltageNormal ()) StatusBUE.st.IntVoltError = 1;
			if (!AllDS18B20Normal ()) StatusBUE.st.OneWireSensorError = 1;
		}
		else
		{
			TOVoltage = SetTime_ms (4930);
			StatusBUE.st.IntVoltError = 0;
		}
		if (InitFisrt) InitVoltage = true;

	}
	InitFisrt = true;
}
//--------------------------------------------------------------------------//
