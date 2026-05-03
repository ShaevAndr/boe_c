/*=============================================================================
  Project: 
  Platform: GD32F470Z
  Filename: drv_DS18B20.c
  Description:
  Programmer:
  Version: 0.0
  Created: 2022.05.12
  Last modified: 2024.02.11
=============================================================================*/
#include <gd32f4xx.h>
#include <stdint.h>
#include <math.h>
//------------------------------------------------------------------------------
//#include "Global.h"
#include "drv_time.h"
#include "drv_1W.h"
#include "ParamApp.h"
#include "drv_DS18B20.h"
//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
typedef enum {DS18B20_Undefined, DS18B20_NoInit, DS18B20_Idle, DS18B20_ResetWait1,
							DS18B20_ResetWait2, DS18B20_ResetWait3, DS18B20_NeedReset,
							DS18B20_NeedSelect1, DS18B20_StartConvertion,
							DS18B20_ReadyStartConvertion, DS18B20_WaitEndConvertion,
							DS18B20_WaitEndConvertion1, DS18B20_ReadResult,
							DS18B20_WaitReadyResult, DS18B20_WaitReadyResult2,
							DS18B20_WaitReadyResult3, DS18B20_NeedSelect2,
							DS18B20_NeedSelect3, DS18B20_NeedSelect4,
							DS18B20_CalculateResult, DS18B20_Error} DS18B20_State;
