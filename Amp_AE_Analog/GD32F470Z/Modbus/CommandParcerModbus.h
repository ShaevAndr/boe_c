/*=============================================================================
2     Project: 
3     Platform: 
4     Filename: CommandParcerModbus.h
5     Description:
6     Version: 0.0
7     Created: 2017.11.23
8     Last modified: 2017.11.23
9============================================================================*/
#ifndef CommandParcerModbus_H
#define CommandParcerModbus_H
	//------------------------------------------------------------------------------
	#include <stdint.h>
	#include <stdbool.h>
	
	//------------------------------------------------------------------------------
	extern const char *UnitDescription;
	extern const uint8_t buildDate [];
	extern const uint8_t buildTime [];	
	
	//------------------------------------------------------------------------------
	//errors unicorn
	#define _ErrorKC              0x01 //?? ???? ???????????? ?? ?????
	#define _ErrorSize            0x02 //????????? ????? ???? ????????????
	#define _ErrorUnKnowType      0x03 //??????????? ??? ??????
	#define _ErrorUnKnowComand    0x04 //??????????? ???????
	#define _ErrorUnCorrParam     0x05 //???????? ????????
	#define _ErrorUnCorrComand    0x06 //???????? ??????? (?????? ??? ??????????)
	#define _ErrorDataNotReady    0x07 //?????? ?? ??????
	#define _ErrorComandImposWork 0x08 //?????????? ?????????? ???????
	#define _ErrorNoSuchParam     0x09
	#define _ErrorInternalError	  0x0A
	#define _ErrorWriteROParam    0x0B
	#define _ErrorTimeout         0x0C
	#define _NoError              0xff //??? ?????? (??? ?????. ?????????????)
	
	//------------------------------------------------------------------------------
  //Modbus Function Codes
	#define _ReadCoils              0x01
	#define _ReadDiscreteInputs     0x02
	#define _ReadHoldingRegisters   0x03
	#define _ReadInputRegisters     0x04
	#define _WriteSingleCoil        0x05
	#define _WriteSingleRegister    0x06
	#define _WriteMultipleCoils     0x0F
	#define _WriteMultipleRegisters 0x10

	//------------------------------------------------------------------------------
	uint8_t CommandProcess (uint8_t NumUART, uint8_t * Buff, uint32_t * pSize);
  void CommandParserInit(void);
	//------------------------------------------------------------------------------
#endif 
