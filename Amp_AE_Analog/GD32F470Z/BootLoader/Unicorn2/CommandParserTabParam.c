/*=============================================================================
2     Project: 
3     Platform: GD32F407
4     Filename: CommandParserTabParam.c
5     Description:
6     Version: 0.0
7     Created: 2022.09.15
8     Last modified: 2023.06.28
9============================================================================*/
#include <stdint.h>
//--------------------------------------------------------------------------//
//#include "../DataProcess.h"
//#include "../CofigTabParam.h"
//#include "../drv_CurrOut4_20.h"
//#include "../Calc_Spectrum.h"
//#include "..\Core\Inc\BOC8_DataProcess.h"
//--------------------------------------------------------------------------//
#include "CommandList.h"
#include "PacketParser.h"
#include "CommandParser.h"
#include "CommandParserFunction.h"
#include "CommandParserTabParam.h"
#include "crc16.h"
//--------------------------------------------------------------------------//
static uint8_t ReadTabParam (	uint8_t NumUART, uint32_t NumParam, uint8_t * B,
															uint32_t offset, uint32_t wordSize, uint32_t stride,
															uint32_t * size)
{
  uint8_t ErrorNum = _NoError;
	(void)NumUART;
	switch (NumParam)
	{
//		case tbpCapture:
//				ErrorNum = ReadTabP_Capture (B, offset, wordSize, stride, size);
//			break;
//		case tbpRawSamples:
//				ErrorNum = ReadTabP_RawSamples (B, offset, wordSize, stride, size);
//			break;
//		case tbpFilteredSamples:
//				ErrorNum = ReadTabP_FilteredSamples (B, offset, wordSize, stride, size);
//			break;

		
		default: ErrorNum = _ErrorUnCorrParam;	break;
	}
	return (ErrorNum);
}
//--------------------------------------------------------------------------//
static uint8_t WriteTabParam (uint8_t NumUART, uint32_t NumParam, uint8_t * B,
															uint32_t offset, uint32_t stride, uint32_t size)
{
  uint8_t ErrorNum = _NoError;
	(void)NumUART;
	
	switch (NumParam)
	{
//		case tbpCapture:
//		case tbpRawSamples:
//		case tbpFilteredSamples:
//				ErrorNum = _ErrorWriteROParam;
//			break;

//		case _TabFactoryCalibrationParameters:
//				ErrorNum = WriteTabP_FactoryCalibrationParameters (B, offset, stride, size);
//			break;
	
		default: ErrorNum = _ErrorUnCorrParam; break;
	}
//	if (_NoError == ErrorNum) Pased_int32_t_to_Buff (B, val);
	return (ErrorNum);
}
//--------------------------------------------------------------------------//
static uint8_t PreparTabParam (uint8_t NumUART, uint32_t NumParam, int32_t column)
{
  uint8_t ErrorNum = _NoError;
	(void)NumUART;
//	(void)column;
	switch (NumParam)
	{
//		case tbpCapture:
//				ErrorNum = PreparTabP_Capture (column);
//			break;
//		case tbpRawSamples:
//				ErrorNum = PreparTabP_RawSamples (column);
//			break;
//		case tbpFilteredSamples:
//				ErrorNum = PreparTabP_FilteredSamples (column);
//			break;

		default: ErrorNum = _ErrorUnCorrParam; break;
	}
	return (ErrorNum);
}
//--------------------------------------------------------------------------//
static uint8_t ProgrPreparTabParam (uint8_t NumUART, uint32_t NumParam,
																		int32_t * rows, int32_t * column,
																		int32_t * curStep, int32_t * stepsCount)
{
  uint8_t ErrorNum = _NoError;
	(void)NumUART;
	*rows = *column = *curStep = *stepsCount = 0;
	switch (NumParam)
	{
//		case tbpCapture:
//				ErrorNum = ProgrPreparTabP_Capture (rows, column, curStep, stepsCount);
//			break;
//		case tbpRawSamples:
//				ErrorNum = ProgrPreparTabP_RawSamples (rows, column, curStep, stepsCount);
//			break;
//		case tbpFilteredSamples:
//				ErrorNum = ProgrPreparTabP_FilteredSamples (rows, column, curStep, stepsCount);
//			break;

		default: ErrorNum = _ErrorUnCorrParam; break;
	}
	return (ErrorNum);
}
//--------------------------------------------------------------------------//
static uint8_t ReleaseTabParam (uint8_t NumUART, uint32_t NumParam)
{
  uint8_t ErrorNum = _NoError;
	(void)NumUART;
	switch (NumParam)
	{
//		case tbpCapture:
//				ErrorNum = ReleaseTabP_Capture ();
//			break;
//		case tbpRawSamples:
//				ErrorNum = ReleaseTabP_RawSamples ();
//			break;
//		case tbpFilteredSamples:
//				ErrorNum = ReleaseTabP_FilteredSamples ();
//			break;

		default: ErrorNum = _ErrorUnCorrParam; break;
	}
	return (ErrorNum);
}
//--------------------------------------------------------------------------//
static uint8_t ReadDescrTabParam (uint8_t NumUART, uint32_t NumParam,
																	uint8_t * B, uint32_t * pSize)
{
  uint8_t ErrorNum = _NoError;
	*pSize = 0;
	(void)B;
	(void)NumUART;
	switch (NumParam)
	{
//		case tbpCapture:
//				ErrorNum = ReadDescrTabP_Capture (B, pSize, _SizeBuffRS485 / 2 - 20);
//			break;
//		case tbpRawSamples:
//				ErrorNum = ReadDescrTabP_RawSamples (B, pSize, _SizeBuffRS485 / 2 - 20);
//			break;
//		case tbpFilteredSamples:
//				ErrorNum = ReadDescrTabP_FilteredSamples (B, pSize, _SizeBuffRS485 / 2 - 20);
//			break;

		default: ErrorNum = _ErrorUnCorrParam; break;
	}
	return (ErrorNum);
}
//--------------------------------------------------------------------------//
uint8_t TabParam (uint8_t NumUART, uint8_t Command,
										uint8_t * B, uint32_t * pSize)
{
  uint8_t ErrorNum = _NoError;
	uint32_t ParamNum;
	uint32_t mode, offset, size, wordSize, stride;
	uint16_t  sCRC;
	int32_t column, rows, curStep, stepsCount;

	switch (Command)
	{	
//		case _RequestNumTabParam:// Запрос количества табличных параметров
//			if (*pSize != 0) {ErrorNum = _ErrorSize; break;}
//			Pased_uint32_t_to_Buff(B, _TabPCount);
//			*pSize = 4;
//			break;
//		case _GetDescrTabParam://Получить описание одного табличного параметра 
//      if (*pSize != 4) {ErrorNum = _ErrorSize; break;}
//      ParamNum = Pased_Buff_to_uint32_t (B);
//			ErrorNum = ReadDescrTabParam (NumUART, ParamNum, B/* + 4*/, pSize);
//			/**pSize += 4;*/
//      break;
//    case _ReadTabParamData:// Чтение данных табличного параметра
//      if (*pSize != 4*5) {ErrorNum = _ErrorSize; break;}
//      ParamNum = Pased_Buff_to_uint32_t (B);
//      offset = Pased_Buff_to_uint32_t (&(B [4]));
//      size = Pased_Buff_to_uint32_t (&(B [4*2]));
//      wordSize = Pased_Buff_to_uint32_t (&(B [4*3]));
//      stride = Pased_Buff_to_uint32_t (&(B [4*4]));
//			ErrorNum = ReadTabParam (NumUART, ParamNum, B + 12, offset, wordSize, stride, &size);
//      Pased_uint32_t_to_Buff (&(B [0]), ParamNum);
//      Pased_uint32_t_to_Buff (&(B [4]), offset);
//      Pased_uint32_t_to_Buff (&(B [8]), size);
//      size += 12;
//      sCRC = CRC16 (B, size);
//      Pased_uint16_t_to_Buff (&(B [size]), sCRC);
//      *pSize = size + 2;
//      break;
//    case _WritingTabParamData:// Запись данных табличного параметра
//      if (*pSize < 4*4+2) {ErrorNum = _ErrorSize; break;}
//      ParamNum = Pased_Buff_to_uint32_t (B);
//      offset = Pased_Buff_to_uint32_t (&(B [4]));
//      stride = Pased_Buff_to_uint32_t (&(B [4*3]));
//			size = *pSize - 4 * 4 - 2;
//      if (Pased_Buff_to_uint16_t (B + size + 4 * 4) != CRC16 (B, size + 4 * 4))
//      {
//      	ErrorNum = _ErrorKC;
//      	break;
//      }
//			ErrorNum = WriteTabParam (NumUART, ParamNum, B + 16, offset, stride, size);
//      Pased_uint32_t_to_Buff (&(B [0]), ParamNum);
//      Pased_uint32_t_to_Buff (&(B [4]), offset);
//      Pased_uint32_t_to_Buff (&(B [8]), size);
//      *pSize = 12;
//      break;
//		case _PreparTabParam:// Подготовка табличного параметра
//			if (*pSize != 4*3) {ErrorNum = _ErrorSize; break;}
//			ParamNum = Pased_Buff_to_uint32_t(B);
//			mode = Pased_Buff_to_uint32_t(&(B [4]));
//			column = Pased_Buff_to_int32_t(&(B [4*2]));
//			if (mode != 0) { // support only read prepare
//				ErrorNum = _ErrorSize;
//				break;
//			}
//			ErrorNum = PreparTabParam (NumUART, ParamNum, column);
//			Pased_uint32_t_to_Buff ((B), ParamNum);
//			*pSize = 4;
//			break;
//		case _ReqProgrPreparTabParam:// Запрос прогресса подготовки табличного параметра
//			if (*pSize != 4) {ErrorNum = _ErrorSize; break;}
//			ParamNum = Pased_Buff_to_uint32_t (B);
//			ErrorNum = ProgrPreparTabParam (NumUART, ParamNum, &rows, &column, &curStep, &stepsCount);
//			Pased_uint32_t_to_Buff (B, ParamNum);
//			Pased_int32_t_to_Buff (&(B [4]), curStep);
//			Pased_int32_t_to_Buff (&(B [2*4]), stepsCount);
//			Pased_int32_t_to_Buff (&(B [3*4]), rows);
//			Pased_int32_t_to_Buff (&(B [4*4]), column);
//			*pSize = 5*4;
//			break;
//		case _ReleaseTabParamData: // Освободить данные табличного параметра
//			if (*pSize != 4) {ErrorNum = _ErrorSize; break;}
//			ParamNum = Pased_Buff_to_uint32_t (B);
//			ErrorNum = ReleaseTabParam (NumUART, ParamNum);
//			Pased_uint32_t_to_Buff (B, ParamNum);
//			*pSize = 4;
//		break;
		default: ErrorNum = _ErrorUnCorrParam; break;
	}
	if (_NoError != ErrorNum) *pSize = 0;
	return (ErrorNum);
}
//--------------------------------------------------------------------------//
