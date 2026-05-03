/*=============================================================================
2	  Project: ADCVibro_Digital
3	  Platform: GD32F470
4	  Filename: drv_MonADC1.c
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
#include "drv_TestPinKT.h"
#include "drv_MonADC1.h"
//--------------------------------------------------------------------------//
#define _ADC1Vref (3.0)
#define _ADC1NumMeans (1023)
#define _ADC1dVref (_ADC1Vref / 4096.0)
//--------------------------------------------------------------------------//
static const float CalibADC1CoeffMul [_ADC1_NumberChannel] = 
{
		10.076f, //_ADC1_Monp1 = 0
		10.076f, //_ADC1_Monn1 = 1
		10.076f, //_ADC1_Monp2 = 2
		10.076f, //_ADC1_Monn2 = 3
		10.076f, //_ADC1_Monp3 = 4
		10.076f, //_ADC1_Monn3 = 5
		10.076f, //_ADC1_Monp4 = 6
		10.076f, //_ADC1_Monn4 = 7
		10.076f, //_ADC1_Monp5 = 8
		10.076f, //_ADC1_Monn5 = 9
		10.076f, //_ADC1_Monp6 = 10
		10.076f, //_ADC1_Monn6 = 11
		10.076f, //_ADC1_Monp7 = 12
		10.076f, //_ADC1_Monn7 = 13
		10.076f, //_ADC1_Monp8 = 14
		10.076f, //_ADC1_Monn8 = 15
};
//--------------------------------------------------------------------------//
static float ADC1ResultAVG [_ADC1_NumberChannel];
static float ADC1ResultMax [_ADC1_NumberChannel];
static float ADC1ResultMin [_ADC1_NumberChannel];
static uint32_t SSumADC1 [_ADC1_NumberChannel];
static uint16_t SMaxADC1 [_ADC1_NumberChannel];
static uint16_t SMinADC1 [_ADC1_NumberChannel];
static uint32_t SCount;
static uint32_t SumADC1 [_ADC1_NumberChannel];
static uint16_t MaxADC1 [_ADC1_NumberChannel];
static uint16_t MinADC1 [_ADC1_NumberChannel];
static bool ADC1ReadyData;

#define _ADC1DMAChannSamplNum (16)
#define _ADC1DMABuffSize (_ADC1_NumberChannel * _ADC1DMAChannSamplNum)
static uint16_t ADC1DMABuff0 [_ADC1DMABuffSize];
static uint16_t ADC1DMABuff1 [_ADC1DMABuffSize];
//--------------------------------------------------------------------------//
void InitMonADC1 (void)
{
	timer_oc_parameter_struct timer_ocintpara;
	timer_parameter_struct timer_initpara;
	dma_multi_data_parameter_struct dma_init_parameter;
	int32_t i;
	
	for (i = 0; i < _ADC1_NumberChannel; i++)
	{
		ADC1ResultAVG [i] = ADC1ResultMax [i] = ADC1ResultMin [i] = 0;
		SSumADC1 [i] = SumADC1 [i] = 0;
		SMaxADC1 [i] = MaxADC1 [i] = SMinADC1 [i] = 0;
		MinADC1 [i] = 0xffff;
	}
	ADC1ReadyData = false;

	rcu_periph_clock_enable (RCU_DMA1);
//	rcu_periph_clock_enable (RCU_TIMER1);
	rcu_periph_clock_enable (RCU_TIMER4);
	rcu_periph_clock_enable (RCU_ADC1);
	rcu_periph_clock_enable (RCU_GPIOA);
	rcu_periph_clock_enable (RCU_GPIOB);
	rcu_periph_clock_enable (RCU_GPIOC);

	/* config the GPIO as analog mode */
	gpio_mode_set (GPIOB, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_1);//_ADC1_Monp1 - IN9
	gpio_mode_set (GPIOB, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_0);//_ADC1_Monn1 - IN8
	gpio_mode_set (GPIOC, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_5);//_ADC1_Monp2 - IN15
	gpio_mode_set (GPIOC, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_4);//_ADC1_Monn2 - IN14
	gpio_mode_set (GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_7);//_ADC1_Monp3 - IN7
	gpio_mode_set (GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_6);//_ADC1_Monn3 - IN6
	gpio_mode_set (GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_5);//_ADC1_Monp4 - IN5
	gpio_mode_set (GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_4);//_ADC1_Monn4 - IN4
	gpio_mode_set (GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_3);//_ADC1_Monp5 - IN3
	gpio_mode_set (GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_0);//_ADC1_Monn5 - IN0
	gpio_mode_set (GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_1);//_ADC1_Monp6 - IN1
	gpio_mode_set (GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_2);//_ADC1_Monn6 - IN2
	gpio_mode_set (GPIOC, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_3);//_ADC1_Monp7 - IN13
	gpio_mode_set (GPIOC, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_2);//_ADC1_Monn7 - IN12
	gpio_mode_set (GPIOC, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_1);//_ADC1_Monp8 - IN11
	gpio_mode_set (GPIOC, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_0);//_ADC1_Monn8 - IN10

	/* reset ADC */
