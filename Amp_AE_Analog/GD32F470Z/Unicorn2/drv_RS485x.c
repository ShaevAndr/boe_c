/*=============================================================================
2	  Project: ADCVibro_Digital
3	  Platform: GD32F470
4	  Filename: drv_RS485x.c
5	  Description:
6	  Version: 0.0
7	  Created: 2022.08.22
8   Last modified: 2024.02.08
9	=============================================================================*/
//#include <gd32f4xx.h>
//#include <stm32h7xx_hal.h>
#include <stdio.h>
#include "global.h" // min()
//--------------------------------------------------------------------------//
#include "drv_RS485x.h"
//#include "drv_TestPinKT.h"
#define _SizeFIFO 256
#define _IndexFIFOMask 0xFF
//--------------------------------------------------------------------------//
typedef struct
{
	uint32_t Init;
	uint32_t NumberOfBytesTxFIFO;
	uint32_t PushIndexToTxFIFO;
	uint32_t PopIndexToTxFIFO;
	uint32_t NumberOfBytesRxFIFO;
	uint32_t PushIndexToRxFIFO;
	uint32_t PopIndexToRxFIFO;
	uint8_t  TxFIFO [_SizeFIFO];
	TRxByte  RxFIFO [_SizeFIFO];
} TRS485State;
//--------------------------------------------------------------------------//
static volatile TRS485State RS485State [_RS485_Count];
static uint8_t Timer12Inited = 0;
//--------------------------------------------------------------------------//
// Timer12: free-running 16-bit counter, тик = 1 мкс, период = 65.536 мс.
// Используется для отметки момента приёма каждого байта в IRQ-обработчике,
// чтобы main-loop парсер мог точно измерять межбайтные/межкадровые паузы
// Modbus RTU без зависимости от джиттера планировщика опроса.
static void Timer12_Init (void)
{
	timer_parameter_struct timer_initpara;
	uint32_t timer_clk;
	uint32_t psc;

	if (Timer12Inited) return;

	rcu_periph_clock_enable (RCU_TIMER12);
	timer_deinit (TIMER12);

	// CK_TIMER12 = CK_APB1*2 (если APB1 prescaler != 1, иначе CK_APB1).
	// PSC выбирается так, чтобы получить 1 МГц (1 мкс на тик).
	timer_clk = rcu_clock_freq_get (CK_APB1);
	if (rcu_clock_freq_get (CK_AHB) != timer_clk) timer_clk *= 2U;
	psc = (timer_clk / 1000000U) - 1U;

	timer_initpara.prescaler         = (uint16_t)psc;
	timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
	timer_initpara.counterdirection  = TIMER_COUNTER_UP;
	timer_initpara.period            = 0xFFFFU;
	timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
	timer_initpara.repetitioncounter = 0U;
	timer_init (TIMER12, &timer_initpara);

	timer_enable (TIMER12);
	Timer12Inited = 1;
}
//--------------------------------------------------------------------------//
uint16_t RS485_GetTick (void)
{
	return ((uint16_t)timer_counter_read (TIMER12));
}
//--------------------------------------------------------------------------//
USART_TypeDef * GetUART (TRS485_Channel Num)
{
	switch (Num)
	{
		case _RS485_1: return ((USART_TypeDef *)USART1);
		case _RS485_2: return ((USART_TypeDef *)USART5);
		case _RS485_3: return ((USART_TypeDef *)UART7);
		default: while (1);
	}
}
//--------------------------------------------------------------------------//
static void TrsDrvEnable (TRS485_Channel Num)
{
	switch (Num)
	{
		case _RS485_1: gpio_bit_set (GPIOB, GPIO_PIN_13); break;
		case _RS485_2: gpio_bit_set (GPIOB, GPIO_PIN_12); break;
		case _RS485_3: gpio_bit_set (GPIOE, GPIO_PIN_2); break;
		default: while (1);
	}}
