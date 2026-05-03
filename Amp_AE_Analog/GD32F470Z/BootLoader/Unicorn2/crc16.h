/*=============================================================================
2     Project: 
3     Platform: 
4     Filename: crc16.h
5     Description:
6     Version: 0.0
7     Created: 2017.11.23
8     Last modified: 2017.11.25
9===========================================================================*/
#ifndef crc16_H
#define crc16_H

	#include <stdint.h>
	//-----------------------------------------------------------------------------
	uint16_t CRC16 (const void *nData, uint32_t wLength);
	uint16_t CRC16_CCITT16 (const void *nData, uint32_t wLength);
	//-----------------------------------------------------------------------------

#endif
