/*=============================================================================
  Project:
  Platform: GD32F470Z
  Filename: drv_1W.c
  Description:
  Programmer:
  Version: 0.0
  Created: 2022.05.12
  Last modified: 2024.12.09
=============================================================================*/
#include <gd32f4xx.h>
#include <stdint.h>
#include <stdbool.h>
//------------------------------------------------------------------------------
#include "drv_time.h"
#include "drv_1W.h"
//--------------------------------------------------------------------------//
#define _Device (USART2)
#define ARRAYNUM(arr_name)	(uint32_t)(sizeof(arr_name)/sizeof(*(arr_name)))
#define USART_DATA_ADDRESS	((uint32_t)&USART_DATA(USART2))
#define _SizeDMABuff (12*8)
//--------------------------------------------------------------------------//
static void OneWire_SetSpeed_ (uint32_t Speed)
{
	usart_deinit (_Device);
	usart_baudrate_set (_Device, Speed);
	usart_halfduplex_enable (_Device);
	usart_transmit_config (_Device, USART_TRANSMIT_ENABLE);
	usart_receive_config (_Device, USART_RECEIVE_ENABLE);
	usart_enable (_Device);
}
//--------------------------------------------------------------------------//
//--------------------------------------------------------------------------//
static uint8_t tx_buffer [_SizeDMABuff];
static uint8_t rx_buffer [_SizeDMABuff];
//--------------------------------------------------------------------------//
static void OneWire_DMA_Deinit (void)
{
	dma_deinit (DMA0, DMA_CH1);
	dma_deinit (DMA0, DMA_CH3);
}
//--------------------------------------------------------------------------//
static void OneWire_DMA_Init (uint32_t Size)
{
    dma_single_data_parameter_struct dma_init_struct;
    /* enable DMA0 */
    rcu_periph_clock_enable(RCU_DMA0);
    /* deinitialize DMA channel7(USART2 tx) */
    dma_single_data_para_struct_init(&dma_init_struct);
    dma_deinit(DMA0, DMA_CH3);
    dma_init_struct.direction = DMA_MEMORY_TO_PERIPH;
    dma_init_struct.memory0_addr = (uint32_t)tx_buffer;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.periph_memory_width = DMA_PERIPH_WIDTH_8BIT;
//    dma_init_struct.number = ARRAYNUM(tx_buffer);
    dma_init_struct.number = Size;
    dma_init_struct.periph_addr = USART_DATA_ADDRESS;
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_single_data_mode_init(DMA0, DMA_CH3, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH3);
    dma_channel_subperipheral_select(DMA0, DMA_CH3, DMA_SUBPERI4);
    
    dma_deinit(DMA0, DMA_CH1);
    dma_init_struct.direction = DMA_PERIPH_TO_MEMORY;
    dma_init_struct.memory0_addr = (uint32_t)rx_buffer;
    dma_single_data_mode_init(DMA0, DMA_CH1, &dma_init_struct);
    /* configure DMA mode */
    dma_circulation_disable(DMA0, DMA_CH1);
    dma_channel_subperipheral_select(DMA0, DMA_CH1, DMA_SUBPERI4);
		
    dma_channel_enable (DMA0, DMA_CH1);
    dma_channel_enable (DMA0, DMA_CH3);
    
    /* USART DMA enable for transmission and reception */
    usart_dma_receive_config (_Device, USART_RECEIVE_DMA_ENABLE);
    usart_dma_transmit_config (_Device, USART_TRANSMIT_DMA_ENABLE);
		
}
//--------------------------------------------------------------------------//
static bool OneWire_ReadyDMA (void)
{
	if (RESET == dma_flag_get (DMA0, DMA_CH3, DMA_INTF_FTFIF))
		return (false);
	if (RESET == dma_flag_get (DMA0, DMA_CH1, DMA_INTF_FTFIF))
		return (false);
	return (true);
}
//--------------------------------------------------------------------------//
static void USARTErrorInteruptEnable (void)
{
	usart_interrupt_enable (_Device, USART_INT_PERR);
	usart_interrupt_enable (_Device, USART_INT_IDLE);
	usart_interrupt_enable (_Device, USART_INT_ERR);
}
//--------------------------------------------------------------------------//
static void USARTErrorInteruptDisable (void)
{
	usart_interrupt_disable (_Device, USART_INT_PERR);
	usart_interrupt_disable (_Device, USART_INT_TBE);
	usart_interrupt_disable (_Device, USART_INT_TC);
	usart_interrupt_disable (_Device, USART_INT_RBNE);
	usart_interrupt_disable (_Device, USART_INT_IDLE);
	usart_interrupt_disable (_Device, USART_INT_LBD);
	usart_interrupt_disable (_Device, USART_INT_CTS);
	usart_interrupt_disable (_Device, USART_INT_ERR);
	usart_interrupt_disable (_Device, USART_INT_EB);
	usart_interrupt_disable (_Device, USART_INT_RT);
}
//--------------------------------------------------------------------------//
static void OneWire_SetSpeed_PIO (uint32_t Speed)
{
	USARTErrorInteruptDisable ();
	OneWire_SetSpeed_ (Speed);
	OneWire_DMA_Deinit ();
}
//--------------------------------------------------------------------------//
static void OneWire_SetSpeed_DMA (uint32_t Speed, uint32_t Size)
{
	OneWire_SetSpeed_ (Speed);
	USARTErrorInteruptEnable ();
	OneWire_DMA_Init (Size);
}
//--------------------------------------------------------------------------//
static void CLRFlag (void)
{
	if (SET == usart_flag_get (_Device, USART_FLAG_EPERR))
		usart_flag_clear (_Device, USART_FLAG_EPERR);
	if (SET == usart_flag_get (_Device, USART_FLAG_FERR))
		usart_flag_clear (_Device, USART_FLAG_FERR);
	if (SET == usart_flag_get (_Device, USART_FLAG_NERR))
		usart_flag_clear (_Device, USART_FLAG_NERR);
	if (SET == usart_flag_get (_Device, USART_FLAG_ORERR))
		usart_flag_clear (_Device, USART_FLAG_ORERR);
	if (SET == usart_flag_get (_Device, USART_FLAG_IDLE))
		usart_flag_clear (_Device, USART_FLAG_IDLE);
}
//--------------------------------------------------------------------------//
static void CLRInterruptFlag (void)
{
	if (SET == usart_interrupt_flag_get (_Device, USART_INT_FLAG_PERR))
	{
		usart_interrupt_flag_clear (_Device, USART_INT_FLAG_PERR);
		usart_flag_clear (_Device, USART_FLAG_EPERR);
	}
	if (SET == usart_interrupt_flag_get (_Device, USART_INT_FLAG_ERR_FERR))
	{
		usart_interrupt_flag_clear (_Device, USART_INT_FLAG_ERR_FERR);
		usart_flag_clear (_Device, USART_FLAG_FERR);
	}
	if (SET == usart_interrupt_flag_get (_Device, USART_INT_FLAG_ERR_NERR))
	{
		usart_interrupt_flag_clear (_Device, USART_INT_FLAG_ERR_NERR);
		usart_flag_clear (_Device, USART_FLAG_NERR);
	}
	if (SET == usart_interrupt_flag_get (_Device, USART_INT_FLAG_ERR_ORERR))
	{
		usart_interrupt_flag_clear (_Device, USART_INT_FLAG_ERR_ORERR);
		usart_flag_clear (_Device, USART_FLAG_ORERR);
	}
	if (SET == usart_interrupt_flag_get (_Device, USART_INT_FLAG_IDLE))
	{
		usart_interrupt_flag_clear (_Device, USART_INT_FLAG_IDLE);
		usart_flag_clear (_Device, USART_FLAG_IDLE);
	}
	if (SET == usart_interrupt_flag_get (_Device, USART_INT_FLAG_RBNE_ORERR))
	{
		usart_interrupt_flag_clear (_Device, USART_INT_FLAG_RBNE_ORERR);
	}
}
//--------------------------------------------------------------------------//
void USART2_IRQHandler (void)
{
	CLRInterruptFlag ();
}
//--------------------------------------------------------------------------//
static uint8_t ExchangeByte (uint8_t byte)
{
	uint8_t ret;
	TTime TO = SetTime_ms (100);
	CLRFlag ();
//	if (usart_flag_get (_Device, USART_FLAG_TC))
//		usart_flag_clear (_Device, USART_FLAG_TC);
//	if (usart_flag_get (_Device, USART_FLAG_RBNE))
//		usart_flag_clear (_Device, USART_FLAG_RBNE);
	if (!usart_flag_get (_Device, USART_FLAG_TBE))
	{
		while (SET == usart_flag_get (_Device, USART_FLAG_TBE))
			if (EndTime (TO)) break;
		while (RESET == usart_flag_get (_Device, USART_FLAG_RBNE))
			if (EndTime (TO)) break;
		while (SET == usart_flag_get (_Device, USART_FLAG_RBNE)) 
		{
			usart_data_receive (_Device);
			if (EndTime (TO)) break;
//			usart_flag_clear (_Device, USART_FLAG_RBNE);
		}
	}

	usart_data_transmit (_Device, (uint16_t)byte);
	while (RESET == usart_flag_get (_Device, USART_FLAG_RBNE))
		if (EndTime (TO)) break;
	ret = (uint8_t)usart_data_receive (_Device);
//	usart_flag_clear (_Device, USART_FLAG_RBNE);
	return (ret);
}
//--------------------------------------------------------------------------//
void OneWire_ini (void)
{
	rcu_periph_clock_enable (RCU_USART2);
	rcu_periph_clock_enable (RCU_GPIOB);

	/* connect port to USART2_Tx */
	gpio_af_set (GPIOB, GPIO_AF_7, GPIO_PIN_10);
	/* configure USART Tx as alternate function push-pull */
	gpio_mode_set (GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_10);
	gpio_output_options_set (GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, GPIO_PIN_10);
	OneWire_SetSpeed_PIO (9600);
}
//--------------------------------------------------------------------------//
bool OneWire_Reset (void)
{
	bool r = false;
	OneWire_SetSpeed_PIO (9600);
	if (0xF0 != ExchangeByte (0xF0)) r = true;
	return (r);
}
//--------------------------------------------------------------------------//
void OneWire_StartReset_DMA (void)
{
	tx_buffer [0] = 0xF0;
	OneWire_SetSpeed_DMA (9600, 1);
}
//--------------------------------------------------------------------------//
bool OneWire_ReadyReset_DMA (bool * Present)
{
	*Present = false;
	if (!OneWire_ReadyDMA ()) return (false);
	if (0xF0 != rx_buffer [0]) *Present = true;
	return (true);
}
//--------------------------------------------------------------------------//
void OneWire_StartReadBit_DMA (void)
{
	tx_buffer [0] = 0xFF;
	OneWire_SetSpeed_DMA (115200, 1);
}
//--------------------------------------------------------------------------//
bool OneWire_ReadyReadBit_DMA (bool * Bit)
{
	*Bit = false;
	if (!OneWire_ReadyDMA ()) return (false);
	if (0xFF == rx_buffer [0]) *Bit = true;
	return (true);
}
//--------------------------------------------------------------------------//
static void OneWire_WriteBit (bool v)
{
	OneWire_SetSpeed_PIO (115200);
	if (v) ExchangeByte (0xFF);
	else  ExchangeByte (0x00);
}
//--------------------------------------------------------------------------//
bool OneWire_ReadBit (void)
{
	bool ret = false;
	OneWire_SetSpeed_PIO (115200);
	if (0xFF == ExchangeByte (0xFF)) ret = true;
	return (ret);
}
//--------------------------------------------------------------------------//
void OneWire_WriteByte (uint8_t v)
{
	uint8_t bitMask;
	
	for (bitMask = 0x01; bitMask; bitMask <<= 1)
		OneWire_WriteBit ( (bitMask & v) ? 1 : 0);
	
}
//--------------------------------------------------------------------------//
void OneWire_WriteBytes (const uint8_t *buf, uint8_t count)
{
	uint8_t i;
  for (i = 0; i < count; i++)  OneWire_WriteByte (buf [i]);
}
//--------------------------------------------------------------------------//
bool OneWire_StartByteExchangeDMA (uint8_t *buf, uint32_t count)
{
	uint32_t i, j;
	if (_SizeDMABuff < (count * 8)) return (false);
  for (i = 0; i < count; i++)
	{
		for (j = 0; j < 8; j++)
			if (0 == ((0x01 << j) & buf [i]))
				tx_buffer [j + 8 * i] = 0x00;
			else
				tx_buffer [j + 8 * i] = 0xFF;
	}
	OneWire_SetSpeed_DMA (115200, count * 8);
	return (true);
}
//--------------------------------------------------------------------------//
bool OneWire_ReadyByteExchangeDMA (uint8_t *buf, uint32_t count)
{
	uint32_t i, j;
	if (!OneWire_ReadyDMA ()) return (false);
	if (0 == count) return (true);
	if (_SizeDMABuff < (count * 8)) return (false);
	for (i = 0; i < count; i++)
	{
		buf [i] = 0;
		for (j = 0; j < 8; j++)
			if (0xFF == rx_buffer [j + 8 * i])
				buf [i] |= 0x01 << j;
	}
	return (true);
}
//--------------------------------------------------------------------------//
uint8_t OneWire_ReadByte (void)
{
	uint8_t bitMask;
	uint8_t r = 0;

	for (bitMask = 0x01; bitMask; bitMask <<= 1)
		if ( OneWire_ReadBit ()) r |= bitMask;
	return r;
}
//--------------------------------------------------------------------------//
void OneWire_ReadBytes (uint8_t *buf, uint8_t count)
{
	uint8_t i;
	for (i = 0; i < count; i++) buf [i] = OneWire_ReadByte ();
}
//--------------------------------------------------------------------------//
void OneWire_SelectROM (const uint8_t rom[8])
{
	uint8_t i;
	OneWire_WriteByte (0x55);// Choose ROM
	for (i = 0; i < 8; i++) OneWire_WriteByte (rom [i]);
}
//--------------------------------------------------------------------------//
void OneWire_SkipROM (void)
{
	OneWire_WriteByte (0xCC);// Skip ROM
}
//--------------------------------------------------------------------------//
static uint8_t LastDiscrepancy = 0;
static bool LastDeviceFlag = 0;
static uint8_t LastFamilyDiscrepancy = 0;
static uint8_t ROM_NO [8] = {0, 0, 0, 0, 0, 0, 0, 0};
//--------------------------------------------------------------------------//
void OneWire_ResetSearch (void)
{
	uint8_t i;
  // reset the search state
  LastDiscrepancy = LastFamilyDiscrepancy = 0;
  LastDeviceFlag = false;
  for(i = 0; i < 8; i++) ROM_NO [i] = 0;
}
//--------------------------------------------------------------------------//
bool OneWire_Search (uint8_t *newAddr)
{
	uint8_t id_bit_number;
	uint8_t last_zero, rom_byte_number;
	uint8_t rom_byte_mask;
	bool search_result, id_bit, cmp_id_bit, search_direction;

	// initialize for search
	id_bit_number = rom_byte_mask = 1;
	last_zero = rom_byte_number = 0;
	search_result = false;

	// if the last call was not the last one
	if (!LastDeviceFlag)
	{
		// 1-Wire reset
		if (!OneWire_Reset ())
		{
			// reset the search
			LastDiscrepancy = LastFamilyDiscrepancy = 0;
			LastDeviceFlag = false;
			return (0);
		}

		// issue the search command
		OneWire_WriteByte (0xF0); // NORMAL SEARCH

		// loop to do the search
		do
		{
			// read a bit and its complement
			id_bit = OneWire_ReadBit();
			cmp_id_bit = OneWire_ReadBit ();

			// check for no devices on 1-wire
			if (id_bit && cmp_id_bit) 
			{
				break;
			} 
			else 
			{
				// all devices coupled have 0 or 1
				if (id_bit != cmp_id_bit)
				{
					search_direction = id_bit;  // bit write value for search
				}
				else
				{
					// if this discrepancy if before the Last Discrepancy
					// on a previous next then pick the same as last time
					if (id_bit_number < LastDiscrepancy)
					{
						search_direction = ((ROM_NO[rom_byte_number] & rom_byte_mask) > 0);
					}
					else
					{
						// if equal to last pick 1, if not then pick 0
						search_direction = (id_bit_number == LastDiscrepancy);
					}
					// if 0 was picked then record its position in LastZero
					if (!search_direction) 
					{
						last_zero = id_bit_number;
						// check for Last discrepancy in family
						if (last_zero < 9)
							LastFamilyDiscrepancy = last_zero;
					}
				}

				// set or clear the bit in the ROM byte rom_byte_number
				// with mask rom_byte_mask
				if (search_direction)
					ROM_NO [rom_byte_number] |= rom_byte_mask;
				else
					ROM_NO [rom_byte_number] &= ~rom_byte_mask;

				// serial number search direction write bit
				OneWire_WriteBit (search_direction);

				// increment the byte counter id_bit_number
				// and shift the mask rom_byte_mask
				id_bit_number++;
				rom_byte_mask <<= 1;

				// if the mask is 0 then go to new SerialNum byte rom_byte_number and reset mask
				if (rom_byte_mask == 0) 
				{
					rom_byte_number++;
					rom_byte_mask = 1;
				}
			}
		}
		while (rom_byte_number < 8);  // loop until through all ROM bytes 0-7

		// if the search was successful then
		if (!(id_bit_number < 65)) 
		{
			// search successful so set LastDiscrepancy,LastDeviceFlag,search_result
			LastDiscrepancy = last_zero;

			// check for last device
			if (LastDiscrepancy == 0)
				LastDeviceFlag = true;
			search_result = true;
		}
	}

	// if no device found then reset counters so next 'search' will be like a first
	if (!search_result || !ROM_NO [0]) 
	{
		LastDiscrepancy = 0;
		LastFamilyDiscrepancy = 0;
		LastDeviceFlag = false;
		search_result = false;
	}
	else
	{
		uint8_t i;
		for (i = 0; i < 8; i++) newAddr [i] = ROM_NO [i];
	}
	return search_result;
}
//--------------------------------------------------------------------------//
// Dow-CRC using polynomial X^8 + X^5 + X^4 + X^0
// Tiny 2x16 entry CRC table created by Arjen Lentz
// See http://lentz.com.au/blog/calculating-crc-with-a-tiny-32-entry-lookup-table
static const uint8_t dscrc2x16_table[] = 
{
	0x00, 0x5E, 0xBC, 0xE2, 0x61, 0x3F, 0xDD, 0x83,
	0xC2, 0x9C, 0x7E, 0x20, 0xA3, 0xFD, 0x1F, 0x41,
	0x00, 0x9D, 0x23, 0xBE, 0x46, 0xDB, 0x65, 0xF8,
	0x8C, 0x11, 0xAF, 0x32, 0xCA, 0x57, 0xE9, 0x74
};

// Compute a Dallas Semiconductor 8 bit CRC. These show up in the ROM
// and the registers.  (Use tiny 2x16 entry CRC table)
uint8_t OneWire_crc8 (const uint8_t *addr, uint8_t len)
{
	uint8_t crc = 0;

	while (len--) {
		crc = *addr++ ^ crc;  // just re-using crc as intermediate
		crc = dscrc2x16_table [crc & 0x0f]
			^ dscrc2x16_table [16 + ((crc >> 4) & 0x0f)];
	}

	return crc;
}
//--------------------------------------------------------------------------//
