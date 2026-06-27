/*=============================================================================
2     Project: 
3     Platform: GD32F470
4     Filename: AccessTabParam.h
5     Description:
6     Version: 0.0
7     Created: 2023.08.31
8     Last modified: 2026.02.08
9============================================================================*/

#ifndef AccessTabParam_H
#define AccessTabParam_H

#include <stdint.h>

uint8_t GetCountTabParam (uint32_t * count);
uint8_t ReadTabParam (uint8_t NumUART, uint32_t NumParam, uint8_t * B,
											uint32_t offset, uint32_t wordSize, uint32_t stride,
											uint32_t * size);
uint8_t WriteTabParam (uint8_t NumUART, uint32_t NumParam, uint8_t * B,
											uint32_t offset, uint32_t stride, uint32_t size);
uint8_t PreparTabParam (uint8_t NumUART, uint32_t NumParam, int32_t column);
uint8_t ProgrPreparTabParam (uint8_t NumUART, uint32_t NumParam,
														int32_t * rows, int32_t * column,
														int32_t * curStep, int32_t * stepsCount);
uint8_t ReleaseTabParam (uint8_t NumUART, uint32_t NumParam);
uint8_t ReadDescrTabParam (uint8_t NumUART, uint32_t NumParam,
													uint8_t * B, uint32_t * pSize);

#endif