//--------------------------------------------------------------------------//
static void TrsDrvDisable (TRS485_Channel Num)
{
	switch (Num)
	{
		case _RS485_1: gpio_bit_reset (GPIOB, GPIO_PIN_13); break;
		case _RS485_2: gpio_bit_reset (GPIOB, GPIO_PIN_12); break;
		case _RS485_3: gpio_bit_reset (GPIOE, GPIO_PIN_2); break;
		default: while (1);
	}
}
//--------------------------------------------------------------------------//
static void RS485_InitGPIO (TRS485_Channel Num)
{
	switch (Num)
	{
		case _RS485_1:
			rcu_periph_clock_enable (RCU_GPIOB);
			rcu_periph_clock_enable (RCU_GPIOD);
			/* connect port to USART1_T/R */
			gpio_mode_set (GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_13);
			gpio_output_options_set (GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO_PIN_13);
			gpio_bit_reset (GPIOB, GPIO_PIN_13);
			/* connect port to USART1_Tx */
			gpio_af_set (GPIOD, GPIO_AF_7, GPIO_PIN_5);
			/* connect port to USART1_Rx */
			gpio_af_set (GPIOD, GPIO_AF_7, GPIO_PIN_6);
			/* configure USART Tx as alternate function push-pull */
			gpio_mode_set (GPIOD, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_5);
			gpio_output_options_set (GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_5);
			/* configure USART Rx as alternate function open drain */
			gpio_mode_set (GPIOD, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_6);
			gpio_output_options_set (GPIOD, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, GPIO_PIN_6);
			break;
		case _RS485_2:
			rcu_periph_clock_enable (RCU_GPIOC | RCU_GPIOB);
			/* connect port to USART5_T/R */
			gpio_mode_set (GPIOB, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_12);
			gpio_output_options_set (GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO_PIN_12);
			gpio_bit_reset (GPIOB, GPIO_PIN_12);
			/* connect port to USART5_Tx */
			gpio_af_set (GPIOC, GPIO_AF_8, GPIO_PIN_6);
			/* connect port to USART5_Rx */
			gpio_af_set (GPIOC, GPIO_AF_8, GPIO_PIN_7);
			/* configure USART Tx as alternate function push-pull */
			gpio_mode_set (GPIOC, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_6);
			gpio_output_options_set (GPIOC, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_6);
			/* configure USART Rx as alternate function open drain */
			gpio_mode_set (GPIOC, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_7);
			gpio_output_options_set (GPIOC, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, GPIO_PIN_7);
			break;
		case _RS485_3:
			rcu_periph_clock_enable (RCU_GPIOE);
			/* connect port to UART7_T/R */
			gpio_mode_set (GPIOE, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_PIN_2);
			gpio_output_options_set (GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO_PIN_2);
			gpio_bit_reset (GPIOE, GPIO_PIN_2);
			/* connect port to UART7_Tx */
			gpio_af_set (GPIOE, GPIO_AF_8, GPIO_PIN_1);
			/* connect port to UART7_Rx */
			gpio_af_set (GPIOE, GPIO_AF_8, GPIO_PIN_0);
			/* configure UART Tx as alternate function push-pull */
			gpio_mode_set (GPIOE, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_1);
			gpio_output_options_set (GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, GPIO_PIN_1);
			/* configure UART Rx as alternate function open drain */
			gpio_mode_set (GPIOE, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_0);
			gpio_output_options_set (GPIOE, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, GPIO_PIN_0);
			break;
		default: while (1);
	}
}
//--------------------------------------------------------------------------//
void RS485_SetSpeed (USART_TypeDef * UARTx, uint32_t Speed)
{
  usart_disable ((uint32_t)UARTx);
	usart_baudrate_set ((uint32_t)UARTx, Speed);
	usart_enable ((uint32_t)UARTx);
}
//--------------------------------------------------------------------------//
//void usart_interrupt_enable (USART_TypeDef * UART, uint32_t Bits)
//{
//	UART->CR1 |= Bits & (USART_CR1_RXNEIE_RXFNEIE_Msk | USART_CR1_TXEIE_TXFNFIE_Msk);
//}
//--------------------------------------------------------------------------//
//void usart_interrupt_disable (USART_TypeDef * UART, uint32_t Bits)
//{
//	UART->CR1 &= ~(Bits & (USART_CR1_RXNEIE_RXFNEIE_Msk | USART_CR1_TXEIE_TXFNFIE_Msk));
//}
//--------------------------------------------------------------------------//
static void RS485_InitUART (TRS485_Channel Num, uint32_t Speed)
{
  uint32_t UART = (uint32_t)(GetUART (Num));
	switch (Num)
	{
		case _RS485_1:
			nvic_irq_enable (USART1_IRQn, 0, 0);
			rcu_periph_clock_enable (RCU_USART1);
			break;
		case _RS485_2:
			nvic_irq_enable (USART5_IRQn, 0, 0);
			rcu_periph_clock_enable (RCU_USART5);
			break;
		case _RS485_3:
			nvic_irq_enable (UART7_IRQn, 0, 0);
			rcu_periph_clock_enable (RCU_UART7);
			break;
		default: while (1);
	}
        
	usart_deinit (UART);
	usart_transmit_config (UART, USART_TRANSMIT_ENABLE);
	usart_receive_config (UART, USART_RECEIVE_ENABLE);
	RS485_SetSpeed ((USART_TypeDef *)UART, Speed);
	usart_interrupt_enable (UART, USART_INT_RBNE);
}
//-----------------------------------------------------------------------------------------------//
void RS485_Init (TRS485_Channel Num, uint32_t Speed)
{
	if (RS485State [Num].Init == 3) return;

	Timer12_Init ();

	RS485State [Num].NumberOfBytesTxFIFO = 0;
	RS485State [Num].PushIndexToTxFIFO = 0;
	RS485State [Num].PopIndexToTxFIFO = 0;

	RS485State [Num].NumberOfBytesRxFIFO = 0;
	RS485State [Num].PushIndexToRxFIFO = 0;
	RS485State [Num].PopIndexToRxFIFO = 0;

	RS485_InitGPIO (Num);
	RS485_InitUART (Num, Speed);
	RS485State [Num].Init = 3;
}
//--------------------------------------------------------------------------//
static void IrqHandler (TRS485_Channel Num)
{
	volatile TRS485State * p = &(RS485State [Num]);
	uint32_t UART = (uint32_t)GetUART (Num);
	
	if(RESET != usart_interrupt_flag_get (UART, USART_INT_FLAG_RBNE))
	{/* receive data */
		uint8_t rb = (uint8_t)usart_data_receive (UART);
//		printf ("%02X", rb);
		if ((_SizeFIFO - 1) > p->NumberOfBytesRxFIFO)
		{
			p->RxFIFO [p->PushIndexToRxFIFO].byte = rb;
			p->RxFIFO [p->PushIndexToRxFIFO].timestamp =
				(uint16_t)timer_counter_read (TIMER12);
			__DMB ();   // данные видимы до инкремента сигнального счётчика
			p->NumberOfBytesRxFIFO++;
			p->PushIndexToRxFIFO++;
			p->PushIndexToRxFIFO &= _IndexFIFOMask;
		}
#ifdef _DebugFIFO
		else
			ErrorOverflowRxFIFO ();
#endif
	}
	if(RESET != usart_interrupt_flag_get (UART, USART_INT_FLAG_TBE))
	{ /* transmit data */
		if (p->NumberOfBytesTxFIFO > 0)
		{
				usart_data_transmit (UART, p->TxFIFO [p->PopIndexToTxFIFO]);
				p->NumberOfBytesTxFIFO--;
				p->PopIndexToTxFIFO++;
				p->PopIndexToTxFIFO &= _IndexFIFOMask;
		}
		else
		{
			usart_interrupt_disable (UART, USART_INT_TBE);
			usart_interrupt_enable (UART, USART_INT_TC);
		}
	}
	if(RESET != usart_interrupt_flag_get (UART, USART_INT_FLAG_TC))
	{ /* transmit data */
		usart_interrupt_disable (UART, USART_INT_TC);
		if (p->NumberOfBytesTxFIFO != 0)
			usart_interrupt_enable (UART, USART_INT_TBE);
		else
			TrsDrvDisable (Num);
	}
	if (RESET != usart_interrupt_flag_get (UART, USART_INT_FLAG_RBNE_ORERR))
		usart_interrupt_flag_clear (UART, USART_INT_FLAG_RBNE_ORERR);
		
/*		
      \arg        USART_INT_FLAG_EB:   end of block interrupt and flag
      \arg        USART_INT_FLAG_RT:   receiver timeout interrupt and flag
      \arg        USART_INT_FLAG_AM:   address match interrupt and flag 
      \arg        USART_INT_FLAG_PERR: parity error interrupt and flag 
      \arg        :  transmitter buffer empty interrupt and flag 
      \arg        USART_INT_FLAG_TC:   transmission complete interrupt and flag
      \arg        : read data buffer not empty interrupt and flag
      \arg        USART_INT_FLAG_RBNE_ORERR: read data buffer not empty interrupt and overrun error flag
      \arg        USART_INT_FLAG_IDLE: IDLE line detected interrupt and flag
      \arg        USART_INT_FLAG_LBD:  LIN break detected interrupt and flag 
      \arg        USART_INT_FLAG_WU:   wakeup from deep-sleep mode interrupt and flag
      \arg        USART_INT_FLAG_CTS:  CTS interrupt and flag
      \arg        USART_INT_FLAG_ERR_NERR:  error interrupt and noise error flag
      \arg        USART_INT_FLAG_ERR_ORERR: error interrupt and overrun error
      \arg        USART_INT_FLAG_ERR_FERR:  error interrupt and frame error flag
*/	
	
	
}
//--------------------------------------------------------------------------//
void USART1_IRQHandler (void) {IrqHandler (_RS485_1);}
//--------------------------------------------------------------------------//
void USART5_IRQHandler (void) {IrqHandler (_RS485_2);}
//--------------------------------------------------------------------------//
void UART7_IRQHandler (void) {IrqHandler (_RS485_3);}
//--------------------------------------------------------------------------//
void RS485_Start_trs (TRS485_Channel Num)
{
	TrsDrvEnable (Num);
	usart_interrupt_enable ((uint32_t)(GetUART (Num)), USART_INT_TBE);
}
//--------------------------------------------------------------------------//
void RS485_PushTxFIFO (TRS485_Channel Num, uint8_t b)
{
	volatile TRS485State * p = &(RS485State [Num]);
	uint32_t t;
	if ((_SizeFIFO - 1) > p->NumberOfBytesTxFIFO)
	{
		p->TxFIFO [p->PushIndexToTxFIFO] = b;
		do {
			t = __LDREXW (&(p->NumberOfBytesTxFIFO));
			t++;
		} while (__STREXW (t, &(p->NumberOfBytesTxFIFO)));
		p->PushIndexToTxFIFO++;
		p->PushIndexToTxFIFO &= _IndexFIFOMask;
		RS485_Start_trs (Num);
	}
#ifdef _DebugFIFO
	else
		ErrorOverflowTxFIFO (TRS485_Channel Num);
#endif
}
//--------------------------------------------------------------------------//
uint32_t RS485_PushTxFIFOBuf (TRS485_Channel Num, const void *pBuff, uint32_t SizeBuff)
{
	volatile TRS485State *p = &(RS485State [Num]);
  uint32_t i, tx_count = min(SizeBuff, RS485_GetNumberOfFreeByteTxFIFO(Num));
  
  if (tx_count > 0)
  {
    for (i = 0; i < tx_count; i++)
    {
      p->TxFIFO [p->PushIndexToTxFIFO] = ((const uint8_t*)pBuff)[i];
      p->PushIndexToTxFIFO++;
      p->PushIndexToTxFIFO &= _IndexFIFOMask;
    }
    do {
      i = __LDREXW (&(p->NumberOfBytesTxFIFO));
      i += tx_count;
    } while (__STREXW (i, &(p->NumberOfBytesTxFIFO)));
    RS485_Start_trs (Num);
  }
  return (tx_count);
}
//--------------------------------------------------------------------------//
TRxByte RS485_PopRxFIFO (TRS485_Channel Num)
{
	volatile TRS485State *p = &(RS485State [Num]);
	uint32_t t;
	TRxByte rx = {0, 0};

	if (p->NumberOfBytesRxFIFO > 0)
	{
		__DMB ();   // увидеть .byte/.timestamp, записанные в IRQ до инкремента счётчика
		rx = p->RxFIFO [p->PopIndexToRxFIFO];
		do {
			t = __LDREXW (&(p->NumberOfBytesRxFIFO));
			t--;
		} while (__STREXW (t, &(p->NumberOfBytesRxFIFO)));
		p->PopIndexToRxFIFO++;
		p->PopIndexToRxFIFO &= _IndexFIFOMask;
	}
#ifdef _DebugFIFO
	else
		RS485_ErrorUnderflowRxFIFO (TRS485_Channel Num);
#endif
	return (rx);
}
//--------------------------------------------------------------------------//
int RS485_ReceiveFIFO(TRS485_Channel Num, void *buf, unsigned int buf_size)
{
  volatile TRS485State *p = &(RS485State [Num]);
	uint32_t i, recv_count = min(p->NumberOfBytesRxFIFO, buf_size);

  if (recv_count > 0)
  {
    for (i = 0; i < recv_count; i++)
    {
      ((uint8_t*)buf)[i] = p->RxFIFO [p->PopIndexToRxFIFO++].byte;
      p->PopIndexToRxFIFO &= _IndexFIFOMask;
    }
    do {
      i = __LDREXW (&(p->NumberOfBytesRxFIFO));
      i -= recv_count;
    } while (__STREXW (i, &(p->NumberOfBytesRxFIFO)));
  }
	return (recv_count);
}
//--------------------------------------------------------------------------//
uint32_t RS485_GetNumberOfByteRxFIFO (TRS485_Channel Num)
{
	return (RS485State [Num].NumberOfBytesRxFIFO);
}
//--------------------------------------------------------------------------//
uint32_t RS485_GetNumberOfFreeByteTxFIFO (TRS485_Channel Num)
{
	return (_SizeFIFO - RS485State [Num].NumberOfBytesTxFIFO);
}
//--------------------------------------------------------------------------//
uint32_t RS485_GetNumberOfByteTxFIFO (TRS485_Channel Num)
{
	return (RS485State [Num].NumberOfBytesTxFIFO);
}
//--------------------------------------------------------------------------//