typedef struct
{
	TTime TO;
	float Temperature;
	uint32_t Error;
	DS18B20_State State;
	uint8_t ROM [8];
	uint8_t RAWData [10];
    uint8_t gap[5];
} StDS18B20_t;
//--------------------------------------------------------------------------//
static StDS18B20_t DS18B20 [_NumDS18B20];
//--------------------------------------------------------------------------//
bool DS18B20NormalWork (uint8_t Num)
{
	if (_NumDS18B20 <= Num) return (false);
	if (!isfinite (DS18B20 [Num].Temperature)) return (false);
	if (DS18B20 [Num].State == DS18B20_Undefined) return (false);
	if (DS18B20 [Num].State == DS18B20_NoInit) return (false);
	if (DS18B20 [Num].State == DS18B20_Error) return (false);
	return (true);
}
//--------------------------------------------------------------------------//
float DS18B20_GetTemperature (uint8_t Num)
{
	if (DS18B20 [Num].State == DS18B20_NoInit) return (-INFINITY);
	if (DS18B20 [Num].State == DS18B20_Error) return (INFINITY);
	return (DS18B20 [Num].Temperature);
}
//--------------------------------------------------------------------------//
float DS18B20_GetMaxTemperature (void)
{
  float maxTemp = -INFINITY;
  int i;
  
	for (i = 0; i < _NumDS18B20; i++)
  {
    if ((DS18B20 [i].State == DS18B20_NoInit) || (DS18B20 [i].State == DS18B20_Error))
      continue;
    if (!isfinite(DS18B20 [i].Temperature))
      continue;
    if (DS18B20 [i].Temperature > maxTemp)
      maxTemp = DS18B20 [i].Temperature;
  }
  return maxTemp;
}
//--------------------------------------------------------------------------//
float DS18B20_GetTempLocation (DS_Location_t Loc)
{
	int i;
	for (i = 0; i < _NumDS18B20; i++)
		if (gParamApp.DSLoc [i] == Loc)
			return (DS18B20 [i].Temperature);
	return (-273.14);
}
//--------------------------------------------------------------------------//
DS_Location_t DS18B20_GetDSLocation (uint8_t Num)
{
	if (_NumDS18B20 <= Num) return (0);
	return (gParamApp.DSLoc [Num]);
}
//--------------------------------------------------------------------------//
void DS18B20_SetDSLocation (uint8_t Num, DS_Location_t Loc)
{
	if (_NumDS18B20 <= Num) return;
	gParamApp.DSLoc [Num] = Loc;
}
//--------------------------------------------------------------------------//
void DS18B20_ini (void)
{
	uint8_t i, j;
	OneWire_ini ();
	for (i = 0; i < _NumDS18B20; i++)
	{
		DS18B20 [i].State = DS18B20_NoInit;
		for (j = 0; j < 8; j++) DS18B20 [i].ROM [j] = 0;
		DS18B20 [i].Temperature = -INFINITY;
	}
	OneWire_ResetSearch ();
	for (i = 0; i < _NumDS18B20; i++)
	{
		if (!OneWire_Search (DS18B20 [i].ROM)) break;
//China DS18B20 BAG Fix
//		if (OneWire_crc8 (DS18B20 [i].ROM, 7) == DS18B20 [i].ROM [7])
			if (DS18B20 [i].ROM [0] == 0x28)
				DS18B20 [i].State = DS18B20_Idle;
	}
	
}
//--------------------------------------------------------------------------//
static bool SingleDS18B20_Routine (StDS18B20_t * P)
{
	int16_t raw;
	uint32_t i, j;
	bool Present, Bit;
	switch (P->State)
	{
		case DS18B20_Error:
			P->Error++;
			P->Temperature = INFINITY;
            __attribute__((fallthrough));
		case DS18B20_Idle:
			OneWire_StartReset_DMA ();
			P->State = DS18B20_ResetWait1;
			return (0);
		case DS18B20_ResetWait1:
			if (OneWire_ReadyReset_DMA (&Present))
			{
				if (Present)
				{
					P->State = DS18B20_NeedSelect1;
					return (0);
				}
				else
				{
					P->State = DS18B20_Idle;
					return (1);
				}
			}
			return (0);
		case DS18B20_NeedSelect1:
			i = 0;
			P->RAWData [i++] = 0x55;
			for (j = 0; j < 8; j++)
				P->RAWData [i++] = P->ROM [j];
			if (OneWire_StartByteExchangeDMA (P->RAWData, i))
				P->State = DS18B20_NeedSelect4;
			else
				P->State = DS18B20_Idle;
			return (0);
		case DS18B20_NeedSelect4:
			if (OneWire_ReadyByteExchangeDMA (P->RAWData, 0))
				P->State = DS18B20_StartConvertion;
			return (0);
		case DS18B20_StartConvertion:
			P->RAWData [0] = 0x44;
			if (OneWire_StartByteExchangeDMA (P->RAWData, 1))
				P->State = DS18B20_ReadyStartConvertion;
			else
				P->State = DS18B20_Idle;
			return (0);
		case DS18B20_ReadyStartConvertion:
			if (OneWire_ReadyByteExchangeDMA (P->RAWData, 0))
			{
				P->TO = SetTime_ms (650);
				P->State = DS18B20_WaitEndConvertion;
			}
			return (0);
		case DS18B20_WaitEndConvertion:
			if (EndTime (P->TO))
			{
				OneWire_StartReadBit_DMA ();
				P->State = DS18B20_WaitEndConvertion1;
			}
			return (0);
		case DS18B20_WaitEndConvertion1:
			if (OneWire_ReadyReadBit_DMA (&Bit))
			{
				if (Bit)
				{
					P->State = DS18B20_NeedReset;
				}
				else
				{
					P->TO = SetTime_ms (350);
					P->State = DS18B20_WaitEndConvertion;
				}
			}
			return (0);
		case DS18B20_NeedReset:
			OneWire_StartReset_DMA ();
			P->State = DS18B20_ResetWait2;
			return (0);
		case DS18B20_ResetWait2:
			if (OneWire_ReadyReset_DMA (&Present))
			{
				if (Present)
					P->State = DS18B20_NeedSelect2;
				else
					P->State = DS18B20_Error;
			}
			return (0);
		case DS18B20_WaitReadyResult:
			OneWire_StartReadBit_DMA ();
			P->State = DS18B20_WaitReadyResult3;
			return (0);
		case DS18B20_WaitReadyResult3:
			if (OneWire_ReadyReadBit_DMA (&Bit))
			{
				if (Bit)
				{
					OneWire_StartReset_DMA ();
					P->State = DS18B20_ResetWait3;
					return (0);
				}
				else
					if (EndTime (P->TO))
					{
						P->State = DS18B20_Error;
						return (1);
					}
			}
			return (0);
		case DS18B20_ResetWait3:
			if (OneWire_ReadyReset_DMA (&Present))
			{
				if (Present)
					P->State = DS18B20_NeedSelect2;
				else
					P->State = DS18B20_Error;
			}
			return (0);
		case DS18B20_NeedSelect2:
			i = 0;
			P->RAWData [i++] = 0x55;
			for (j = 0; j < 8; j++)
				P->RAWData [i++] = P->ROM [j];
			if (OneWire_StartByteExchangeDMA (P->RAWData, i))
				P->State = DS18B20_NeedSelect3;
			else
				P->State = DS18B20_Idle;
			return (0);
		case DS18B20_NeedSelect3:
			if (OneWire_ReadyByteExchangeDMA (P->RAWData, 0))
				P->State = DS18B20_ReadResult;
			return (0);
		case DS18B20_ReadResult:
			i = 0;
			P->RAWData [i++] = 0xBE;
			for (j = 0; j < 9; j++)
				P->RAWData [i++] = 0xFF;
			if (OneWire_StartByteExchangeDMA (P->RAWData, i))
				P->State = DS18B20_WaitReadyResult2;
			else
				P->State = DS18B20_Idle;
			return (0);
		case DS18B20_WaitReadyResult2:
			if (OneWire_ReadyByteExchangeDMA (P->RAWData, 10))
				P->State = DS18B20_CalculateResult;
			return (0);
		case DS18B20_CalculateResult:
			if (OneWire_crc8 (P->RAWData + 1, 8) != P->RAWData [8 + 1])
			{
				P->State = DS18B20_Error;
				return (1);
			}
			raw = (int16_t)((P->RAWData [1+1] << 8) | P->RAWData [0+1]);
			P->Temperature = (float)raw;
			P->Temperature *= 0.0625f;// 1.0/16.0;
			P->State = DS18B20_Idle;
			return (1);
		case DS18B20_NoInit:
            __attribute__((fallthrough));
		default:
			return (1);
	}
}
//--------------------------------------------------------------------------//
void DS18B20_Routine (void)
{
	static uint8_t Num = 0;
	bool NextDS18B20 = 0;
	NextDS18B20 = SingleDS18B20_Routine (&(DS18B20 [Num]));
	if (NextDS18B20) Num++;
	if (Num >= _NumDS18B20) Num = 0;
}
//--------------------------------------------------------------------------//
