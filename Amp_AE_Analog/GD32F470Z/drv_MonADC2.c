/*=============================================================================
2	  Project: ADCVibro_Digital
3	  Platform: GD32F470
4	  Filename: drv_MonADC2.c
5	  Description:
6	  Version: 0.0
7	  Created: 2022.08.23
8   Last modified: 2024.12.09
9	=============================================================================*/
#include <gd32f4xx.h>
#include <stdbool.h>
#include <string.h> 
//-----------------------------------------------------------------------------
#include "drv_time.h"
#include "drv_MonADC2.h"
//--------------------------------------------------------------------------//
#define _ADC2Vref (3.0)
#define _ADC2NumMeans (1023)
#define _ADC2dVref (_ADC2Vref / 4096.0)
//--------------------------------------------------------------------------//
static const float CalibADC2CoeffMul [_ADC2_NumberChannel] = 
{
		2.8200f, //_ADC2_p3_3Vd = 0
		4.3200f, //_ADC2_p5Va = 1
		4.3200f, //_ADC2_p7V = 2
		19.200f, //_ADC2_p24V = 3
		19.200f, //_ADC2_p27V = 4
		2.2100f, //_ADC2_Vocm = 5
};
//--------------------------------------------------------------------------//
static float ADC2ResultAVG [_ADC2_NumberChannel];
static float ADC2ResultMax [_ADC2_NumberChannel];
static float ADC2ResultMin [_ADC2_NumberChannel];
static uint32_t SSumADC2 [_ADC2_NumberChannel];
static uint16_t SMaxADC2 [_ADC2_NumberChannel];
static uint16_t SMinADC2 [_ADC2_NumberChannel];
static uint32_t SCoun;
static uint32_t SumADC2 [_ADC2_NumberChannel];
static uint16_t MaxADC2 [_ADC2_NumberChannel];
static uint16_t MinADC2 [_ADC2_NumberChannel];
static bool ADC2ReadyData;

