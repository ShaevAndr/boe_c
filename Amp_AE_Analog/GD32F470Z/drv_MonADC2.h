/*=============================================================================
2	  Project: ADCVibro_Digital
3	  Platform: GD32F470
4	  Filename: drv_MonADC2.h
5	  Description:
6	  Version: 0.0
7	  Created: 2022.08.23
8   Last modified: 2024.12.09
9	=============================================================================*/
#ifndef drv_MonADC2_H
#define drv_MonADC2_H
	//------------------------------------------------------------------------//
	typedef enum 
	{
		_ADC2_p3_3Vd		= 0,
		_ADC2_p5Va			= 1,
		_ADC2_p7V				= 2,
		_ADC2_p24V			= 3,
		_ADC2_p27V			= 4,
		_ADC2_Vocm			= 5,
		
		_ADC2_NumberChannel = 6
	} TADC2Channel;
		//------------------------------------------------------------------------//
	void InitMonADC2 (void);
	void RoutineMonADC2 (void);
	void DMA1_Channel1_IRQHandler (void);
	
	float ResultDataMonADC2AVG (TADC2Channel Chan);
	float ResultDataMonADC2Max (TADC2Channel Chan);
	float ResultDataMonADC2Min (TADC2Channel Chan);
	//------------------------------------------------------------------------//
#endif 
