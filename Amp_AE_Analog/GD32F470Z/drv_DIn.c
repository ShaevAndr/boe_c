/*=============================================================================
2	  Project: 
3	  Platform: APM32F035
4	  Filename: drv_DIn.c
5	  Description:
6	  Version: 0.0
7	  Created: 2022.08.23
8   Last modified: 2025.04.14
9	=============================================================================*/
//#include "apm32f035.h"
//#include "apm32f035_rcm.h"
//#include "apm32f035_misc.h"
//#include "apm32f035_gpio.h"
//#include "apm32f035_tmr.h"
#include <gd32f4xx.h>
#include <string.h> 
//-----------------------------------------------------------------------------
#include "drv_time.h"
#include "drv_DIn.h"
//--------------------------------------------------------------------------//
typedef struct
{
	TTime TO;
	TTime FreqTO;
	float Frequency;
	uint16_t Count;
	bool State;
	bool OldState;
	bool Change;
} TDInStruct;
//--------------------------------------------------------------------------//
#define _PollingPeriod (10)//ms
#define _FreqTO (10000)//ms
//--------------------------------------------------------------------------//
static TDInStruct DIn [_DInNum];
//--------------------------------------------------------------------------//
static bool GetPinState (TNumDIn Num)
{
	switch (Num)
	{
		case _DIn1:
			return (gpio_input_bit_get (GPIOB, GPIO_PIN_8) == SET ? true : false);
		case _DIn2:
			return (gpio_input_bit_get (GPIOD, GPIO_PIN_2) == SET ? true : false);
		default:
			return (false);
	}
}
//--------------------------------------------------------------------------//
static void InitTMR (TNumDIn Num)
{
	timer_parameter_struct timer_initpara;
	uint32_t TMR;

	switch (Num)
	{
		case _DIn1:
			TMR = TIMER1;
			rcu_periph_clock_enable (RCU_TIMER1);
			rcu_periph_clock_enable (RCU_GPIOB);
			gpio_af_set (GPIOB, GPIO_AF_1, GPIO_PIN_8);
			gpio_mode_set (GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_8);
			gpio_output_options_set (GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, GPIO_PIN_8);
			gpio_bit_set (GPIOB, GPIO_PIN_8);
			break;
		case _DIn2:
			TMR = TIMER2;
			rcu_periph_clock_enable (RCU_TIMER2);
			rcu_periph_clock_enable (RCU_GPIOD);
			gpio_af_set (GPIOD, GPIO_AF_2, GPIO_PIN_2);
			gpio_mode_set (GPIOD, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_2);
			gpio_output_options_set (GPIOD, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, GPIO_PIN_2);
			gpio_bit_set (GPIOD, GPIO_PIN_2);
			break;
		default: return;
	}

	/* TIMER configuration */
	timer_initpara.prescaler         = 0U;
	timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
	timer_initpara.counterdirection  = TIMER_COUNTER_UP;
	timer_initpara.period            = 65535U;
	timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
	timer_initpara.repetitioncounter = 0U;
	timer_init (TMR, &timer_initpara);

	timer_input_trigger_source_select (TMR, TIMER_SMCFG_TRGSEL_ETIFP);
	timer_external_clock_mode1_config (TMR, TIMER_EXT_TRI_PSC_OFF, TIMER_ETP_FALLING, 15U);
	
	timer_enable (TMR);
}
//--------------------------------------------------------------------------//
void InitDIn (void)
{
	int i;
	InitTMR (_DIn1);
	InitTMR (_DIn2);

	for (i = 0; i < _DInNum; i++)
	{
		DIn [i].Change = false;
		DIn [i].OldState = DIn [i].State = GetPinState ((TNumDIn)i);
		DIn [i].Count = 0;
		DIn [i].Frequency = 0.0f;
		DIn [i].TO = SetTime_ms (_PollingPeriod);
		DIn [i].FreqTO = SetTime_ms (_FreqTO);

	}
}
//--------------------------------------------------------------------------//
bool GetStateDIn (TNumDIn Num)
{
	return (DIn [Num].State);
}
//--------------------------------------------------------------------------//
uint32_t GetStateDInsU32 (void)
{
	return ((GetStateDIn (_DIn1) & 1) | ((GetStateDIn (_DIn2) & 1) << 1));
}
//--------------------------------------------------------------------------//
//float GetStateDInsf (void)
//{
//	return ((float)GetStateDInsU32 ());
//}
//--------------------------------------------------------------------------//
bool GetChangeDIn (TNumDIn Num)
{
	bool ret = DIn [Num].Change;
	DIn [Num].Change = false;
	return (ret);
}
//--------------------------------------------------------------------------//
uint32_t GetChangeDInsU32 (void)
{
	return ((GetChangeDIn (_DIn1) & 1) | ((GetChangeDIn (_DIn2) & 1) << 1));
}
//--------------------------------------------------------------------------//
//float GetChangeDInsf (void)
//{
//	uint32_t i = (DIn [_DIn1].State & 1) | ((DIn [_DIn2].State & 1) << 1);
//	return ((float)i);
//}
//--------------------------------------------------------------------------//
float GetFrequencyDIn (TNumDIn Num)
{
	return (DIn [Num].Frequency);
}
//--------------------------------------------------------------------------//
static uint16_t TMR_counter_read (TNumDIn Num)
{
	switch (Num)
	{
		case _DIn1: return (timer_counter_read (TIMER1) & 0xFFFF);
		case _DIn2: return (timer_counter_read (TIMER2) & 0xFFFF);
		default: return (0);
	}
}
//--------------------------------------------------------------------------//
static void RoutineSingleDin (TNumDIn Num)
{
	TDInStruct * p = &(DIn [Num]);
	if (!EndTime (p->TO)) return;
	p->TO = SetTime_ms (_PollingPeriod);
	
	p->OldState = p->State;
	p->State = GetPinState (Num);
	if (p->OldState != p->State) p->Change = true;

  if (EndTime (p->FreqTO))
  {
		TTime T1, T2;
		uint16_t New, Old;
    do
    {
      T1 = GetTime ();
      New = TMR_counter_read (Num);
      T2 = GetTime ();
    } while (DiffTime (T1, T2) > 50.0e-6);
    Old = p->Count;
    p->Count = New;
    if (New >= Old) New -= Old;
    else New = 0xFFFF - (Old - New);
    p->Frequency = (float)New;
    p->Frequency /= DiffTime (p->FreqTO, T1) + 0.001 * ((float)_FreqTO);
    p->FreqTO =  SetRelativTime_ms (T1, _FreqTO);
  }
}
//--------------------------------------------------------------------------//
void RoutineDin (void)
{
	static TNumDIn Num = _DIn1;
	RoutineSingleDin (Num);
	Num++;
	if (Num >= _DInNum) Num = _DIn1;
}
//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//