//	adc_deinit ();
	/* configure the ADC clock for all the ADCs */
//	adc_clock_config (ADC_ADCCK_HCLK_DIV20);
//	adc_clock_config (ADC_ADCCK_HCLK_DIV5);

	/* ADC synchronization */
	/* configure the ADC sync mode */
//	adc_sync_mode_config (ADC_SYNC_MODE_INDEPENDENT);
	/* configure ADC sync DMA mode selection */
//	adc_sync_dma_config (ADC_SYNC_DMA_DISABLE);

	/* configure ADC resolution */
	adc_resolution_config (ADC1, ADC_RESOLUTION_12B);
	/* configure ADC data alignment */
//	adc_data_alignment_config (ADC2, ADC_DATAALIGN_LEFT);
	adc_data_alignment_config (ADC1, ADC_DATAALIGN_RIGHT);
	/* configure ADC external trigger source */
//	adc_external_trigger_source_config (ADC1, ADC_ROUTINE_CHANNEL, ADC_EXTTRIG_ROUTINE_T1_CH1);
	adc_external_trigger_source_config (ADC1, ADC_ROUTINE_CHANNEL, ADC_EXTTRIG_ROUTINE_T4_CH1);
	/* enable ADC external trigger */
	adc_external_trigger_config (ADC1, ADC_ROUTINE_CHANNEL, EXTERNAL_TRIGGER_RISING);

	/* enable or disable ADC special function */
	adc_special_function_config (ADC1, ADC_SCAN_MODE, ENABLE);
	adc_special_function_config (ADC1, ADC_CONTINUOUS_MODE, DISABLE);
	adc_special_function_config (ADC1, ADC_INSERTED_CHANNEL_AUTO, DISABLE);

	/* configure ADC discontinuous mode */
	adc_discontinuous_mode_config (ADC1, ADC_CHANNEL_DISCON_DISABLE, 0);
	/* configure the length of regular channel group or inserted channel group */
	adc_channel_length_config (ADC1, ADC_ROUTINE_CHANNEL, _ADC1_NumberChannel);
	/* configure ADC regular channel */
	adc_routine_channel_config (ADC1, (uint8_t)_ADC1_Monp1,		ADC_CHANNEL_9,	ADC_SAMPLETIME_28);
	adc_routine_channel_config (ADC1, (uint8_t)_ADC1_Monn1,		ADC_CHANNEL_8,	ADC_SAMPLETIME_28);
	adc_routine_channel_config (ADC1, (uint8_t)_ADC1_Monp2,		ADC_CHANNEL_15,	ADC_SAMPLETIME_28);
	adc_routine_channel_config (ADC1, (uint8_t)_ADC1_Monn2,		ADC_CHANNEL_14,	ADC_SAMPLETIME_28);
	adc_routine_channel_config (ADC1, (uint8_t)_ADC1_Monp3,		ADC_CHANNEL_7,	ADC_SAMPLETIME_28);
	adc_routine_channel_config (ADC1, (uint8_t)_ADC1_Monn3,		ADC_CHANNEL_6,	ADC_SAMPLETIME_28);
	adc_routine_channel_config (ADC1, (uint8_t)_ADC1_Monp4,		ADC_CHANNEL_5,	ADC_SAMPLETIME_28);
	adc_routine_channel_config (ADC1, (uint8_t)_ADC1_Monn4,		ADC_CHANNEL_4,	ADC_SAMPLETIME_28);
	adc_routine_channel_config (ADC1, (uint8_t)_ADC1_Monp5,		ADC_CHANNEL_3,	ADC_SAMPLETIME_28);
	adc_routine_channel_config (ADC1, (uint8_t)_ADC1_Monn5,		ADC_CHANNEL_0,	ADC_SAMPLETIME_28);
	adc_routine_channel_config (ADC1, (uint8_t)_ADC1_Monp6,		ADC_CHANNEL_1,	ADC_SAMPLETIME_28);
	adc_routine_channel_config (ADC1, (uint8_t)_ADC1_Monn6,		ADC_CHANNEL_2,	ADC_SAMPLETIME_28);
	adc_routine_channel_config (ADC1, (uint8_t)_ADC1_Monp7,		ADC_CHANNEL_13,	ADC_SAMPLETIME_28);
	adc_routine_channel_config (ADC1, (uint8_t)_ADC1_Monn7,		ADC_CHANNEL_12,	ADC_SAMPLETIME_28);
	adc_routine_channel_config (ADC1, (uint8_t)_ADC1_Monp8,		ADC_CHANNEL_11,	ADC_SAMPLETIME_28);
	adc_routine_channel_config (ADC1, (uint8_t)_ADC1_Monn8,		ADC_CHANNEL_10,	ADC_SAMPLETIME_28);

	/* disable ADC oversample mode */
	adc_oversample_mode_disable (ADC1);

	/* when DMA=1, the DMA engine issues a request at end of each regular conversion */
	adc_dma_request_after_last_enable (ADC1);
	/* enable DMA request */
	adc_dma_mode_enable (ADC1);

	memset (ADC1DMABuff0, 0, sizeof (ADC1DMABuff0));
	memset (ADC1DMABuff1, 0, sizeof (ADC1DMABuff1));
	
	dma_multi_data_para_struct_init (&dma_init_parameter);
	/* deinitialize DMA1_CH0 */
	dma_deinit (DMA1, DMA_CH2);

	nvic_irq_enable (DMA1_Channel2_IRQn, 3, 3);

	dma_init_parameter.periph_addr = (int32_t)&ADC_RDATA (ADC1);		/*!< peripheral base address */
	dma_init_parameter.periph_width = DMA_PERIPH_WIDTH_16BIT;				/*!< transfer data size of peripheral */
	dma_init_parameter.periph_inc = DMA_PERIPH_INCREASE_DISABLE;		/*!< peripheral increasing mode */  
	dma_init_parameter.memory0_addr = (uint32_t)ADC1DMABuff0;						/*!< memory 0 base address */
	dma_init_parameter.memory_width = DMA_MEMORY_WIDTH_16BIT;				/*!< transfer data size of memory */
	dma_init_parameter.memory_inc = DMA_MEMORY_INCREASE_ENABLE;			/*!< memory increasing mode */
	dma_init_parameter.memory_burst_width = DMA_MEMORY_BURST_SINGLE;/*!< multi data mode enable */
	dma_init_parameter.periph_burst_width = DMA_PERIPH_BURST_SINGLE;/*!< multi data mode enable */
	dma_init_parameter.critical_value = DMA_FIFO_2_WORD;						/*!< FIFO critical */
	dma_init_parameter.circular_mode = DMA_CIRCULAR_MODE_DISABLE;		/*!< DMA circular mode */
	dma_init_parameter.direction = DMA_PERIPH_TO_MEMORY;						/*!< channel data transfer direction */
	dma_init_parameter.number = _ADC1DMABuffSize;												/*!< channel transfer number */
	dma_init_parameter.priority = DMA_PRIORITY_HIGH;								/*!< channel priority level */
	
	dma_multi_data_mode_init (DMA1, DMA_CH2, &dma_init_parameter);
	dma_channel_subperipheral_select (DMA1, DMA_CH2, DMA_SUBPERI1);
	/* DMA switch buffer mode config */
	dma_switch_buffer_mode_enable (DMA1, DMA_CH2, ENABLE);
	dma_switch_buffer_mode_config (DMA1, DMA_CH2, (uint32_t)ADC1DMABuff1, DMA_MEMORY_0);

	dma_flag_clear (DMA1, DMA_CH2, DMA_FLAG_FEE);
	dma_flag_clear (DMA1, DMA_CH2, DMA_FLAG_SDE);
	dma_flag_clear (DMA1, DMA_CH2, DMA_FLAG_TAE);
	dma_flag_clear (DMA1, DMA_CH2, DMA_FLAG_HTF);
	dma_flag_clear (DMA1, DMA_CH2, DMA_FLAG_FTF);

	dma_interrupt_enable (DMA1, DMA_CH2, DMA_CHXCTL_FTFIE);
	dma_interrupt_enable (DMA1, DMA_CH2, DMA_CHXFCTL_FEEIE);

	/* enable DMA channel */
	dma_channel_enable (DMA1, DMA_CH2);

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
	adc_enable (ADC1);
	
	Delay_ms (1);
	
