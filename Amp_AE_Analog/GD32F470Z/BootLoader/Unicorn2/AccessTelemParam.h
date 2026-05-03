/*=============================================================================
2     Project: 
3     Platform: STM32G743
4     Filename: AccessTelemParam.h
5     Description:
6     Version: 0.0
7     Created: 2025.01.28
8     Last modified: 2025.01.28
9============================================================================*/
#ifndef AccessTelemParam_H
#define AccessTelemParam_H
	//--------------------------------------------------------------------------//
	#include <stdint.h>
	//--------------------------------------------------------------------------//
	#include "CommandList.h"
	#include "CommandParser.h"
	//--------------------------------------------------------------------------//
	int8_t AccessTelemParam (TelemParam_t NumParam, float * Value);
//	int8_t AccessDescTelemParam (TelemParam_t NumParam, DescriptParam_t * P);
	//--------------------------------------------------------------------------//
#endif //AccessTelemParam_H