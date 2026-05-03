#ifndef __main_bootloader_H__
#define __main_bootloader_H__

	#include "stdint.h"
	#include "stdbool.h"
	#include "drv_EEPROM.h"
	//void DoInit (void);
	void Pre_Init (void);

	uint64_t GetMACAddr (void);
	void SetEnableLoadApp (int Enable);
	bool GetEnableLoadApp (void);
	
	extern TMTDDevice SPmtdDev;

#endif /* __main_bootloader_H__ */
