/*=============================================================================
  Project: 
  Platform: GD32F470Z
  Filename: drv_DS18B20.h
  Description:
  Programmer: 
  Version: 0.0
  Created: 2022.05.12
  Last modified: 2024.02.11
=============================================================================*/
#ifndef __drv_DS18B20_H__
#define __drv_DS18B20_H__
	//--------------------------------------------------------------------------//
	#include <stdint.h>
	#include <stdbool.h>
	//--------------------------------------------------------------------------//
	//#define _NumDS18B20 (4)
	//--------------------------------------------------------------------------//
	typedef enum {_DSLocIP = 0, _DSLocCore = 1, _NumDS18B20} DS_Location_t;
	//--------------------------------------------------------------------------//
	void DS18B20_ini (void);

	bool DS18B20NormalWork (uint8_t Num);
	
	float DS18B20_GetTemperature (uint8_t Num);
	float DS18B20_GetTempLocation (DS_Location_t Loc);
	DS_Location_t DS18B20_GetDSLocation (uint8_t Num);
	void DS18B20_SetDSLocation (uint8_t Num, DS_Location_t Loc);
	void DS18B20_Routine (void);
	float DS18B20_GetMaxTemperature (void);
	//--------------------------------------------------------------------------//
#endif /*__drv_DS18B20_H__*/
