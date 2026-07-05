/*=============================================================================
2     Project: BOCv8
3     Platform: STM32G743
4     Filename: CommandRoutine.c
5     Description:
6     Version: 0.0
7     Created: 2017.11.23
8     Last modified: 2017.11.23
9============================================================================*/
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
//--------------------------------------------------------------------------//
#include "drv_time.h"
#include "drv_UUID.h"
//#include "drv_mtd.h"
#include "crc16.h"
#include "Unicorn2Routine.h"
#include "PacketParser.h"
//#include "ParamSaved.h"
#include "ParamSystem.h"
#include "CommandList.h"
#include "CommandParserFunction.h"
#include "CommandParserFloatParam.h"
#include "CommandParserIntParam.h"
#include "CommandParserTelemParam.h"
#include "CommandParserTabParam.h"
#include "CommandFlashMem.h"
#include "unicorn_uart_speed.h"
#include "CommandParser.h"
//--------------------------------------------------------------------------//
static char UnitDescriptionBuf [128];
const char *UnitDescription = UnitDescriptionBuf;
const uint8_t buildDate [] = __DATE__;
const uint8_t buildTime [] = __TIME__;

#include "UnitDescriptionJSON.h" 
//--------------------------------------------------------------------------//
static bool ReadDeviceTypeVersion = false;
//--------------------------------------------------------------------------//
bool GetReadDeviceTypeVersion (void) {return (ReadDeviceTypeVersion);}
//--------------------------------------------------------------------------//
uint8_t CommandProcess (uint8_t NumUART, uint8_t * Buff, uint32_t * pSize)
{
  uint8_t ErrorNum = _NoError;
  uint16_t j;
  TUUID uuid;
  int16_t i;
  int32_t i32;
  uint8_t Command = Buff [1];

  ErrorNum = _NoError;
  switch (Command)
  {
    case _WriteUUID://команда записать UUID
      if (*pSize != 18) {ErrorNum = _ErrorSize; break;}
			ErrorNum = _ErrorWriteROParam;
			break;
/*
      if (!CheckEmptyUUID ()) {ErrorNum = _ErrorComandImposWork; break;}

      for (i = sizeof (TUUID) - 1; i >= 0 ; i--)
        uuid.byte [i] = Buff [2 + i];
      SetUUID (uuid);
      *pSize = 2;
      break;
*/
    case _ReadingUUID://команда чтение UUID
      if (*pSize != 2) {ErrorNum = _ErrorSize; break;}
      uuid = GetUUID ();
      for (i = sizeof (TUUID) - 1; i >= 0 ; i--)
        Buff [2 + i] = uuid.byte [i];
      *pSize = 2 + 16;
      break;
//====================================================
	  case _WriteFlashMemPage:	//команда запись страници Flash памяти
	  case _ReadFlashMemPage:		//команда чтения страницы Flash памяти
	  case _ReadFlashMemParam:	//команда чтения параметров Flash памяти
	  case _ReadDescrFlashMem:	//получить описание flash-памяти
			ErrorNum = FlashParam (NumUART, Command, Buff + 2, pSize);
			break;
//====================================================
		case _RequestNumTabParam:			//Запрос количества табличных параметров
		case _GetDescrTabParam:				//Получить описание табличного параметра
		case _ReadTabParamData:				//Чтение данных табличного параметра
		case _WritingTabParamData: 		//Запись данных табличного параметра
		case _PreparTabParam:					//Подготовка табличного параметра
		case _ReqProgrPreparTabParam:	//Запрос прогресса подготовки
		case _ReleaseTabParamData:		//Освободить данные параметра
			*pSize -= 2;
			ErrorNum = TabParam (NumUART, Command, Buff + 2, pSize);
			if (_NoError == ErrorNum) *pSize += 2;
			break;
//====================================================
    case _RequestNumParam://Запрос количества доступных параметров
    {
    	uint8_t * p = Buff + 2;
      if (*pSize != 2) {ErrorNum = _ErrorSize; break;}
      Pased_uint32_t_to_Buff (p    , _FPCount  ); //количество параметров типа float
      Pased_uint32_t_to_Buff (p + 4, _IPCount  ); //количество параметров типа int
      Pased_uint32_t_to_Buff (p + 8, _TelPCount); //количество парамеров телеметрии
      *pSize = 2 + 12;
      break;
    }
//====================================================
/*
    case _WriteOneFloatParam://Записать один параметр типа float
    case _WriteAllFloatParam://Записать все параметры типа float
    case _GetDescrOneFloatParam://Получить описание одного параметра типа float
    case _ReadOneFloatParam://Считать один параметр типа float
    case _ReadAllFloatParam://Считать все параметры типа float
			ErrorNum = FloatParam (NumUART, Command, &(Buff [2]), pSize);
      break;
*/
//====================================================
    case _WriteOneIntParam://Записать один параметр типа int
    case _WriteAllIntParam://Записать все параметры типа int
    case _GetDescrOneIntParam://Получить описание одного параметра типа int
    case _ReadOneIntParam://Считать один параметр типа int
    case _ReadAllIntParam://Считать все параметры типа int
			ErrorNum = IntParam (NumUART, Command, &(Buff [2]), pSize);
      break;
//====================================================
    case _ReadOneTelemParam://Считать один параметр телеметрии
    case _ReadAllTelemParam://Считать все данные телеметрии
    case _GetDescrOneTelemParam://Получить описание одного параметра телеметрии
			ErrorNum = TelemParam (NumUART, Command, &(Buff [2]), pSize);
      break;
//=====================================================     
    case _GetJSON: // чтение описания в формате JSON
      if (*pSize != 10) { ErrorNum = _ErrorSize; break; }
      {
      	int32_t MaxSegSize, Offset, SegSize;
      	int32_t sizeofUD_JSON = sizeof (UD_JSON) - 1;
        Offset = Pased_Buff_to_int32_t(&(Buff [2]));
        MaxSegSize = Pased_Buff_to_int32_t(&(Buff [6]));
        if (Offset >= sizeofUD_JSON || Offset < 0 || MaxSegSize < 1)
        {
        	ErrorNum = _ErrorUnCorrParam;
        	break;
        }
        SegSize = (_SizeBuffRS485 - 2) / 2 - (2 + 4 * 4 + 2);
        if (SegSize > MaxSegSize) SegSize = MaxSegSize;
        if (SegSize + Offset > sizeofUD_JSON) SegSize = sizeofUD_JSON - Offset;
        Pased_int32_t_to_Buff(&Buff[2], Offset);
        Pased_int32_t_to_Buff(&Buff[2 + 4], SegSize);
        Pased_int32_t_to_Buff(&Buff[2 + 2 * 4], sizeofUD_JSON);
        for (i32 = 0; i32 < SegSize; i32++)
        	Buff [2 + 3 * 4 + i32] = UD_JSON [Offset + i32];
				i32 = CRC16_CCITT16 (Buff + 2, (uint16_t)(3 * 4 + SegSize));
        Pased_int32_t_to_Buff(&(Buff [2 + 3 * 4 + SegSize]), i32);
        *pSize = 2 + 3 * 4 + (uint32_t)SegSize + 4;
      }
      break;
//=====================================================        
    case _ResetDevice: // RESET
      if (*pSize != 6) { ErrorNum = _ErrorSize; break; }
      if (Pased_Buff_to_uint32_t(&Buff[2]) != 0x55aa1234) {
        ErrorNum = _ErrorUnCorrParam;
        break;
      }
      *pSize = 2;
      NeedToReset (NumUART);
      break;
//=====================================================
    case _SetUARTSpeed: // Set UART speed
      if (*pSize != 10) { ErrorNum = _ErrorSize; break; }
      if (Pased_Buff_to_uint32_t(&Buff[2]) != 0x12345678) {
        ErrorNum = _ErrorUnCorrParam;
        break;
      }
      i32 = Pased_Buff_to_uint32_t(&Buff[6]);
      if ((i32 & 0xFFFF0000) != 0) {
        ErrorNum = _ErrorUnCorrParam;
        break;
      }
      if ((((~i32) >> 8) & 0xFF) != (i32 & 0xFF)) {
        ErrorNum = _ErrorUnCorrParam;
        break;
      }
      {
        uint8_t speed = i32 & 0xFF;
        if (speed > _speedMaxNum) {
          ErrorNum = _ErrorUnCorrParam;
          break;
        }
        SetUARTSpeed (NumUART, speed);
      }
      break;
//=====================================================
    case _SetAddress: // Set Modbus address
      if (*pSize != 10) { ErrorNum = _ErrorSize; break; }
      if (Pased_Buff_to_uint32_t(&Buff[2]) != 0x23456789) {
        ErrorNum = _ErrorUnCorrParam;
        break;
      }
      i32 = Pased_Buff_to_uint32_t(&Buff[6]);
      if ((i32 & 0xFFFF0000) != 0) {
        ErrorNum = _ErrorUnCorrParam;
        break;
      }
      if ((((~i32) >> 8) & 0xFF) != (i32 & 0xFF)) {
        ErrorNum = _ErrorUnCorrParam;
        break;
      }
      {
        uint8_t addr = i32 & 0xFF;
        if ((addr < 1) || (addr > 254)) {
          ErrorNum = _ErrorUnCorrParam;
          break;
        }
        SetDeviceAddress (NumUART, addr);
      }
      break;
//=====================================================        
    case _GetDeviceTypeVersion://команда чтения типа и версии устройства
      if (*pSize != 2) ErrorNum = _ErrorSize;
      if (ErrorNum != _NoError)   break;

      Buff [0x02] = _TypeDev / 0x0100;
      Buff [0x03] = _TypeDev % 0x0100;
      Buff [0x04] = _VerDev / 0x0100;
      Buff [0x05] = _VerDev % 0x0100;
      j = (uint16_t)sprintf ((char *)(Buff + 6), "%s", UnitDescription);
      *pSize = 6 + j;
			ReadDeviceTypeVersion = true;
      break;
    default:
      ErrorNum = _ErrorUnKnowComand;
      break;
  }
  return (ErrorNum);
}

static const char* monthToQuarter(int month)
{
  if ((month >= 0) && (month < 3))
    return "I";
  if ((month >= 3) && (month < 6))
    return "II";
  if ((month >= 6) && (month < 9))
    return "III";
  if ((month >= 9) && (month < 12))
    return "IV";
  return "_";
}

static const char* deviceDateOfManufactureToStr(char* buf, int bufSize, uint64_t dateOfManuf)
{
  struct tm tm;
  time_t timestamp = dateOfManuf;
  
  if (_localtime_r(&timestamp, &tm) == NULL)
  {
    return NULL;
  }
  snprintf(buf, bufSize, "%d %s", 1900+tm.tm_year, monthToQuarter(tm.tm_mon));
  return buf;
}

void CommandParserInit(void)
{
  char manufBuf[12];
  snprintf(UnitDescriptionBuf, sizeof(UnitDescriptionBuf),
    "Блок усилителей электронный (БУЭ-8) (Boot) %s Зав.№%03d (SW %s %s)",
    deviceDateOfManufactureToStr(manufBuf, sizeof(manufBuf), gParamSystem.device.DateOfManufacture),
    gParamSystem.device.SerialNumber, buildDate, buildTime);
}
