/*=============================================================================
  Project:
  Platform: GD32F470ZI
  Filename: drv_InternalFMC.c
  Description:
  Programmer:
  Version: 2.0
  Created: 2006.01.16
  Last modified: 2025.04.06
=============================================================================*/
#include <stdint.h>
#include <stdio.h>
//-----------------------------------------------------------------------------
#include "drv_InternalFMC.h"
#include "Global.h"
//--------------------------------------------------------------------------//
#ifdef __BootLoader__
	#define _FlashStartAddr (_StartApplicationAddress)
	#define _FlashSize (_SizeApplication)
	#define _FlashPageSize (4 * 1024)
#elif defined(__Aplication__)
	#define _FlashStartAddr (_StartBootLoaderAddress)
	#define _FlashSize (_SizeBootLoader)
	#define _FlashPageSize (4 * 1024)
#else
#error "Bootloader or Application must be specified"
#endif
#define _FlashPageCount (_FlashSize / _FlashPageSize)
//--------------------------------------------------------------------------//
int32_t GetInternalFlashPageCount (void) {return (_FlashPageCount);}
//--------------------------------------------------------------------------//
int32_t GetInternalFlashPageSize (void) {return (_FlashPageSize);}
//--------------------------------------------------------------------------//
int32_t GetInternalFlashDescription (uint8_t * p)
{
	#ifdef __BootLoader__
		return (sprintf ((char *)p, "Application"));
	#endif
	#ifdef __Aplication__
		return (sprintf ((char *)p, "Bootloader"));
	#endif
}
//--------------------------------------------------------------------------//
int32_t ReadInternalFlashPage (uint32_t PageNum, uint8_t * p)
{
	int i;
	uint8_t * pFlash;
	if (PageNum >= _FlashPageCount) return (-2);
	if (p == NULL) return (-3);
	pFlash = (uint8_t *)(_FlashStartAddr + PageNum * _FlashPageSize);
	for (i = 0; i < _FlashPageSize; i++)
		p [i] = *pFlash++;
	return (0);
}
//--------------------------------------------------------------------------//
int32_t WriteInternalFlashPage (uint32_t PageNum, uint8_t * p)
{
	int i;
	uint8_t * pFlash;
	int32_t res;
	if (PageNum >= _FlashPageCount) return (-2);
	if (p == NULL) return (-3);
	pFlash = (uint8_t *)(_FlashStartAddr + PageNum * _FlashPageSize);
	fmc_unlock ();
	res = fmc_page_erase ((uint32_t)pFlash);	
	if (res == FMC_READY)
		for (i = 0; i < _FlashPageSize; i++)
		{
			res = fmc_byte_program ((uint32_t)(pFlash++), p [i]);
			if (res != FMC_READY)
				break;
		}
	fmc_lock ();
	if (res != FMC_READY) return (-1);
	return (0);
}
//--------------------------------------------------------------------------//
