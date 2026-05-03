/*=============================================================================
  Project: ADCVibro
  Platform:
  Filename: drv_EEPROM.c
  Description:
  Programmer:
  Version: 1.0
  Created: 2024.03.23
  Last modified: 2024.03.23
=============================================================================*/
#include <stdint.h>

#include "global.h"
//#include "logger.h"
#include "drv_time.h"
#include "drv_SPI.h"
#include "drv_EEPROM.h"

/* Status Register Bits */
#define SPI_SR_WIP  (0x01 << 0)
#define SPI_SR_WEL  (0x01 << 1)
#define SPI_SR_AAI  (0x01 << 6)

enum Command {
//  JedecRDID         = 0x9F,
//  RamtronReadSN     = 0xC3,
//  CypressRUID       = 0x4B,

  JedecWRSR          = 0x01,
  JedecByteProgram   = 0x02,
  JedecRead          = 0x03,
  JedecWRDI          = 0x04,
  JedecRDSR          = 0x05,
  JedecWREN          = 0x06,
//  JedecByteProgram4  = 0x12,
//  JedecRead4         = 0x13,

//  ChipErase          = 0x60,
};

//#define MAX_CHIPID_LENGTH  (16)

//extern int readStatusRegister(const TMTDDevice *mtdDev);
//extern int writeStatusRegister(const TMTDDevice *mtdDev, uchar value);
static int setProtectionEEPROM (const TMTDDevice *mtdDev, bool wpen, bool bp0, bool bp1);

//#define MAX_MANUFACTURER_NAME_LENGTH  (20)
/*
struct MTDManufacturerData {
  u8            mbank, id;
  char          name[MAX_MANUFACTURER_NAME_LENGTH];
  enum MTDManufacturer  manufacturer;
};

const struct MTDManufacturerData MTDManufacturers[] = {
{
  .mbank  = 5,
  .id     = 0xC2,
  .name   = "Ramtron",
  .manufacturer = MTDManufRamtron,
},{
  .mbank  = 0,
  .id     = 0x01,
  .name   = "Cypress",
  .manufacturer = MTDManufCypress,
	
//			MTDManufMicrochip,
//		MTDManufSTMicroelectronics

}
};

#define ManufacturersCount  ARRAY_SIZE(MTDManufacturers)
*/
#define MAX_NAME_LENGTH  (16)

typedef struct {
//  uint32_t	size;
  uint32_t	BlockSize;
  uint32_t	BlocksCount;
  uint8_t		addrSize;
//  ushort   id;
  int32_t		maxSpeed;
  char     name[MAX_NAME_LENGTH];
  bool			serialNumber;
  uint32_t	serialNumberAddr;
  uint32_t	serialNumberSize;
  enum MTDAlgorithm  algorithm;
} TEEPROMSpiParams;

static const TEEPROMSpiParams EEPROMParams [] = {
{
	.BlockSize = 64,
	.BlocksCount = 512,
  .addrSize = 2,
  .maxSpeed = 5.0e6,
  .name = "STM_M95256",
  .serialNumber = false,
  .serialNumberAddr = 0,
  .serialNumberSize = 0,
  .algorithm = MTDAlgoEEPROM25
},{
	.BlockSize = 16,
	.BlocksCount = 16,
  .addrSize = 1,
  .maxSpeed = 5.0e6,
  .name = "MC_25AA02E48",
  .serialNumber = true,
  .serialNumberAddr = 0xFA,
  .serialNumberSize = 6,
  .algorithm = MTDAlgoEEPROM25,
}
};
#define EEPROMParamsCount  ARRAY_SIZE(EEPROMParams)

void initMTD (TMTDDevice *mtdDev, TSPINUM busNum, TSPICS csNum, int32_t freq, TSPIMODE spiMode, TEEPROMType type)
{
	const TEEPROMSpiParams * P = &(EEPROMParams [type]);
	
	mtdDev->busNum = busNum;
	mtdDev->csNum = csNum;
	mtdDev->spiMode = spiMode;
	mtdDev->freq = min (P->maxSpeed, freq);
	spi_configure_cs (mtdDev->csNum);

	mtdDev->blockSize = P->BlockSize;
	mtdDev->blocksCount = P->BlocksCount;
	mtdDev->size = P->BlockSize * P->BlocksCount;
	mtdDev->serialNumberAvailable = P->serialNumber;
	mtdDev->serialNumberAddr = P->serialNumberAddr;
	mtdDev->serialNumberSize = P->serialNumberSize;
	
	mtdDev->algo = P->algorithm;
	mtdDev->addrSize = P->addrSize;
	mtdDev->eraseCmd = 0;
	
	mtdDev->readChunkSize = P->BlockSize;
	mtdDev->writeChunkSize = P->BlockSize;
	mtdDev->blockCycles = -1;
	
	setProtectionEEPROM (mtdDev, true, false, false);
}


