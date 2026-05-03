/*=============================================================================
2     Project: 
3     Platform: 
4     Filename: drv_Mona.c
5     Description:
6     Version: 0.0
7     Created: 2023.10.06
8     Last modified: 2024.02.20
9===========================================================================*/
#include <gd32f4xx.h>
#include "math.h"
//-----------------------------------------------------------------------------
#include "drv_time.h"
#include "crc8_CCITT.h"
#include "drv_UART.h"
#include "drv_Monp.h"
//-----------------------------------------------------------------------------
#define _UART (UART3)
//-----------------------------------------------------------------------------
#pragma pack(4)
typedef struct
{
	float Voltage_RefInt_Min;
	float Voltage_RefInt_AVG;
	float Voltage_RefInt_Max;
	float Voltage_Up_Min;
	float Voltage_Up_AVG;
	float Voltage_Up_Max;
	float Voltage_Iin_Min;
	float Voltage_Iin_AVG;
	float Voltage_Iin_Max;
	float Voltage_Pow_Min;
	float Voltage_Pow_AVG;
	float Voltage_Pow_Max;
	float Voltage_Vcc_Min;
	float Voltage_Vcc_AVG;
	float Voltage_Vcc_Max;
	float Voltage_3_3Vp_Min;
	float Voltage_3_3Vp_AVG;
	float Voltage_3_3Vp_Max;
	float TempSensor;
} TMonpParam;
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
#pragma pack(4)
typedef struct
{
	uint32_t Sinxr;
	uint32_t UpTime;
	TMonpParam St;
	uint32_t BuildTime;
	uint16_t Reserv;
	uint16_t KS;
} TMonpRAWStruct;
//-----------------------------------------------------------------------------
#define _MonpSynchroWord (0x80808080)
#define _UARTNum (_UARTMonp)
//-----------------------------------------------------------------------------
#pragma pack(4)
typedef union
{
  TMonpRAWStruct St;
  uint8_t Byte [sizeof(TMonpRAWStruct)];
} TMonpRAWUnion;
//-----------------------------------------------------------------------------
typedef enum {_MonpStIdly, _MonpStRxData} TMonpState;
//-----------------------------------------------------------------------------
#pragma pack(4)
typedef struct
{
	TTime TOEndPacket;
	TTime TOReceivePacket;
	uint32_t PassCounter;
	uint32_t ErrorSizeCounter;
	uint32_t ErrorSinxrCounter;
	uint32_t ErrorCRCCounter;
	uint32_t ErrorTOCounter;
	uint32_t RxSinxr;
	uint32_t DataIndex;
	uint32_t DataSize;
	TMonpRAWUnion RAWData;
	TMonpParam Monp;
	uint32_t UpTime;
	uint32_t BuildTime;
	TMonpState GD32State;
	uint8_t GAP [3];
} TStMonpState;
//-----------------------------------------------------------------------------
static TStMonpState MonpState;
//-----------------------------------------------------------------------------
static void ClearMonpData (TStMonpState * pSt)
{
	pSt->Monp.Voltage_RefInt_Min = 0.0;
	pSt->Monp.Voltage_RefInt_AVG = 0.0;
	pSt->Monp.Voltage_RefInt_Max = 0.0;
	pSt->Monp.Voltage_Up_Min = 0.0;
	pSt->Monp.Voltage_Up_AVG = 0.0;
	pSt->Monp.Voltage_Up_Max = 0.0;
	pSt->Monp.Voltage_Iin_Min = 0.0;
	pSt->Monp.Voltage_Iin_AVG = 0.0;
	pSt->Monp.Voltage_Iin_Max = 0.0;
	pSt->Monp.Voltage_Pow_Min = 0.0;
	pSt->Monp.Voltage_Pow_AVG = 0.0;
	pSt->Monp.Voltage_Pow_Max = 0.0;
	pSt->Monp.Voltage_Vcc_Min = 0.0;
	pSt->Monp.Voltage_Vcc_AVG = 0.0;
	pSt->Monp.Voltage_Vcc_Max = 0.0;
	pSt->Monp.Voltage_3_3Vp_Min = 0.0;
	pSt->Monp.Voltage_3_3Vp_AVG = 0.0;
	pSt->Monp.Voltage_3_3Vp_Max = 0.0;
	pSt->Monp.TempSensor = 0.0;

	pSt->BuildTime = 0;
	pSt->UpTime = 0;
}
//-----------------------------------------------------------------------------
static void CalculateGD32Data (TStMonpState * pSt)
{
	pSt->Monp = pSt->RAWData.St.St;
	pSt->UpTime = pSt->RAWData.St.UpTime;
  pSt->BuildTime = pSt->RAWData.St.BuildTime;
}
//-----------------------------------------------------------------------------
uint32_t GetMonpBuildTime (void)
{
	return (MonpState.BuildTime);
}
//-----------------------------------------------------------------------------
uint32_t GetMonpUpTime (void)
{
	return (MonpState.UpTime);
}
//-----------------------------------------------------------------------------
float GetMonpParam (TMonpParamEnum Param)
{
	float * p = (float *)(&(MonpState.Monp));
	return (p [Param]);
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
static uint32_t GD32GetNumberOfByteRxFIFO (void)
{
	return (UARTGetNumberOfByteRxFIFO (_UART));
}
//-----------------------------------------------------------------------------
static uint8_t GD32PopRxFIFO (void)
{
	return (UARTPopRxFIFO (_UART));
}
//-----------------------------------------------------------------------------
static int32_t PutByteToMonpRAWData (TStMonpState * pSt, uint8_t Byte)
{
	if (pSt->DataSize >= sizeof (TMonpRAWStruct))
		return (-1);
	pSt->DataSize++;
	pSt->RAWData.Byte [pSt->DataIndex] = Byte;
	pSt->DataIndex++;
	return (0);
}
//-----------------------------------------------------------------------------
static int32_t EndOfSizePacket (TStMonpState * pSt)
{
	if (pSt->DataSize >= sizeof(TMonpRAWStruct))
		return (1);
	else
		return (0);
}
//-----------------------------------------------------------------------------
static void CLRMonpRAWData (TStMonpState * pSt)
{
	pSt->DataSize =	pSt->DataIndex = 0;
}
//-----------------------------------------------------------------------------
static int32_t ParserPacket (TStMonpState * pSt)
{
	uint16_t u16;
	if (pSt->DataSize != sizeof(TMonpRAWStruct))
	{
		pSt->ErrorSizeCounter++;
		return (-1);
	}
	if (pSt->RAWData.St.Sinxr != _MonpSynchroWord)
	{
		pSt->ErrorSinxrCounter++;
		return (-2);
	}
  u16 = CRC8_CCITT (pSt->RAWData.Byte, sizeof (TMonpRAWStruct) - 2);
  if (u16 != pSt->RAWData.St.KS)
	{
		pSt->ErrorCRCCounter++;
    return (-3);
	}
	CalculateGD32Data (pSt);
	pSt->PassCounter++;
	return (0);
}
//-----------------------------------------------------------------------------
#define _TOEndPacket (50)
#define _TOReceivePacket (3000)
//-----------------------------------------------------------------------------
static void OneMonaRoutine (void)
{
	uint32_t NumRxByte;
	uint8_t b;
	uint32_t ui;
	TStMonpState * pSt = &(MonpState);

	if (EndTime (pSt->TOReceivePacket))
	{
		ClearMonpData (pSt);
		pSt->TOReceivePacket = SetTime_ms (_TOReceivePacket);
		pSt->GD32State = _MonpStIdly;
		pSt->ErrorTOCounter++;
	}

	if (!EndTime (pSt->TOEndPacket)) return;
	pSt->TOEndPacket = SetTime_ms (_TOEndPacket);

	NumRxByte = GD32GetNumberOfByteRxFIFO ();

	while (NumRxByte)
	{
		b = GD32PopRxFIFO ();
		NumRxByte--;
		switch (pSt->GD32State)
		{
			case _MonpStIdly: default:
				ui = b;
				ui &= 0x000000FF;
				pSt->RxSinxr <<= 8;
				pSt->RxSinxr |= ui;
				if (pSt->RxSinxr == _MonpSynchroWord)
				{
					pSt->GD32State = _MonpStRxData;
					CLRMonpRAWData (pSt);
					PutByteToMonpRAWData (pSt, (uint8_t)(pSt->RxSinxr >> 24));
					PutByteToMonpRAWData (pSt, (uint8_t)(pSt->RxSinxr >> 16));
					PutByteToMonpRAWData (pSt, (uint8_t)(pSt->RxSinxr >> 8));
					PutByteToMonpRAWData (pSt, (uint8_t)(pSt->RxSinxr));
					pSt->RxSinxr = ~_MonpSynchroWord;
				}
				break;
			case _MonpStRxData:
				PutByteToMonpRAWData (pSt, b);
				if (EndOfSizePacket (pSt) != 0)
				{
					pSt->GD32State = _MonpStIdly;
					if (ParserPacket (pSt) == 0)
						pSt->TOReceivePacket = SetTime_ms (_TOReceivePacket);
				}
				break;
		}
	}
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
uint32_t MonpPassPacketCount (void)
{
	return (MonpState.PassCounter);
}
//-----------------------------------------------------------------------------
uint32_t MonpErrorPacketCount (void)
{
	return (MonpState.ErrorSizeCounter
  					+ MonpState.ErrorSinxrCounter
  					+ MonpState.ErrorCRCCounter
						+ MonpState.ErrorTOCounter);
}
//-----------------------------------------------------------------------------
//uint32_t GD32E230ErrorSizeCount (void) {return (GD32State.ErrorSizeCounter);}
//uint32_t GD32E230ErrorSinxrCount (void) {return (GD32State.ErrorSinxrCounter);}
//uint32_t GD32E230ErrorCRCCount (void) {return (GD32State.ErrorCRCCounter);}
//uint32_t GD32E230ErrorTOCount (void) {return (GD32State.ErrorTOCounter);}
//-----------------------------------------------------------------------------
void MonpRoutine (void)
{
	static TTime TO = 0;
	if (!EndTime (TO)) return;
	TO = SetTime_ms (10);
	OneMonaRoutine ();
}
//-----------------------------------------------------------------------------
void MonpInit (void)
{
	ClearMonpData (&(MonpState));
	MonpState.RxSinxr = ~_MonpSynchroWord;
	MonpState.ErrorSizeCounter = 0;
	MonpState.ErrorSinxrCounter = 0;
	MonpState.ErrorCRCCounter = 0;
	MonpState.ErrorTOCounter = 0;
	MonpState.PassCounter = 0;
	//Init_FPGA_UART (_UARTNum, _speed9600);
	UartInit (_UART, 9600);
}
//-----------------------------------------------------------------------------
