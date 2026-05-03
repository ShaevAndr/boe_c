/*=============================================================================
  Project: 
  Platform: GD32F470ZI
  Filename: drv_InternalFMC.h
  Description:
  Programmer: 
  Version: 1.0
  Created: 2006.01.16
  Last modified: 2025.04.06
=============================================================================*/
#ifndef __drv_InternalFMC_H__
#define __drv_InternalFMC_H__
	//-----------------------------------------------------------------------------
	#include "stdint.h"
	//-----------------------------------------------------------------------------
	int32_t GetInternalFlashPageCount (void);
	int32_t GetInternalFlashPageSize (void);
	int32_t GetInternalFlashDescription (uint8_t * p);
	int32_t ReadInternalFlashPage (uint32_t PageNum, uint8_t * p);
	int32_t WriteInternalFlashPage (uint32_t PageNum, uint8_t * p);
	//-----------------------------------------------------------------------------
#endif /* drv_InternalFMC_H */