#define _ADC2DMAChannSamplNum (16)
#define _ADC2DMABuffSize (_ADC2_NumberChannel * _ADC2DMAChannSamplNum)
static uint16_t ADC2DMABuff0 [_ADC2DMABuffSize];
static uint16_t ADC2DMABuff1 [_ADC2DMABuffSize];
//--------------------------------------------------------------------------//
void InitMonADC2 (void)
{
	timer_oc_parameter_struct timer_ocintpara;
	timer_parameter_struct timer_initpara;
	dma_multi_data_parameter_struct dma_init_parameter;
	int32_t i;
	
	for (i = 0; i < _ADC2_NumberChannel; i++)
	{
		ADC2ResultAVG [i] = ADC2ResultMax [i] = ADC2ResultMin [i] = 0;
		SSumADC2 [i] = SumADC2 [i] = 0;
		SMaxADC2 [i] = MaxADC2 [i] = SMinADC2 [i] = 0;
		MinADC2 [i] = 0xffff;
	}
	ADC2ReadyData = false;

	rcu_periph_clock_enable (RCU_DMA1);
//	rcu_periph_clock_enable (RCU_TIMER1);
	rcu_periph_clock_enable (RCU_TIMER4);
	rcu_periph_clock_enable (RCU_ADC2);
	rcu_periph_clock_enable (RCU_GPIOF);

	/* config the GPIO as analog mode */
	gpio_mode_set (GPIOF, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_5);//_ADC2_p3_3Vd - IN15
	gpio_mode_set (GPIOF, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_3);//_ADC2_p5Va - IN9
	gpio_mode_set (GPIOF, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_9);//_ADC2_p7V - IN7
	gpio_mode_set (GPIOF, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_8);//_ADC2_p24V - IN6
	gpio_mode_set (GPIOF, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_4);//_ADC2_p27V - IN14
	gpio_mode_set (GPIOF, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_10);//_ADC2_Vocm - IN8

	/* reset ADC */
//	adc_deinit ();
	/* configure the ADC clock for all the ADCs */
//	adc_clock_config (ADC_ADCCK_HCLK_DIV20);

	/* ADC synchronization */
	/* configure the ADC sync mode */
//	adc_sync_mode_config (ADC_SYNC_MODE_INDEPENDENT);
	/* configure ADC sync DMA mode selection */
//	adc_sync_dma_config (ADC_SYNC_DMA_DISABLE);

	/* configure ADC resolution */
	adc_resolution_config (ADC2, ADC_RESOLUTION_12B);
	/* configure ADC data alignment */
//	adc_data_alignment_config (ADC2, ADC_DATAALIGN_LEFT);
	adc_data_alignment_config (ADC2, ADC_DATAALIGN_RIGHT);
	/* configure ADC external trigger source */
//	adc_external_trigger_source_config (ADC2, ADC_ROUTINE_CHANNEL, ADC_EXTTRIG_ROUTINE_T1_CH1);
	adc_external_trigger_source_config (ADC2, ADC_ROUTINE_CHANNEL, ADC_EXTTRIG_ROUTINE_T4_CH1);
	/* enable ADC external trigger */
	adc_external_trigger_config (ADC2, ADC_ROUTINE_CHANNEL, EXTERNAL_TRIGGER_RISING);

	/* enable or disable ADC special function */
	adc_special_function_config (ADC2, ADC_SCAN_MODE, ENABLE);
	adc_special_function_config (ADC2, ADC_CONTINUOUS_MODE, DISABLE);
	adc_special_function_config (ADC2, ADC_INSERTED_CHANNEL_AUTO, DISABLE);

	/* configure ADC discontinuous mode */
	adc_discontinuous_mode_config (ADC2, ADC_CHANNEL_DISCON_DISABLE, 0);
	/* configure the length of regular channel group or inserted channel group */
	adc_channel_length_config (ADC2, ADC_ROUTINE_CHANNEL, _ADC2_NumberChannel);
	/* configure ADC regular channel */

	adc_routine_channel_config (ADC2, (uint8_t)_ADC2_p3_3Vd,	ADC_CHANNEL_15,	ADC_SAMPLETIME_28);
	adc_routine_channel_config (ADC2, (uint8_t)_ADC2_p5Va,		ADC_CHANNEL_9,	ADC_SAMPLETIME_28);
	adc_routine_channel_config (ADC2, (uint8_t)_ADC2_p7V,			ADC_CHANNEL_7,	ADC_SAMPLETIME_28);
	adc_routine_channel_config (ADC2, (uint8_t)_ADC2_p24V,		ADC_CHANNEL_6,	ADC_SAMPLETIME_28);
	adc_routine_channel_config (ADC2, (uint8_t)_ADC2_p27V,		ADC_CHANNEL_14,	ADC_SAMPLETIME_28);
	adc_routine_channel_config (ADC2, (uint8_t)_ADC2_Vocm,		ADC_CHANNEL_8,	ADC_SAMPLETIME_28);

	/* disable ADC oversample mode */
	adc_oversample_mode_disable (ADC2);

	/* when DMA=1, the DMA engine issues a request at end of each regular conversion */
	adc_dma_request_after_last_enable (ADC2);
	/* enable DMA request */
	adc_dma_mode_enable (ADC2);

	memset (ADC2DMABuff0, 0, sizeof (ADC2DMABuff0));
	memset (ADC2DMABuff1, 0, sizeof (ADC2DMABuff1));
	
	dma_multi_data_para_struct_init (&dma_init_parameter);
	/* deinitialize DMA1_CH1 */
	dma_deinit (DMA1, DMA_CH1);

	nvic_irq_enable (DMA1_Channel1_IRQn, 3, 3);

	dma_init_parameter.periph_addr = (int32_t)&ADC_RDATA (ADC2);		/*!< peripheral base address */
	dma_init_parameter.periph_width = DMA_PERIPH_WIDTH_16BIT;				/*!< transfer data size of peripheral */
	dma_init_parameter.periph_inc = DMA_PERIPH_INCREASE_DISABLE;		/*!< peripheral increasing mode */  
	dma_init_parameter.memory0_addr = (uint32_t)ADC2DMABuff0;						/*!< memory 0 base address */
	dma_init_parameter.memory_width = DMA_MEMORY_WIDTH_16BIT;				/*!< transfer data size of memory */
	dma_init_parameter.memory_inc = DMA_MEMORY_INCREASE_ENABLE;			/*!< memory increasing mode */
	dma_init_parameter.memory_burst_width = DMA_MEMORY_BURST_SINGLE;/*!< multi data mode enable */
	dma_init_parameter.periph_burst_width = DMA_PERIPH_BURST_SINGLE;/*!< multi data mode enable */
	dma_init_parameter.critical_value = DMA_FIFO_2_WORD;						/*!< FIFO critical */
	dma_init_parameter.circular_mode = DMA_CIRCULAR_MODE_DISABLE;		/*!< DMA circular mode */
	dma_init_parameter.direction = DMA_PERIPH_TO_MEMORY;						/*!< channel data transfer direction */
	dma_init_parameter.number = _ADC2DMABuffSize;												/*!< channel transfer number */
	dma_init_parameter.priority = DMA_PRIORITY_HIGH;								/*!< channel priority level */
	
	dma_multi_data_mode_init (DMA1, DMA_CH1, &dma_init_parameter);
	dma_channel_subperipheral_select (DMA1, DMA_CH1, DMA_SUBPERI2);
	/* DMA switch buffer mode config */
	dma_switch_buffer_mode_enable (DMA1, DMA_CH1, ENABLE);
	dma_switch_buffer_mode_config (DMA1, DMA_CH1, (uint32_t)ADC2DMABuff1, DMA_MEMORY_0);

	dma_flag_clear (DMA1, DMA_CH1, DMA_FLAG_FEE);
	dma_flag_clear (DMA1, DMA_CH1, DMA_FLAG_SDE);
	dma_flag_clear (DMA1, DMA_CH1, DMA_FLAG_TAE);
	dma_flag_clear (DMA1, DMA_CH1, DMA_FLAG_HTF);
	dma_flag_clear (DMA1, DMA_CH1, DMA_FLAG_FTF);

	dma_interrupt_enable (DMA1, DMA_CH1, DMA_CHXCTL_FTFIE);
	dma_interrupt_enable (DMA1, DMA_CH1, DMA_CHXFCTL_FEEIE);

	/* enable DMA channel */
	dma_channel_enable (DMA1, DMA_CH1);

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
//	timer_channel_output_shadow_config (TIMER1, TIMER_CH_1, TIMER_OC_SHADOW_DISABLE);	
	timer_channel_output_shadow_config (TIMER4, TIMER_CH_1, TIMER_OC_SHADOW_DISABLE);	
	
	/* enable TIMER */
//	timer_enable (TIMER1);
	timer_enable (TIMER4);
	
	/* enable ADC interface */
	adc_enable (ADC2);
	
	Delay_ms (1);
	
/* ADC calibration and reset calibration */
//	adc_calibration_enable (ADC2);
}
//--------------------------------------------------------------------------//
static void DataProcessMonADC2 (uint16_t * p)
{
	static uint32_t Count = 0;
	int32_t N, S;
	uint16_t d;
	
	for (N = 0; N < _ADC2_NumberChannel; N++)
	{
		for (S = 0; S < _ADC2DMAChannSamplNum; S++)
		{
			d = p [S * _ADC2_NumberChannel + N];
			SumADC2 [N] += d;
			if (d > MaxADC2 [N]) MaxADC2 [N] = d;
			if (d < MinADC2 [N]) MinADC2 [N] = d;
		}
	}
	Count += _ADC2DMAChannSamplNum;
	if (_ADC2NumMeans <= Count)
	{
		for (N = 0; N < _ADC2_NumberChannel; N++)
		{
			SSumADC2 [N] = SumADC2 [N];
			SMaxADC2 [N] = MaxADC2 [N];
			SMinADC2 [N] = MinADC2 [N];
			SumADC2 [N] = 0;
			MaxADC2 [N] = 0;
			MinADC2 [N] = 0xffff;
		}
		SCoun = Count;
		Count = 0;
		ADC2ReadyData = true;
	}
}
//--------------------------------------------------------------------------//
void DMA1_Channel1_IRQHandler (void)
{
	
	uint16_t * pRx;
//	int32_t i;
	
	if (SET == dma_interrupt_flag_get (DMA1, DMA_CH1, DMA_INT_FLAG_FTF))
	{
		dma_interrupt_flag_clear (DMA1, DMA_CH1, DMA_INT_FLAG_FTF);
		if (DMA_MEMORY_0 == dma_using_memory_get (DMA1, DMA_CH1)) pRx = ADC2DMABuff1;
		else pRx = ADC2DMABuff0;
		DataProcessMonADC2 (pRx);
	}
	if (SET == dma_interrupt_flag_get (DMA1, DMA_CH1, DMA_INT_FLAG_FEE))
	{
		dma_interrupt_flag_clear (DMA1, DMA_CH1, DMA_INT_FLAG_FEE);
	}
}
//--------------------------------------------------------------------------//
static float MonADC2ScalVal (TADC2Channel Chan, float val)
{
	val *= (float)_ADC2dVref;
	val *= CalibADC2CoeffMul [Chan];
	return (val);
}
//--------------------------------------------------------------------------//
static void MinMaxSwap (TADC2Channel Chan, float * Min, float * Max)
{
	float f;
	if (CalibADC2CoeffMul [Chan] < 0.0f)
	{
		f = *Min;
		*Min = *Max;
		*Max = f;
	}
}
//--------------------------------------------------------------------------//
void RoutineMonADC2 (void)
{
	if (true == ADC2ReadyData)
	{

//	gpio_bit_set (GPIOB, GPIO_PIN_2);

		float f;
		int32_t i;
		uint32_t Count = SCoun;;

		for (i = 0; i < _ADC2_NumberChannel; i++)
		{
			ADC2ResultAVG [i] = (float)(SSumADC2 [i]);
			ADC2ResultMax [i] = SMaxADC2 [i];
			ADC2ResultMin [i] = SMinADC2 [i];
		}
		ADC2ReadyData = false;
		f = 1.0f / (float)(Count);
		for (i = 0; i < _ADC2_NumberChannel; i++)
		{
			ADC2ResultAVG [i] *= f;
			ADC2ResultAVG [i] = MonADC2ScalVal ((TADC2Channel)i, ADC2ResultAVG [i]);
			ADC2ResultMax [i] = MonADC2ScalVal ((TADC2Channel)i, ADC2ResultMax [i]);
			ADC2ResultMin [i] = MonADC2ScalVal ((TADC2Channel)i, ADC2ResultMin [i]);
			MinMaxSwap ((TADC2Channel)i, &(ADC2ResultMin [i]), &(ADC2ResultMax [i]));

		}

//	gpio_bit_reset (GPIOB, GPIO_PIN_2);
	
	}
}
//--------------------------------------------------------------------------//
float ResultDataMonADC2AVG (TADC2Channel Chan)
{
	if (/*Chan < 0 || */Chan >= _ADC2_NumberChannel) return (0.0);
	else return (ADC2ResultAVG [Chan]);
}
//--------------------------------------------------------------------------//
float ResultDataMonADC2Max (TADC2Channel Chan)
{
	if (/*Chan < 0 || */Chan >= _ADC2_NumberChannel) return (0.0);
	else return (ADC2ResultMax [Chan]);
}
//--------------------------------------------------------------------------//
float ResultDataMonADC2Min (TADC2Channel Chan)
{
	if (/*Chan < 0 || */Chan >= _ADC2_NumberChannel) return (0.0);
	else return (ADC2ResultMin [Chan]);
}
//--------------------------------------------------------------------------//

