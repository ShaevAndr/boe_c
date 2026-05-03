/*=============================================================================
2     Project: 
3     Platform: GD32F407
4     Filename: CommandParserTabParam.h
5     Description:
6     Version: 0.0
7     Created: 2023.08.31
8     Last modified: 2023.08.31
9============================================================================*/
#ifndef CommandParserTabParam_H
#define CommandParserTabParam_H
	//------------------------------------------------------------------------------
	#include <stdint.h>
	//------------------------------------------------------------------------------
	uint8_t TabParam (uint8_t NumUART, uint8_t Command,	uint8_t * B,
										uint32_t * pSize);
	//--------------------------------------------------------------------------//
#endif
