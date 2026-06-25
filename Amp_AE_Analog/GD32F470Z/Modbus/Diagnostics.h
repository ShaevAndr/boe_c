/*=============================================================================
     Project:
     Platform: GD32F470
     Filename: Diagnostics.h
     Description: FC 0x08 - Diagnostics extensions for table parameters.
============================================================================*/
#ifndef DIAGNOSTICS_H
#define DIAGNOSTICS_H

#include <stdint.h>

#define MODBUS_DIAG_TABLE_COUNT 0x0005U
#define MODBUS_DIAG_TABLE_DESCRIPTION 0x0006U
#define MODBUS_DIAG_TABLE_PREPARE 0x0007U
#define MODBUS_DIAG_TABLE_PREPARE_PROGRESS 0x0008U
#define MODBUS_DIAG_TABLE_RELEASE 0x0009U

uint8_t Diagnostics(
	uint8_t NumUART,
	uint8_t Command,
	uint8_t *B,
	uint32_t *pSize);

#endif
