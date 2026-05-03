
#ifndef __GLOBAL_H__
#define __GLOBAL_H__
//typedef unsigned long long uint64_t;
//typedef unsigned int uint32_t;
//typedef unsigned short uint16_t;
//typedef unsigned char uint8_t;

//typedef long long int64_t;
//typedef int int32_t;
//typedef short int16_t;
//typedef char int8_t;
#include "gd32f4xx.h"

#define _StartBootLoaderAddress (FLASH_BASE)
#define _SizeBootLoader (1*128*1024)
#define _SizeApplication (15*128*1024)
#define _StartApplicationAddress (_StartBootLoaderAddress + _SizeBootLoader)

#define EnablePrivilegedMode() __asm("SVC #0")

#define _Max_u64 0xFFFFFFFFFFFFFFFFLL
#define _Max_u32 0xFFFFFFFFL

typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;
typedef unsigned int uint;
typedef unsigned long long u64;
typedef unsigned int u32;
typedef unsigned short u16;
typedef unsigned char u8;

#define likely(x)    __builtin_expect(!!(x), 1)
#define unlikely(x)  __builtin_expect(!!(x), 0)
#define barrier()    __asm__ __volatile__("": : :"memory")

// The expression ARRAY_SIZE(a) is a compile-time constant of type
// size_t which represents the number of elements of the given
// array. You should only use ARRAY_SIZE on statically allocated
// arrays.
#define ARRAY_SIZE(a)  (sizeof(a) / sizeof(*(a)))

static inline int min (int a, int b)
{
    return (a < b ? a : b);
}

static inline int max (int a, int b)
{
    return (a > b ? a : b);
}

//#define GLOBAL_CLOG_LEVEL (0)

#endif // __GLOBAL_H__
