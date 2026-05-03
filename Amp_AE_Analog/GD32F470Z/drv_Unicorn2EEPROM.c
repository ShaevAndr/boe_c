

//--------------------------------------------------------------------------//
#include "gd32f4xx.h"
#include <stdio.h>
//--------------------------------------------------------------------------//
//#include "Global.h"
#include "drv_EEPROM.h"
#include "..\main_application.h"
//--------------------------------------------------------------------------//
int32_t GetSettingFlashPageCount (void) {return (SPmtdDev.blocksCount);}
//--------------------------------------------------------------------------//
int32_t GetSettingFlashPageSize (void) {return (SPmtdDev.blockSize);}
//--------------------------------------------------------------------------//
int32_t GetSettingFlashDescription (uint8_t * p)
{
	return (sprintf ((char *)p, "Flash memory for saving settings"));
}
//--------------------------------------------------------------------------//
int32_t ReadSettingFlashPage (uint32_t PageNum, uint8_t * p)
{
	if (PageNum >= GetSettingFlashPageCount ()) return (-1);
	return (mtdRead (&SPmtdDev, PageNum * GetSettingFlashPageSize (), p, GetSettingFlashPageSize ()));
}
//--------------------------------------------------------------------------//
int32_t WriteSettingFlashPage (uint32_t PageNum, uint8_t * p)
{
	if (PageNum >= GetSettingFlashPageCount ()) return (-1);
	return (mtdWrite (&SPmtdDev, PageNum * GetSettingFlashPageSize (), p, GetSettingFlashPageSize ()));
}
//--------------------------------------------------------------------------//

