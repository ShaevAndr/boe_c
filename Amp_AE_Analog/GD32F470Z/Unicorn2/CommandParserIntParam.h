/*=============================================================================
2     Project: 
3     Platform: GD32F407
4     Filename: CommandParserIntParam.h
5     Description:
6     Version: 0.0
7     Created: 2022.09.15
8     Last modified: 2023.06.28
9============================================================================*/
#ifndef CommandParserIntParam_H
#define CommandParserIntParam_H
	//------------------------------------------------------------------------------
	#include <stdint.h>
	//------------------------------------------------------------------------------
	uint8_t IntParam (uint8_t NumUART, uint8_t Command,	uint8_t * B,
										uint32_t * pSize);
	//--------------------------------------------------------------------------//
#endif
