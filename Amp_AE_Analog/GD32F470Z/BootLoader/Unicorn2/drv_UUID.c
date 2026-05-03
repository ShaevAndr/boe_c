/*=============================================================================
2  Project: 
3  Platform: GD32F407
4  Filename: drv_UUID.c
5  Description:
6  Version: 0.0
7  Created: 2022.08.26
8  Last modified: 2023.06.28
9=============================================================================*/
#include <gd32f4xx.h>
//------------------------------------------------------------------------------
#include "drv_UUID.h"
//------------------------------------------------------------------------------
#if defined(GD32F450) || defined(GD32F405) || defined(GD32F407) || defined(GD32F470) || defined(GD32F425) || defined(GD32F427)
#define UNIQUE_ID_0 ((uint32_t)0x1FFF7A10)/*Unique device ID UNIQUE_ID[31: 0] */
#define UNIQUE_ID_1 ((uint32_t)0x1FFF7A14)/*Unique device ID UNIQUE_ID[63:32] */
#define UNIQUE_ID_2 ((uint32_t)0x1FFF7A18)/*Unique device ID UNIQUE_ID[95:64] */
#elif defined(GD32H7XX)
#define UNIQUE_ID_0 ((uint32_t)0x1FF0F7E8)/*Unique device ID UNIQUE_ID[31: 0] */
#define UNIQUE_ID_1 ((uint32_t)0x1FF0F7EC)/*Unique device ID UNIQUE_ID[63:32] */
#define UNIQUE_ID_2 ((uint32_t)0x1FF0F7F0)/*Unique device ID UNIQUE_ID[95:64] */
#elif defined(GD32F1XX)
#define UNIQUE_ID_0 ((uint32_t)0x1FF1E800)/*Unique device ID UNIQUE_ID[31: 0] */
#define UNIQUE_ID_1 ((uint32_t)0x1FF1E804)/*Unique device ID UNIQUE_ID[63:32] */
#define UNIQUE_ID_2 ((uint32_t)0x1FF1E808)/*Unique device ID UNIQUE_ID[95:64] */
#else
#error "Unknown MCU"
#endif
//------------------------------------------------------------------------------
typedef union SI_UU32
{
  uint32_t u32;       ///< The 4-byte value as a 32-bit unsigned integer.
  uint8_t u8[4];      ///< The 4-byte value as 4 unsigned 8-bit integers.
} UU32_t;
//------------------------------------------------------------------------------
TUUID GetUUID (void)
{
  TUUID r;
  UU32_t u;

  u.u32 = REG32 (UNIQUE_ID_0);
  r.byte [sizeof (r) - 1] = u.u8 [0];
  r.byte [sizeof (r) - 2] = u.u8 [1];
  r.byte [sizeof (r) - 3] = u.u8 [2];
  r.byte [sizeof (r) - 4] = u.u8 [3];

  u.u32 = REG32 (UNIQUE_ID_1);
  r.byte [sizeof (r) - 5] = u.u8 [0];
  r.byte [sizeof (r) - 6] = u.u8 [1];
  r.byte [sizeof (r) - 7] = u.u8 [2];
  r.byte [sizeof (r) - 8] = u.u8 [3];

  u.u32 = REG32 (UNIQUE_ID_2);
  r.byte [sizeof (r) -  9] = u.u8 [0];
  r.byte [sizeof (r) - 10] = u.u8 [1];
  r.byte [sizeof (r) - 11] = u.u8 [2];
  r.byte [sizeof (r) - 12] = u.u8 [3];

  u.u32 = SCB->CPUID;
  r.byte [sizeof (r) - 13] = u.u8 [0];
  r.byte [sizeof (r) - 14] = u.u8 [1];
  r.byte [sizeof (r) - 15] = u.u8 [2];
  r.byte [sizeof (r) - 16] = u.u8 [3];

  return (r);
}
//------------------------------------------------------------------------------
const char* uuid_to_string(TUUID uuid, char *str, int maxsize)
{
  static char const hexdigits[17] = "0123456789ABCDEF";
  char *p = str;

  if (maxsize < UUID_STR_LENGTH)
  {
    //CLOG_ERROR("Not enough space in buffer %d", maxLen);
    return NULL;
  }
  for (int i = 0; i < UUID_LENGTH; i++)
  {
    if ((i == 4) || (i == 6) || (i == 8) || (i == 10))
      *p++ = '-';
    size_t tmp = uuid.byte[i];
    *p++ = hexdigits[tmp >> 4];
    *p++ = hexdigits[tmp & 15];
  }
  *p = '\0';

  return str;
}
