/*=============================================================================
2     Project: Amp_AE
3     Platform: BF533
4     Filename: CommandParserTelemParam.c
5     Description:
6     Version: 0.0
7     Created: 2022.09.15
8     Last modified: 2022.09.15
9============================================================================*/
#include <stdint.h>
#include <stdio.h>
//--------------------------------------------------------------------------//
#include "drv_time.h"
#include "CommandList.h"
#include "CommandParser.h"
#include "CommandParserFunction.h"
#include "CommandParserTelemParam.h"

#include "AccessTelemParam.h"
//--------------------------------------------------------------------------//
static void PB (uint8_t * B, float Param)
{
	Pased_float_to_Buff (B, Param);
}
//--------------------------------------------------------------------------//
static void PBN (uint8_t * B, uint32_t NParam, float Param)
{
	Pased_float_to_Buff (&(B [sizeof (float) * NParam]), Param);
}
//--------------------------------------------------------------------------//
static uint8_t ReadDescrTelemParam (uint8_t NumUART, uint32_t NumParam,
    uint8_t * B, uint32_t * pSize)
{
  uint8_t ErrorNum = _NoError;
	DescriptParam_t DP = {.formatStr = "", .name = "", .uniqueId = "",
                        .unit = "", .maxValuei = 0, .minValuei = 0,
                        .maxValuef = 0.0f, .minValuef = 0.0f};
	(void)NumUART;
	return (_ErrorUnKnowComand);
/*
	ErrorNum = AccessDescTelemParam (NumParam, &DP);
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
	}
	return (ErrorNum);
*/
}
//--------------------------------------------------------------------------//
uint8_t TelemParam (uint8_t NumUART, uint8_t Command,
										uint8_t * B, uint32_t * pSize)
{
  uint8_t ErrorNum = _NoError;
	TelemParam_t ParamNum = (TelemParam_t)Pased_Buff_to_uint32_t (B);
	float val = Pased_Buff_to_float (B + 4);
	int i;
	switch (Command)
	{	
    case _GetDescrOneTelemParam:
      if (*pSize != 6) {ErrorNum = _ErrorSize; break;}
			ErrorNum = ReadDescrTelemParam (NumUART, ParamNum, B + 4, pSize);
			*pSize += 6;
      break;
      
    case _ReadOneTelemParam:
      if (*pSize != 6) {ErrorNum = _ErrorSize; break;}
			ErrorNum = AccessTelemParam (ParamNum, &val);
			Pased_float_to_Buff (B + 4, val);
			*pSize = 10;
      break;
      
    case _ReadAllTelemParam:
      if (*pSize != 2) {ErrorNum = _ErrorSize; break;}
			*pSize = 0;
			if (!_TelPCount) {ErrorNum = _ErrorUnCorrParam; break;}
			for (i = 0; i < _TelPCount; i++)
			{
				ErrorNum = AccessTelemParam ((TelemParam_t) i, &val);
				PBN (B, i, val);
				if (ErrorNum != _NoError) break;
			}
			*pSize = _TelPCount * sizeof(float);
			*pSize += 2;
      break;
	}
	if (_NoError != ErrorNum) *pSize = 0;
	return (ErrorNum);
}
//--------------------------------------------------------------------------//
