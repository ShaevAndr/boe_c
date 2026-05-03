/*=============================================================================
2     Project: 
3     Platform: GD32F407
4     Filename: CommandParserTelemParam.h
5     Description:
6     Version: 0.0
7     Created: 2022.09.15
8     Last modified: 2023.06.28
9============================================================================*/
#ifndef CommandParserTelemParam_H
#define CommandParserTelemParam_H
	//------------------------------------------------------------------------------
	#include <stdint.h>
	//------------------------------------------------------------------------------
	uint8_t TelemParam (uint8_t NumUART, uint8_t Command,
											uint8_t * B, uint32_t * pSize);
	//--------------------------------------------------------------------------//
#endif
