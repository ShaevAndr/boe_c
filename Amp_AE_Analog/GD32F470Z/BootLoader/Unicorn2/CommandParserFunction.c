/*=============================================================================
2     Project: 
3     Platform: GD32F407
4     Filename: CommandParserFunction.c
5     Description:
6     Version: 0.0
7     Created: 2022.09.15
8     Last modified: 2023.06.28
9============================================================================*/
#include <stdint.h>
//-----------------------------------------------------------------------------
#include "CommandParserFunction.h"
//-----------------------------------------------------------------------------
uint32_t min_uint32 (uint32_t a, uint32_t b)
{
	if (a < b) return (a);
	else return (b);
}


//--------------------------------------------------------------------------//
uint32_t Pased_Buff_to_uint32_t (uint8_t * Buff)
{
  uint32_t t = 0;
  uint8_t i;

  for (i = 0; i <= 3; i++)
  {
    t <<= 8;
    t |= Buff [i];
  }
  return (t);
}
//--------------------------------------------------------------------------//
void Pased_uint32_t_to_Buff (uint8_t * Buff, uint32_t in)
{
  uint32_t t;
  int8_t i;

  t = *((uint32_t *)(&(in)));
  for (i = 3; i >= 0; i--)
  {
    Buff [(uint8_t)i] = t & 0x000000ff;
    t >>= 8;
  }
}
//--------------------------------------------------------------------------//
void Pased_uint16_t_to_Buff(uint8_t * Buff, uint16_t in)
{
  uint16_t t;
  int32_t i;

  t = *((uint16_t *)(&(in)));
  for (i = 1; i >= 0; i--)
  {
    Buff [i] = t & 0x000000ff;
    t >>= 8;
  }
}
//--------------------------------------------------------------------------//
uint16_t Pased_Buff_to_uint16_t (uint8_t * Buff)
{
  return ((uint16_t)(Buff [1] | (Buff [0] << 8)));
}
//--------------------------------------------------------------------------//
int32_t Pased_Buff_to_int32_t (uint8_t * Buff)
{
  int32_t t = 0;
  uint8_t i;

  for (i = 0; i <= 3; i++)
  {
    t <<= 8;
    t |= Buff [i];
  }
  return (t);
}
//--------------------------------------------------------------------------//
void Pased_int32_t_to_Buff (uint8_t * Buff, int32_t in)
{
  int32_t t;
  int8_t i;

  t = *((int32_t *)(&(in)));
  for (i = 3; i >= 0; i--)
  {
    Buff [(uint8_t)i] = t & 0x000000ff;
    t >>= 8;
  }
}
//--------------------------------------------------------------------------//
float Pased_Buff_to_float (uint8_t * Buff)
{
  int8_t i;
  uint32_t t = 0;
  float f;

  for (i = 3; i >= 0; i--)
  {
    t <<= 8;
    t |= Buff [(uint8_t)i];
  }
  f = *((float *)(&t));
  return (f);
}
//--------------------------------------------------------------------------//
void Pased_float_to_Buff (uint8_t * Buff, float f)
{
  int8_t i;
  uint32_t t;

  t = *((uint32_t *)(&(f)));
  for (i = 0; i < 4; i++)
  {
    Buff [(uint8_t)i] = t & 0x000000ff;
    t >>= 8;
  }
}
//--------------------------------------------------------------------------//
uint8_t CalculateCheckSum (uint8_t * P, uint32_t N)
{
  uint16_t S = 0;
  uint32_t i;
  
  for (i = 0; i < N; i++)
  {
    S += P [i];
    while (S > 0x00ff) {
      S &= 0x00ff;
      S++;
    }
  }
  S ^= 0x00ff;
  return (S & 0x00ff);
}
//--------------------------------------------------------------------------//
