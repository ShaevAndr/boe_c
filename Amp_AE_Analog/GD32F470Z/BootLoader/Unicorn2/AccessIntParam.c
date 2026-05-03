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
//--------------------------------------------------------------------------//
#include "drv_time.h"
#include "PacketParser.h"
//--------------------------------------------------------------------------//
#include "..\main_bootloader.h"
#include "DeviceUsageStatistics.h"
#include "ParamSystem.h"
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
		/*
		case ipModbusAddress:
			switch (AcessMode)
			{
				case _PAM_WO:
				case _PAM_RW:
					if (*Value > 254) *Value = 254;
					if (*Value < 1) *Value = 1;
					gParamSystem.rs485Modbus [GetCurrUARTNumber ()].ModbusAddress = *Value;
					SaveParamSystem ();
					if (AcessMode == _PAM_WO) break;
				case _PAM_RO:
					*Value = gParamSystem.rs485Modbus [GetCurrUARTNumber ()].ModbusAddress;
					break;
				default:
					break;
			}
			break;
		*/
		case ipEnableLoadApp:
			switch (AccessMode)
			{
				case _PAM_WO:
				case _PAM_RW:
					SetEnableLoadApp (*Value);
					if (AccessMode == _PAM_WO) break;
				case _PAM_RO:
					*Value = GetEnableLoadApp ();
					break;
				default:
					break;
			}
			break;

		case ipMACAddrUpperThreeBytes:
			switch (AccessMode)
			{
				case _PAM_WO:
				case _PAM_RW:
					Ret = _ErrorWriteROParam;
					break;
				case _PAM_RO:
					*Value = 0xFFFFFF & (GetMACAddr () >> 24);
					break;
				default:
					break;
			}
			break;
		case ipMACAddrLowThreeBytes:
			switch (AccessMode)
			{
				case _PAM_WO:
				case _PAM_RW:
					Ret = _ErrorWriteROParam;
					break;
				case _PAM_RO:
					*Value = 0xFFFFFF & GetMACAddr ();
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

		case _IPFactorySettingsLock:
			switch (AccessMode)
			{
				case _PAM_WO:
				case _PAM_RW:
					Ret = _ErrorWriteROParam;
					break;
				case _PAM_RO:
          *Value = isKeyUnlocked (luktFactorySettings);
					break;
				default:
					break;
			}
			break;
		case _IPFactorySettingsKey:
			switch (AccessMode)
			{
				case _PAM_WO:
				case _PAM_RW:
          SetKey(luktFactorySettings, *Value);
					if (AccessMode == _PAM_WO) break;
				case _PAM_RO:
          *Value = GetKey (luktFactorySettings);
					break;
				default:
					break;
			}
			break;
		case ipDeviceDateOfManufacture:
			switch (AccessMode)
			{
				case _PAM_WO:
				case _PAM_RW:
          if (!isKeyUnlocked (luktFactorySettings))
          {
            Ret = _ErrorWriteROParam;
            break;
          }
          gParamSystem.device.DateOfManufacture = *Value;
          SaveParamSystem ();
					break;
				case _PAM_RO:
					*Value = (uint32_t)gParamSystem.device.DateOfManufacture;
					break;
				default:
					break;
			}
			break;
		case ipDeviceSerialNumber:
			switch (AccessMode)
			{
				case _PAM_WO:
				case _PAM_RW:
          if (!isKeyUnlocked (luktFactorySettings))
          {
            Ret = _ErrorWriteROParam;
            break;
          }
          gParamSystem.device.SerialNumber = *Value;
          SaveParamSystem ();
					break;
				case _PAM_RO:
					*Value = gParamSystem.device.SerialNumber;
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
/*
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
		case ipModbusAddress:
			strncpy (P->name,			"ModbusAddress",	sizeof (P->name));
			strncpy (P->unit,			"",					sizeof (P->unit));
			strncpy (P->formatStr,"%d",				sizeof (P->formatStr));
			strncpy (P->uniqueId,	"ModbusAddress",		sizeof (P->uniqueId));
			P->minValuei = _kMinDeviceAddress;
			P->maxValuei = _kMaxDeviceAddress;
			break;
		case ipEnableLoadApp:
			strncpy (P->name,			"Enable load application",	sizeof (P->name));
			strncpy (P->unit,			"",					sizeof (P->unit));
			strncpy (P->formatStr,"%d",				sizeof (P->formatStr));
			strncpy (P->uniqueId,	"EnableLoadApp",		sizeof (P->uniqueId));
			P->minValuei = 0;
			P->maxValuei = 1;
			break;
		case ipMACAddrUpperThreeBytes:
			strncpy (P->name,			"MACAddrUpperThreeBytes",	sizeof (P->name));
			strncpy (P->unit,			"",					sizeof (P->unit));
			strncpy (P->formatStr,"0x%06X",				sizeof (P->formatStr));
			strncpy (P->uniqueId,	"MACAddrUpperThreeBytes",		sizeof (P->uniqueId));
			strncpy (P->access,		"RO",				sizeof (P->access));
			break;
		case ipMACAddrLowThreeBytes:
			strncpy (P->name,			"MACAddrLowThreeBytes",	sizeof (P->name));
			strncpy (P->unit,			"",					sizeof (P->unit));
			strncpy (P->formatStr,"0x%06X",				sizeof (P->formatStr));
			strncpy (P->uniqueId,	"MACAddrLowThreeBytes",		sizeof (P->uniqueId));
			strncpy (P->access,		"RO",				sizeof (P->access));
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
*/
//--------------------------------------------------------------------------//
