//--------------------------------------------------------------------------//
#include <gd32f4xx.h>
#include <stdbool.h>
//--------------------------------------------------------------------------//
#include "drv_time.h"
#include "drv_LEDs.h"
//--------------------------------------------------------------------------//
static uint8_t PingActiviti = 0;
//--------------------------------------------------------------------------//
#define LED_Hear_CLK_ENABLE RCU_GPIOE
#define LED_Hear_Pin GPIO_PIN_6
#define LED_Hear_GPIO_Port GPIOE
#define LED_Act_CLK_ENABLE RCU_GPIOE
#define LED_Act_Pin GPIO_PIN_5
#define LED_Act_GPIO_Port GPIOE
#define LED_Error_CLK_ENABLE RCU_GPIOC
#define LED_Error_Pin GPIO_PIN_13
#define LED_Error_GPIO_Port GPIOC
//--------------------------------------------------------------------------//
void Preinit_LEDs (void)
{
	rcu_periph_clock_enable (LED_Hear_CLK_ENABLE);
	rcu_periph_clock_enable (LED_Act_CLK_ENABLE);
	rcu_periph_clock_enable (LED_Error_CLK_ENABLE);
	//LED_Hear - PE6
//	gpio_mode_set (GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_6);
//	gpio_output_options_set (GPIOE, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, GPIO_PIN_6);
//	gpio_bit_reset (GPIOE, GPIO_PIN_6);
	gpio_mode_set (LED_Hear_GPIO_Port, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, LED_Hear_Pin);
	gpio_output_options_set (LED_Hear_GPIO_Port, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, LED_Hear_Pin);
	gpio_bit_reset (LED_Hear_GPIO_Port, LED_Hear_Pin);
	
	//LED_Act - PE5 (TIMER8_CH0)
//	gpio_mode_set (GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_5);
//	gpio_output_options_set (GPIOE, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, GPIO_PIN_5);
//	gpio_bit_reset (GPIOE, GPIO_PIN_5);
	gpio_mode_set (LED_Act_GPIO_Port, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, LED_Act_Pin);
	gpio_output_options_set (LED_Act_GPIO_Port, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, LED_Act_Pin);
	gpio_bit_reset (LED_Act_GPIO_Port, LED_Act_Pin);

	//LED_Error - PC13
//	gpio_mode_set (GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_13);
//	gpio_output_options_set (GPIOC, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, GPIO_PIN_13);
//	gpio_bit_reset (GPIOC, GPIO_PIN_13);
	gpio_mode_set (LED_Error_GPIO_Port, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, LED_Error_Pin);
	gpio_output_options_set (LED_Error_GPIO_Port, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, LED_Error_Pin);
	gpio_bit_reset (LED_Error_GPIO_Port, LED_Error_Pin);
}
//--------------------------------------------------------------------------//
void Preinit_SetLEDState (PreinitLED_t LED, PreinitLEDState_t State)
{
	bit_status PinState;
	switch (State)
	{
		case _PILED_Off: PinState = SET; break;
		case _PILED_On: PinState = RESET; break;
		default: return;
	}
	switch (LED)
	{
		case _PILED_Hear:
			gpio_bit_write (LED_Hear_GPIO_Port, LED_Hear_Pin, PinState);
			break;
		case _PILED_Act:
			gpio_bit_write (LED_Act_GPIO_Port, LED_Act_Pin, PinState);
			break;
		case _PILED_Error:
			gpio_bit_write (LED_Error_GPIO_Port, LED_Error_Pin, PinState);
			break;
		default: return;
	}
}
//--------------------------------------------------------------------------//
void init_LEDs (void)
{
	timer_oc_parameter_struct timer_ocintpara;
	timer_parameter_struct timer_initpara;
	
	rcu_periph_clock_enable (RCU_GPIOE);
	rcu_periph_clock_enable (RCU_TIMER8);
  rcu_timer_clock_prescaler_config (RCU_TIMER_PSC_MUL4);
	
	//LED_Hear - PE6
//	gpio_mode_set (GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_6);
//	gpio_output_options_set (GPIOE, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, GPIO_PIN_6);
//	gpio_bit_reset (GPIOE, GPIO_PIN_6);
	gpio_mode_set (LED_Hear_GPIO_Port, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, LED_Hear_Pin);
	gpio_output_options_set (LED_Hear_GPIO_Port, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, LED_Hear_Pin);
	gpio_bit_reset (LED_Hear_GPIO_Port, LED_Hear_Pin);

	//LED_Act - PE5 (TIMER8_CH0)
//	gpio_af_set (GPIOE, GPIO_AF_3, GPIO_PIN_5);
//	gpio_mode_set (GPIOE, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_5);
//	gpio_output_options_set (GPIOE, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, GPIO_PIN_5);
	gpio_af_set (LED_Act_GPIO_Port, GPIO_AF_3, LED_Act_Pin);
	gpio_mode_set (LED_Act_GPIO_Port, GPIO_MODE_AF, GPIO_PUPD_PULLUP, LED_Act_Pin);
	gpio_output_options_set (LED_Act_GPIO_Port, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, LED_Act_Pin);

	//LED_Error - PC13
//	gpio_mode_set (GPIOC, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_13);
//	gpio_output_options_set (GPIOC, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, GPIO_PIN_13);
//	gpio_bit_reset (GPIOC, GPIO_PIN_13);
	gpio_mode_set (LED_Error_GPIO_Port, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, LED_Error_Pin);
	gpio_output_options_set (LED_Error_GPIO_Port, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, LED_Error_Pin);
	gpio_bit_reset (LED_Error_GPIO_Port, LED_Error_Pin);

	//Init TIMER 8
	timer_deinit (TIMER8);
//	timer_primary_output_config (TIMER8, ENABLE);
	/* TIMER2 configuration */
	timer_initpara.prescaler         = 3999;
	timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
	timer_initpara.counterdirection  = TIMER_COUNTER_UP;
	timer_initpara.period            = 2000;
	timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
	timer_initpara.repetitioncounter = 0;
	timer_init (TIMER8, &timer_initpara);
	/* CH1 configuration in PWM mode */
	timer_ocintpara.ocpolarity  = TIMER_OC_POLARITY_LOW;
	timer_ocintpara.outputstate = TIMER_CCX_ENABLE;
	timer_ocintpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
	timer_ocintpara.outputnstate = TIMER_CCXN_DISABLE;
	timer_ocintpara.ocidlestate  = TIMER_OC_IDLE_STATE_HIGH;
	timer_ocintpara.ocnidlestate = TIMER_OC_IDLE_STATE_HIGH;
	timer_channel_output_config (TIMER8, TIMER_CH_0, &timer_ocintpara);
	/* CH1 configuration in PWM mode1, duty cycle 0% */
	timer_channel_output_pulse_value_config (TIMER8, TIMER_CH_0, 1000);
	timer_channel_output_mode_config (TIMER8, TIMER_CH_0, TIMER_OC_MODE_PWM1);
	timer_channel_output_shadow_config (TIMER8, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);
	/* auto-reload preload enable */
	timer_auto_reload_shadow_enable (TIMER8);
	timer_single_pulse_mode_config (TIMER8, TIMER_SP_MODE_SINGLE);
	timer_flag_clear (TIMER8, TIMER_FLAG_UP);
	timer_flag_clear (TIMER8, TIMER_FLAG_CH0);
	PingActiviti = 0;
}
//--------------------------------------------------------------------------//
void PingActivitiLED (void) {PingActiviti = 1;}
//--------------------------------------------------------------------------//
static void RoutineActivitiLED (void)
{
	if (!PingActiviti) return;
	if ((TIMER_CTL0 (TIMER8) & (uint32_t)TIMER_CTL0_CEN) != 0) return;
	PingActiviti = 0;
	timer_flag_clear (TIMER8, TIMER_FLAG_CH0);
	timer_enable (TIMER8);
}
//--------------------------------------------------------------------------//
static TModeHLED Mode;
static TModeHLED ModeNew;
static bool ChangeModeOrColor;
//--------------------------------------------------------------------------//
static void LED_Hear_On (void)
{
	gpio_bit_reset (LED_Hear_GPIO_Port, LED_Hear_Pin);
}
//--------------------------------------------------------------------------//
static void LED_Hear_Off (void)
{
	gpio_bit_set (LED_Hear_GPIO_Port, LED_Hear_Pin);
}
//--------------------------------------------------------------------------//
static void RoutineHearLED (void)
{
	static uint8_t HLEDUpdeteAppState = 0;
	static TTime LEDTO = 0;
	static bool LED = false;
	
	if (ChangeModeOrColor && EndTime (LEDTO))
	{
		Mode = ModeNew;
		ChangeModeOrColor = false;
	}
	switch (Mode)
	{
		default:
		case _HLEDOff: 
			LED = false;
			break;
		case _HLEDOn:
			LED = true;
			break;
		case _HLEDABlink:
			if (EndTime (LEDTO))
			{
				if (LED) LEDTO = SetTime_ms (900);
				else LEDTO = SetTime_ms (100);
				LED = !LED;
			}
			break;
		case _HLEDFBlink:
			if (EndTime (LEDTO))
			{
				LEDTO = SetTime_ms (100);
				LED = !LED;
			}
			break;
		case _HLEDSBlink:
			if (EndTime (LEDTO))
			{
				LEDTO = SetTime_ms (500);
				LED = !LED;
			}
			break;
		case _HLEDUpdeteApp:
			switch (HLEDUpdeteAppState)
			{
				case 0: //Long Off
					LED = false;
					if (EndTime (LEDTO))
					{
						LEDTO = SetTime_ms (100);
						HLEDUpdeteAppState++;
					}
					break;
				case 1: //On first
					LED = true;
					if (EndTime (LEDTO))
					{
						LEDTO = SetTime_ms (100);
						HLEDUpdeteAppState++;
					}
					break;
				case 2: //Short Off
					LED = false;
					if (EndTime (LEDTO))
					{
						LEDTO = SetTime_ms (100);
						HLEDUpdeteAppState++;
					}
					break;
				case 3: //On second
					LED = true;
					if (EndTime (LEDTO))
					{
						LEDTO = SetTime_ms (300);
						HLEDUpdeteAppState = 0U;
					}
					break;
			}
			break;
	}
	if (LED) LED_Hear_On ();
	else LED_Hear_Off ();	
}
//--------------------------------------------------------------------------//
void SetModeHLED (TModeHLED ModeLED/*, TColorHLED Color*/)
{
	ModeNew = ModeLED;
	ChangeModeOrColor = true;
}
//--------------------------------------------------------------------------//
static void LED_Error_On (void)
{
	gpio_bit_reset (LED_Error_GPIO_Port, LED_Error_Pin);
}
//--------------------------------------------------------------------------//
static void LED_Error_Off (void)
{
	gpio_bit_set (LED_Error_GPIO_Port, LED_Error_Pin);
}
//--------------------------------------------------------------------------//
static TModeErrorLED ModeErrorLED = _EFatalError;
static TModeErrorLED ErrorModeNew = _EFatalError;
static bool ChangeModeError;
//--------------------------------------------------------------------------//
static void RoutineErrorLED (void)
{
	static TTime LEDTO = 0;
	static bool LED = false;
	
	if (ChangeModeError && EndTime (LEDTO))
	{
		ModeErrorLED = ErrorModeNew;
		ChangeModeError = false;
	}
	switch (ModeErrorLED)
	{
		default:
		case _ENoError: 
			LED = false;
			break;
		case _EEnvError:
			if (EndTime (LEDTO))
			{
				if (LED) LEDTO = SetTime_ms (1900);
				else LEDTO = SetTime_ms (100);
				LED = !LED;
			}
			break;
		case _EIntError:
			if (EndTime (LEDTO))
			{
				LEDTO = SetTime_ms (1000);
				LED = !LED;
			}
			break;
		case _EFatalError:
			if (EndTime (LEDTO))
			{
				LEDTO = SetTime_ms (100);
				LED = !LED;
			}
			break;
	}
	if (LED) LED_Error_On ();
	else LED_Error_Off ();	
}
//--------------------------------------------------------------------------//
void SetModeErrorLED (TModeErrorLED ModeLED)
{
	ErrorModeNew = ModeLED;
	ChangeModeError = true;
}
//--------------------------------------------------------------------------//
void RoutineLEDs (void)
{
	RoutineActivitiLED ();
	RoutineHearLED ();
	RoutineErrorLED ();
}
//--------------------------------------------------------------------------//
