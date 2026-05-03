/*=============================================================================
2     Project: 
3     Platform: GD32F407
4     Filename: CommandParserFunction.h
5     Description:
6     Version: 0.0
7     Created: 2022.09.15
8     Last modified: 2023.06.28
9============================================================================*/
#ifndef CommandParserFunction_H
#define CommandParserFunction_H
	//------------------------------------------------------------------------------
	#include <stdint.h>
	//------------------------------------------------------------------------------
	uint32_t min_uint32 (uint32_t a, uint32_t b);
	uint32_t Pased_Buff_to_uint32_t (uint8_t * Buff);
	void Pased_uint32_t_to_Buff (uint8_t * Buff, uint32_t in);
	void Pased_uint16_t_to_Buff(uint8_t * Buff, uint16_t in);
	uint16_t Pased_Buff_to_uint16_t (uint8_t * Buff);
	int32_t Pased_Buff_to_int32_t (uint8_t * Buff);
	void Pased_int32_t_to_Buff (uint8_t * Buff, int32_t in);
	float Pased_Buff_to_float (uint8_t * Buff);
	void Pased_float_to_Buff (uint8_t * Buff, float f);
	uint8_t CalculateCheckSum (uint8_t * P, uint32_t N);
	//--------------------------------------------------------------------------//

#endif
