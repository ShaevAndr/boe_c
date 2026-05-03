/*=============================================================================
2	  Project: ADCVibro_Digital
3	  Platform: GD32F470
4	  Filename: drv_MonADC0.c
5	  Description:
6	  Version: 0.0
7	  Created: 2022.08.23
8   Last modified: 2024.02.08
9	=============================================================================*/
#include <gd32f4xx.h>
#include <stdbool.h>
#include <string.h> 
//-----------------------------------------------------------------------------
#include "drv_time.h"
#include "drv_MonADC0.h"
//--------------------------------------------------------------------------//
#define _ADC0Vref (3.0)
#define _ADC0NumMeans (1023)
#define _ADC0dVref (_ADC0Vref / 4096.0)
#define _TempSlope (4.1e-3)
#define _TempOfset (1.45)
//--------------------------------------------------------------------------//
static const float CalibADC0CoeffMul [_ADC0_NumberChannel] = 
{
		1.0000f, //_ADC0_Temp = 16
		4.0000f, //_ADC0_Vbat = 17
};
//--------------------------------------------------------------------------//
static float ADC0ResultAVG [_ADC0_NumberChannel];
static float ADC0ResultMax [_ADC0_NumberChannel];
static float ADC0ResultMin [_ADC0_NumberChannel];
static uint32_t SSumADC0 [_ADC0_NumberChannel];
static uint16_t SMaxADC0 [_ADC0_NumberChannel];
static uint16_t SMinADC0 [_ADC0_NumberChannel];
static uint32_t SCount;
static uint32_t SumADC0 [_ADC0_NumberChannel];
static uint16_t MaxADC0 [_ADC0_NumberChannel];
static uint16_t MinADC0 [_ADC0_NumberChannel];
static bool ADC0ReadyData;