/* ADC calibration and reset calibration */
//	adc_calibration_enable (ADC1);
}
//--------------------------------------------------------------------------//
static void DataProcessMonADC1 (uint16_t * p)
{
	static uint32_t Count = 0;
	int32_t N, S;
	uint16_t d;
	
	for (N = 0; N < _ADC1_NumberChannel; N++)
	{
		for (S = 0; S < _ADC1DMAChannSamplNum; S++)
		{
			d = p [S * _ADC1_NumberChannel + N];
			SumADC1 [N] += d;
			if (d > MaxADC1 [N]) MaxADC1 [N] = d;
			if (d < MinADC1 [N]) MinADC1 [N] = d;
		}
	}
	Count += _ADC1DMAChannSamplNum;
	if (_ADC1NumMeans <= Count)
	{
		for (N = 0; N < _ADC1_NumberChannel; N++)
		{
			SSumADC1 [N] = SumADC1 [N];
			SMaxADC1 [N] = MaxADC1 [N];
			SMinADC1 [N] = MinADC1 [N];
			SumADC1 [N] = 0;
			MaxADC1 [N] = 0;
			MinADC1 [N] = 0xffff;
		}
		SCount = Count;
		Count = 0;
		ADC1ReadyData = true;
	}
}
//--------------------------------------------------------------------------//
void DMA1_Channel2_IRQHandler (void)
{
	uint16_t * pRx;
	SetKT_PIN101 ();
	
	if (SET == dma_interrupt_flag_get (DMA1, DMA_CH2, DMA_INT_FLAG_FTF))
	{
		dma_interrupt_flag_clear (DMA1, DMA_CH2, DMA_INT_FLAG_FTF);
		if (DMA_MEMORY_0 == dma_using_memory_get (DMA1, DMA_CH2)) pRx = ADC1DMABuff1;
		else pRx = ADC1DMABuff0;
		DataProcessMonADC1 (pRx);
	}
	if (SET == dma_interrupt_flag_get (DMA1, DMA_CH2, DMA_INT_FLAG_FEE))
	{
		dma_interrupt_flag_clear (DMA1, DMA_CH2, DMA_INT_FLAG_FEE);
	}
	ResetKT_PIN101 ();
}
//--------------------------------------------------------------------------//
static float MonADC1ScalVal (TADC1Channel Chan, float val)
{
	val *= (float)_ADC1dVref;
	val *= CalibADC1CoeffMul [Chan];
	return (val);
}
//--------------------------------------------------------------------------//
static void MinMaxSwap (TADC1Channel Chan, float * Min, float * Max)
{
	float f;
	if (CalibADC1CoeffMul [Chan] < 0.0f)
	{
		f = *Min;
		*Min = *Max;
		*Max = f;
	}
}
//--------------------------------------------------------------------------//
void RoutineMonADC1 (void)
{
	if (true == ADC1ReadyData)
	{

//	gpio_bit_set (GPIOB, GPIO_PIN_2);

		int32_t i;
		float f;
		uint32_t Count = SCount;
		
		for (i = 0; i < _ADC1_NumberChannel; i++)
		{
			ADC1ResultAVG [i] = (float)(SSumADC1 [i]);
			ADC1ResultMax [i] = SMaxADC1 [i];
			ADC1ResultMin [i] = SMinADC1 [i];
		}
		ADC1ReadyData = false;
		f = 1.0f / (float)(Count);
		for (i = 0; i < _ADC1_NumberChannel; i++)
		{
			ADC1ResultAVG [i] *= f;
			ADC1ResultAVG [i] = MonADC1ScalVal ((TADC1Channel)i, ADC1ResultAVG [i]);
			ADC1ResultMax [i] = MonADC1ScalVal ((TADC1Channel)i, ADC1ResultMax [i]);
			ADC1ResultMin [i] = MonADC1ScalVal ((TADC1Channel)i, ADC1ResultMin [i]);
			MinMaxSwap ((TADC1Channel)i, &(ADC1ResultMin [i]), &(ADC1ResultMax [i]));

		}

//	gpio_bit_reset (GPIOB, GPIO_PIN_2);
	
	}
}
//--------------------------------------------------------------------------//
float ResultDataMonADC1AVG (TADC1Channel Chan)
{
	if (/*Chan < 0 || */Chan >= _ADC1_NumberChannel) return (0.0);
	else return (ADC1ResultAVG [Chan]);
}
//--------------------------------------------------------------------------//
float ResultDataMonADC1Max (TADC1Channel Chan)
{
	if (/*Chan < 0 || */Chan >= _ADC1_NumberChannel) return (0.0);
	else return (ADC1ResultMax [Chan]);
}
//--------------------------------------------------------------------------//
float ResultDataMonADC1Min (TADC1Channel Chan)
{
	if (/*Chan < 0 || */Chan >= _ADC1_NumberChannel) return (0.0);
	else return (ADC1ResultMin [Chan]);
}
//--------------------------------------------------------------------------//