static void doWriteEnable(const TMTDDevice *mtdDev)
{
  spi_enable_cs(mtdDev->csNum);
//  Delay_us(1); // wait 1 mks
  spi_exchange8(mtdDev->busNum, JedecWREN);
 // Delay_us(1); // wait 1 mks
  spi_disable_cs(mtdDev->csNum);
  Delay_us(1); // wait 1 mks
}

static int readStatusRegister(const TMTDDevice *mtdDev)
{
  u8 status;

  spi_prepare_bus (mtdDev->busNum, mtdDev->freq, mtdDev->spiMode);
  spi_enable_cs(mtdDev->csNum);
//  Delay_us(1); // wait 1 mks
  spi_exchange8(mtdDev->busNum, JedecRDSR);
  status = spi_exchange8(mtdDev->busNum, 0xFF);
//  Delay_us(1); // wait 1 mks
  spi_disable_cs(mtdDev->csNum);
  Delay_us(1); // wait 1 mks

  return status;
}

static int writeStatusRegister(const TMTDDevice *mtdDev, uchar value)
{
  spi_prepare_bus (mtdDev->busNum, mtdDev->freq, mtdDev->spiMode);
  doWriteEnable(mtdDev);
  spi_enable_cs(mtdDev->csNum);
//  Delay_us(1); // wait 1 mks
  spi_exchange8(mtdDev->busNum, JedecWRSR);
  spi_exchange8(mtdDev->busNum, value);
//  Delay_us(1); // wait 1 mks
  spi_disable_cs(mtdDev->csNum);
  Delay_us(1); // wait 1 mks

  return value;
}

static int setProtectionEEPROM (const TMTDDevice *mtdDev, bool wpen, bool bp0, bool bp1)
{
  uchar status = 0;

  if (wpen)
    status |= 1 << 7;
  if (bp0)
    status |= 1 << 2;
  if (bp1)
    status |= 1 << 3;
  if (writeStatusRegister(mtdDev, status) < 0) {
//    CLOG_ERROR("spiWriteStatusRegister(0x%02X) failed", status);
    return -1;
  }
  return 0;
}

static int waitWriteInProgress(const TMTDDevice *mtdDev, int timeoutMSec)
{
  TTime endTime;
  int res;

  //spi_prepare_bus (mtdDev->busNum, mtdDev->freq, mtdDev->spiMode);
  if (timeoutMSec > 0)
    endTime = SetTime_ms (timeoutMSec);
  while (1) {
    res = readStatusRegister(mtdDev);
    if (unlikely(res < 0))
      break;
    if ((uchar)res & SPI_SR_WIP)
    { // write in progress
      if (timeoutMSec > 0)
      { // check timeout
        if (unlikely(EndTime(endTime))) {
//          CLOG_WARNING("Timeout %d expired", timeoutMSec);
          return 1;
        }
      }
      barrier();
      continue;
    }
    res = 0;
    break;
  }
  return res;
}

