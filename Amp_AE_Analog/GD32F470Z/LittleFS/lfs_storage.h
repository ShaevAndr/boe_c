/*
 * lfs storage layout for STM32G743
 *
 */
#ifndef LFS_STORAGE_H
#define LFS_STORAGE_H

#define LFS_NO_MALLOC

#define LFS_RW_SIZE         (16)
#define LFS_CACHE_SIZE      LFS_RW_SIZE*4
#define LFS_LOOKAHEAD_SIZE  (16)

#include "lfs.h"
#include "..\drv_EEPROM.h"

extern lfs_t main_storage;

int storage_init (TMTDDevice *dev);

#endif
