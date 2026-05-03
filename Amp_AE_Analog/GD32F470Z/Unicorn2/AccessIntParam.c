/*=============================================================================
2     Project: 
3     Platform: GD32F407
4     Filename: CommandParserIntParam.c
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
#include "PacketParser.h"
//--------------------------------------------------------------------------//
#include "..\main_application.h"
#include "DeviceUsageStatistics.h"
#include "Unicorn2Routine.h"
#include "..\ParamApp.h"
#include "ParamSystem.h"
#include "..\drv_ICP.h"
#include "../drv_Monp.h"
#include "../drv_DIn.h"
#include "../CheckBUE.h"
#include "../CheckEnviron.h"
#include "../drv_DS18B20.h"
//--------------------------------------------------------------------------//
#include "CommandList.h"
#include "CommandParser.h"
#include "AccessIntParam.h"
//--------------------------------------------------------------------------//
int8_t AccessIntParam (IntParam_t NumParam, int32_t * Value, ParamAccessMode_t AccessMode)
{
	int8_t Ret = _NoError;
	switch (NumParam)
	{
		case _IP_DevAddr:
			switch (AccessMode)
			{
				case _PAM_WO:
				case _PAM_RW:
					if (*Value > 254) *Value = 254;
					if (*Value < 1) *Value = 1;
					gParamSystem.rs485Modbus [GetCurrUARTNumber ()].ModbusAddress = *Value;
					SaveParamSystem ();
					if (AccessMode == _PAM_WO) break;
				case _PAM_RO:
					*Value = gParamSystem.rs485Modbus [GetCurrUARTNumber ()].ModbusAddress;
					break;
				default:
					break;
			}
			break;
		case _IP_KuOP0: case _IP_KuOP1: case _IP_KuOP2: case _IP_KuOP3:
		case _IP_KuOP4: case _IP_KuOP5: case _IP_KuOP6: case _IP_KuOP7:
			switch (AccessMode)
			{
				case _PAM_WO:
				case _PAM_RW:
					if (*Value < -1) *Value = -1;
					if (*Value > 0) *Value = 0;
					gParamApp.Ku_OP [NumParam - 1] = *Value;
					SaveParamApp ();
					if (AccessMode == _PAM_WO) break;
				case _PAM_RO:
					*Value = (int32_t)(gParamApp.Ku_OP [NumParam - 1]);
					break;
				default:
					break;
			}
			break;
		case _IP_PowerOP:
			switch (AccessMode)
			{
				case _PAM_WO:
				case _PAM_RW:
					SetICPDisablePower (*Value & 0xFF);
					if (AccessMode == _PAM_WO) break;
				case _PAM_RO:
					*Value = (int32_t)GetICPDisablePower ();
					break;
				default:
					break;
			}
			break;
		case _IP_LotwaferID: case _IP_DXYpos: case _IP_System:
		case _IP_LotwaferIDMSP1: case _IP_DXYposMSP1: case _IP_SystemMSP1:
			switch (AccessMode)
			{
				case _PAM_WO:
				case _PAM_RW:
					Ret = _ErrorWriteROParam;
					break;
				case _PAM_RO:
					*Value = 0;
					break;
				default:
					break;
			}
			break;
		case _IP_BuildMSP1:
			switch (AccessMode)
			{
				case _PAM_WO:
				case _PAM_RW:
					Ret = _ErrorWriteROParam;
					break;
				case _PAM_RO:
					*Value = (int32_t)GetMonpBuildTime ();
					break;
				default:
					break;
			}
			break;

		case _IP_LotwaferIDMSP2: case _IP_DXYposMSP2: case _IP_SystemMSP2:
		case _IP_BuildMSP2: case _IP_HWDevVer: case _IP_HFXTOFFGCount:
			switch (AccessMode)
			{
				case _PAM_WO:
				case _PAM_RW:
					Ret = _ErrorWriteROParam;
					break;
				case _PAM_RO:
					*Value = 0;
					break;
				default:
					break;
			}
			break;

		case _IPMonStateDIn:
			switch (AccessMode)
			{
				case _PAM_WO:
				case _PAM_RW:
					Ret = _ErrorWriteROParam;
					break;
				case _PAM_RO:
					*Value = GetStateDInsU32 ();
					break;
				default:
					break;
			}
			break;
		case _IPMonChangeDIn:
			switch (AccessMode)
			{
				case _PAM_WO:
				case _PAM_RW:
					Ret = _ErrorWriteROParam;
					break;
				case _PAM_RO:
					*Value = GetChangeDInsU32 ();
					break;
				default:
					break;
			}
			break;

		case _IPStatusBUE:
			switch (AccessMode)
			{
				case _PAM_WO:
				case _PAM_RW:
					Ret = _ErrorWriteROParam;
					break;
				case _PAM_RO:
					*Value = (int32_t)(GetStatusBUE ().ui32);
					break;
				default:
					break;
			}
			break;
		case _IPStatusEnv:
			switch (AccessMode)
			{
				case _PAM_WO:
				case _PAM_RW:
					Ret = _ErrorWriteROParam;
					break;
				case _PAM_RO:
					*Value = (int32_t)(GetStatusEnv ().ui32);
					break;
				default:
					break;
			}
			break;
		case _IPStatusInterVolt:
			switch (AccessMode)
			{
				case _PAM_WO:
				case _PAM_RW:
					Ret = _ErrorWriteROParam;
					break;
				case _PAM_RO:
					*Value = (int32_t)(GetStatusInternalVoltage ().ui32);
					break;
				default:
					break;
			}
			break;
		case _IP_DS0_Location: case _IP_DS1_Location:
			switch (AccessMode)
			{
				case _PAM_WO:
				case _PAM_RW:
					DS18B20_SetDSLocation (NumParam - _IP_DS0_Location, *Value);
					SaveParamApp ();
					if (AccessMode == _PAM_WO) break;
				case _PAM_RO:
					*Value = DS18B20_GetDSLocation (NumParam - _IP_DS0_Location);
					break;
				default:
					break;
			}
			break;

		case ipTotalOperatingTime:
			switch (AccessMode)
			{
				case _PAM_WO:
				case _PAM_RW:
					Ret = _ErrorWriteROParam;
					break;
				case _PAM_RO:
					*Value = GetTotalOperatingTime ();
					break;
				default:
					break;
			}
			break;
		case ipNumberOfBootloaderLaunches:
			switch (AccessMode)
			{
				case _PAM_WO:
				case _PAM_RW:
					Ret = _ErrorWriteROParam;
					break;
				case _PAM_RO:
					*Value = GetNumberOfBootloaderLaunches ();
					break;
				default:
					break;
			}
			break;
		case ipNumberOfApplicationLaunches:
			switch (AccessMode)
			{
				case _PAM_WO:
				case _PAM_RW:
					Ret = _ErrorWriteROParam;
					break;
				case _PAM_RO:
					*Value = GetNumberOfApplicationLaunches ();
					break;
				default:
					break;
			}
			break;
		case ipNumberErrorWriteDeviceUsStat:
			switch (AccessMode)
			{
				case _PAM_WO:
				case _PAM_RW:
					Ret = _ErrorWriteROParam;
					break;
				case _PAM_RO:
					*Value = GetNumberErrorWriteDeviceUsStat ();
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
int8_t AccessDescIntParam (IntParam_t NumParam, DescriptParam_t * P)
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
		case _IP_DevAddr:
			strncpy (P->name, "DevAddr", sizeof (P->name));
			strncpy (P->unit, "_", sizeof (P->unit));
			strncpy (P->formatStr, "%u", sizeof (P->formatStr));
			strncpy (P->uniqueId, "DevAddr", sizeof (P->uniqueId));
			P->minValuei = 1;
			P->maxValuei = 254;
			break;
		case _IP_KuOP0: case _IP_KuOP1: case _IP_KuOP2: case _IP_KuOP3:
		case _IP_KuOP4: case _IP_KuOP5: case _IP_KuOP6: case _IP_KuOP7:
			snprintf (P->name, sizeof (P->name), "Ku_OP%d", (int)(NumParam - _IP_KuOP0));
			strncpy (P->unit, "_", sizeof (P->unit));
			strncpy (P->formatStr, "%d", sizeof (P->formatStr));
			snprintf (P->uniqueId, sizeof (P->uniqueId), "KuOP%d", (int)(NumParam - _IP_KuOP0));
			P->minValuei = -1;
			P->maxValuei = 0;
			break;
		case _IP_PowerOP:
			strncpy (P->name, "Power_OP", sizeof (P->name));
			strncpy (P->formatStr, "0x%02X", sizeof (P->formatStr));
			strncpy (P->uniqueId, "PowerOP", sizeof (P->uniqueId));
			P->minValuei = 0;
			P->maxValuei = 255;
			break;
		case _IP_LotwaferID:
			strncpy (P->name, "Lot_waferID", sizeof (P->name));
			strncpy (P->formatStr, "0x%08X", sizeof (P->formatStr));
			strncpy (P->uniqueId, "LotwaferID", sizeof (P->uniqueId));
			strncpy (P->access, "RO", sizeof (P->access));
			P->minValuei = 0x80000000;
			P->maxValuei = 0x7FFFFFFF;
			break;
		case _IP_DXYpos:
			strncpy (P->name, "DieXYposition", sizeof (P->name));
			strncpy (P->formatStr, "0x%08X", sizeof (P->formatStr));
			strncpy (P->uniqueId, "DXYpos", sizeof (P->uniqueId));
			strncpy (P->access, "RO", sizeof (P->access));
			P->minValuei = 0x80000000;
			P->maxValuei = 0x7FFFFFFF;
			break;
		case _IP_System:
			strncpy (P->name, "System", sizeof (P->name));
			strncpy (P->formatStr, "0x%08X", sizeof (P->formatStr));
			strncpy (P->uniqueId, "System", sizeof (P->uniqueId));
			strncpy (P->access, "RO", sizeof (P->access));
			P->minValuei = 0x80000000;
			P->maxValuei = 0x7FFFFFFF;
			break;
		case _IP_LotwaferIDMSP1:
			strncpy (P->name, "Lot_waferID_MSP1", sizeof (P->name));
			strncpy (P->formatStr, "0x%08X", sizeof (P->formatStr));
			strncpy (P->uniqueId, "LotwaferIDMSP1", sizeof (P->uniqueId));
			strncpy (P->access, "RO", sizeof (P->access));
			P->minValuei = 0x80000000;
			P->maxValuei = 0x7FFFFFFF;
			break;
		case _IP_DXYposMSP1:
			strncpy (P->name, "DieXYposition_MSP1", sizeof (P->name));
			strncpy (P->formatStr, "0x%08X", sizeof (P->formatStr));
			strncpy (P->uniqueId, "DXYposMSP1", sizeof (P->uniqueId));
			strncpy (P->access, "RO", sizeof (P->access));
			P->minValuei = 0x80000000;
			P->maxValuei = 0x7FFFFFFF;
			break;
		case _IP_SystemMSP1:
			strncpy (P->name, "System_MSP1", sizeof (P->name));
			strncpy (P->formatStr, "0x%08X", sizeof (P->formatStr));
			strncpy (P->uniqueId, "SystemMSP1", sizeof (P->uniqueId));
			strncpy (P->access, "RO", sizeof (P->access));
			P->minValuei = 0x80000000;
			P->maxValuei = 0x7FFFFFFF;
			break;
		case _IP_BuildMSP1:
			strncpy (P->name, "BuildTime_MSP1", sizeof (P->name));
			strncpy (P->unit, "С", sizeof (P->unit));
			strncpy (P->formatStr, "%u", sizeof (P->formatStr));
			strncpy (P->uniqueId, "BuildMSP1", sizeof (P->uniqueId));
			strncpy (P->access, "RO", sizeof (P->access));
			P->minValuei = 0x80000000;
			P->maxValuei = 0x7FFFFFFF;
			break;
		case _IP_LotwaferIDMSP2:
			strncpy (P->name, "Lot_waferID_MSP2", sizeof (P->name));
			strncpy (P->formatStr, "0x%08X", sizeof (P->formatStr));
			strncpy (P->uniqueId, "LotwaferIDMSP2", sizeof (P->uniqueId));
			strncpy (P->access, "RO", sizeof (P->access));
			P->minValuei = 0x80000000;
			P->maxValuei = 0x7FFFFFFF;
			break;
		case _IP_DXYposMSP2:
			strncpy (P->name, "DieXYposition_MSP2", sizeof (P->name));
			strncpy (P->formatStr, "0x%08X", sizeof (P->formatStr));
			strncpy (P->uniqueId, "DXYposMSP2", sizeof (P->uniqueId));
			strncpy (P->access, "RO", sizeof (P->access));
			P->minValuei = 0x80000000;
			P->maxValuei = 0x7FFFFFFF;
			break;
		case _IP_SystemMSP2:
			strncpy (P->name, "System_MSP2", sizeof (P->name));
			strncpy (P->formatStr, "0x%08X", sizeof (P->formatStr));
			strncpy (P->uniqueId, "SystemMSP2", sizeof (P->uniqueId));
			strncpy (P->access, "RO", sizeof (P->access));
			P->minValuei = 0x80000000;
			P->maxValuei = 0x7FFFFFFF;
			break;
		case _IP_BuildMSP2:
			strncpy (P->name, "BuildTime_MSP2", sizeof (P->name));
			strncpy (P->unit, "С", sizeof (P->unit));
			strncpy (P->formatStr, "%u", sizeof (P->formatStr));
			strncpy (P->uniqueId, "BuildMSP2", sizeof (P->uniqueId));
			strncpy (P->access, "RO", sizeof (P->access));
			P->minValuei = 0x80000000;
			P->maxValuei = 0x7FFFFFFF;
			break;
		case _IP_HWDevVer:
			strncpy (P->name, "HWDevVer", sizeof (P->name));
			strncpy (P->formatStr, "%u", sizeof (P->formatStr));
			strncpy (P->uniqueId, "HWDevVer", sizeof (P->uniqueId));
			strncpy (P->access, "RO", sizeof (P->access));
			P->minValuei = 0x80000000;
			P->maxValuei = 0x7FFFFFFF;
			break;
		case _IP_HFXTOFFGCount:
			strncpy (P->name, "HFXTOFFGCount", sizeof (P->name));
			strncpy (P->formatStr, "%u", sizeof (P->formatStr));
			strncpy (P->uniqueId, "HFXTOFFGCount", sizeof (P->uniqueId));
			strncpy (P->access, "RO", sizeof (P->access));
			P->minValuei = 0x80000000;
			P->maxValuei = 0x7FFFFFFF;
			break;

		case _IPMonStateDIn:
			strncpy (P->name, "IPMonStateDIn", sizeof (P->name));
			strncpy (P->formatStr, "%u", sizeof (P->formatStr));
			strncpy (P->uniqueId, "State DIn", sizeof (P->uniqueId));
			strncpy (P->access, "RO", sizeof (P->access));
			P->minValuei = 0;
			P->maxValuei = 3;
			break;
		case _IPMonChangeDIn:
			strncpy (P->name, "IPMonChangeDIn", sizeof (P->name));
			strncpy (P->formatStr, "%u", sizeof (P->formatStr));
			strncpy (P->uniqueId, "Change DIn", sizeof (P->uniqueId));
			strncpy (P->access, "RO", sizeof (P->access));
			P->minValuei = 0;
			P->maxValuei = 3;
			break;

		case _IPStatusBUE:
			strncpy (P->name, "StatusBUE", sizeof (P->name));
			strncpy (P->formatStr, "%u", sizeof (P->formatStr));
			strncpy (P->uniqueId, "StatusBUE", sizeof (P->uniqueId));
			strncpy (P->access, "RO", sizeof (P->access));
			P->minValuei = 0;
			P->maxValuei = 0;
			break;
		case _IPStatusEnv:
			strncpy (P->name, "StatusEnv", sizeof (P->name));
			strncpy (P->formatStr, "%u", sizeof (P->formatStr));
			strncpy (P->uniqueId, "StatusEnv", sizeof (P->uniqueId));
			strncpy (P->access, "RO", sizeof (P->access));
			P->minValuei = 0;
			P->maxValuei = 0;
			break;
		case _IPStatusInterVolt:
			strncpy (P->name, "StatusInternalVoltage", sizeof (P->name));
			strncpy (P->formatStr, "%u", sizeof (P->formatStr));
			strncpy (P->uniqueId, "IPStatusInterVolt", sizeof (P->uniqueId));
			strncpy (P->access, "RO", sizeof (P->access));
			P->minValuei = 0;
			P->maxValuei = 0;
			break;

		case _IP_DS0_Location:
			strncpy (P->name, "DS0_Location", sizeof (P->name));
			strncpy (P->formatStr, "%d", sizeof (P->formatStr));
			strncpy (P->uniqueId, "DS0_Location", sizeof (P->uniqueId));
			P->minValuei = 0;
			P->maxValuei = 1;
			break;
		case _IP_DS1_Location:
			strncpy (P->name, "DS1_Location", sizeof (P->name));
			strncpy (P->formatStr, "%d", sizeof (P->formatStr));
			strncpy (P->uniqueId, "DS1_Location", sizeof (P->uniqueId));
			P->minValuei = 0;
			P->maxValuei = 1;
			break;		

		case ipTotalOperatingTime:
			strncpy (P->name,			"Total operating time",	sizeof (P->name));
			strncpy (P->unit,			"hours",		sizeof (P->unit));
			strncpy (P->formatStr,"%d",				sizeof (P->formatStr));
			strncpy (P->uniqueId,	"TotalOperatingTime",		sizeof (P->uniqueId));
			strncpy (P->access,		"RO",				sizeof (P->access));
			break;
		case ipNumberOfBootloaderLaunches:
			strncpy (P->name,			"NumberO of bootloader launches",	sizeof (P->name));
			strncpy (P->unit,			"",					sizeof (P->unit));
			strncpy (P->formatStr,"%d",				sizeof (P->formatStr));
			strncpy (P->uniqueId,	"NumberOfBootloaderLaunches",		sizeof (P->uniqueId));
			strncpy (P->access,		"RO",				sizeof (P->access));
			break;
		case ipNumberOfApplicationLaunches:
			strncpy (P->name,			"Number of application launches",	sizeof (P->name));
			strncpy (P->unit,			"",					sizeof (P->unit));
			strncpy (P->formatStr,"%d",				sizeof (P->formatStr));
			strncpy (P->uniqueId,	"NumberOfApplicationLaunches",		sizeof (P->uniqueId));
			strncpy (P->access,		"RO",				sizeof (P->access));
			break;
		case ipNumberErrorWriteDeviceUsStat:
			strncpy (P->name,			"Number error write device usage statistic",	sizeof (P->name));
			strncpy (P->unit,			"",					sizeof (P->unit));
			strncpy (P->formatStr,"%d",				sizeof (P->formatStr));
			strncpy (P->uniqueId,	"NumberErrorWriteDeviceUsStat",		sizeof (P->uniqueId));
			strncpy (P->access,		"RO",				sizeof (P->access));
			break;
		
		default: Ret = _ErrorUnCorrParam; break;
	}
	return (Ret);
}
//--------------------------------------------------------------------------//