static int spiNByteRead(const TMTDDevice *mtdDev, uint address, uchar *buf,
    uint len)
{
  switch (mtdDev->addrSize)
  {
  case 1:
    spi_prepare_bus (mtdDev->busNum, mtdDev->freq, mtdDev->spiMode);
    spi_enable_cs (mtdDev->csNum);
//		Delay_us(1); // wait 1 mks
    spi_exchange8 (mtdDev->busNum, JedecRead);
    spi_exchange8 (mtdDev->busNum, address);
    break;
  case 2:
    spi_prepare_bus (mtdDev->busNum, mtdDev->freq, mtdDev->spiMode);
    spi_enable_cs (mtdDev->csNum);
//		Delay_us(1); // wait 1 mks
    spi_exchange8 (mtdDev->busNum, JedecRead);
    spi_exchange16 (mtdDev->busNum, address);
    break;
  case 3:
    spi_prepare_bus (mtdDev->busNum, mtdDev->freq, mtdDev->spiMode);
    spi_enable_cs (mtdDev->csNum);
//		Delay_us(1); // wait 1 mks
    spi_exchange8 (mtdDev->busNum, JedecRead);
    spi_exchange24 (mtdDev->busNum, address);
    break;
  default:
//    CLOG_ERROR("Unhandled addrSize %d", mtdDev->addrSize);
    return -1;
  }
  spi_bufRx (mtdDev->busNum, buf, len);
//  Delay_us(1); // wait 1 mks
  spi_disable_cs (mtdDev->csNum);
  Delay_us(1); // wait 1 mks

  return len;
}

static int spiNByteWrite(const TMTDDevice *mtdDev, uint address,
    const uchar *buf, uint len, int timeout)
{
  int res;

  switch (mtdDev->addrSize)
  {
  case 1:
    spi_prepare_bus (mtdDev->busNum, mtdDev->freq, mtdDev->spiMode);
    doWriteEnable (mtdDev);
    spi_enable_cs (mtdDev->csNum);
//		Delay_us(1); // wait 1 mks
    spi_exchange8 (mtdDev->busNum, JedecByteProgram);
    spi_exchange8 (mtdDev->busNum, address);
    break;
  case 2:
    spi_prepare_bus (mtdDev->busNum, mtdDev->freq, mtdDev->spiMode);
    doWriteEnable (mtdDev);
    spi_enable_cs (mtdDev->csNum);
//		Delay_us(1); // wait 1 mks
    spi_exchange8 (mtdDev->busNum, JedecByteProgram);
    spi_exchange16 (mtdDev->busNum, address);
    break;
  case 3:
    spi_prepare_bus (mtdDev->busNum, mtdDev->freq, mtdDev->spiMode);
    doWriteEnable (mtdDev);
    spi_enable_cs (mtdDev->csNum);
//		Delay_us(1); // wait 1 mks
    spi_exchange8 (mtdDev->busNum, JedecByteProgram);
    spi_exchange24 (mtdDev->busNum, address);
    break;
  default:
//    CLOG_ERROR("Unhandled addrSize %d", mtdDev->addrSize);
    return -1;
  }
  spi_bufTx (mtdDev->busNum, buf, len);
//  Delay_us(1); // wait 1 mks
  spi_disable_cs (mtdDev->csNum);
  Delay_us(1); // wait 1 mks
  res = len;
  if (timeout >= 0) {
    res = waitWriteInProgress (mtdDev, timeout);
    if (unlikely (res < 0))
      return res;
    if (unlikely (res > 0)) {
//      CLOG_ERROR("Timeout %d msec", timeout);
      res = -1;
    }
  }
  return res;
}

static int spiReadChunked (const TMTDDevice *mtdDev, uint address, uchar *buf,
    uint len, uint chunkSize)
{
  uint i, j, areaSize, startHere, lenHere, toRead;
  int res = 0;

  // Limit for multi-die 4-byte-addressing chips
  if (mtdDev->readChunkSize > 0)
    areaSize = min(mtdDev->readChunkSize, 16 * 1024 * 1024U);
  else
    areaSize = min(mtdDev->size, 16 * 1024 * 1024U);
  /* Warning: This loop has a very unusual condition and body.
   * The loop needs to go through each area with at least one affected
   * byte. The lowest area number is (start / area_size) since that
   * division rounds down. The highest area number we want is the area
   * where the last byte of the range lives. That last byte has the
   * address (start + len - 1), thus the highest area number is
   * (start + len - 1) / area_size. Since we want to include that last
   * area as well, the loop condition uses <=.
   */
  for (i = address / areaSize; i <= (address + len - 1) / areaSize; i++) {
    /* Byte position of the first byte in the range in this area. */
    /* starthere is an offset to the base address of the chip. */
    startHere = max (address, i * areaSize);
    /* Length of bytes in the range in this area. */
    lenHere = min(address + len, (i + 1) * areaSize) - startHere;
    for (j = 0; j < lenHere; j += chunkSize) {
      toRead = min(chunkSize, lenHere - j);
      res = spiNByteRead(mtdDev, startHere + j, buf + startHere - address + j, toRead);
      if (unlikely(res < 0))
        break;
    }
    if (unlikely(res < 0))
      break;
  }
//  if (res < 0)
//    CLOG_ERROR("Failed: address %u, len %u, chunkSize %u", address, len, chunkSize);

  return res;
}

