#ifndef __drv_Unicorn2EEPROM_H__
#define __drv_Unicorn2EEPROM_H__

	#include "stdint.h"

	int32_t GetSettingFlashPageCount (void);
	int32_t GetSettingFlashPageSize (void);
	int32_t GetSettingFlashDescription (uint8_t * p);
	int32_t ReadSettingFlashPage (uint32_t PageNum, uint8_t * p);
	int32_t WriteSettingFlashPage (uint32_t PageNum, uint8_t * p);

#endif /* __drv_Unicorn2EEPROM_H__ */
