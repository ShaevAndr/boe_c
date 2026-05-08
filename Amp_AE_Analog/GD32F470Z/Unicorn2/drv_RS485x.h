/*=============================================================================
2	  Project: ADCVibro_Digital
3	  Platform: GD32F470
4	  Filename: drv_RS485x.h
5	  Description:
6	  Version: 0.0
7	  Created: 2024.02.08
8   Last modified: 2024.02.08
9	=============================================================================*/
#ifndef drv_RS485x_H
#define drv_RS485x_H
	#include <gd32f4xx.h>
	#include <stdint.h>
	//------------------------------------------------------------------------//
	typedef enum
	{
		_RS485_1      = 0,
		_RS485_2      = 1,
		_RS485_3	    = 2,
		_RS485_Count  = 3
	} TRS485_Channel;
	typedef uint32_t USART_TypeDef;
	//------------------------------------------------------------------------//
	// Принятый байт с таймстампом (Timer12, 1 тик = 1 мкс, разрядность 16 бит,
	// период переполнения 65.536 мс — больше любого Modbus RTU t3.5).
	typedef struct
	{
		uint8_t  byte;
		uint16_t timestamp;
	} TRxByte;
	//------------------------------------------------------------------------//
	void RS485_SetSpeed (USART_TypeDef * UARTx, uint32_t Speed);
	USART_TypeDef * GetUART (TRS485_Channel Num);
//	void usart_interrupt_enable (USART_TypeDef * UART, uint32_t Bits);
//	void usart_interrupt_disable (USART_TypeDef * UART, uint32_t Bits);
	void RS485_Init (TRS485_Channel Num, uint32_t Speed);
	void RS485_Start_trs (TRS485_Channel Num);

	void USART1_IRQHandler (void);
	void USART5_IRQHandler (void);
	void UART7_IRQHandler (void);

	//------------------------------------------------------------------------//
	#undef _DebugFIFO

	void RS485_PushTxFIFO (TRS485_Channel Num, unsigned char b);
	TRxByte RS485_PopRxFIFO (TRS485_Channel Num);
	int RS485_ReceiveFIFO(TRS485_Channel Num, void *buf, unsigned int buf_size);
	uint16_t RS485_GetTick (void);

	uint32_t RS485_PushTxFIFOBuf (TRS485_Channel Num, const void *pBuff, uint32_t SizeBuff);

	uint32_t RS485_GetNumberOfByteRxFIFO (TRS485_Channel Num);
	uint32_t RS485_GetNumberOfFreeByteTxFIFO (TRS485_Channel Num);
	uint32_t RS485_GetNumberOfByteTxFIFO (TRS485_Channel Num);

	#ifdef _RS485_DebugFIFO
		void RS485_ErrorOverflowTxFIFO (TRS485_Channel Num);
		void RS485_ErrorUnderflowTxFIFO (TRS485_Channel Num);
		void RS485_ErrorOverflowRxFIFO (TRS485_Channel Num);
		void RS485_ErrorUnderflowRxFIFO (TRS485_Channel Num);
	#endif

#endif 
