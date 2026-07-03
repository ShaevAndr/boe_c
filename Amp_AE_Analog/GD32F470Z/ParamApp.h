/*=============================================================================
  Project: 
  Platform: 
  Filename: ParamApp.h
  Description:
  Programmer:
  Version: 1.0
  Created: 2024.05.30
  Last modified: 2024.05.30
=============================================================================*/
#ifndef __ParamApp_H__
#define __ParamApp_H__
//-----------------------------------------------------------------------------
#include <stdint.h>
//-----------------------------------------------------------------------------
//#include "drv_DIO.h"
//#include "DataProcess.h"
#include "lfs_storage.h"
#include "drv_DS18B20.h"
#include "drv_EEPROM.h"
#include "Unicorn2Routine.h"
//-----------------------------------------------------------------------------
#ifdef STRUCT_TO_STR
  #ifndef DECLARESTRUCT
  #define DECLARESTRUCT(X, NAME) X; uint8_t NAME[]=#X;
  #endif
#else
  #ifndef DECLARESTRUCT
  #define DECLARESTRUCT(X, NAME) X;
  #endif
#endif

#pragma pack(1)
DECLARESTRUCT
(
typedef struct
{
	uint32_t 	hashFNV;
	
	DS_Location_t DSLoc [_NumDS18B20];
	float ICPMinVoltage;
  int8_t Ku_OP [8];

	uint16_t	CRC16;
} TParamApp;
, structTParamApp)
#pragma pack()
//-----------------------------------------------------------------------------
extern TParamApp gParamApp;
//--------------------------------------------------------------------------//
int32_t GetFlashCount (void);
int32_t GetFlashPageCount (uint32_t FlashNum);
int32_t GetFlashPageSize (uint32_t FlashNum);
int32_t GetFlashDescription (uint32_t FlashNum, uint8_t * p);
int32_t ReadFlashPage (uint32_t FlashNum, uint32_t PageNum, uint8_t * p);
int32_t WriteFlashPage (uint32_t FlashNum, uint32_t PageNum, uint8_t * p);
//-----------------------------------------------------------------------------
uint32_t GetCFG_size (void);
uint32_t GetCFG_curr_addr (void);
//-----------------------------------------------------------------------------
void SaveParamApp (void);
void LoadParamApp (lfs_t *storage);
void RoutineSaveParamApp (void);
//-----------------------------------------------------------------------------
void SetStateSaveParam (int32_t val);
int32_t GetStateSaveParam (void);
//void WriteToFlashParam (const TMTDDevice *mtdDev); // for test
//-----------------------------------------------------------------------------
//uint32_t FNV1Hash (uint8_t *buf);
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//uint8_t CheckEmptyUUID (void);
//-----------------------------------------------------------------------------
//TUUID GetUUID (void);
//-----------------------------------------------------------------------------
//void SetUUID (TUUID d);
//-----------------------------------------------------------------------------
#endif /*__ParamSaved_H__ */
