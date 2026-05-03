/*=============================================================================
2     Project: 
3     Platform: GD32F407
4     Filename: AcessTelemParam.c
5     Description:
6     Version: 0.0
7     Created: 2025.01.28
8     Last modified: 2025.01.28
9============================================================================*/
#include <stdint.h>
#include <string.h>
#include <stdio.h>
//--------------------------------------------------------------------------//
#include "drv_time.h"
#include "../CheckBUE.h"
#include "../drv_MonADC0.h"
#include "../drv_MonADC1.h"
#include "../drv_MonADC2.h"
#include "../drv_DS18B20.h"
#include "../drv_Monp.h"
#include "../CheckEnviron.h"
#include "../drv_ICP.h"
#include "../drv_DIn.h"
//--------------------------------------------------------------------------//
#include "CommandList.h"
#include "CommandParser.h"
#include "AccessTelemParam.h"
//--------------------------------------------------------------------------//
int8_t AccessTelemParam (TelimParam_t NumParam, float * Value)
{
	int8_t Ret = _NoError;
	switch (NumParam)
	{
		case _Tel_ADC_AvgP0:		*Value = ResultDataMonADC1AVG (_ADC1_Monp1);	break;
		case _Tel_ADC_AvgN0:		*Value = ResultDataMonADC1AVG (_ADC1_Monn1);	break;
		case _Tel_ADC_AvgP1:		*Value = ResultDataMonADC1AVG (_ADC1_Monp2);	break;
		case _Tel_ADC_AvgN1:		*Value = ResultDataMonADC1AVG (_ADC1_Monn2);	break;
		case _Tel_ADC_AvgP2:		*Value = ResultDataMonADC1AVG (_ADC1_Monp3);	break;
		case _Tel_ADC_AvgN2:		*Value = ResultDataMonADC1AVG (_ADC1_Monn3);	break;
		case _Tel_ADC_AvgP3:		*Value = ResultDataMonADC1AVG (_ADC1_Monp4);	break;
		case _Tel_ADC_AvgN3:		*Value = ResultDataMonADC1AVG (_ADC1_Monn4);	break;
		case _Tel_ADC_AvgP4:		*Value = ResultDataMonADC1AVG (_ADC1_Monp5);	break;
		case _Tel_ADC_AvgN4:		*Value = ResultDataMonADC1AVG (_ADC1_Monn5);	break;
		case _Tel_ADC_AvgP5:		*Value = ResultDataMonADC1AVG (_ADC1_Monp6);	break;
		case _Tel_ADC_AvgN5:		*Value = ResultDataMonADC1AVG (_ADC1_Monn6);	break;
		case _Tel_ADC_AvgP6:		*Value = ResultDataMonADC1AVG (_ADC1_Monp7);	break;
		case _Tel_ADC_AvgN6:		*Value = ResultDataMonADC1AVG (_ADC1_Monn7);	break;
		case _Tel_ADC_AvgP7:		*Value = ResultDataMonADC1AVG (_ADC1_Monp8);	break;
		case _Tel_ADC_AvgN7:		*Value = ResultDataMonADC1AVG (_ADC1_Monn8);	break;
		
		case _Tel_ADC_Max3P3:		*Value = ResultDataMonADC2Max (_ADC2_p3_3Vd);	break;
		case _Tel_ADC_Min3P3:		*Value = ResultDataMonADC2Min (_ADC2_p3_3Vd);	break;
		case _Tel_ADC_Avg3P3:		*Value = ResultDataMonADC2AVG (_ADC2_p3_3Vd);	break;
		case _Tel_ADC_MaxN5Va:	*Value = -5.0f;																break;
		case _Tel_ADC_MinN5Va:	*Value = -5.0f;																break;
		case _Tel_ADC_AvgN5Va:	*Value = -5.0f;																break;
		case _Tel_ADC_MaxP5Va:	*Value = ResultDataMonADC2Max (_ADC2_p5Va);		break;
		case _Tel_ADC_MinP5Va:	*Value = ResultDataMonADC2Min (_ADC2_p5Va);		break;
		case _Tel_ADC_AvgP5Va:	*Value = ResultDataMonADC2AVG (_ADC2_p5Va);		break;
		case _Tel_ADC_MaxP24Va:	*Value = ResultDataMonADC2Max (_ADC2_p24V);		break;
		case _Tel_ADC_MinP24Va:	*Value = ResultDataMonADC2Min (_ADC2_p24V);		break;
		case _Tel_ADC_AvgP24Va:	*Value = ResultDataMonADC2AVG (_ADC2_p24V);		break;
		case _Tel_Temp_Avg:			*Value = ResultDataMonADC0AVG (_ADC0_Temp);		break;
		case _Tel_Sync:					*Value = 800.0f;															break;
		case _Tel_UpTime:				*Value = fGetTime ();													break;
		case _Tel_PG:						*Value = 1.0f;																break;
		case _Tel_Power_OP_Log:	*Value = GetOPPowerState ();									break;
		
		case _Tel_MSP1_UpTime:	*Value = (float)(GetMonpUpTime ());						break;
		case _Tel_MSP1_Max_Cur:	*Value = GetMonpParam (_Monp_Iin_Max);				break;
		case _Tel_MSP1_Min_Cur:	*Value = GetMonpParam (_Monp_Iin_Min);				break;
		case _Tel_MSP1_Avg_Cur:	*Value = GetMonpParam (_Monp_Iin_AVG);				break;
		case _Tel_MSP1_Up_Max:	*Value = GetMonpParam (_Monp_Up_Max);					break;
		case _Tel_MSP1_Up_Min:	*Value = GetMonpParam (_Monp_Up_Min);					break;
		case _Tel_MSP1_Up_AVG:	*Value = GetMonpParam (_Monp_Up_AVG);					break;
		case _Tel_MSP1_VCC_Max:	*Value = GetMonpParam (_Monp_Vcc_Max);				break;
		case _Tel_MSP1_VCC_Min:	*Value = GetMonpParam (_Monp_Vcc_Min);				break;
		case _Tel_MSP1_VCC_AVG:	*Value = GetMonpParam (_Monp_Vcc_AVG);				break;
		case _Tel_MSP1_Pow_Max:	*Value = GetMonpParam (_Monp_Pow_Max);				break;
		case _Tel_MSP1_Pow_Min:	*Value = GetMonpParam (_Monp_Pow_Min);				break;
		case _Tel_MSP1_Pow_AVG:	*Value = GetMonpParam (_Monp_Pow_AVG);				break;
		case _Tel_MSP1_Temp_AVG:*Value = GetMonpParam (_Monp_TempSensor);			break;
		case _Tel_MSP2_UpTime:	*Value = 0.0f;																break;
		case _Tel_MSP2_Max_Cur:	*Value = 0.0f;																break;
		case _Tel_MSP2_Min_Cur:	*Value = 0.0f;																break;
		case _Tel_MSP2_Avg_Cur:	*Value = 0.0f;																break;
		case _Tel_MSP2_Up_Max:	*Value = 0.0f;																break;
		case _Tel_MSP2_Up_Min:	*Value = 0.0f;																break;
		case _Tel_MSP2_Up_AVG:	*Value = 0.0f;																break;
		case _Tel_MSP2_VCC_Max:	*Value = 0.0f;																break;
		case _Tel_MSP2_VCC_Min:	*Value = 0.0f;																break;
		case _Tel_MSP2_VCC_AVG:	*Value = 0.0f;																break;
		case _Tel_MSP2_Pow_Max:	*Value = 0.0f;																break;
		case _Tel_MSP2_Pow_Min:	*Value = 0.0f;																break;
		case _Tel_MSP2_Pow_AVG:	*Value = 0.0f;																break;
		case _Tel_MSP2_Temp_AVG:*Value = 0.0f;																break;
		
		case _TelMonStateDIn:		*Value = (float)GetStateDInsU32 ();						break;
		case _TelMonChangeDIn:	*Value = (float)GetChangeDInsU32 ();					break;
		case _Tel_Mona_FreqDIn1:*Value = GetFrequencyDIn (_DIn1);							break;
		case _Tel_Mona_FreqDIn2:*Value = GetFrequencyDIn (_DIn2);							break;

		case _TelStatusBUE:			*Value = (float)(GetStatusBUENoCLR ().ui32);	break;
		case _TelStatusEnv:			*Value = (float)(GetStatusEnvNoCLR ().ui32);	break;
		case _TelStatusInterVolt:*Value = (float)(GetStatusInternalVoltageNoCLR ().ui32);break;

		case _Tel_Monp_ErrorPacketCount:*Value = MonpErrorPacketCount ();			break;
		case _Tel_Monp_PassPacketCount:	*Value = MonpPassPacketCount ();			break;

		case _Tel_DSCore_Temp:	*Value = DS18B20_GetTempLocation (_DSLocCore);break;
		case _Tel_DSIP_Temp:		*Value = DS18B20_GetTempLocation (_DSLocIP);	break;

		case _Tel_DS0_Temp:			*Value = DS18B20_GetTemperature (0);					break;
		case _Tel_DS1_Temp:			*Value = DS18B20_GetTemperature (1);					break;
		
		case _Tel_ADC_Maxp7V:		*Value = ResultDataMonADC2Max (_ADC2_p7V);		break;
		case _Tel_ADC_Minp7V:		*Value = ResultDataMonADC2Min (_ADC2_p7V);		break;
		case _Tel_ADC_Avgp7V:		*Value = ResultDataMonADC2AVG (_ADC2_p7V);		break;
		case _Tel_ADC_Maxp27V:	*Value = ResultDataMonADC2Max (_ADC2_p27V);		break;
		case _Tel_ADC_Minp27V:	*Value = ResultDataMonADC2Min (_ADC2_p27V);		break;
		case _Tel_ADC_Avgp27V:	*Value = ResultDataMonADC2AVG (_ADC2_p27V);		break;
		case _Tel_ADC_MaxVocm:	*Value = ResultDataMonADC2Max (_ADC2_Vocm);		break;
		case _Tel_ADC_MinVocm:	*Value = ResultDataMonADC2Min (_ADC2_Vocm);		break;
		case _Tel_ADC_AvgVocm:	*Value = ResultDataMonADC2AVG (_ADC2_Vocm);		break;

		default: Ret = _ErrorUnCorrParam; 																		break;
	}
	return (Ret);
}
//--------------------------------------------------------------------------//
int8_t AccessDescTelemParam (TelimParam_t NumParam, DescriptParam_t * P)
{
	int8_t Ret = _NoError;
	strncpy (P->name, "", sizeof (P->name));
	strncpy (P->unit, "", sizeof (P->unit));
	strncpy (P->formatStr, "", sizeof (P->formatStr));
	strncpy (P->uniqueId, "", sizeof (P->uniqueId));
	strncpy (P->access, "RO", sizeof (P->access));
	P->minValuef = 0.0f;
	P->maxValuef = 0.0f;
	
	switch (NumParam)
	{
		case _Tel_ADC_AvgP0: case _Tel_ADC_AvgP1: case _Tel_ADC_AvgP2:
		case _Tel_ADC_AvgP3: case _Tel_ADC_AvgP4: case _Tel_ADC_AvgP5:
		case _Tel_ADC_AvgP6: case _Tel_ADC_AvgP7:
			snprintf (P->name, sizeof (P->name), "ADC_Avg_MonP%d", (int)(NumParam >> 1));
			strncpy (P->unit, "V", sizeof (P->unit));
			strncpy (P->formatStr, "%.2f", sizeof (P->formatStr));
			snprintf (P->uniqueId, sizeof (P->uniqueId), "ADC_AvgP%d", (int)(NumParam >> 1));
			strncpy (P->access, "", sizeof (P->access));
			P->minValuef = 14.0f;
			P->maxValuef = 20.0f;
			break;
		case _Tel_ADC_AvgN0: case _Tel_ADC_AvgN1: case _Tel_ADC_AvgN2:
		case _Tel_ADC_AvgN3: case _Tel_ADC_AvgN4: case _Tel_ADC_AvgN5:
		case _Tel_ADC_AvgN6: case _Tel_ADC_AvgN7:
			snprintf (P->name, sizeof (P->name), "ADC_Avg _MonN%d", (int)(NumParam >> 1));
			strncpy (P->unit, "V", sizeof (P->unit));
			strncpy (P->formatStr, "%.2f", sizeof (P->formatStr));
			snprintf (P->uniqueId, sizeof (P->uniqueId), "ADC_AvgN%d", (int)(NumParam >> 1));
			strncpy (P->access, "", sizeof (P->access));
			P->minValuef = 4.0f;
			P->maxValuef = 10.0f;
			break;
		case _Tel_ADC_Max3P3:
			strncpy (P->name, "ADC_Max_Mon_3P3VD", sizeof (P->name));
			strncpy (P->unit, "V", sizeof (P->unit));
			strncpy (P->formatStr, "%.2f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "ADC_Max3P3", sizeof (P->uniqueId));
			P->minValuef = 3.1f;
			P->maxValuef = 3.5f;
			break;
		case _Tel_ADC_Min3P3:
			strncpy (P->name, "ADC_Min_Mon_3P3VD", sizeof (P->name));
			strncpy (P->unit, "V", sizeof (P->unit));
			strncpy (P->formatStr, "%.2f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "ADC_Min3P3", sizeof (P->uniqueId));
			P->minValuef = 3.1f;
			P->maxValuef = 3.5f;
			break;
		case _Tel_ADC_Avg3P3:
			strncpy (P->name, "ADC_Avg_Mon_3P3VD", sizeof (P->name));
			strncpy (P->unit, "V", sizeof (P->unit));
			strncpy (P->formatStr, "%.2f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "ADC_Avg3P3", sizeof (P->uniqueId));
			P->minValuef = 3.1f;
			P->maxValuef = 3.5f;
			break;
		case _Tel_ADC_MaxN5Va:
			strncpy (P->name, "ADC_Max_MonN5Va", sizeof (P->name));
			strncpy (P->unit, "V", sizeof (P->unit));
			strncpy (P->formatStr, "%.2f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "ADC_MaxN5Va", sizeof (P->uniqueId));
			P->minValuef = -5.3f;
			P->maxValuef = -4.7f;
			break;
		case _Tel_ADC_MinN5Va:
			strncpy (P->name, "ADC_Min_MonN5Va", sizeof (P->name));
			strncpy (P->unit, "V", sizeof (P->unit));
			strncpy (P->formatStr, "%.2f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "ADC_MinN5Va", sizeof (P->uniqueId));
			P->minValuef = -5.3f;
			P->maxValuef = -4.7f;
			break;
		case _Tel_ADC_AvgN5Va:
			strncpy (P->name, "ADC_Avg_MonN5Va", sizeof (P->name));
			strncpy (P->unit, "V", sizeof (P->unit));
			strncpy (P->formatStr, "%.2f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "ADC_AvgN5Va", sizeof (P->uniqueId));
			P->minValuef = -5.3f;
			P->maxValuef = -4.7f;
			break;
		case _Tel_ADC_MaxP5Va:
			strncpy (P->name, "ADC_Max_MonP5Va", sizeof (P->name));
			strncpy (P->unit, "V", sizeof (P->unit));
			strncpy (P->formatStr, "%.2f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "ADC_MaxP5Va", sizeof (P->uniqueId));
			P->minValuef = 4.7f;
			P->maxValuef = 5.3f;
			break;
		case _Tel_ADC_MinP5Va:
			strncpy (P->name, "ADC_Min_MonP5Va", sizeof (P->name));
			strncpy (P->unit, "V", sizeof (P->unit));
			strncpy (P->formatStr, "%.2f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "ADC_MinP5Va", sizeof (P->uniqueId));
			P->minValuef = 4.7f;
			P->maxValuef = 5.3f;
			break;
		case _Tel_ADC_AvgP5Va:
			strncpy (P->name, "ADC_Avg_MonP5Va", sizeof (P->name));
			strncpy (P->unit, "V", sizeof (P->unit));
			strncpy (P->formatStr, "%.2f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "ADC_AvgP5Va", sizeof (P->uniqueId));
			P->minValuef = 4.7f;
			P->maxValuef = 5.3f;
			break;
		case _Tel_ADC_MaxP24Va:
			strncpy (P->name, "ADC_Max_MonP24Va", sizeof (P->name));
			strncpy (P->unit, "V", sizeof (P->unit));
			strncpy (P->formatStr, "%.1f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "ADC_MaxP24Va", sizeof (P->uniqueId));
			P->minValuef = 24.0f;
			P->maxValuef = 24.5f;
			break;
		case _Tel_ADC_MinP24Va:
			strncpy (P->name, "ADC_Min_MonP24Va", sizeof (P->name));
			strncpy (P->unit, "V", sizeof (P->unit));
			strncpy (P->formatStr, "%.1f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "ADC_MinP24Va", sizeof (P->uniqueId));
			P->minValuef = 24.0f;
			P->maxValuef = 24.5f;
			break;
		case _Tel_ADC_AvgP24Va:
			strncpy (P->name, "ADC_Avg_MonP24Va", sizeof (P->name));
			strncpy (P->unit, "V", sizeof (P->unit));
			strncpy (P->formatStr, "%.1f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "ADC_AvgP24Va", sizeof (P->uniqueId));
			P->minValuef = 24.0f;
			P->maxValuef = 24.5f;
			break;
		case _Tel_Temp_Avg:
			strncpy (P->name, "Temp_Avg", sizeof (P->name));
			strncpy (P->unit, "С", sizeof (P->unit));
			strncpy (P->formatStr, "%.1f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "Temp_Avg", sizeof (P->uniqueId));
			P->minValuef = -40.0f;
			P->maxValuef = 85.0f;
			break;
		case _Tel_Sync:
			strncpy (P->name, "Sync", sizeof (P->name));
			strncpy (P->unit, "kHz", sizeof (P->unit));
			strncpy (P->formatStr, "%.0f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "Sync", sizeof (P->uniqueId));
			P->minValuef = 700.0f;
			P->maxValuef = 800.0f;
			break;
		case _Tel_UpTime:
			strncpy (P->name, "UpTime", sizeof (P->name));
			strncpy (P->unit, "s", sizeof (P->unit));
			strncpy (P->formatStr, "%.1f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "UpTime", sizeof (P->uniqueId));
			P->minValuef = 0.0f;
			P->maxValuef = 1.84467440737E+19f;
			break;
		case _Tel_PG:
			strncpy (P->name, "PG", sizeof (P->name));
			strncpy (P->unit, "_", sizeof (P->unit));
			strncpy (P->formatStr, "%.0f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "PG", sizeof (P->uniqueId));
			P->minValuef = 0.0f;
			P->maxValuef = 3.0f;
			break;
		case _Tel_Power_OP_Log:
			strncpy (P->name, "Power_OP_Log", sizeof (P->name));
			strncpy (P->unit, "_", sizeof (P->unit));
			strncpy (P->formatStr, "%.0f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "Power_OP_Log", sizeof (P->uniqueId));
			P->minValuef = 0.0f;
			P->maxValuef = 0.0f;
			break;
		case _Tel_MSP1_UpTime:
			strncpy (P->name, "MSP1_UpTime", sizeof (P->name));
			strncpy (P->unit, "s", sizeof (P->unit));
			strncpy (P->formatStr, "%.0f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "MSP1_UpTime", sizeof (P->uniqueId));
			P->minValuef = 0.0f;
			P->maxValuef = 1.84467440737E+19f;
			break;
		case _Tel_MSP1_Max_Cur:
			strncpy (P->name, "MSP1_Max_Current", sizeof (P->name));
			strncpy (P->unit, "A", sizeof (P->unit));
			strncpy (P->formatStr, "%.3f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "MSP1_Max_Cur", sizeof (P->uniqueId));
			P->minValuef = 0.0f;
			P->maxValuef = 0.4f;
			break;
		case _Tel_MSP1_Min_Cur:
			strncpy (P->name, "MSP1_Min_Current", sizeof (P->name));
			strncpy (P->unit, "A", sizeof (P->unit));
			strncpy (P->formatStr, "%.3f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "MSP1_Min_Cur", sizeof (P->uniqueId));
			P->minValuef = 0.0f;
			P->maxValuef = 0.4f;
			break;
		case _Tel_MSP1_Avg_Cur:
			strncpy (P->name, "MSP1_Avg_Current", sizeof (P->name));
			strncpy (P->unit, "A", sizeof (P->unit));
			strncpy (P->formatStr, "%.3f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "MSP1_Avg_Cur", sizeof (P->uniqueId));
			P->minValuef = 0.0f;
			P->maxValuef = 0.4f;
			break;
		case _Tel_MSP1_Up_Max:
			strncpy (P->name, "MSP1_Max_Up", sizeof (P->name));
			strncpy (P->unit, "V", sizeof (P->unit));
			strncpy (P->formatStr, "%.1f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "MSP1_Up_Max", sizeof (P->uniqueId));
			P->minValuef = 19.0f;
			P->maxValuef = 29.0f;
			break;
		case _Tel_MSP1_Up_Min:
			strncpy (P->name, "MSP1_Min_Up", sizeof (P->name));
			strncpy (P->unit, "V", sizeof (P->unit));
			strncpy (P->formatStr, "%.1f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "MSP1_Up_Min", sizeof (P->uniqueId));
			P->minValuef = 19.0f;
			P->maxValuef = 29.0f;
			break;
		case _Tel_MSP1_Up_AVG:
			strncpy (P->name, "MSP1_Avg_Up", sizeof (P->name));
			strncpy (P->unit, "V", sizeof (P->unit));
			strncpy (P->formatStr, "%.1f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "MSP1_Up_AVG", sizeof (P->uniqueId));
			P->minValuef = 19.0f;
			P->maxValuef = 29.0f;
			break;
		case _Tel_MSP1_VCC_Max:
			strncpy (P->name, "MSP1_Max_VCC", sizeof (P->name));
			strncpy (P->unit, "V", sizeof (P->unit));
			strncpy (P->formatStr, "%.2f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "MSP1_VCC_Max", sizeof (P->uniqueId));
			P->minValuef = 10.0f;
			P->maxValuef = 13.5f;
			break;
		case _Tel_MSP1_VCC_Min:
			strncpy (P->name, "MSP1_Min_VCC", sizeof (P->name));
			strncpy (P->unit, "V", sizeof (P->unit));
			strncpy (P->formatStr, "%.2f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "MSP1_VCC_Min", sizeof (P->uniqueId));
			P->minValuef = 10.0f;
			P->maxValuef = 13.5f;
			break;
		case _Tel_MSP1_VCC_AVG:
			strncpy (P->name, "MSP1_Avg_VCC", sizeof (P->name));
			strncpy (P->unit, "V", sizeof (P->unit));
			strncpy (P->formatStr, "%.2f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "MSP1_VCC_AVG", sizeof (P->uniqueId));
			P->minValuef = 10.0f;
			P->maxValuef = 13.5f;
			break;
		case _Tel_MSP1_Pow_Max:
			strncpy (P->name, "MSP1_Max_Pow", sizeof (P->name));
			strncpy (P->unit, "W", sizeof (P->unit));
			strncpy (P->formatStr, "%.1f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "MSP1_Pow_Max", sizeof (P->uniqueId));
			P->minValuef = 0.0f;
			P->maxValuef = 7.0f;
			break;
		case _Tel_MSP1_Pow_Min:
			strncpy (P->name, "MSP1_Min_Pow", sizeof (P->name));
			strncpy (P->unit, "W", sizeof (P->unit));
			strncpy (P->formatStr, "%.1f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "MSP1_Pow_Min", sizeof (P->uniqueId));
			P->minValuef = 0.0f;
			P->maxValuef = 7.0f;
			break;
		case _Tel_MSP1_Pow_AVG:
			strncpy (P->name, "MSP1_Avg_Pow", sizeof (P->name));
			strncpy (P->unit, "W", sizeof (P->unit));
			strncpy (P->formatStr, "%.1f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "MSP1_Pow_AVG", sizeof (P->uniqueId));
			P->minValuef = 0.0f;
			P->maxValuef = 7.0f;
			break;
		case _Tel_MSP1_Temp_AVG:
			strncpy (P->name, "MSP1_Avg_Temp", sizeof (P->name));
			strncpy (P->unit, "C", sizeof (P->unit));
			strncpy (P->formatStr, "%.1f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "MSP1_Temp_AVG", sizeof (P->uniqueId));
			P->minValuef = -40.0f;
			P->maxValuef = 85.0f;
			break;
		case _Tel_MSP2_UpTime:
			strncpy (P->name, "MSP2_UpTime", sizeof (P->name));
			strncpy (P->unit, "s", sizeof (P->unit));
			strncpy (P->formatStr, "%.0f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "MSP2_UpTime", sizeof (P->uniqueId));
			P->minValuef = 0.0f;
			P->maxValuef = 1.84467440737E+19f;
			break;
		case _Tel_MSP2_Max_Cur:
			strncpy (P->name, "MSP2_Max_Current", sizeof (P->name));
			strncpy (P->unit, "A", sizeof (P->unit));
			strncpy (P->formatStr, "%.3f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "MSP2_Max_Cur", sizeof (P->uniqueId));
			P->minValuef = 0.0f;
			P->maxValuef = 0.4f;
			break;
		case _Tel_MSP2_Min_Cur:
			strncpy (P->name, "MSP2_Min_Current", sizeof (P->name));
			strncpy (P->unit, "A", sizeof (P->unit));
			strncpy (P->formatStr, "%.3f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "MSP2_Min_Cur", sizeof (P->uniqueId));
			P->minValuef = 0.0f;
			P->maxValuef = 0.4f;
			break;
		case _Tel_MSP2_Avg_Cur:
			strncpy (P->name, "MSP2_Avg_Current", sizeof (P->name));
			strncpy (P->unit, "A", sizeof (P->unit));
			strncpy (P->formatStr, "%.3f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "MSP2_Avg_Cur", sizeof (P->uniqueId));
			P->minValuef = 0.0f;
			P->maxValuef = 0.4f;
			break;
		case _Tel_MSP2_Up_Max:
			strncpy (P->name, "MSP2_Max_Up", sizeof (P->name));
			strncpy (P->unit, "V", sizeof (P->unit));
			strncpy (P->formatStr, "%.1f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "MSP2_Up_Max", sizeof (P->uniqueId));
			P->minValuef = 18.0f;
			P->maxValuef = 30.0f;
			break;
		case _Tel_MSP2_Up_Min:
			strncpy (P->name, "MSP2_Min_Up", sizeof (P->name));
			strncpy (P->unit, "V", sizeof (P->unit));
			strncpy (P->formatStr, "%.1f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "MSP2_Up_Min", sizeof (P->uniqueId));
			P->minValuef = 18.0f;
			P->maxValuef = 30.0f;
			break;
		case _Tel_MSP2_Up_AVG:
			strncpy (P->name, "MSP2_Avg_Up", sizeof (P->name));
			strncpy (P->unit, "V", sizeof (P->unit));
			strncpy (P->formatStr, "%.1f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "MSP2_Up_AVG", sizeof (P->uniqueId));
			P->minValuef = 18.0f;
			P->maxValuef = 30.0f;
			break;
		case _Tel_MSP2_VCC_Max:
			strncpy (P->name, "MSP2_Max_VCC", sizeof (P->name));
			strncpy (P->unit, "V", sizeof (P->unit));
			strncpy (P->formatStr, "%.2f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "MSP2_VCC_Max", sizeof (P->uniqueId));
			P->minValuef = 7.0f;
			P->maxValuef = 8.0f;
			break;
		case _Tel_MSP2_VCC_Min:
			strncpy (P->name, "MSP2_Min_VCC", sizeof (P->name));
			strncpy (P->unit, "V", sizeof (P->unit));
			strncpy (P->formatStr, "%.2f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "MSP2_VCC_Min", sizeof (P->uniqueId));
			P->minValuef = 7.0f;
			P->maxValuef = 8.0f;
			break;
		case _Tel_MSP2_VCC_AVG:
			strncpy (P->name, "MSP2_Avg_VCC", sizeof (P->name));
			strncpy (P->unit, "V", sizeof (P->unit));
			strncpy (P->formatStr, "%.2f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "MSP2_VCC_AVG", sizeof (P->uniqueId));
			P->minValuef = 7.0f;
			P->maxValuef = 8.0f;
			break;
		case _Tel_MSP2_Pow_Max:
			strncpy (P->name, "MSP2_Max_Pow", sizeof (P->name));
			strncpy (P->unit, "W", sizeof (P->unit));
			strncpy (P->formatStr, "%.1f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "MSP2_Pow_Max", sizeof (P->uniqueId));
			P->minValuef = 0.0f;
			P->maxValuef = 7.0f;
			break;
		case _Tel_MSP2_Pow_Min:
			strncpy (P->name, "MSP2_Min_Pow", sizeof (P->name));
			strncpy (P->unit, "W", sizeof (P->unit));
			strncpy (P->formatStr, "%.1f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "MSP2_Pow_Min", sizeof (P->uniqueId));
			P->minValuef = 0.0f;
			P->maxValuef = 7.0f;
			break;
		case _Tel_MSP2_Pow_AVG:
			strncpy (P->name, "MSP2_Avg_Pow", sizeof (P->name));
			strncpy (P->unit, "W", sizeof (P->unit));
			strncpy (P->formatStr, "%.1f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "MSP2_Pow_AVG", sizeof (P->uniqueId));
			P->minValuef = 0.0f;
			P->maxValuef = 7.0f;
			break;
		case _Tel_MSP2_Temp_AVG:
			strncpy (P->name, "MSP2_Avg_Temp", sizeof (P->name));
			strncpy (P->unit, "C", sizeof (P->unit));
			strncpy (P->formatStr, "%.1f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "MSP2_Temp_AVG", sizeof (P->uniqueId));
			P->minValuef = -40.0f;
			P->maxValuef = 85.0f;
			break;

		case _TelMonStateDIn:
			strncpy (P->name, "TelMonStateDIn", sizeof (P->name));
			strncpy (P->formatStr, "%.0f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "State DIn", sizeof (P->uniqueId));
			P->minValuei = 0;
			P->maxValuei = 3;
			break;
		case _TelMonChangeDIn:
			strncpy (P->name, "TelMonChangeDIn", sizeof (P->name));
			strncpy (P->formatStr, "%.0f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "Change DIn", sizeof (P->uniqueId));
			P->minValuei = 0;
			P->maxValuei = 3;
			break;
		case _Tel_Mona_FreqDIn1:
			strncpy (P->name, "Mona_FreqDIn1", sizeof (P->name));
			strncpy (P->formatStr, "%.1f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "Freq DIn1", sizeof (P->uniqueId));
			P->minValuei = 0;
			P->maxValuei = 1000;
			break;
		case _Tel_Mona_FreqDIn2:
			strncpy (P->name, "Mona_FreqDIn2", sizeof (P->name));
			strncpy (P->formatStr, "%.1f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "Freq DIn2", sizeof (P->uniqueId));
			P->minValuei = 0;
			P->maxValuei = 1000;
			break;

		case _TelStatusBUE:
			strncpy (P->name, "StatusBUE", sizeof (P->name));
			strncpy (P->unit, "_", sizeof (P->unit));
			strncpy (P->formatStr, "%.0f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "TelStatusBUE", sizeof (P->uniqueId));
			P->minValuef = -0.05f;
			P->maxValuef = 0.05f;
			break;
		case _TelStatusEnv:
			strncpy (P->name, "StatusEnv", sizeof (P->name));
			strncpy (P->unit, "_", sizeof (P->unit));
			strncpy (P->formatStr, "%.0f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "TelStatusEnv", sizeof (P->uniqueId));
			P->minValuef = -0.05f;
			P->maxValuef = 0.05f;
			break;
		case _TelStatusInterVolt:
			strncpy (P->name, "StatusInternalVoltage", sizeof (P->name));
			strncpy (P->unit, "_", sizeof (P->unit));
			strncpy (P->formatStr, "%.0f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "TelStatusInterVolt", sizeof (P->uniqueId));
			P->minValuef = -0.05f;
			P->maxValuef = 0.05f;
			break;

		case _Tel_Monp_ErrorPacketCount:
			strncpy (P->name, "Monp_ErrorPacketCount", sizeof (P->name));
			strncpy (P->unit, "_", sizeof (P->unit));
			strncpy (P->formatStr, "%.0f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "MonpErrorPacketCount", sizeof (P->uniqueId));
			P->minValuef = 0.0f;
			P->maxValuef = 1.84467440737E+19f;
			break;
		case _Tel_Monp_PassPacketCount:
			strncpy (P->name, "Monp_PassPacketCount", sizeof (P->name));
			strncpy (P->unit, "_", sizeof (P->unit));
			strncpy (P->formatStr, "%.0f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "MonpPassPacketCount", sizeof (P->uniqueId));
			P->minValuef = 0.0f;
			P->maxValuef = 1.84467440737E+19f;//7.0
			break;

		case _Tel_DSCore_Temp:
			strncpy (P->name, "DSCore_Temp", sizeof (P->name));
			strncpy (P->unit, "C", sizeof (P->unit));
			strncpy (P->formatStr, "%.1f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "DSCoreTemp", sizeof (P->uniqueId));
			P->minValuef = -40.0f;
			P->maxValuef = 85.0f;
			break;
		case _Tel_DSIP_Temp:
			strncpy (P->name, "DSIP_Temp", sizeof (P->name));
			strncpy (P->unit, "C", sizeof (P->unit));
			strncpy (P->formatStr, "%.1f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "DSIPTemp", sizeof (P->uniqueId));
			P->minValuef = -40.0f;
			P->maxValuef = 85.0f;
			break;

		case _Tel_DS0_Temp:
			strncpy (P->name, "DS0_Temp", sizeof (P->name));
			strncpy (P->unit, "C", sizeof (P->unit));
			strncpy (P->formatStr, "%.1f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "DS0Temp", sizeof (P->uniqueId));
			P->minValuef = -40.0f;
			P->maxValuef = 85.0f;
			break;
		case _Tel_DS1_Temp:
			strncpy (P->name, "DS1_Temp", sizeof (P->name));
			strncpy (P->unit, "C", sizeof (P->unit));
			strncpy (P->formatStr, "%.1f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "DS1Temp", sizeof (P->uniqueId));
			P->minValuef = -40.0f;
			P->maxValuef = 85.0f;
			break;
		
		case _Tel_ADC_Maxp7V:
			strncpy (P->name, "ADC_Max_MonP7V", sizeof (P->name));
			strncpy (P->unit, "V", sizeof (P->unit));
			strncpy (P->formatStr, "%.1f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "ADC_MaxP7V", sizeof (P->uniqueId));
			P->minValuef = 7.0f;
			P->maxValuef = 9.0f;
			break;
		case _Tel_ADC_Minp7V:
			strncpy (P->name, "ADC_Min_MonP7V", sizeof (P->name));
			strncpy (P->unit, "V", sizeof (P->unit));
			strncpy (P->formatStr, "%.1f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "ADC_MinP7V", sizeof (P->uniqueId));
			P->minValuef = 7.0f;
			P->maxValuef = 9.0f;
			break;
		case _Tel_ADC_Avgp7V:
			strncpy (P->name, "ADC_Avg_MonP7V", sizeof (P->name));
			strncpy (P->unit, "V", sizeof (P->unit));
			strncpy (P->formatStr, "%.1f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "ADC_AvgP7V", sizeof (P->uniqueId));
			P->minValuef = 7.0f;
			P->maxValuef = 9.0f;
			break;
		case _Tel_ADC_Maxp27V:
			strncpy (P->name, "ADC_Max_MonP27V", sizeof (P->name));
			strncpy (P->unit, "V", sizeof (P->unit));
			strncpy (P->formatStr, "%.1f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "ADC_MaxP27V", sizeof (P->uniqueId));
			P->minValuef = 26.0f;
			P->maxValuef = 30.0f;
			break;
		case _Tel_ADC_Minp27V:
			strncpy (P->name, "ADC_Min_MonP27V", sizeof (P->name));
			strncpy (P->unit, "V", sizeof (P->unit));
			strncpy (P->formatStr, "%.1f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "ADC_MinP27V", sizeof (P->uniqueId));
			P->minValuef = 26.0f;
			P->maxValuef = 30.0f;
			break;
		case _Tel_ADC_Avgp27V:
			strncpy (P->name, "ADC_Avg_MonP27V", sizeof (P->name));
			strncpy (P->unit, "V", sizeof (P->unit));
			strncpy (P->formatStr, "%.1f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "ADC_AvgP27V", sizeof (P->uniqueId));
			P->minValuef = 26.0f;
			P->maxValuef = 30.0f;
			break;
		case _Tel_ADC_MaxVocm:
			strncpy (P->name, "ADC_Max_MonVocm", sizeof (P->name));
			strncpy (P->unit, "V", sizeof (P->unit));
			strncpy (P->formatStr, "%.1f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "ADC_MaxVocm", sizeof (P->uniqueId));
			P->minValuef = 2.2f;
			P->maxValuef = 2.7f;
			break;
		case _Tel_ADC_MinVocm:
			strncpy (P->name, "ADC_Min_MonVocm", sizeof (P->name));
			strncpy (P->unit, "V", sizeof (P->unit));
			strncpy (P->formatStr, "%.1f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "ADC_MinVocm", sizeof (P->uniqueId));
			P->minValuef = 2.2f;
			P->maxValuef = 2.7f;
			break;
		case _Tel_ADC_AvgVocm:
			strncpy (P->name, "ADC_Avg_MonVocm", sizeof (P->name));
			strncpy (P->unit, "V", sizeof (P->unit));
			strncpy (P->formatStr, "%.1f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "ADC_AvgVocm", sizeof (P->uniqueId));
			P->minValuef = 2.2f;
			P->maxValuef = 2.7f;
			break;

		default: Ret = _ErrorUnCorrParam; break;
	}
	return (Ret);
}
