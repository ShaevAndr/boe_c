/*=============================================================================
     Project:
     Platform: GD32F470
     Filename: ReadDeviceIdentification.h
     Description: FC 0x08 - Read Device Identification
     Version: 1.0
     Created: 2026.06.22
============================================================================*/
#ifndef READ_DEVICE_IDENTIFICATION_H
#define READ_DEVICE_IDENTIFICATION_H

#include <stdint.h>

uint8_t ReadDeviceIdentification(
	uint8_t NumUART,
	uint8_t Command,
	uint8_t *B,
	uint32_t *pSize);

#endif
