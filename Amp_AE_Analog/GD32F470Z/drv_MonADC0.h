/*=============================================================================
2	  Project: ADCVibro_Digital
3	  Platform: GD32F470
4	  Filename: drv_MonADC0.h
5	  Description:
6	  Version: 0.0
7	  Created: 2022.08.23
8   Last modified: 2024.02.08
9	=============================================================================*/
#ifndef drv_MonADC0_H
#define drv_MonADC0_H
	//------------------------------------------------------------------------//
	typedef enum 
	{
		_ADC0_Temp			= 0,
		_ADC0_Vbat			= 1,
		_ADC0_NumberChannel = 2
	} TADC0Channel;
		//------------------------------------------------------------------------//
	void InitMonADC0 (void);
	void RoutineMonADC0 (void);
	void DMA1_Channel4_IRQHandler (void);
	
	float ResultDataMonADC0AVG (TADC0Channel Chan);
	float ResultDataMonADC0Max (TADC0Channel Chan);
	float ResultDataMonADC0Min (TADC0Channel Chan);
	//------------------------------------------------------------------------//
#endif 
