/*=============================================================================
  Project:  Amp_AE
  Platform: GD32F470Z
  Filename: drv_KuOP.c
  Description:
  Programmer: 
  Version: 0.0
  Created: 2017.11.23
  Last modified: 2024.12.11
=============================================================================*/
#include <gd32f4xx.h>
//-----------------------------------------------------------------------------
#include "Global.h"
#include "drv_time.h"
#include "ParamApp.h"
#include "drv_KuOP.h"
//-----------------------------------------------------------------------------
static void SetKuOP (uint8_t Chann, int8_t Ku)
{
  switch (Ku)
  {
		default:
			switch (Chann)
			{
				case 0: gpio_bit_reset (GPIOE, GPIO_PIN_10); break;
				case 1: gpio_bit_reset (GPIOE, GPIO_PIN_9 ); break;
				case 2: gpio_bit_reset (GPIOE, GPIO_PIN_8 ); break;
				case 3: gpio_bit_reset (GPIOE, GPIO_PIN_7 ); break;
				case 4: gpio_bit_reset (GPIOG, GPIO_PIN_1 ); break;
				case 5: gpio_bit_reset (GPIOG, GPIO_PIN_0 ); break;
				case 6: gpio_bit_reset (GPIOF, GPIO_PIN_15); break;
				case 7: gpio_bit_reset (GPIOF, GPIO_PIN_14); break;
				default: break;
			}
			break;
/*		case 1:
			switch (Chann)
			{
				case 0: gpio_bit_reset (GPIOE, GPIO_PIN_10); break;
				case 1: gpio_bit_reset (GPIOE, GPIO_PIN_9 ); break;
				case 2: gpio_bit_reset (GPIOE, GPIO_PIN_8 ); break;
				case 3: gpio_bit_reset (GPIOE, GPIO_PIN_7 ); break;
				case 4: gpio_bit_reset (GPIOG, GPIO_PIN_1 ); break;
				case 5: gpio_bit_reset (GPIOG, GPIO_PIN_0 ); break;
				case 6: gpio_bit_reset (GPIOF, GPIO_PIN_15); break;
				case 7: gpio_bit_reset (GPIOF, GPIO_PIN_14); break;
				default: break;
			}
			break;
		case 2:*/
		case -1:
			switch (Chann)
			{
				case 0: gpio_bit_set (GPIOE, GPIO_PIN_10); break;
				case 1: gpio_bit_set (GPIOE, GPIO_PIN_9 ); break;
				case 2: gpio_bit_set (GPIOE, GPIO_PIN_8 ); break;
				case 3: gpio_bit_set (GPIOE, GPIO_PIN_7 ); break;
				case 4: gpio_bit_set (GPIOG, GPIO_PIN_1 ); break;
				case 5: gpio_bit_set (GPIOG, GPIO_PIN_0 ); break;
				case 6: gpio_bit_set (GPIOF, GPIO_PIN_15); break;
				case 7: gpio_bit_set (GPIOF, GPIO_PIN_14); break;
				default: break;
			}
			break;
/*		case 3:
			switch (Chann)
			{
				case 0: gpio_bit_set (GPIOE, GPIO_PIN_10); break;
				case 1: gpio_bit_set (GPIOE, GPIO_PIN_9 ); break;
				case 2: gpio_bit_set (GPIOE, GPIO_PIN_8 ); break;
				case 3: gpio_bit_set (GPIOE, GPIO_PIN_7 ); break;
				case 4: gpio_bit_set (GPIOG, GPIO_PIN_1 ); break;
				case 5: gpio_bit_set (GPIOG, GPIO_PIN_0 ); break;
				case 6: gpio_bit_set (GPIOF, GPIO_PIN_15); break;
				case 7: gpio_bit_set (GPIOF, GPIO_PIN_14); break;
				default: break;
			}
			break;
*/	}
}
//-----------------------------------------------------------------------------
void Init_KuOP (void)
{
	rcu_periph_clock_enable (RCU_GPIOE);
	rcu_periph_clock_enable (RCU_GPIOF);
	rcu_periph_clock_enable (RCU_GPIOG);

	gpio_mode_set (GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_7
										| GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10);
	gpio_output_options_set (GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_7
										| GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10);
	gpio_bit_set (GPIOE, GPIO_PIN_7 | GPIO_PIN_8 | GPIO_PIN_9 | GPIO_PIN_10);

	gpio_mode_set (GPIOF, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_14
										| GPIO_PIN_15);
	gpio_output_options_set (GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_14
										| GPIO_PIN_15);
	gpio_bit_set (GPIOF, GPIO_PIN_14 | GPIO_PIN_15);
	
	gpio_mode_set (GPIOG, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_0
										| GPIO_PIN_1);
	gpio_output_options_set (GPIOG, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_0
										| GPIO_PIN_1);
	gpio_bit_set (GPIOG, GPIO_PIN_0 | GPIO_PIN_1);
}
//-----------------------------------------------------------------------------
void KuOPRoutine (void)
{
  static TTime TO = 0;
  int8_t i;

  if (EndTime (TO))
  {
    TO = SetTime_ms (1000);
    for (i = 0; i < 8; i++)
      SetKuOP (i, gParamApp.Ku_OP [i]);
  }
}
//-----------------------------------------------------------------------------
