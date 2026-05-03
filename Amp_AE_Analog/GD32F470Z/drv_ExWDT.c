//--------------------------------------------------------------------------//
#include <gd32f4xx.h>
//--------------------------------------------------------------------------//
#include "drv_ExWDT.h"
//--------------------------------------------------------------------------//
#define _PORTRCU_ExWDT_WDI RCU_GPIOB
#define _PORT_ExWDT_WDI GPIOB
#define _PIN_ExWDT_WDI GPIO_PIN_9
//--------------------------------------------------------------------------//
static bit_status WDI = RESET;
//--------------------------------------------------------------------------//
void Disable_ExWDT (void)
{
	#ifdef _ExWDTEnable
		rcu_periph_clock_enable (_PORTRCU_ExWDT_WDI);
		gpio_mode_set (_PORT_ExWDT_WDI, GPIO_MODE_INPUT, GPIO_PUPD_NONE, _PIN_ExWDT_WDI);
		gpio_output_options_set (_PORT_ExWDT_WDI, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, _PIN_ExWDT_WDI);
		gpio_bit_set (_PORT_ExWDT_WDI, _PIN_ExWDT_WDI);
	#endif
}
//--------------------------------------------------------------------------//
void Enable_ExWDT (void)
{
	#ifdef _ExWDTEnable
		rcu_periph_clock_enable (_PORTRCU_ExWDT_WDI);
		gpio_mode_set (_PORT_ExWDT_WDI, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, _PIN_ExWDT_WDI);
		gpio_output_options_set (_PORT_ExWDT_WDI, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, _PIN_ExWDT_WDI);
		Restart_ExWDT ();
	#endif
}
//--------------------------------------------------------------------------//
void Restart_ExWDT (void)
{
	#ifdef _ExWDTEnable
		gpio_bit_write (_PORT_ExWDT_WDI, _PIN_ExWDT_WDI, WDI);
		WDI = !WDI;
	#endif
}
//--------------------------------------------------------------------------//
