/*=============================================================================
  Project:
  Platform: GD32F470Z
  Filename: drv_1W.h
  Description:
  Programmer:
  Version: 0.0
  Created: 2022.05.12
  Last modified: 2024.02.11
=============================================================================*/
#ifndef __drv_1W_H__
#define __drv_1W_H__
	//--------------------------------------------------------------------------//
	#include <stdbool.h>
	#include <stdint.h>
	//--------------------------------------------------------------------------//
	void OneWire_ini (void);
	
	void USART2_IRQHandler (void);

	bool OneWire_Reset (void);
	bool OneWire_ReadBit (void);
	void OneWire_WriteByte (uint8_t v);
	void OneWire_WriteBytes (const uint8_t *buf, uint8_t count);
	uint8_t OneWire_ReadByte (void);
	void OneWire_SelectROM (const uint8_t rom[8]);
	void OneWire_ReadBytes (uint8_t *buf, uint8_t count);

	void OneWire_StartReset_DMA (void);
	bool OneWire_ReadyReset_DMA (bool * Present);
	void OneWire_StartReadBit_DMA (void);
	bool OneWire_ReadyReadBit_DMA (bool * Bit);
	bool OneWire_StartByteExchangeDMA (uint8_t *buf, uint32_t count);
	bool OneWire_ReadyByteExchangeDMA (uint8_t *buf, uint32_t count);

	void OneWire_ResetSearch (void);
	bool OneWire_Search (uint8_t *newAddr);
	void OneWire_SkipROM (void);
	uint8_t OneWire_crc8 (const uint8_t *addr, uint8_t len);
	//--------------------------------------------------------------------------//
#endif /*__drv_1W_H__*/
