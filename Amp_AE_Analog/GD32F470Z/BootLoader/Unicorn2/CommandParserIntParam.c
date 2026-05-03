/*=============================================================================
2     Project: 
3     Platform: GD32F407
4     Filename: CommandParserIntParam.c
5     Description:
6     Version: 0.0
7     Created: 2022.09.15
8     Last modified: 2023.06.28
9============================================================================*/
#include <stdint.h>
#include <stdio.h>
//--------------------------------------------------------------------------//
#include "CommandList.h"
#include "CommandParser.h"
#include "AccessIntParam.h"
#include "CommandParserFunction.h"
#include "CommandParserIntParam.h"
//--------------------------------------------------------------------------//
static void PasedBuff (uint8_t * B, uint32_t NParam, int32_t Param)
{
	Pased_int32_t_to_Buff (&(B [sizeof (int32_t) * NParam]), Param);
}
//--------------------------------------------------------------------------//
static uint8_t ReadDescrIntParam (uint8_t NumUART, uint32_t NumParam,
																	uint8_t * B, uint32_t * pSize)
{
  uint8_t ErrorNum = _NoError;
	DescriptParam_t DP = {.formatStr = "", .name = "", .uniqueId = "",
                        .unit = "", .maxValuei = 0, .minValuei = 0,
                        .maxValuef = 0.0f, .minValuef = 0.0f};
	(void)NumUART;
	return (_ErrorUnKnowComand);
/*
	ErrorNum = AccessDescIntParam (NumParam, &DP);
	*pSize = 0;
	if (ErrorNum == _NoError)
	{
		*pSize =  sprintf ((char *)(B + *pSize), "%s", DP.name);
		*pSize += 1;
		*pSize += sprintf ((char *)(B + *pSize), "%s", DP.unit);
		*pSize += 1;
		*pSize += sprintf ((char *)(B + *pSize), "%s", DP.formatStr);
		*pSize += 1;
		*pSize += sprintf ((char *)(B + *pSize), "%s", DP.uniqueId);
		*pSize += 1;
    *pSize += sprintf ((char *)(B + *pSize), "%d;%d", DP.minValuei, DP.maxValuei);
		*pSize += 1;
		*pSize += sprintf ((char *)(B + *pSize), "%s", DP.access);
		*pSize += 1;
	}
	return (ErrorNum);
*/
}
//--------------------------------------------------------------------------//
uint8_t IntParam (uint8_t NumUART, uint8_t Command,
										uint8_t * B, uint32_t * pSize)
{
  uint8_t ErrorNum = _NoError;
	IntParam_t ParamNum = (IntParam_t)Pased_Buff_to_uint32_t (B);
	int32_t val = Pased_Buff_to_uint32_t (B + 4);
	int i;

	switch (Command)
	{	
    case _WriteOneIntParam:
      if (*pSize != 10) {ErrorNum = _ErrorSize; break;}
			ErrorNum = AccessIntParam (ParamNum, &val, _PAM_RW);
			if (_NoError == ErrorNum) Pased_int32_t_to_Buff (B + 4, val);
      *pSize = 10;
      break;
      
    case _WriteAllIntParam:
      ErrorNum = _ErrorComandImposWork;
      break;
      
    case _GetDescrOneIntParam:
      if (*pSize != 6) {ErrorNum = _ErrorSize; break;}
			ErrorNum = ReadDescrIntParam (NumUART, ParamNum, B + 4, pSize);
			*pSize += 6;
      break;
      
    case _ReadOneIntParam:
      if (*pSize != 6) {ErrorNum = _ErrorSize; break;}
			ErrorNum = AccessIntParam ((IntParam_t) ParamNum, &val, _PAM_RO);
			Pased_int32_t_to_Buff (B + 4, val);
			*pSize = 10;
      break;
      
    case _ReadAllIntParam:
      if (*pSize != 2) {ErrorNum = _ErrorSize; break;}
			*pSize = 0;
			if (!_IPCount) {ErrorNum = _ErrorUnCorrParam; break;}
			for (i = 0; i < _IPCount; i++)
			{
				ErrorNum = AccessIntParam ((IntParam_t) i, &val, _PAM_RO);
				PasedBuff (B, i, val);
				if (ErrorNum != _NoError) break;
			}
			*pSize = _IPCount * sizeof(float);
			*pSize += 2;
      break;
	}
	if (_NoError != ErrorNum) *pSize = 0;
	return (ErrorNum);
}
//--------------------------------------------------------------------------//
