/*=============================================================================
2     Project:
3     Platform: GD32F470
4     Filename: CommandParserTabParam.c
5     Description: Parser for table parameter commands
6     Version: 1.0
7     Created: 2022.09.15
8     Last modified: 2026.07.05
============================================================================*/

#include <stdint.h>

#include "CommandList.h"
#include "CommandParser.h"
#include "CommandParserFunction.h"
#include "AccessTabParam.h"
#include "CommandParserTabParam.h"
#include "crc16.h"

uint8_t TabParam(uint8_t NumUART, uint8_t Command, uint8_t *B,
	uint32_t *pSize)
{
	uint8_t ErrorNum = _NoError;
	uint32_t ParamNum;
	uint32_t offset;
	uint32_t size;
	uint32_t wordSize;
	uint32_t stride;
	uint16_t sCRC;
	int32_t mode;
	int32_t column;
	int32_t rows;
	int32_t curStep;
	int32_t stepsCount;

	if ((B == 0) || (pSize == 0))
		return _ErrorSize;

	switch (Command)
	{
		case _RequestNumTabParam:
			if (*pSize != 0U)
			{
				ErrorNum = _ErrorSize;
				break;
			}
			Pased_uint32_t_to_Buff(B, _TblPCount);
			*pSize = 4U;
			break;

		case _GetDescrTabParam:
			if (*pSize != 4U)
			{
				ErrorNum = _ErrorSize;
				break;
			}
			ParamNum = Pased_Buff_to_uint32_t(B);
			ErrorNum = ReadDescrTabParam(NumUART, ParamNum, B, pSize);
			break;

		case _ReadTabParamData:
			if (*pSize != (4U * 5U))
			{
				ErrorNum = _ErrorSize;
				break;
			}
			ParamNum = Pased_Buff_to_uint32_t(B);
			offset = Pased_Buff_to_uint32_t(B + 4U);
			size = Pased_Buff_to_uint32_t(B + 8U);
			wordSize = Pased_Buff_to_uint32_t(B + 12U);
			stride = Pased_Buff_to_uint32_t(B + 16U);

			ErrorNum = ReadTabParam(NumUART, ParamNum, B + 12U,
				offset, wordSize, stride, &size);
			if (ErrorNum != _NoError)
				break;

			Pased_uint32_t_to_Buff(B, ParamNum);
			Pased_uint32_t_to_Buff(B + 4U, offset);
			Pased_uint32_t_to_Buff(B + 8U, size);
			size += 12U;
			sCRC = CRC16(B, size);
			Pased_uint16_t_to_Buff(B + size, sCRC);
			*pSize = size + 2U;
			break;

		case _WritingTabParamData:
			if (*pSize < (4U * 4U + 2U))
			{
				ErrorNum = _ErrorSize;
				break;
			}
			ParamNum = Pased_Buff_to_uint32_t(B);
			offset = Pased_Buff_to_uint32_t(B + 4U);
			size = Pased_Buff_to_uint32_t(B + 8U);
			stride = Pased_Buff_to_uint32_t(B + 12U);

			if ((size > (*pSize - 4U * 4U - 2U)) ||
				(*pSize != (4U * 4U + size + 2U)))
			{
				ErrorNum = _ErrorSize;
				break;
			}
			if (Pased_Buff_to_uint16_t(B + 4U * 4U + size) !=
				CRC16(B, 4U * 4U + size))
			{
				ErrorNum = _ErrorKC;
				break;
			}

			ErrorNum = WriteTabParam(NumUART, ParamNum, B + 16U,
				offset, stride, size);
			if (ErrorNum != _NoError)
				break;

			Pased_uint32_t_to_Buff(B, ParamNum);
			Pased_uint32_t_to_Buff(B + 4U, offset);
			Pased_uint32_t_to_Buff(B + 8U, size);
			*pSize = 12U;
			break;

		case _PreparTabParam:
			if (*pSize != (4U * 3U))
			{
				ErrorNum = _ErrorSize;
				break;
			}
			ParamNum = Pased_Buff_to_uint32_t(B);
			mode = Pased_Buff_to_int32_t(B + 4U);
			column = Pased_Buff_to_int32_t(B + 8U);
			if (mode != 0)
			{
				ErrorNum = _ErrorSize;
				break;
			}
			ErrorNum = PreparTabParam(NumUART, ParamNum, column);
			if (ErrorNum != _NoError)
				break;
			Pased_uint32_t_to_Buff(B, ParamNum);
			*pSize = 4U;
			break;

		case _ReqProgrPreparTabParam:
			if (*pSize != 4U)
			{
				ErrorNum = _ErrorSize;
				break;
			}
			ParamNum = Pased_Buff_to_uint32_t(B);
			ErrorNum = ProgrPreparTabParam(NumUART, ParamNum, &rows,
				&column, &curStep, &stepsCount);
			if (ErrorNum != _NoError)
				break;
			Pased_uint32_t_to_Buff(B, ParamNum);
			Pased_int32_t_to_Buff(B + 4U, curStep);
			Pased_int32_t_to_Buff(B + 8U, stepsCount);
			Pased_int32_t_to_Buff(B + 12U, rows);
			Pased_int32_t_to_Buff(B + 16U, column);
			*pSize = 4U * 5U;
			break;

		case _ReleaseTabParamData:
			if (*pSize != 4U)
			{
				ErrorNum = _ErrorSize;
				break;
			}
			ParamNum = Pased_Buff_to_uint32_t(B);
			ErrorNum = ReleaseTabParam(NumUART, ParamNum);
			if (ErrorNum != _NoError)
				break;
			Pased_uint32_t_to_Buff(B, ParamNum);
			*pSize = 4U;
			break;

		default:
			ErrorNum = _ErrorUnCorrParam;
			break;
	}

	if (ErrorNum != _NoError)
		*pSize = 0U;
	return ErrorNum;
}
