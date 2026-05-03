

#ifndef __drv_SPI_H__
#define __drv_SPI_H__
	//--------------------------------------------------------------------------//
	#include <stdbool.h>
	#include <stdint.h>
	//--------------------------------------------------------------------------//
	typedef enum {_SPI_CS_EE1} TSPICS;
	typedef enum {_SPI_MODE_0, _SPI_MODE_1, _SPI_MODE_2, _SPI_MODE_3} TSPIMODE;
	typedef enum {_SPI4} TSPINUM;
	//--------------------------------------------------------------------------//
	//#define _SPI_MODE_0 (0)
	//#define _SPI_MODE_1 (CPHA)
	//#define _SPI_MODE_2 (CPOL)
	//#define _SPI_MODE_3 (CPOL|CPHA)
	//--------------------------------------------------------------------------//
	void spi_configure_cs (TSPICS cs_num);
	void spi_prepare_bus (TSPINUM bus_num, int32_t freq, uint16_t spiMode);
	void spi_enable_cs (TSPICS cs_num);
	void spi_disable_cs (TSPICS cs_num);
	uint8_t spi_exchange8 (TSPINUM bus_num, uint8_t data);
	uint16_t spi_exchange16 (TSPINUM bus_num, uint16_t data);
	uint32_t spi_exchange24 (TSPINUM bus_num, uint32_t data);
	uint32_t spi_exchange32 (TSPINUM bus_num, uint32_t data);
	uint64_t spi_exchange64 (TSPINUM bus_num, uint64_t data);
	void spi_bufTx (TSPINUM bus_num, const void *tx, int32_t len);
	void spi_bufRx (TSPINUM bus_num, void *rx, int32_t len);
	//--------------------------------------------------------------------------//
#endif /*__drv_SPI_H__*/

