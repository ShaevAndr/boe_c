/*=============================================================================
  Project: ADCVibro
  Platform: GD32F470Z
  Filename: drv_EEPROM.h
  Description:
  Programmer: 
  Version: 1.0
  Created: 2024.03.23
  Last modified: 2024.03.23
=============================================================================*/
#ifndef __drv_EEPROM_H__
#define __drv_EEPROM_H__
	//--------------------------------------------------------------------------//
	#include <stdbool.h>
	#include <stdint.h>
	//--------------------------------------------------------------------------//
	#include "drv_SPI.h"
	//--------------------------------------------------------------------------//
	#define _SIZE_BUFFER_FLASH 512
	#define _MAX_PAGE_FLASH 4095
	//--------------------------------------------------------------------------//

	enum MTDManufacturer {
		MTDManufUnknown,
//		MTDManufRamtron,
//		MTDManufCypress,
		MTDManufMicrochip,
		MTDManufSTMicroelectronics
	};
	//--------------------------------------------------------------------------//
	enum MTDAlgorithm {
		MTDAlgoUnknown,
		MTDAlgoFRAM25,
		MTDAlgoEEPROM25,
//		MTDAlgoFlash25,
	};
	//--------------------------------------------------------------------------//
	typedef struct {
		TSPINUM		busNum;
		TSPICS		csNum;
		TSPIMODE	spiMode;
		int32_t		freq, blockCycles;
		uint8_t		manufacturerIDbank, manufacturerID;
		uint16_t	deviceID;
		bool		serialNumberAvailable;
		uint32_t	serialNumberAddr;
		uint32_t	serialNumberSize;
		uint32_t	size, blockSize, blocksCount, readChunkSize, writeChunkSize;
		enum MTDManufacturer	manuf;
		enum MTDAlgorithm			algo;
		uint8_t		addrSize, eraseCmd;
	} TMTDDevice;
	
//	typedef enum {_STM_M95256 = 0, _MC_25AA02E48 = 1} TEEPROMType;
	typedef enum {_STM_M95256 = 0} TEEPROMType;
	//--------------------------------------------------------------------------//
	void initMTD (TMTDDevice *mtdDev, TSPINUM busNum, TSPICS csNum, int32_t freq, TSPIMODE spiMode, TEEPROMType type);
//	extern int mtdProbeJedec (TMTDDevice *mtdDev);
	int mtdRead (const TMTDDevice *mtdDev, uint32_t addr, void *buf, uint32_t size);
	int mtdWrite (const TMTDDevice *mtdDev, uint32_t addr, const void *buf, uint32_t size);
	int mtdReadSerial (const TMTDDevice *mtdDev, void *buf, uint32_t size);
	//--------------------------------------------------------------------------//
#endif /*__drv_EEPROM_H__*/