#define _ADC0DMAChannSamplNum (16)
#define _ADC0DMABuffSize (_ADC0_NumberChannel * _ADC0DMAChannSamplNum)
static uint16_t ADC0DMABuff0 [_ADC0DMABuffSize];
static uint16_t ADC0DMABuff1 [_ADC0DMABuffSize];
//--------------------------------------------------------------------------//
void InitMonADC0 (void)
{
	timer_oc_parameter_struct timer_ocintpara;
	timer_parameter_struct timer_initpara;
	dma_multi_data_parameter_struct dma_init_parameter;
	int32_t i;
	
	for (i = 0; i < _ADC0_NumberChannel; i++)
	{
		ADC0ResultAVG [i] = ADC0ResultMax [i] = ADC0ResultMin [i] = 0;
		SSumADC0 [i] = SumADC0 [i] = 0;
		SMaxADC0 [i] = MaxADC0 [i] = SMinADC0 [i] = 0;
		MinADC0 [i] = 0xffff;
	}
	ADC0ReadyData = false;

	rcu_periph_clock_enable (RCU_DMA1);
//	rcu_periph_clock_enable (RCU_TIMER1);
	rcu_periph_clock_enable (RCU_TIMER4);
	rcu_periph_clock_enable (RCU_ADC0);

	/* config the GPIO as analog mode */
//	gpio_mode_set (GPIOC, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_0);//_ADC0_Monn8 - IN10

	/* reset ADC */
	adc_deinit ();
	/* configure the ADC clock for all the ADCs */
	adc_clock_config (ADC_ADCCK_HCLK_DIV20);
//	adc_clock_config (ADC_ADCCK_HCLK_DIV5);

	/* ADC synchronization */
	/* configure the ADC sync mode */
	adc_sync_mode_config (ADC_SYNC_MODE_INDEPENDENT);
	/* configure ADC sync DMA mode selection */
	adc_sync_dma_config (ADC_SYNC_DMA_DISABLE);

	/* configure ADC resolution */
	adc_resolution_config (ADC0, ADC_RESOLUTION_12B);
	/* configure ADC data alignment */
//	adc_data_alignment_config (ADC2, ADC_DATAALIGN_LEFT);
	adc_data_alignment_config (ADC0, ADC_DATAALIGN_RIGHT);
	/* configure ADC external trigger source */
//	adc_external_trigger_source_config (ADC0, ADC_ROUTINE_CHANNEL, ADC_EXTTRIG_ROUTINE_T1_CH1);
	adc_external_trigger_source_config (ADC0, ADC_ROUTINE_CHANNEL, ADC_EXTTRIG_ROUTINE_T4_CH1);
	/* enable ADC external trigger */
	adc_external_trigger_config (ADC0, ADC_ROUTINE_CHANNEL, EXTERNAL_TRIGGER_RISING);

	/* enable or disable ADC special function */
	adc_special_function_config (ADC0, ADC_SCAN_MODE, ENABLE);
	adc_special_function_config (ADC0, ADC_CONTINUOUS_MODE, DISABLE);
	adc_special_function_config (ADC0, ADC_INSERTED_CHANNEL_AUTO, DISABLE);

	/* configure temperature sensor and internal reference voltage channel or VBAT channel function */
	adc_channel_16_to_18 (ADC_TEMP_VREF_CHANNEL_SWITCH, ENABLE);
	adc_channel_16_to_18 (ADC_VBAT_CHANNEL_SWITCH, ENABLE);
	
	/* configure ADC discontinuous mode */
	adc_discontinuous_mode_config (ADC0, ADC_CHANNEL_DISCON_DISABLE, 0);
	/* configure the length of regular channel group or inserted channel group */
	adc_channel_length_config (ADC0, ADC_ROUTINE_CHANNEL, _ADC0_NumberChannel);
	/* configure ADC regular channel */
	adc_routine_channel_config (ADC0, (uint8_t)_ADC0_Temp,  	ADC_CHANNEL_16, ADC_SAMPLETIME_480);
	adc_routine_channel_config (ADC0, (uint8_t)_ADC0_Vbat,  	ADC_CHANNEL_18, ADC_SAMPLETIME_480);

	/* disable ADC oversample mode */
	adc_oversample_mode_disable (ADC0);

	/* when DMA=1, the DMA engine issues a request at end of each regular conversion */
	adc_dma_request_after_last_enable (ADC0);
	/* enable DMA request */
	adc_dma_mode_enable (ADC0);

	memset (ADC0DMABuff0, 0, sizeof (ADC0DMABuff0));
	memset (ADC0DMABuff1, 0, sizeof (ADC0DMABuff1));
	
	dma_multi_data_para_struct_init (&dma_init_parameter);
	/* deinitialize DMA1_CH0 */
	dma_deinit (DMA1, DMA_CH4);

	nvic_irq_enable (DMA1_Channel4_IRQn, 3, 2);

	dma_init_parameter.periph_addr = (int32_t)&ADC_RDATA (ADC0);		/*!< peripheral base address */
	dma_init_parameter.periph_width = DMA_PERIPH_WIDTH_16BIT;				/*!< transfer data size of peripheral */
	dma_init_parameter.periph_inc = DMA_PERIPH_INCREASE_DISABLE;		/*!< peripheral increasing mode */  
	dma_init_parameter.memory0_addr = (uint32_t)ADC0DMABuff0;						/*!< memory 0 base address */
	dma_init_parameter.memory_width = DMA_MEMORY_WIDTH_16BIT;				/*!< transfer data size of memory */
	dma_init_parameter.memory_inc = DMA_MEMORY_INCREASE_ENABLE;			/*!< memory increasing mode */
	dma_init_parameter.memory_burst_width = DMA_MEMORY_BURST_SINGLE;/*!< multi data mode enable */
	dma_init_parameter.periph_burst_width = DMA_PERIPH_BURST_SINGLE;/*!< multi data mode enable */
	dma_init_parameter.critical_value = DMA_FIFO_2_WORD;						/*!< FIFO critical */
	dma_init_parameter.circular_mode = DMA_CIRCULAR_MODE_DISABLE;		/*!< DMA circular mode */
	dma_init_parameter.direction = DMA_PERIPH_TO_MEMORY;						/*!< channel data transfer direction */
	dma_init_parameter.number = _ADC0DMABuffSize;												/*!< channel transfer number */
	dma_init_parameter.priority = DMA_PRIORITY_HIGH;								/*!< channel priority level */
	
	dma_multi_data_mode_init (DMA1, DMA_CH4, &dma_init_parameter);
	dma_channel_subperipheral_select (DMA1, DMA_CH4, DMA_SUBPERI0);
	/* DMA switch buffer mode config */
	dma_switch_buffer_mode_enable (DMA1, DMA_CH4, ENABLE);
	dma_switch_buffer_mode_config (DMA1, DMA_CH4, (uint32_t)ADC0DMABuff1, DMA_MEMORY_0);

	dma_flag_clear (DMA1, DMA_CH4, DMA_FLAG_FEE);
	dma_flag_clear (DMA1, DMA_CH4, DMA_FLAG_SDE);
	dma_flag_clear (DMA1, DMA_CH4, DMA_FLAG_TAE);
	dma_flag_clear (DMA1, DMA_CH4, DMA_FLAG_HTF);
	dma_flag_clear (DMA1, DMA_CH4, DMA_FLAG_FTF);

	dma_interrupt_enable (DMA1, DMA_CH4, DMA_CHXCTL_FTFIE);
	dma_interrupt_enable (DMA1, DMA_CH4, DMA_CHXFCTL_FEEIE);

	/* enable DMA channel */
	dma_channel_enable (DMA1, DMA_CH4);

	/* TIMER configuration */
	timer_initpara.prescaler         = 39U;
	timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
	timer_initpara.counterdirection  = TIMER_COUNTER_UP;
	timer_initpara.period            = 999U;
	timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
	timer_initpara.repetitioncounter = 0U;
//	timer_init (TIMER1, &timer_initpara);
	timer_init (TIMER4, &timer_initpara);
	/* CH1 configuration in PWM mode1 */
	timer_ocintpara.ocpolarity  = TIMER_OC_POLARITY_HIGH;
	timer_ocintpara.outputstate = TIMER_CCX_ENABLE;
//	timer_channel_output_config (TIMER1, TIMER_CH_1, &timer_ocintpara);
	timer_channel_output_config (TIMER4, TIMER_CH_1, &timer_ocintpara);

//	timer_channel_output_pulse_value_config (TIMER1, TIMER_CH_1, 4U);
	timer_channel_output_pulse_value_config (TIMER4, TIMER_CH_1, 4U);
//	timer_channel_output_mode_config (TIMER1, TIMER_CH_1, TIMER_OC_MODE_PWM1);
	timer_channel_output_mode_config (TIMER4, TIMER_CH_1, TIMER_OC_MODE_PWM1);
//	timer_channel_output_shadow_config(TIMER1, TIMER_CH_1, TIMER_OC_SHADOW_DISABLE);	
	timer_channel_output_shadow_config (TIMER4, TIMER_CH_1, TIMER_OC_SHADOW_DISABLE);	
	
	/* enable TIMER */
//	timer_enable (TIMER1);
	timer_enable (TIMER4);
	
	/* enable ADC interface */
	adc_enable (ADC0);
	
	Delay_ms (1);
	
/* ADC calibration and reset calibration */
//	adc_calibration_enable (ADC0);
}
//--------------------------------------------------------------------------//
static void DataProcessMonADC0 (uint16_t * p)
{
	static uint32_t Count = 0;
	int32_t N, S;
	uint16_t d;
	
	for (N = 0; N < _ADC0_NumberChannel; N++)
	{
		for (S = 0; S < _ADC0DMAChannSamplNum; S++)
		{
			d = p [S * _ADC0_NumberChannel + N];
			SumADC0 [N] += d;
			if (d > MaxADC0 [N]) MaxADC0 [N] = d;
			if (d < MinADC0 [N]) MinADC0 [N] = d;
		}
	}
	Count += _ADC0DMAChannSamplNum;
	if (_ADC0NumMeans <= Count)
	{
		for (N = 0; N < _ADC0_NumberChannel; N++)
		{
			SSumADC0 [N] = SumADC0 [N];
			SMaxADC0 [N] = MaxADC0 [N];
			SMinADC0 [N] = MinADC0 [N];
			SumADC0 [N] = 0;
			MaxADC0 [N] = 0;
			MinADC0 [N] = 0xffff;
		}
		SCount = Count;
		Count = 0;
		ADC0ReadyData = true;
	}
}
//--------------------------------------------------------------------------//
void DMA1_Channel4_IRQHandler (void)
{
	uint16_t * pRx;
	
	if (SET == dma_interrupt_flag_get (DMA1, DMA_CH4, DMA_INT_FLAG_FTF))
	{
		dma_interrupt_flag_clear (DMA1, DMA_CH4, DMA_INT_FLAG_FTF);
		if (DMA_MEMORY_0 == dma_using_memory_get (DMA1, DMA_CH4)) pRx = ADC0DMABuff1;
		else pRx = ADC0DMABuff0;
		DataProcessMonADC0 (pRx);
	}
	if (SET == dma_interrupt_flag_get (DMA1, DMA_CH4, DMA_INT_FLAG_FEE))
	{
		dma_interrupt_flag_clear (DMA1, DMA_CH4, DMA_INT_FLAG_FEE);
	}
}
//--------------------------------------------------------------------------//
static float MonADC0ScalVal (TADC0Channel Chan, float val)
{
	val *= (float)_ADC0dVref;
	if (Chan != _ADC0_Temp) val *= CalibADC0CoeffMul [Chan];
	else val = ((float)_TempOfset - val) * (float)(1.0 / _TempSlope) + 25.0f;
	return (val);
}
//--------------------------------------------------------------------------//
static void MinMaxSwap (TADC0Channel Chan, float * Min, float * Max)
{
	float f;
	if (CalibADC0CoeffMul [Chan] < 0.0f)
	{
		f = *Min;
		*Min = *Max;
		*Max = f;
	}
}
//--------------------------------------------------------------------------//
void RoutineMonADC0 (void)
{
	if (true == ADC0ReadyData)
	{

//	gpio_bit_set (GPIOB, GPIO_PIN_2);

		int32_t i;
		float f;
		uint32_t Count = SCount;
		
		for (i = 0; i < _ADC0_NumberChannel; i++)
		{
			ADC0ResultAVG [i] = (float)(SSumADC0 [i]);
			ADC0ResultMax [i] = SMaxADC0 [i];
			ADC0ResultMin [i] = SMinADC0 [i];
		}
		ADC0ReadyData = false;
		f = 1.0f / (float)(Count);
		for (i = 0; i < _ADC0_NumberChannel; i++)
		{
			ADC0ResultAVG [i] *= f;
			ADC0ResultAVG [i] = MonADC0ScalVal ((TADC0Channel)i, ADC0ResultAVG [i]);
			ADC0ResultMax [i] = MonADC0ScalVal ((TADC0Channel)i, ADC0ResultMax [i]);
			ADC0ResultMin [i] = MonADC0ScalVal ((TADC0Channel)i, ADC0ResultMin [i]);
			MinMaxSwap ((TADC0Channel)i, &(ADC0ResultMin [i]), &(ADC0ResultMax [i]));

		}

//	gpio_bit_reset (GPIOB, GPIO_PIN_2);
	
	}
}
//--------------------------------------------------------------------------//
float ResultDataMonADC0AVG (TADC0Channel Chan)
{
	if (/*Chan < 0 || */Chan >= _ADC0_NumberChannel) return (0.0);
	else return (ADC0ResultAVG [Chan]);
}
//--------------------------------------------------------------------------//
float ResultDataMonADC0Max (TADC0Channel Chan)
{
	if (/*Chan < 0 || */Chan >= _ADC0_NumberChannel) return (0.0);
	else return (ADC0ResultMax [Chan]);
}
//--------------------------------------------------------------------------//
float ResultDataMonADC0Min (TADC0Channel Chan)
{
	if (/*Chan < 0 || */Chan >= _ADC0_NumberChannel) return (0.0);
	else return (ADC0ResultMin [Chan]);
}
//--------------------------------------------------------------------------//

