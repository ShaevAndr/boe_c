/*=============================================================================
2     Project: 
3     Platform: GD32F407
4     Filename: CommandParserFloatParam.c
5     Description:
6     Version: 0.0
7     Created: 2025.01.28
8     Last modified: 2025.01.28
9============================================================================*/
#include <stdint.h>
#include <string.h>
#include <stdio.h>
//--------------------------------------------------------------------------//
#include "..\ParamApp.h"
#include "CommandParser.h"
#include "AccessFloatParam.h"
//--------------------------------------------------------------------------//
int8_t AccessFloatParam (FloatParam_t NumParam, float * Value, ParamAccessMode_t AccessMode)
{
	int8_t Ret = _NoError;
	switch (NumParam)
	{

		case _FP_CALMADCMonP_0: case _FP_CALMADCMonN_0: case _FP_CALMADCMonP_1:
		case _FP_CALMADCMonN_1: case _FP_CALMADCMonP_2: case _FP_CALMADCMonN_2:
		case _FP_CALMADCMonP_3: case _FP_CALMADCMonN_3: case _FP_CALMADCMonP_4:
		case _FP_CALMADCMonN_4: case _FP_CALMADCMonP_5: case _FP_CALMADCMonN_5:
		case _FP_CALMADCMonP_6: case _FP_CALMADCMonN_6: case _FP_CALMADCMonP_7:
		case _FP_CALMADCMonN_7:
		case _FP_CALMADCMon3P3VD: case _FP_CALMADCMonN5Va: case _FP_CALMADCMonP5Va:
		case _FP_CALMADCMonP24Va: case _FP_CALMMSP1Uiin: case _FP_CALMMSP1Up:
		case _FP_CALMMSP1VCC: case _FP_CALMMSP2Uiin: case _FP_CALMMSP2Up:
		case _FP_CALMMSP2VCC:
			switch (AccessMode)
			{
				case _PAM_WO:
				case _PAM_RW:
					//SetHistorySize (*Value);
					if (AccessMode == _PAM_WO) break;
				case _PAM_RO:
					*Value = 1.0f;
					break;
				default:
					break;
			}
			break;
		case _FP_CALAADCMonP_0: case _FP_CALAADCMonN_0: case _FP_CALAADCMonP_1:
		case _FP_CALAADCMonN_1: case _FP_CALAADCMonP_2: case _FP_CALAADCMonN_2:
		case _FP_CALAADCMonP_3: case _FP_CALAADCMonN_3: case _FP_CALAADCMonP_4:
		case _FP_CALAADCMonN_4: case _FP_CALAADCMonP_5: case _FP_CALAADCMonN_5:
		case _FP_CALAADCMonP_6: case _FP_CALAADCMonN_6: case _FP_CALAADCMonP_7:
		case _FP_CALAADCMonN_7: case _FP_CALAADCMon3P3VD: case _FP_CALAADCMonN5Va:
		case _FP_CALAADCMonP5Va: case _FP_CALAADCMonP24Va: case _FP_CALAMSP1Uiin:
		case _FP_CALAMSP1Up: case _FP_CALAMSP1VCC: case _FP_CALAMSP2Uiin:
		case _FP_CALAMSP2Up: case _FP_CALAMSP2VCC:
			switch (AccessMode)
			{
				case _PAM_WO:
				case _PAM_RW:
					//SetHistorySize (*Value);
					if (AccessMode == _PAM_WO) break;
				case _PAM_RO:
					*Value = 0.0f;
					break;
				default:
					break;
			}
			break;
			
		case _FP_ICPMinVoltage:
			switch (AccessMode)
			{
				case _PAM_WO:
				case _PAM_RW:
					gParamApp.ICPMinVoltage = *Value;
					SaveParamApp ();
					if (AccessMode == _PAM_WO) break;
				case _PAM_RO:
					*Value = gParamApp.ICPMinVoltage;
					break;
				default:
					break;
			}
			break;

		default:
			Ret = _ErrorUnCorrParam;
			break;
	}
	return (Ret);
}
//--------------------------------------------------------------------------//
int8_t AccessDescFloatParam (FloatParam_t NumParam, DescriptParam_t * P)
{
	int8_t Ret = _NoError;
	strncpy (P->name, "", sizeof (P->name));
	strncpy (P->unit, "", sizeof (P->unit));
	strncpy (P->formatStr, "", sizeof (P->formatStr));
	strncpy (P->uniqueId, "", sizeof (P->uniqueId));
	strncpy (P->access, "RW", sizeof (P->access));
	P->minValuef = 0.0f;
	P->maxValuef = 0.0f;
	P->minValuei = 0;
	P->maxValuei = 0;
	
	switch (NumParam)
	{
		case _FP_CALMADCMonP_0: case _FP_CALMADCMonP_1: case _FP_CALMADCMonP_2:
		case _FP_CALMADCMonP_3: case _FP_CALMADCMonP_4: case _FP_CALMADCMonP_5:
		case _FP_CALMADCMonP_6: case _FP_CALMADCMonP_7:
			snprintf (P->name, sizeof (P->name), "CAL_MUL_ADC_MonP%d", (int)(NumParam >> 2));
			strncpy (P->unit,			"_",					sizeof (P->unit));
			strncpy (P->formatStr,"%f",				sizeof (P->formatStr));
			snprintf (P->uniqueId, sizeof (P->uniqueId), "CALMADCMonP%d", (int)(NumParam >> 2));
			P->minValuef = 0.0f;
			P->maxValuef = 1.84467440737E+19f;
			break;
		case _FP_CALAADCMonP_0: case _FP_CALAADCMonP_1: case _FP_CALAADCMonP_2:
		case _FP_CALAADCMonP_3: case _FP_CALAADCMonP_4: case _FP_CALAADCMonP_5:
		case _FP_CALAADCMonP_6: case _FP_CALAADCMonP_7:
			snprintf (P->name, sizeof (P->name), "CAL_ADD_ADC_MonP%d", (int)(NumParam >> 2));
			strncpy (P->unit,			"_",					sizeof (P->unit));
			strncpy (P->formatStr,"%f",				sizeof (P->formatStr));
			snprintf (P->uniqueId, sizeof (P->uniqueId), "CALAADCMonP%d", (int)(NumParam >> 2));
			P->minValuef = 0.0f;
			P->maxValuef = 1.84467440737E+19f;
			break;
		case _FP_CALMADCMonN_0: case _FP_CALMADCMonN_1: case _FP_CALMADCMonN_2:
		case _FP_CALMADCMonN_3: case _FP_CALMADCMonN_4: case _FP_CALMADCMonN_5:
		case _FP_CALMADCMonN_6: case _FP_CALMADCMonN_7:
			snprintf (P->name, sizeof (P->name), "CAL_MUL_ADC_MonN%d", (int)(NumParam >> 2));
			strncpy (P->unit,			"_",					sizeof (P->unit));
			strncpy (P->formatStr,"%f",				sizeof (P->formatStr));
			snprintf (P->uniqueId, sizeof (P->uniqueId), "CALMADCMonN%d", (int)(NumParam >> 2));
			P->minValuef = 0.0f;
			P->maxValuef = 1.84467440737E+19f;
			break;
		case _FP_CALAADCMonN_0: case _FP_CALAADCMonN_1: case _FP_CALAADCMonN_2:
		case _FP_CALAADCMonN_3: case _FP_CALAADCMonN_4: case _FP_CALAADCMonN_5:
		case _FP_CALAADCMonN_6: case _FP_CALAADCMonN_7:
			snprintf (P->name, sizeof (P->name), "CAL_ADD_ADC_MonN%d", (int)(NumParam >> 2));
			strncpy (P->unit,			"_",					sizeof (P->unit));
			strncpy (P->formatStr,"%f",				sizeof (P->formatStr));
			snprintf (P->uniqueId, sizeof (P->uniqueId), "CALAADCMonN%d", (int)(NumParam >> 2));
			P->minValuef = 0.0f;
			P->maxValuef = 1.84467440737E+19f;
			break;
		case _FP_CALMADCMon3P3VD:
			strncpy (P->name,			"CAL_MUL_ADC_Mon3P3VD",	sizeof (P->name));
			strncpy (P->unit,			"",					sizeof (P->unit));
			strncpy (P->formatStr,"%f",				sizeof (P->formatStr));
			strncpy (P->uniqueId,	"CALMADCMon3P3VD",		sizeof (P->uniqueId));
			P->minValuef = 0.0f;
			P->maxValuef = 1.84467440737E+19f;
			break;
		case _FP_CALAADCMon3P3VD:
			strncpy (P->name,			"CAL_ADD_ADC_Mon3P3VD",	sizeof (P->name));
			strncpy (P->unit,			"",					sizeof (P->unit));
			strncpy (P->formatStr,"%f",				sizeof (P->formatStr));
			strncpy (P->uniqueId,	"CALAADCMon3P3VD",		sizeof (P->uniqueId));
			P->minValuef = 0.0f;
			P->maxValuef = 1.84467440737E+19f;
			break;
		case _FP_CALMADCMonN5Va:
			strncpy (P->name,			"CAL_MUL_ADC_MonN5Va",	sizeof (P->name));
			strncpy (P->unit,			"",					sizeof (P->unit));
			strncpy (P->formatStr,"%f",				sizeof (P->formatStr));
			strncpy (P->uniqueId,	"CALMADCMonN5Va",		sizeof (P->uniqueId));
			P->minValuef = 0.0f;
			P->maxValuef = 1.84467440737E+19f;
			break;
		case _FP_CALAADCMonN5Va:
			strncpy (P->name,			"CAL_ADD_ADC_MonN5Va",	sizeof (P->name));
			strncpy (P->unit,			"",					sizeof (P->unit));
			strncpy (P->formatStr,"%f",				sizeof (P->formatStr));
			strncpy (P->uniqueId,	"CALAADCMonN5Va",		sizeof (P->uniqueId));
			P->minValuef = 0.0f;
			P->maxValuef = 1.84467440737E+19f;
			break;
		case _FP_CALMADCMonP5Va:
			strncpy (P->name,			"CAL_MUL_ADC_MonP5Va",	sizeof (P->name));
			strncpy (P->unit,			"",					sizeof (P->unit));
			strncpy (P->formatStr,"%f",				sizeof (P->formatStr));
			strncpy (P->uniqueId,	"CALMADCMonP5Va",		sizeof (P->uniqueId));
			P->minValuef = 0.0f;
			P->maxValuef = 1.84467440737E+19f;
			break;
		case _FP_CALAADCMonP5Va:
			strncpy (P->name,			"CAL_ADD_ADC_MonP5Va",	sizeof (P->name));
			strncpy (P->unit,			"",					sizeof (P->unit));
			strncpy (P->formatStr,"%f",				sizeof (P->formatStr));
			strncpy (P->uniqueId,	"CALAADCMonP5Va",		sizeof (P->uniqueId));
			P->minValuef = 0.0f;
			P->maxValuef = 1.84467440737E+19f;
			break;
		case _FP_CALMADCMonP24Va:
			strncpy (P->name,			"CAL_MUL_ADC_MonP24Va",	sizeof (P->name));
			strncpy (P->unit,			"",					sizeof (P->unit));
			strncpy (P->formatStr,"%f",				sizeof (P->formatStr));
			strncpy (P->uniqueId,	"CALMADCMonP24Va",		sizeof (P->uniqueId));
			P->minValuef = 0.0f;
			P->maxValuef = 1.84467440737E+19f;
			break;
		case _FP_CALAADCMonP24Va:
			strncpy (P->name,			"CAL_ADD_ADC_MonP24Va",	sizeof (P->name));
			strncpy (P->unit,			"",					sizeof (P->unit));
			strncpy (P->formatStr,"%f",				sizeof (P->formatStr));
			strncpy (P->uniqueId,	"CALAADCMonP24Va",		sizeof (P->uniqueId));
			P->minValuef = 0.0f;
			P->maxValuef = 1.84467440737E+19f;
			break;
		case _FP_CALMMSP1Uiin:
			strncpy (P->name,			"CAL_MUL_MSP1_Uiin",	sizeof (P->name));
			strncpy (P->unit,			"",					sizeof (P->unit));
			strncpy (P->formatStr,"%f",				sizeof (P->formatStr));
			strncpy (P->uniqueId,	"CALMMSP1Uiin",		sizeof (P->uniqueId));
			P->minValuef = 0.0f;
			P->maxValuef = 1.84467440737E+19f;
			break;
		case _FP_CALAMSP1Uiin:
			strncpy (P->name,			"CAL_ADD_MSP1_Uiin",	sizeof (P->name));
			strncpy (P->unit,			"",					sizeof (P->unit));
			strncpy (P->formatStr,"%f",				sizeof (P->formatStr));
			strncpy (P->uniqueId,	"CALAMSP1Uiin",		sizeof (P->uniqueId));
			P->minValuef = 0.0f;
			P->maxValuef = 1.84467440737E+19f;
			break;
		case _FP_CALMMSP1Up:
			strncpy (P->name,			"CAL_MUL_MSP1_Up",	sizeof (P->name));
			strncpy (P->unit,			"",					sizeof (P->unit));
			strncpy (P->formatStr,"%f",				sizeof (P->formatStr));
			strncpy (P->uniqueId,	"CALMMSP1Up",		sizeof (P->uniqueId));
			P->minValuef = 0.0f;
			P->maxValuef = 1.84467440737E+19f;
			break;
		case _FP_CALAMSP1Up:
			strncpy (P->name,			"CAL_ADD_MSP1_Up",	sizeof (P->name));
			strncpy (P->unit,			"",					sizeof (P->unit));
			strncpy (P->formatStr,"%f",				sizeof (P->formatStr));
			strncpy (P->uniqueId,	"CALAMSP1Up",		sizeof (P->uniqueId));
			P->minValuef = 0.0f;
			P->maxValuef = 1.84467440737E+19f;
			break;
		case _FP_CALMMSP1VCC:
			strncpy (P->name,			"CAL_MUL_MSP1_VCC",	sizeof (P->name));
			strncpy (P->unit,			"",					sizeof (P->unit));
			strncpy (P->formatStr,"%f",				sizeof (P->formatStr));
			strncpy (P->uniqueId,	"CALMMSP1VCC",		sizeof (P->uniqueId));
			P->minValuef = 0.0f;
			P->maxValuef = 1.84467440737E+19f;
			break;
		case _FP_CALAMSP1VCC:
			strncpy (P->name,			"CAL_ADD_MSP1_VCC",	sizeof (P->name));
			strncpy (P->unit,			"",					sizeof (P->unit));
			strncpy (P->formatStr,"%f",				sizeof (P->formatStr));
			strncpy (P->uniqueId,	"CALAMSP1VCC",		sizeof (P->uniqueId));
			P->minValuef = 0.0f;
			P->maxValuef = 1.84467440737E+19f;
			break;
		case _FP_CALMMSP2Uiin:
			strncpy (P->name,			"CAL_MUL_MSP2_Uiin",	sizeof (P->name));
			strncpy (P->unit,			"",					sizeof (P->unit));
			strncpy (P->formatStr,"%f",				sizeof (P->formatStr));
			strncpy (P->uniqueId,	"CALMMSP2Uiin",		sizeof (P->uniqueId));
			P->minValuef = 0.0f;
			P->maxValuef = 1.84467440737E+19f;
			break;
		case _FP_CALAMSP2Uiin:
			strncpy (P->name,			"CAL_ADD_MSP2_Uiin",	sizeof (P->name));
			strncpy (P->unit,			"",					sizeof (P->unit));
			strncpy (P->formatStr,"%f",				sizeof (P->formatStr));
			strncpy (P->uniqueId,	"CALAMSP2Uiin",		sizeof (P->uniqueId));
			P->minValuef = 0.0f;
			P->maxValuef = 1.84467440737E+19f;
			break;
		case _FP_CALMMSP2Up:
			strncpy (P->name,			"CAL_MUL_MSP2_Up",	sizeof (P->name));
			strncpy (P->unit,			"",					sizeof (P->unit));
			strncpy (P->formatStr,"%f",				sizeof (P->formatStr));
			strncpy (P->uniqueId,	"CALMMSP2Up",		sizeof (P->uniqueId));
			P->minValuef = 0.0f;
			P->maxValuef = 1.84467440737E+19f;
			break;
		case _FP_CALAMSP2Up:
			strncpy (P->name,			"CAL_ADD_MSP2_Up",	sizeof (P->name));
			strncpy (P->unit,			"",					sizeof (P->unit));
			strncpy (P->formatStr,"%f",				sizeof (P->formatStr));
			strncpy (P->uniqueId,	"CALAMSP2Up",		sizeof (P->uniqueId));
			P->minValuef = 0.0f;
			P->maxValuef = 1.84467440737E+19f;
			break;
		case _FP_CALMMSP2VCC:
			strncpy (P->name,			"CAL_MUL_MSP2_VCC",	sizeof (P->name));
			strncpy (P->unit,			"",					sizeof (P->unit));
			strncpy (P->formatStr,"%f",				sizeof (P->formatStr));
			strncpy (P->uniqueId,	"CALMMSP2VCC",		sizeof (P->uniqueId));
			P->minValuef = 0.0f;
			P->maxValuef = 1.84467440737E+19f;
			break;
		case _FP_CALAMSP2VCC:
			strncpy (P->name,			"CAL_ADD_MSP2_VCC",	sizeof (P->name));
			strncpy (P->unit,			"",					sizeof (P->unit));
			strncpy (P->formatStr,"%f",				sizeof (P->formatStr));
			strncpy (P->uniqueId,	"CALAMSP2VCC",		sizeof (P->uniqueId));
			P->minValuef = 0.0f;
			P->maxValuef = 1.84467440737E+19f;
			break;
		case _FP_ICPMinVoltage:
			strncpy (P->name,			"ICPMinVoltage",	sizeof (P->name));
			strncpy (P->unit,			"V",					sizeof (P->unit));
			strncpy (P->formatStr,"%f",				sizeof (P->formatStr));
			strncpy (P->uniqueId,	"ICPMinVoltage",		sizeof (P->uniqueId));
			P->minValuef = 0.0f;
			P->maxValuef = 1.84467440737E+19f;
			break;		
		
		default: Ret = _ErrorUnCorrParam; break;
	}
	return (Ret);
}
