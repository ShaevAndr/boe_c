/*=============================================================================
2     Project: 
3     Platform: GD32F470
4     Filename: AccessTabParam.c
5     Description:
6     Version: 0.0
7     Created: 2022.09.15
8     Last modified: 2026.02.08
9============================================================================*/

#include <stdint.h>

#include "LockUnlock.h"
#include "CommandList.h"
#include "CommandParser.h"
#include "CommandParserFunction.h"
#include "CommandParserTabParam.h"
#include "AccessTabParam.h"

uint8_t ReadTabParam (uint8_t NumUART, uint32_t NumParam, uint8_t * B,
											uint32_t offset, uint32_t wordSize, uint32_t stride,
											uint32_t * size)
{
  uint8_t ErrorNum = _NoError;
	(void)NumUART;
	switch (NumParam)
	{
		case _tblpRS485Params:
			ErrorNum = ReadTabP_DataHistorySyncTime (B, offset, wordSize, stride, size);
			break;

		default: ErrorNum = _ErrorUnCorrParam;	break;
	}
	return (ErrorNum);
}
//--------------------------------------------------------------------------//
uint8_t WriteTabParam (uint8_t NumUART, uint32_t NumParam, uint8_t * B,
															uint32_t offset, uint32_t stride, uint32_t size)
{
  uint8_t ErrorNum = _NoError;
  
	(void)NumUART;
	switch (NumParam)
	{
		case _tblpRS485Params:
			ErrorNum = WriteTabP_FactoryCalibrationParameters (B, offset, stride, size);
			break;
		

		default: ErrorNum = _ErrorUnCorrParam; break;
	}
//	if (_NoError == ErrorNum) Pased_int32_t_to_Buff (B, val);
	return (ErrorNum);
}
//--------------------------------------------------------------------------//