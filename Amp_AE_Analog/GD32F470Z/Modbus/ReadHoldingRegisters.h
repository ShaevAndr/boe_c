/*=============================================================================
2     Project: 
3     Platform: 
4     Filename: ReadHoldingRegisters.h
5     Description:
6     Version: 0.0
7     Created: 03.05.26
8     Last modified: 2017.11.23
9============================================================================*/

#ifndef ReadHoldingRegisters_H
#define ReadHoldingRegisters_H
#include <stdint.h>

uint8_t ReadHoldingsRegisters (uint8_t NumUART, uint8_t Command,	uint8_t * B, uint32_t * pSize);
#endif