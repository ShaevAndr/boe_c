/*=============================================================================
2	  Project:
3	  Platform: GD32F407
4	  Filename: drv_UART.h
5	  Description:
6	  Version: 0.0
7	  Created: 2023.10.26
8   Last modified: 2024.06.26
9	=============================================================================*/
#ifndef _drv_UART_H
#define _drv_UART_H

	#include <stdint.h>
	//------------------------------------------------------------------------//
//	void UART_SetSpeed (uint32_t UARTNum, uint32_t Speed);
	void UartInit (uint32_t UARTNum, uint32_t Speed);
//	void UARTStart_trs (void);

//	void USART1_Handler (void);
//	void UART3_Handler (void);
	void UART3_IRQHandler (void);
	
	//------------------------------------------------------------------------//
	#undef _DebugFIFO

//	void UARTPushTxFIFO (uint32_t UARTNum, unsigned char b);
	unsigned char UARTPopRxFIFO (uint32_t UARTNum);

//	uint32_t UARTPushTxFIFOBuf (uint32_t UARTNum, const uint8_t * pBuff, uint32_t SizeBuff);

	uint32_t UARTGetNumberOfByteRxFIFO (uint32_t UARTNum);
//	uint32_t UARTGetNumberOfFreeByteTxFIFO (uint32_t UARTNum);
//	uint32_t UARTGetNumberOfByteTxFIFO (uint32_t UARTNum);

#endif 
