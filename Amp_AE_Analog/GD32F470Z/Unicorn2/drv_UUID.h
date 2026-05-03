/*=============================================================================
2  Project: 
3  Platform: 
4  Filename: drv_UUID.h
5  Description:
6  Version: 0.0
7  Created: 2022.08.22
8  Last modified: 2022.08.22
9=============================================================================*/
#ifndef drv_UUID_H
#define drv_UUID_H
//------------------------------------------------------------------------------
#include <stdint.h>
//------------------------------------------------------------------------------
#define UUID_LENGTH     (16)
#define UUID_STR_LENGTH (37)
typedef struct
{
  uint8_t byte [UUID_LENGTH];
} TUUID;
//------------------------------------------------------------------------------
TUUID GetUUID (void);
//------------------------------------------------------------------------------
const char* uuid_to_string(TUUID uuid, char *str, int maxsize);
#endif 
