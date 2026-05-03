
//--------------------------------------------------------------------------//
#include <gd32f4xx.h>
#include <stdbool.h>
//--------------------------------------------------------------------------//
#include "drv_SPI.h"
//--------------------------------------------------------------------------//
#define _PORTRCU_SPI_CS_EE1 RCU_GPIOE
#define _PORT_SPI_CS_EE1 GPIOE
#define _PIN_SPI_CS_EE1 GPIO_PIN_11
//--------------------------------------------------------------------------//
void spi_configure_cs (TSPICS cs_num)
{
	switch (cs_num)
	{
		case _SPI_CS_EE1://PD4
			rcu_periph_clock_enable (_PORTRCU_SPI_CS_EE1);
			gpio_mode_set (_PORT_SPI_CS_EE1, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, _PIN_SPI_CS_EE1);
			gpio_output_options_set (_PORT_SPI_CS_EE1, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, _PIN_SPI_CS_EE1);
			gpio_bit_set (_PORT_SPI_CS_EE1, _PIN_SPI_CS_EE1);
			break;
	}
}
//--------------------------------------------------------------------------//
static void Init_SPI (int32_t SPITNum, int32_t freq, uint16_t spiMode)
{
	spi_parameter_struct spi_init_struct;
	uint32_t pclk_freq;

	if (SPITNum != SPI4) return;
	
	rcu_periph_clock_enable (RCU_GPIOE);
	rcu_periph_clock_enable (RCU_SPI4);

	/* connect port to SPI4_SCK */
	gpio_af_set (GPIOE, GPIO_AF_6, GPIO_PIN_12);
	/* connect port to SPI4_MOSI */
	gpio_af_set (GPIOE, GPIO_AF_6, GPIO_PIN_14);
	/* connect port to SPI4_MISO */
	gpio_af_set (GPIOE, GPIO_AF_6, GPIO_PIN_13);
	
	/* configure SPI4_SCK as alternate function push-pull */
	gpio_mode_set (GPIOE, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_12);
	gpio_output_options_set (GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO_PIN_12);
	/* configure SPI4_MOSI Rx as alternate function push-pull */
	gpio_mode_set (GPIOE, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_14);
	gpio_output_options_set (GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO_PIN_14);
	/* configure SPI4_MISO as alternate function push-cown */
	gpio_mode_set (GPIOE, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_13);
	gpio_output_options_set (GPIOE, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO_PIN_13);



	spi_i2s_deinit (SPI4);
	spi_struct_para_init (&spi_init_struct);
	/* configure SPI4 parameter */
	spi_init_struct.device_mode          = SPI_MASTER;
	spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
	spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
	spi_init_struct.nss                  = SPI_NSS_SOFT;
	spi_init_struct.endian               = SPI_ENDIAN_MSB;
	switch (spiMode)
	{
		case _SPI_MODE_0: spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE; break;
		case _SPI_MODE_1: spi_init_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_1EDGE; break;
		case _SPI_MODE_2: spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_2EDGE; break;
		case _SPI_MODE_3: spi_init_struct.clock_polarity_phase = SPI_CK_PL_HIGH_PH_2EDGE; break;
	}
	pclk_freq = rcu_clock_freq_get (CK_APB2);
	pclk_freq /= freq;
	if (pclk_freq > 128) spi_init_struct.prescale = SPI_PSC_256;
	else if (pclk_freq > 64) spi_init_struct.prescale = SPI_PSC_128;
	else if (pclk_freq > 32) spi_init_struct.prescale = SPI_PSC_64;
	else if (pclk_freq > 16) spi_init_struct.prescale = SPI_PSC_32;
	else if (pclk_freq > 8) spi_init_struct.prescale = SPI_PSC_16;
	else if (pclk_freq > 4) spi_init_struct.prescale = SPI_PSC_8;
	else if (pclk_freq > 2) spi_init_struct.prescale = SPI_PSC_4;
	else spi_init_struct.prescale = SPI_PSC_2;
	
	spi_init (SPI4, &spi_init_struct);
	spi_nss_output_disable (SPI4);
	spi_enable (SPI4);
}
//--------------------------------------------------------------------------//
void spi_prepare_bus (TSPINUM bus_num, int32_t freq, uint16_t spiMode)
{
  switch (bus_num) {
    case _SPI4:
			Init_SPI (SPI4, freq, spiMode);
      break;
    default: // error
      return;
  }
}
//--------------------------------------------------------------------------//
void spi_enable_cs (TSPICS cs_num)
{
	switch (cs_num)
	{
		case _SPI_CS_EE1:
			gpio_bit_reset (_PORT_SPI_CS_EE1, _PIN_SPI_CS_EE1);
			break;
		default:
			break;
	}
}
//--------------------------------------------------------------------------//
void spi_disable_cs (TSPICS cs_num)
{
	switch (cs_num)
	{
		case _SPI_CS_EE1:
			gpio_bit_set (_PORT_SPI_CS_EE1, _PIN_SPI_CS_EE1);
			break;
		default:
			break;
	}
}
//--------------------------------------------------------------------------//
static uint8_t USART_exchange8 (int32_t USARTnum, uint8_t data)
{
	while (RESET == usart_flag_get (USARTnum, USART_FLAG_TBE));
	usart_data_transmit (USARTnum, data);
	while (RESET == usart_flag_get (USARTnum, USART_FLAG_RBNE));
	return (usart_data_receive (USARTnum));
}
//--------------------------------------------------------------------------//
static uint8_t SPI_exchange8 (int32_t SPITnum, uint8_t data)
{
	while (RESET == spi_i2s_flag_get (SPITnum, SPI_FLAG_TBE));
	spi_i2s_data_transmit (SPITnum, data);
	while (SET == spi_i2s_flag_get (SPITnum, SPI_FLAG_TRANS));
	return (spi_i2s_data_receive (SPITnum));
}
//--------------------------------------------------------------------------//
uint8_t spi_exchange8 (TSPINUM bus_num, uint8_t data)
{
  uint8_t r;

  switch (bus_num) {
    case _SPI4:
			return (SPI_exchange8 (SPI4, data));
    default: // error
      return 0;
  }
}
//--------------------------------------------------------------------------//
uint16_t spi_exchange16 (TSPINUM bus_num, uint16_t data)
{
  uint16_t r;

  r = spi_exchange8(bus_num, data >> 8);
  r <<= 8;
  r |= spi_exchange8(bus_num, data & 0xFF);

  return r;
}
//--------------------------------------------------------------------------//
uint32_t spi_exchange24 (TSPINUM bus_num, uint32_t data)
{
  uint32_t r;

  r = spi_exchange8(bus_num, (data >> 16) &0xFF);
  r <<= 8;
  r |= spi_exchange8(bus_num, (data >> 8) &0xFF);
  r <<= 8;
  r |= spi_exchange8(bus_num, data & 0xFF);

  return r;
}
//--------------------------------------------------------------------------//
uint32_t spi_exchange32 (TSPINUM bus_num, uint32_t data)
{
  uint32_t r;

  r = spi_exchange8(bus_num, (data >> 24) &0xFF);
  r <<= 8;
  r |= spi_exchange8(bus_num, (data >> 16) &0xFF);
  r <<= 8;
  r |= spi_exchange8(bus_num, (data >> 8) &0xFF);
  r <<= 8;
  r |= spi_exchange8(bus_num, data & 0xFF);

  return r;
}
//--------------------------------------------------------------------------//
uint64_t spi_exchange64 (TSPINUM bus_num, uint64_t data)
{
  uint64_t r;

  r  = spi_exchange8 (bus_num, (data >> 56) & 0xFF);
  r <<= 8;
  r |= spi_exchange8 (bus_num, (data >> 48) & 0xFF);
  r <<= 8;
  r |= spi_exchange8 (bus_num, (data >> 40) & 0xFF);
  r <<= 8;
  r |= spi_exchange8 (bus_num, (data >> 32) & 0xFF);
  r <<= 8;

  r |= spi_exchange8 (bus_num, (data >> 24) & 0xFF);
  r <<= 8;
  r |= spi_exchange8 (bus_num, (data >> 16) & 0xFF);
  r <<= 8;
  r |= spi_exchange8 (bus_num, (data >> 8) & 0xFF);
  r <<= 8;
  r |= spi_exchange8 (bus_num, data & 0xFF);

  return r;
}
//--------------------------------------------------------------------------//
void spi_bufTx (TSPINUM bus_num, const void *tx, int len)
{
  uint16_t dummy;
  int i;

  switch (bus_num) {
    case _SPI4:
      for (i = 0; i < len; i++)
				SPI_exchange8 (SPI4, ((const uint8_t*)tx)[i]);
      break;
    default: // error
      return;
  }
}
//--------------------------------------------------------------------------//
void spi_bufRx (TSPINUM bus_num, void *rx, int len)
{
  int i;

  switch (bus_num) {
    case _SPI4:
      for (i = 0; i < len; i++)
				((uint8_t*)rx)[i] = SPI_exchange8 (SPI4, 0xFF);
      break;
    default: // error
      return;
  }
}
//--------------------------------------------------------------------------//
