/*=============================================================================
2	  Project:
3	  Platform: GD32F470
4	  Filename: drv_uart.c
5	  Description:
6	  Version: 0.0
7	  Created: 2022.08.22
8   Last modified: 2024.06.26
9	=============================================================================*/
#include <gd32f4xx.h>
//--------------------------------------------------------------------------//
#include "drv_UART.h"
#define _SizeFIFO 256
#define _InedexFIFOMask 0xFF
#define _ChannNum (1)
//--------------------------------------------------------------------------//
//static volatile uint8_t TxFIFO [_SizeFIFO] __attribute__((aligned(_SizeFIFO)));
static volatile uint8_t RxFIFO [_ChannNum][_SizeFIFO] __attribute__((aligned(_SizeFIFO)));
//--------------------------------------------------------------------------//
//static volatile uint32_t NumberOfBytesTxFIFO = 0;
//static volatile uint32_t PushIndexToTxFIFO = 0;
//static volatile uint32_t PopIndexToTxFIFO = 0;
//--------------------------------------------------------------------------//
static volatile uint32_t NumberOfBytesRxFIFO [_ChannNum] = {0};
static volatile uint32_t PushIndexToRxFIFO [_ChannNum] = {0};
static volatile uint32_t PopIndexToRxFIFO [_ChannNum] = {0};
//--------------------------------------------------------------------------//
static uint32_t GetBuffNumForUARTNum (uint32_t UARTNum)
{
	return (0);
}
//--------------------------------------------------------------------------//
static void UART_SetSpeed (uint32_t UARTNum, uint32_t Speed)
{
//	usart_deinit (UART3);
//	usart_baudrate_set (UART3, Speed);
//	usart_transmit_config (UART3, USART_TRANSMIT_ENABLE);
//	usart_enable (UART3);

	if (UARTNum != UART3) return;
	usart_deinit (UARTNum);
	usart_baudrate_set (UARTNum, Speed);
	usart_receive_config (UARTNum, USART_RECEIVE_ENABLE);
	usart_enable (UARTNum);
	usart_interrupt_enable (UARTNum, USART_INT_RBNE);
}
//-----------------------------------------------------------------------------------------------//
void UartInit (uint32_t UARTNum, uint32_t Speed)
{
//	NumberOfBytesTxFIFO = 0;
//	PushIndexToTxFIFO = 0;
//	PopIndexToTxFIFO = 0;

	NumberOfBytesRxFIFO [0] = 0;
	PushIndexToRxFIFO [0] = 0;
	PopIndexToRxFIFO [0] = 0;

	if (UARTNum == UART3)
	{
		nvic_irq_enable (UART3_IRQn, 0, 0);
		rcu_periph_clock_enable (RCU_GPIOC);
		rcu_periph_clock_enable (RCU_UART3);

		/* connect port to USART5_Rx */
		gpio_af_set (GPIOC, GPIO_AF_7, GPIO_PIN_11);

		/* configure USART5 Rx as alternate function push-pull */
		gpio_mode_set (GPIOC, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_11);
	}
	
	UART_SetSpeed (UARTNum, Speed);
}
//--------------------------------------------------------------------------//
static void UART_Handler (uint32_t UARTNum, uint32_t BuffNum)
{
	if(RESET != usart_interrupt_flag_get (UARTNum, USART_INT_FLAG_RBNE))
	{/* receive data */
		uint8_t rb = (uint8_t)usart_data_receive (UARTNum);
		if ((_SizeFIFO - 1) > NumberOfBytesRxFIFO [BuffNum])
		{
			RxFIFO [BuffNum][PushIndexToRxFIFO [BuffNum]] = rb;
			NumberOfBytesRxFIFO [BuffNum]++;
			PushIndexToRxFIFO [BuffNum]++;
			PushIndexToRxFIFO [BuffNum] &= _InedexFIFOMask;
		}
#ifdef _DebugFIFO
		else
			ErrorOverflowRxFIFO ();
#endif
	}
	if(RESET != usart_interrupt_flag_get (UARTNum, USART_INT_FLAG_TBE))
	{ /* transmit data */
		usart_interrupt_disable (UARTNum, USART_INT_TBE);
	}
	
	if (RESET != usart_interrupt_flag_get (UARTNum, USART_INT_FLAG_RBNE_ORERR))
		usart_interrupt_flag_clear (UARTNum, USART_INT_FLAG_RBNE_ORERR);
}
//--------------------------------------------------------------------------//
void UART3_IRQHandler (void)
{
	UART_Handler (UART3, GetBuffNumForUARTNum (UART3));
}
//--------------------------------------------------------------------------//
/*
void UARTStart_trs (void)
{
	usart_interrupt_disable (UART3, USART_INT_TBE);
}
*/
//--------------------------------------------------------------------------//
/*
void UARTPushTxFIFO (unsigned char b)
{
	uint32_t t;
	if ((_SizeFIFO - 1) > NumberOfBytesTxFIFO)
	{
		TxFIFO [PushIndexToTxFIFO] = b;
		do {
			t = __LDREXW (&NumberOfBytesTxFIFO);
			t++;
		} while (__STREXW (t, &NumberOfBytesTxFIFO));
		PushIndexToTxFIFO++;
		PushIndexToTxFIFO &= _InedexFIFOMask;
		usart_interrupt_enable (UART3, USART_INT_TBE);
	}
#ifdef _DebugFIFO
	else
		ErrorOverflowTxFIFO ();
#endif
}
*/
//--------------------------------------------------------------------------//
/*
uint32_t UARTPushTxFIFOBuf (const uint8_t * pBuff, uint32_t SizeBuff)
{
	uint32_t i, ret = PM_UARTGetNumberOfFreeByteTxFIFO ();
	if (SizeBuff < ret) ret = SizeBuff;
	for (i = 0; i < ret; i++)
		PM_UARTPushTxFIFO (pBuff [i]);
	return (ret);
}
*/
//--------------------------------------------------------------------------//
unsigned char UARTPopRxFIFO (uint32_t UARTNum)
{
	uint8_t b = 0;
	uint32_t t;
	uint32_t Num = GetBuffNumForUARTNum (UARTNum);
	if (NumberOfBytesRxFIFO > 0)
	{
		b = RxFIFO [Num][PopIndexToRxFIFO [Num]];
		do {
			t = __LDREXW (&NumberOfBytesRxFIFO [Num]);
			t--;
		} while (__STREXW (t, &NumberOfBytesRxFIFO [Num]));
		PopIndexToRxFIFO [Num]++;
		PopIndexToRxFIFO [Num] &= _InedexFIFOMask;
	}
#ifdef _DebugFIFO
	else
		ErrorUnderflowRxFIFO ();
#endif
	return (b);
}
//--------------------------------------------------------------------------//
uint32_t UARTGetNumberOfByteRxFIFO (uint32_t UARTNum)
{
	uint32_t Num = GetBuffNumForUARTNum (UARTNum);
	return (NumberOfBytesRxFIFO [Num]);
}
//--------------------------------------------------------------------------//
/*
uint32_t PM_UARTGetNumberOfFreeByteTxFIFO (uint32_t UARTNum)
{
	uint32_t Num = GetBuffNumForUARTNum (UARTNum);
	return (_SizeFIFO - NumberOfBytesTxFIFO [Num]);
}
//--------------------------------------------------------------------------//
uint32_t PM_UARTGetNumberOfByteTxFIFO (uint32_t UARTNum)
{
	uint32_t Num = GetBuffNumForUARTNum (UARTNum);
	return (NumberOfBytesTxFIFO [Num]);
}
*/
//--------------------------------------------------------------------------//