static int spiWriteChunked (const TMTDDevice *mtdDev, uint address,
    const uchar *buf, uint len, uint chunkSize, bool checkStatusReg)
{
  int res = 0;
  uint i, j, startHere, lenHere, toWrite, pageSize;
  /* FIXME: page_size is the wrong variable. We need max_writechunk_size
   * in struct flashctx to do this properly. All chips using
   * spi_chip_write_256 have page_size set to max_writechunk_size, so
   * we're OK for now.
   */

  if (mtdDev->writeChunkSize > 0)
    pageSize = mtdDev->writeChunkSize;
  else
    pageSize = 256;
  /* Warning: This loop has a very unusual condition and body.
   * The loop needs to go through each page with at least one affected
   * byte. The lowest page number is (start / page_size) since that
   * division rounds down. The highest page number we want is the page
   * where the last byte of the range lives. That last byte has the
   * address (start + len - 1), thus the highest page number is
   * (start + len - 1) / page_size. Since we want to include that last
   * page as well, the loop condition uses <=.
   */
  for (i = address / pageSize; i <= (address + len - 1) / pageSize; i++)
  {
    /* Byte position of the first byte in the range in this page. */
    /* starthere is an offset to the base address of the chip. */
    startHere = max (address, i * pageSize);
    /* Length of bytes in the range in this page. */
    lenHere = min(address + len, (i + 1) * pageSize) - startHere;
    for (j = 0; j < lenHere; j += chunkSize)
    {
      toWrite = min(chunkSize, lenHere - j);
      res = spiNByteWrite(mtdDev, startHere + j, buf + startHere - address + j,
                          toWrite, (checkStatusReg ? 5000 : -1));
      if (unlikely(res < 0))
        break;
    }
    if (unlikely(res < 0))
      break;
  }
//  if (res < 0)
//    CLOG_ERROR("Failed: address %u, len %u, chunkSize %u, checkStatusReg %d",
//               address, len, chunkSize, checkStatusReg);
  return res;
}

int mtdRead (const TMTDDevice *mtdDev, uint addr, void *buf, uint size)
{
  int res;

  //CLOG_DEBUG("blockNum %u, size %u, offset %u => addr %u",
  //           blockNum, size, offset, addr);
  switch (mtdDev->algo)
  {
  case MTDAlgoFRAM25:
    //res = spiReadChunked(mtdDev, addr, static_cast<uchar*>(buf), size, kFRAMChunkSize);
    res = spiReadChunked (mtdDev, addr, (uchar*)buf, size, mtdDev->readChunkSize);
    break;
  case MTDAlgoEEPROM25:
    res = spiReadChunked (mtdDev, addr, (uchar*)buf, size, mtdDev->readChunkSize);
    break;
  default:
//    CLOG_ERROR("Unhandled algorithm %d", mtdDev->algo);
    return -1;
  }
  return res;
}

int mtdWrite(const TMTDDevice *mtdDev, uint addr, const void *buf, uint size)
{
  int res;

  //CLOG_DEBUG("blockNum %u, size %u, offset %u => addr %u",
  //           blockNum, size, offset, addr);
  switch (mtdDev->algo)
  {
  case MTDAlgoFRAM25:
    res = spiWriteChunked(mtdDev, addr, (const uchar*)buf, size,
        mtdDev->writeChunkSize, false);
    break;
  case MTDAlgoEEPROM25:
    res = spiWriteChunked(mtdDev, addr, (const uchar*)buf, size,
        mtdDev->writeChunkSize, true);
    break;
		
  default:
//    CLOG_ERROR("Unhandled algorithm %d", mtdDev->algo);
    return -1;
  }
  return res;
}

int mtdReadSerial (const TMTDDevice *mtdDev, void *buf, uint size)
{
  int res = -1;
	if (!mtdDev->serialNumberAvailable) return (res);
	size = min (size, mtdDev->serialNumberSize);
	res = mtdRead (mtdDev, mtdDev->serialNumberAddr, buf, size);
  return res;
}

