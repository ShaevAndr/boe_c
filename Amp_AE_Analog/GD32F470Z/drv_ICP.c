/*=============================================================================
  Project: 
  Platform: GD32F470Z
  Filename: drv_ICP.h
  Description:
  Programmer: 
  Version: 0.0
  Created: 2017.11.23
  Last modified: 2024.12.10
=============================================================================*/
#include <gd32f4xx.h>
//-----------------------------------------------------------------------------
#include "Global.h"
#include "drv_time.h"
#include "drv_MonADC1.h"
#include "drv_MonADC2.h"
#include "ParamApp.h"
#include "drv_ICP.h"
//-----------------------------------------------------------------------------
static uint8_t ICPDisablePower = 0;
//-----------------------------------------------------------------------------
static uint8_t State = 0;
static uint8_t StateOut = 0;
static uint8_t TO [8];
//-----------------------------------------------------------------------------
#define _StateOn 0
#define _StateOff 1
//-----------------------------------------------------------------------------
uint8_t GetICPDisablePower (void)
{
	return (ICPDisablePower);
}
//-----------------------------------------------------------------------------
void SetICPDisablePower (uint8_t DisablePower)
{
	ICPDisablePower = DisablePower;
}
//-----------------------------------------------------------------------------
static void OffICPGPIO (uint8_t n, uint8_t state)
{
	if (state == _StateOn) 
		switch (n)
		{
			case 0: gpio_bit_set (GPIOF, GPIO_PIN_13); break;
			case 1: gpio_bit_set (GPIOF, GPIO_PIN_12); break;
			case 2: gpio_bit_set (GPIOF, GPIO_PIN_11); break;
			case 3: gpio_bit_set (GPIOF, GPIO_PIN_2); break;
			case 4: gpio_bit_set (GPIOF, GPIO_PIN_1); break;
			case 5: gpio_bit_set (GPIOF, GPIO_PIN_0); break;
			case 6: gpio_bit_set (GPIOC, GPIO_PIN_15); break;
			case 7: gpio_bit_set (GPIOC, GPIO_PIN_14); break;
			default: break;
		}
	else
		switch (n)
		{
			case 0: gpio_bit_reset (GPIOF, GPIO_PIN_13); break;
			case 1: gpio_bit_reset (GPIOF, GPIO_PIN_12); break;
			case 2: gpio_bit_reset (GPIOF, GPIO_PIN_11); break;
			case 3: gpio_bit_reset (GPIOF, GPIO_PIN_2); break;
			case 4: gpio_bit_reset (GPIOF, GPIO_PIN_1); break;
			case 5: gpio_bit_reset (GPIOF, GPIO_PIN_0); break;
			case 6: gpio_bit_reset (GPIOC, GPIO_PIN_15); break;
			case 7: gpio_bit_reset (GPIOC, GPIO_PIN_14); break;
			default: break;
		}
}
//-----------------------------------------------------------------------------
static float GetChannMonP (int32_t NChann)
{
	switch (NChann)
	{
		case 0: return (ResultDataMonADC1AVG (_ADC1_Monp1));
		case 1: return (ResultDataMonADC1AVG (_ADC1_Monp2));
		case 2: return (ResultDataMonADC1AVG (_ADC1_Monp3));
		case 3: return (ResultDataMonADC1AVG (_ADC1_Monp4));
		case 4: return (ResultDataMonADC1AVG (_ADC1_Monp5));
		case 5: return (ResultDataMonADC1AVG (_ADC1_Monp6));
		case 6: return (ResultDataMonADC1AVG (_ADC1_Monp7));
		case 7: return (ResultDataMonADC1AVG (_ADC1_Monp8));
		default: return (0.0f);
	}
}
//-----------------------------------------------------------------------------
static float GetChannMonN (int32_t NChann)
{
	switch (NChann)
	{
		case 0: return (ResultDataMonADC1AVG (_ADC1_Monn1));
		case 1: return (ResultDataMonADC1AVG (_ADC1_Monn2));
		case 2: return (ResultDataMonADC1AVG (_ADC1_Monn3));
		case 3: return (ResultDataMonADC1AVG (_ADC1_Monn4));
		case 4: return (ResultDataMonADC1AVG (_ADC1_Monn5));
		case 5: return (ResultDataMonADC1AVG (_ADC1_Monn6));
		case 6: return (ResultDataMonADC1AVG (_ADC1_Monn7));
		case 7: return (ResultDataMonADC1AVG (_ADC1_Monn8));
		default: return (0.0f);
	}
}
//-----------------------------------------------------------------------------
void Init_OPPower (void)
{
	int8_t i;

	rcu_periph_clock_enable (RCU_GPIOC);
	rcu_periph_clock_enable (RCU_GPIOF);

	gpio_mode_set (GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_14 | GPIO_PIN_15);
	gpio_output_options_set (GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ,  GPIO_PIN_14 | GPIO_PIN_15);
	gpio_bit_set (GPIOC,  GPIO_PIN_14 | GPIO_PIN_15);

	gpio_mode_set (GPIOF, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_0
										| GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_11 | GPIO_PIN_12
										| GPIO_PIN_13);
	gpio_output_options_set (GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ,  GPIO_PIN_0
										| GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_11 | GPIO_PIN_12
										| GPIO_PIN_13);
	gpio_bit_set (GPIOF,  GPIO_PIN_0
										| GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_11 | GPIO_PIN_12
										| GPIO_PIN_13);

    for (i = 7; i >= 0; i--)
    {
        OffICPGPIO ((uint8_t)i, _StateOff);
        TO [(uint8_t)i] = i + 2;
    }

    ICPDisablePower = 0;
    StateOut = 0xFF;
    State = 0xFF;
}
//-----------------------------------------------------------------------------
void OPPowerRoutine (void)
{
    static TTime T = 0;

    int8_t i;

    if (!EndTime (T))
        return;
    T = SetTime_ms (1000);

    for (i = 7; i >= 0; i--)
    {
        if (((ICPDisablePower >> i) & 0x01) == 1)
        {
            State |= 0x01 << i;
            StateOut |= 0x01 << i;
            TO [(uint8_t)i] = 5;
        }
        else
        {
            if ((GetChannMonP (i) < (0.5 * ResultDataMonADC2AVG (_ADC2_p24V)))
                || ((GetChannMonP (i) - GetChannMonN (i)) < gParamApp.ICPMinVoltage))
            {
                if (TO [(uint8_t)i] == 0)
                {
                  if (((State >> i) & 0x01) == 0)
                  {
                      State |= 0x01 << i;
                      StateOut |= 0x01 << i;
                      TO [(uint8_t)i] = 55;
                  }
                  else
                  {
                      State &= ~(0x01 << i);
                      TO [(uint8_t)i] = 5;
                  }

                }
                else
                    TO [(uint8_t)i]--;
           }
            else
            {
                TO [(uint8_t)i] = 5;
                StateOut &= ~(0x01 << i);
            }
        }
        if (((State >> i) & 0x01) == 0)
            OffICPGPIO (i, _StateOn);
        else
            OffICPGPIO (i, _StateOff);
    }
}
//-----------------------------------------------------------------------------
uint8_t GetOPPowerState (void)
{
    return (StateOut);
}
//-----------------------------------------------------------------------------


