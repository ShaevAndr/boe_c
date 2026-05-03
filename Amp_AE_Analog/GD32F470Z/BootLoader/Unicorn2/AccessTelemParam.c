/*=============================================================================
2     Project: 
3     Platform: GD32F407
4     Filename: AcessTelemParam.c
5     Description:
6     Version: 0.0
7     Created: 2025.01.28
8     Last modified: 2025.01.28
9============================================================================*/
#include <stdint.h>
#include <string.h>
//--------------------------------------------------------------------------//
#include "drv_time.h"
//--------------------------------------------------------------------------//
#include "CommandList.h"
#include "CommandParser.h"
#include "AccessTelemParam.h"
//--------------------------------------------------------------------------//
int8_t AccessTelemParam (TelemParam_t NumParam, float * Value)
{
	int8_t Ret = _NoError;
	switch (NumParam)
	{
//		case tpLatticeUptime:			*Value = fGetTime ();			break;
		case tpCoreUptime:				*Value = fGetTime ();			break;

		default: Ret = _ErrorUnCorrParam; break;
	}
	return (Ret);
}
//--------------------------------------------------------------------------//
/*
int8_t AccessDescTelemParam (TelemParam_t NumParam, DescriptParam_t * P)
{
	int8_t Ret = _NoError;
	strncpy (P->name, "", sizeof (P->name));
	strncpy (P->unit, "", sizeof (P->unit));
	strncpy (P->formatStr, "", sizeof (P->formatStr));
	strncpy (P->uniqueId, "", sizeof (P->uniqueId));
	strncpy (P->access, "", sizeof (P->access));
	P->minValuef = 0.0f;
	P->maxValuef = 0.0f;
	
	switch (NumParam)
	{
		case tpLatticeUptime:
			strncpy (P->name, "Lattice uptime", sizeof (P->name));
			strncpy (P->unit, "sec", sizeof (P->unit));
			strncpy (P->formatStr, "%.2f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "LatticeUptime", sizeof (P->uniqueId));
			P->minValuef = 0.0f;
			P->maxValuef = 1.84467440737E+19f;
			break;
		case tpCoreUptime:
			strncpy (P->name, "Core uptime", sizeof (P->name));
			strncpy (P->unit, "sec", sizeof (P->unit));
			strncpy (P->formatStr, "%.0f", sizeof (P->formatStr));
			strncpy (P->uniqueId, "CoreUptime", sizeof (P->uniqueId));
			P->minValuef = 0.0f;
			P->maxValuef = 1.84467440737E+19f;
			break;
		default: Ret = _ErrorUnCorrParam; break;
	}
	return (Ret);
}
*/
