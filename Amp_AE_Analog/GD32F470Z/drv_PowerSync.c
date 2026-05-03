//--------------------------------------------------------------------------//
#include <gd32f4xx.h>
//--------------------------------------------------------------------------//
#include "drv_PowerSync.h"
//--------------------------------------------------------------------------//
void init_PowerSync (void)
{
	timer_oc_parameter_struct timer_ocintpara;
	timer_parameter_struct timer_initpara;
	
	rcu_periph_clock_enable (RCU_GPIOF);
	rcu_periph_clock_enable (RCU_TIMER9);
	rcu_periph_clock_enable (RCU_TIMER10);
  rcu_timer_clock_prescaler_config (RCU_TIMER_PSC_MUL4);
	
	//Sync - PF6; SyncSD - PF7
	gpio_af_set (GPIOF, GPIO_AF_3, GPIO_PIN_6 | GPIO_PIN_7);
	gpio_mode_set (GPIOF, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_6 | GPIO_PIN_7);
	gpio_output_options_set (GPIOF, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_6 | GPIO_PIN_7);

	//Init TIMER 9
	timer_deinit (TIMER9);
	timer_deinit (TIMER10);
//	timer_primary_output_config (TIMER9, ENABLE);
	/* TIMER9 configuration */
	timer_initpara.prescaler         = 0;
	timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
	timer_initpara.counterdirection  = TIMER_COUNTER_UP;
	timer_initpara.period            = 99;
	timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
	timer_initpara.repetitioncounter = 0;
	timer_init (TIMER9, &timer_initpara);
	/* TIMER10 configuration */
	timer_initpara.period            = 49;
	timer_init (TIMER10, &timer_initpara);
	/* CH0 configuration in PWM mode */
	timer_ocintpara.ocpolarity  = TIMER_OC_POLARITY_LOW;
	timer_ocintpara.outputstate = TIMER_CCX_ENABLE;
	timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
	timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
	timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_HIGH;
	timer_ocintpara.ocnidlestate = TIMER_OC_IDLE_STATE_HIGH;
	timer_channel_output_config (TIMER9,  TIMER_CH_0, &timer_ocintpara);
	timer_channel_output_config (TIMER10, TIMER_CH_0, &timer_ocintpara);
	/* CH0 configuration in PWM mode1, duty cycle 0% */
	timer_channel_output_pulse_value_config (TIMER9,  TIMER_CH_0, 2);
	timer_channel_output_pulse_value_config (TIMER10, TIMER_CH_0, 5);
	timer_channel_output_mode_config (TIMER9,  TIMER_CH_0, TIMER_OC_MODE_PWM1);
	timer_channel_output_mode_config (TIMER10, TIMER_CH_0, TIMER_OC_MODE_PWM1);
	timer_channel_output_shadow_config (TIMER9,  TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);
	timer_channel_output_shadow_config (TIMER10, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);
	/* auto-reload preload enable */
	timer_auto_reload_shadow_enable (TIMER9);
	timer_auto_reload_shadow_enable (TIMER10);
	timer_single_pulse_mode_config (TIMER9,  TIMER_SP_MODE_REPETITIVE);
	timer_single_pulse_mode_config (TIMER10, TIMER_SP_MODE_REPETITIVE);
	
	timer_enable (TIMER9);
	timer_enable (TIMER10);
}
//--------------------------------------------------------------------------//
