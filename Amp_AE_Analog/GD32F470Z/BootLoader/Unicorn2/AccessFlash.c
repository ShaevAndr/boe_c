/*=============================================================================
  Project:
  Platform: GD32F470ZI
  Filename: AccessFlash.c
  Description:
  Programmer:
  Version: 2.0
  Created: 2006.01.16
  Last modified: 2025.04.06
=============================================================================*/
#include <stdint.h>
//#include <string.h>
//#include <cdefBF533.h>
//#include <stdio.h>
//-----------------------------------------------------------------------------
//#include "Time.h"
//#include "drv_SPI.h"
//#include "drv_mtd.h"
//#include "crc16.h"
//#include "MVTP_DataProcessFiltersCoef.h"
//#include "MVTP_DataProcessFunctions.h"
//-----------------------------------------------------------------------------
#include "..\drv_Unicorn2EEPROM.h"
#include "drv_InternalFMC.h"
#include "AccessFlash.h"
//-----------------------------------------------------------------------------
int32_t GetFlashCount (void)
{
	return (_FlashCount);
}
//--------------------------------------------------------------------------//
int32_t GetFlashPageCount (TypeFlash_t FlashNum)
{
	int32_t res;
	switch (FlashNum)
	{
		case _SettingFlash: res = GetSettingFlashPageCount (); break;
		case _InternalFlash: res = GetInternalFlashPageCount (); break;
		default: res = (0); break;
	}
	return (res);
}
//--------------------------------------------------------------------------//
int32_t GetFlashPageSize (TypeFlash_t FlashNum)
{
	int32_t res;
	switch (FlashNum)
	{
		case _SettingFlash: res = GetSettingFlashPageSize (); break;
		case _InternalFlash: res = GetInternalFlashPageSize (); break;
		default: res = (0); break;
	}
	return (res);
}
//--------------------------------------------------------------------------//
/*
int32_t GetFlashDescription (TypeFlash_t FlashNum, uint8_t * p)
{
	switch (FlashNum)
	{
		case _SettingFlash: return (GetSettingFlashDescription (p));
		case _InternalFlash: return (GetInternalFlashDescription (p));
		default: return (0);
	}
}
*/
//--------------------------------------------------------------------------//
int32_t ReadFlashPage (TypeFlash_t FlashNum, uint32_t PageNum, uint8_t * p)
{
	int32_t res = 0;
	switch (FlashNum)
	{
		case _SettingFlash: res = ReadSettingFlashPage (PageNum, p); break;
		case _InternalFlash: res = ReadInternalFlashPage (PageNum, p); break;
		default: res = -1; break;
	}
	return (res);
}
//--------------------------------------------------------------------------//
int32_t WriteFlashPage (TypeFlash_t FlashNum, uint32_t PageNum, uint8_t * p)
{
	int32_t res = 0;
	switch (FlashNum)
	{
		case _SettingFlash: res = WriteSettingFlashPage (PageNum, p); break;
		case _InternalFlash: res = WriteInternalFlashPage (PageNum, p); break;
		default: res = -1; break;
	}
	return (res);
}
//--------------------------------------------------------------------------//
