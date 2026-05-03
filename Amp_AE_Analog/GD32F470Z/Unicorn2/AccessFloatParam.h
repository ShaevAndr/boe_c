/*=============================================================================
2     Project: 
3     Platform: STM32G743
4     Filename: AccessFloatParam.h
5     Description:
6     Version: 0.0
7     Created: 2025.01.28
8     Last modified: 2025.01.28
9============================================================================*/
#ifndef AccessFloatParam_H
#define AccessFloatParam_H
	//--------------------------------------------------------------------------//
	#include <stdint.h>
	//--------------------------------------------------------------------------//
	#include "CommandList.h"
	#include "CommandParser.h"
	//--------------------------------------------------------------------------//
	int8_t AccessFloatParam (FloatParam_t NumParam, float * Value, ParamAccessMode_t AcessMode);
	int8_t AccessDescFloatParam (FloatParam_t NumParam, DescriptParam_t * P);
	//--------------------------------------------------------------------------//
#endif //AccessFloatParam_H