/*=============================================================================
2	  Project: ADCVibro_Digital
3	  Platform: GD32F470
4	  Filename: drv_MonADC1.h
5	  Description:
6	  Version: 0.0
7	  Created: 2022.08.23
8   Last modified: 2024.02.08
9	=============================================================================*/
#ifndef drv_MonADC1_H
#define drv_MonADC1_H
	//------------------------------------------------------------------------//
	typedef enum 
	{
		_ADC1_Monp1			= 0,
		_ADC1_Monn1			= 1,
		_ADC1_Monp2			= 2,
		_ADC1_Monn2			= 3,
		_ADC1_Monp3			= 4,
		_ADC1_Monn3			= 5,
		_ADC1_Monp4			= 6,
		_ADC1_Monn4			= 7,
		_ADC1_Monp5			= 8,
		_ADC1_Monn5			= 9,
		_ADC1_Monp6			= 10,
		_ADC1_Monn6			= 11,
		_ADC1_Monp7			= 12,
		_ADC1_Monn7			= 13,
		_ADC1_Monp8			= 14,
		_ADC1_Monn8			= 15,
		_ADC1_NumberChannel = 16
	} TADC1Channel;
		//------------------------------------------------------------------------//
	void InitMonADC1 (void);
	void RoutineMonADC1 (void);
	void DMA1_Channel2_IRQHandler (void);
	
	float ResultDataMonADC1AVG (TADC1Channel Chan);
	float ResultDataMonADC1Max (TADC1Channel Chan);
	float ResultDataMonADC1Min (TADC1Channel Chan);
	//------------------------------------------------------------------------//
#endif 
