/*=============================================================================
     Project:
     Platform: GD32F470
     Filename: WriteFileRecord.h
     Description: FC 0x15 - Write File Record
============================================================================*/
#ifndef WRITE_FILE_RECORD_H
#define WRITE_FILE_RECORD_H

#include <stdint.h>

uint8_t WriteFileRecord(
	uint8_t NumUART,
	uint8_t Command,
	uint8_t *B,
	uint32_t *pSize);

#endif
