/*=============================================================================
  Project: 
  Platform: GD32F470ZI
  Filename: AccessFlash.h
  Description:
  Programmer: 
  Version: 1.0
  Created: 2006.01.16
  Last modified: 2025.04.06
=============================================================================*/
#ifndef __AccessFlash_H__
#define __AccessFlash_H__
	//-----------------------------------------------------------------------------
	#include "stdint.h"
	//-----------------------------------------------------------------------------
	typedef enum {_SettingFlash = 0, _InternalFlash = 1, _FlashCount} TypeFlash_t;
	//--------------------------------------------------------------------------//
	int32_t GetFlashCount (void);
	int32_t GetFlashPageCount (TypeFlash_t FlashNum);
	int32_t GetFlashPageSize (TypeFlash_t FlashNum);
	int32_t GetFlashDescription (TypeFlash_t FlashNum, uint8_t * p);
	int8_t ReadFlashPage (TypeFlash_t FlashNum, uint32_t PageNum, uint8_t * p);
	int8_t WriteFlashPage (TypeFlash_t FlashNum, uint32_t PageNum, uint8_t * p);
	//-----------------------------------------------------------------------------
#endif /* AccessFlash_H */
