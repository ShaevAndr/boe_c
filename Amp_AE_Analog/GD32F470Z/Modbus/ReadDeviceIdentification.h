/*=============================================================================
     Project:
     Platform: GD32F470
     Filename: ReadDeviceIdentification.h
     Description: FC 0x2B / MEI 0x0E - Read Device Identification
     Version: 2.0
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
