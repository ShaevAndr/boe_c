//--------------------------------------------------------------------------//
#include <gd32f4xx.h>
//--------------------------------------------------------------------------//
#include "drv_TestPinKT.h"
//--------------------------------------------------------------------------//
void init_TestPinKT (void)
{
	rcu_periph_clock_enable (RCU_GPIOA);
	//KT_PIN101 - PA9; KT_PIN102 - PA10
	gpio_mode_set (GPIOA, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_9 | GPIO_PIN_10);
	gpio_output_options_set (GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_9 | GPIO_PIN_10);
	gpio_bit_reset (GPIOA, GPIO_PIN_9 | GPIO_PIN_10);
}
//--------------------------------------------------------------------------//
void SetKT_PIN102 (void) {gpio_bit_set (GPIOA, GPIO_PIN_10);}
//--------------------------------------------------------------------------//
void ResetKT_PIN102 (void) {gpio_bit_reset (GPIOA, GPIO_PIN_10);}
//--------------------------------------------------------------------------//
void ToggleKT_PIN102 (void) {gpio_bit_toggle (GPIOA, GPIO_PIN_10);}
//--------------------------------------------------------------------------//
void SetKT_PIN101 (void) {gpio_bit_set (GPIOA, GPIO_PIN_9);}
//--------------------------------------------------------------------------//
void ResetKT_PIN101 (void) {gpio_bit_reset (GPIOA, GPIO_PIN_9);}
//--------------------------------------------------------------------------//
void ToggleKT_PIN101 (void) {gpio_bit_toggle (GPIOA, GPIO_PIN_9);}
//--------------------------------------------------------------------------//
