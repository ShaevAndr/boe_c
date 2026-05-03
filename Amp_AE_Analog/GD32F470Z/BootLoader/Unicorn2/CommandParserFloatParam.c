/*=============================================================================
2     Project: 
3     Platform: GD32F407
4     Filename: CommandParserFloatParam.c
5     Description:
6     Version: 0.0
7     Created: 2022.09.15
8     Last modified: 2023.06.28
9============================================================================*/
#include <stdint.h>
#include <stdio.h>
//--------------------------------------------------------------------------//
#include "AccessFloatParam.h"
#include "CommandList.h"
#include "CommandParser.h"
#include "CommandParserFunction.h"
#include "CommandParserFloatParam.h"
//--------------------------------------------------------------------------//
static void PasedBuff (uint8_t * B, uint32_t NParam, float Param)
{
	Pased_float_to_Buff (&(B [sizeof (float) * NParam]), Param);
}
//--------------------------------------------------------------------------//
static uint8_t ReadDescrFloatParam (uint8_t NumUART, uint32_t NumParam,
																		uint8_t * B, uint32_t * pSize)
{
  uint8_t ErrorNum = _NoError;
	DescriptParam_t DP = {.formatStr = "", .name = "", .uniqueId = "",
                        .unit = "", .maxValuei = 0, .minValuei = 0,
                        .maxValuef = 0.0f, .minValuef = 0.0f};
	(void)NumUART;
	ErrorNum = AccessDescFloatParam (NumParam, &DP);
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
    *pSize += sprintf ((char *)(B + *pSize), "%f;%f", DP.minValuef, DP.maxValuef);
		*pSize += 1;
		*pSize += sprintf ((char *)(B + *pSize), "%s", DP.access);
		*pSize += 1;
	}
	return (ErrorNum);
}
//--------------------------------------------------------------------------//
uint8_t FloatParam (uint8_t NumUART, uint8_t Command,
										uint8_t * B, uint32_t * pSize)
{
  uint8_t ErrorNum = _NoError;
	FloatParam_t ParamNum = (FloatParam_t)Pased_Buff_to_uint32_t (B);
	float val = Pased_Buff_to_float (B + 4);
	int i;
	switch (Command)
	{	
    case _WriteOneFloatParam:
      if (*pSize != 10) {ErrorNum = _ErrorSize; break;}
			ErrorNum = AccessFloatParam (ParamNum, &val, _PAM_RW);
			if (_NoError == ErrorNum) Pased_float_to_Buff (B + 4, val);
      *pSize = 10;
      break;
      
    case _WriteAllFloatParam:
      ErrorNum = _ErrorComandImposWork;
      break;
      
    case _GetDescrOneFloatParam:
      if (*pSize != 6) {ErrorNum = _ErrorSize; break;}
			ErrorNum = ReadDescrFloatParam (NumUART, ParamNum, B + 4, pSize);
			*pSize += 6;
      break;
      
    case _ReadOneFloatParam:
      if (*pSize != 6) {ErrorNum = _ErrorSize; break;}
			ErrorNum = AccessFloatParam (ParamNum, &val, _PAM_RO);
			Pased_float_to_Buff (B + 4, val);
			*pSize = 10;
      break;
      
    case _ReadAllFloatParam:
      if (*pSize != 2) {ErrorNum = _ErrorSize; break;}
			*pSize = 0;
			if (!_FPCount) {ErrorNum = _ErrorUnCorrParam; break;}
			for (i = 0; i < _FPCount; i++)
			{
				ErrorNum = AccessFloatParam ((FloatParam_t) i, &val, _PAM_RO);
				PasedBuff (B, i, val);
				if (ErrorNum != _NoError) break;
			}
			*pSize = _FPCount * sizeof(float);
			*pSize += 2;
      break;
	}
	if (_NoError != ErrorNum) *pSize = 0;
	return (ErrorNum);
}
//--------------------------------------------------------------------------//
