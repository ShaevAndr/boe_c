/*=============================================================================
2     Project: 
3     Platform: 
4     Filename: WriteMultipleRegisters.h
5     Description:
6     Version: 0.0
7     Created: 03.05.26
8     Last modified: 2017.11.23
9============================================================================*/

#ifndef WriteMultipleRegisters_H
#define WriteMultipleRegisters_H
#include <stdint.h>

uint8_t WriteMultipleRegisters (uint8_t NumUART, uint8_t Command,	uint8_t * B, uint32_t * pSize);
#endif