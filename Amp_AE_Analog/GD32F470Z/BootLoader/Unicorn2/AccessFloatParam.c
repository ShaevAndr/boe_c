/*=============================================================================
2     Project: 
3     Platform: GD32F407
4     Filename: CommandParserFloatParam.c
5     Description:
6     Version: 0.0
7     Created: 2025.01.28
8     Last modified: 2025.01.28
9============================================================================*/
#include <stdint.h>
#include <string.h>
//--------------------------------------------------------------------------//
#include "CommandParser.h"
#include "AccessFloatParam.h"
//--------------------------------------------------------------------------//
int8_t AccessFloatParam (FloatParam_t NumParam, float * Value, ParamAccessMode_t AcessMode)
{
	int8_t Ret = _NoError;
	switch (NumParam)
	{
		default:
			Ret = _ErrorUnCorrParam;
			break;
	}
	return (Ret);
}
//--------------------------------------------------------------------------//
int8_t AccessDescFloatParam (FloatParam_t NumParam, DescriptParam_t * P)
{
	int8_t Ret = _NoError;
	strncpy (P->name, "", sizeof (P->name));
	strncpy (P->unit, "", sizeof (P->unit));
	strncpy (P->formatStr, "", sizeof (P->formatStr));
	strncpy (P->uniqueId, "", sizeof (P->uniqueId));
	strncpy (P->access, "RW", sizeof (P->access));
	P->minValuef = 0.0f;
	P->maxValuef = 0.0f;
	P->minValuei = 0;
	P->maxValuei = 0;
	
	switch (NumParam)
	{
		default: Ret = _ErrorUnCorrParam; break;
	}
	return (Ret);
}
