/*=============================================================================
2     Project: 
3     Platform: 
4     Filename: CommandRoutine.h
5     Description:
6     Version: 0.0
7     Created: 2017.11.23
8     Last modified: 2017.11.23
9============================================================================*/
#ifndef CommandRoutine_H
#define CommandRoutine_H
	//------------------------------------------------------------------------------
	#include <stdint.h>
	#include <stdbool.h>
	//------------------------------------------------------------------------------
	#define _TypeDev (0x0320)
	#define _VerDev (0x0100)

	extern const char *UnitDescription;
	extern const uint8_t buildDate [];
	extern const uint8_t buildTime [];	
	//------------------------------------------------------------------------------
	typedef struct {
		char name [128];
		char unit [16];
		char formatStr [16];
		char uniqueId [32];
		float minValuef;
		float maxValuef;
		int32_t minValuei;
		int32_t maxValuei;
		char access [4];
	} DescriptParam_t;
	//------------------------------------------------------------------------------
	//ошибки при обработке "новых" команд:
	#define _ErrorKC              0x01 //КС поля пользователя не верна
	#define _ErrorSize            0x02 //ошибочная длина поля пользователя
	#define _ErrorUnKnowType      0x03 //неизвестный тип пакета
	#define _ErrorUnKnowComand    0x04 //неизвестная команда
	#define _ErrorUnCorrParam     0x05 //неверный параметр
	#define _ErrorUnCorrComand    0x06 //неверная команда (ошибка при выполнении)
	#define _ErrorDataNotReady    0x07 //данные не готовы
	#define _ErrorComandImposWork 0x08 //невозможно обработать команду
	#define _ErrorNoSuchParam     0x09
	#define _ErrorInternalError	  0x0A
	#define _ErrorWriteROParam    0x0B
	#define _ErrorTimeout         0x0C
	#define _NoError              0xff //Нет ошибки (для внутр. использования)
	//------------------------------------------------------------------------------
	#define _WriteFlashMemPage 			(0x43)
	#define _ReadFlashMemPage 			(0x44)
	#define _ReadFlashMemParam 			(0x45)
	#define _ReadDescrFlashMem 			(0x46)

	#define _WriteOneFloatParam			(0xE1)
	#define _WriteOneIntParam				(0xE2)
	#define _ReadOneTelemParam			(0xE3)
	#define _WriteAllFloatParam			(0xE4)
	#define _WriteAllIntParam				(0xE5)
	#define _ReadAllTelemParam			(0xE6)
	#define _GetDescrOneFloatParam	(0xE7)
	#define _GetDescrOneIntParam		(0xE8)
	#define _GetDescrOneTelemParam	(0xE9)
	#define _ReadOneFloatParam			(0xEA)
	#define _ReadOneIntParam				(0xEB)
	#define _ReadAllFloatParam			(0xEC)
	#define _ReadAllIntParam				(0xED)

	#define _RequestNumParam				(0xE0)

	#define _WriteUUID							(0x06)
	#define _ReadingUUID						(0x07)

	#define _RequestNumTabParam			(0x50)
	#define _GetDescrTabParam				(0x51)
	#define _ReadTabParamData				(0x52)
	#define _WritingTabParamData		(0x53)
	#define _PreparTabParam					(0x54)
	#define _ReqProgrPreparTabParam	(0x55)
	#define _ReleaseTabParamData		(0x56)
    
  #define _GetJSON                (0xEE)
  #define _ResetDevice    		    (0xFA)
  #define _SetUARTSpeed    		    (0xFB)
  #define _SetAddress     		    (0xFC)
  #define _GetDeviceTypeVersion	  (0xFF)
	//------------------------------------------------------------------------------
	bool GetReadDeviceTypeVersion (void);
	uint8_t CommandProcess (uint8_t NumUART, uint8_t * Buff, uint32_t * pSize);
	uint8_t getDeviceTypeVersion (uint8_t * Buff); //Возвращает размер
  void CommandParserInit(void);
	//------------------------------------------------------------------------------
#endif 
