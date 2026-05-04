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
#include "../Unicorn2/drv_time.h"
#include "../Unicorn2/drv_UUID.h"
//#include "drv_mtd.h"
#include "../Unicorn2/crc16.h"
#include "../Unicorn2/Unicorn2Routine.h"
#include "../Global/ParamSystem.h"
#include "ReadHoldingRegisters.h"
#include "ReadInputRegisters.h"
#include "WriteSingleRegister.h"
#include "WriteMultipleRegisters.h"
#include "ErrorHandler.h"
#include "CommandParcerModbus.h"
//--------------------------------------------------------------------------//
static char UnitDescriptionBuf [128];
const char *UnitDescription = UnitDescriptionBuf;
const uint8_t buildDate [] = __DATE__;
const uint8_t buildTime [] = __TIME__;

//--------------------------------------------------------------------------//
static bool ReadDeviceTypeVersion = false;
//--------------------------------------------------------------------------//
bool GetReadDeviceTypeVersion (void) {return (ReadDeviceTypeVersion);}
//--------------------------------------------------------------------------//
uint8_t CommandProcess (uint8_t NumUART, uint8_t * Buff, uint32_t * pSize)
{
  uint8_t ErrorNum = _NoError;
  uint8_t Command = Buff [0];

  ErrorNum = _NoError;
  switch (Command)
  {
		case _ReadHoldingRegisters:
			ErrorNum = ReadHoldingsRegisters(NumUART, Command, Buff, pSize);
			break;
		case _ReadInputRegisters:
			ErrorNum = ReadInputRegisters(NumUART, Command, Buff, pSize);
			break;
		case _WriteSingleRegister:
			ErrorNum = WriteSingleRegister(NumUART, Command, Buff, pSize);
			break;
		case _WriteMultipleRegisters:
			ErrorNum = WriteMultipleRegisters(NumUART, Command, Buff, pSize);
			break;
		default:
			ErrorNum = _IllegalFunction;
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
    "???? ?????????? ??????????? (???-8) %s ???.?%03d (SW %s %s)",
    deviceDateOfManufactureToStr(manufBuf, sizeof(manufBuf), gParamSystem.device.DateOfManufacture),
    gParamSystem.device.SerialNumber, buildDate, buildTime);
}
