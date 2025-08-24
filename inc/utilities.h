#ifndef __CHIP8_UTILS_H__
#define __CHIP8_UTILS_H__

#include <stdlib.h>


#define BIG_END16U(value)       (((value >> 8) & 0xFF) | ((value & 0xFF) << 8))

#define ARRAY_LEN(arr, type)    (sizeof(arr) / sizeof(type))   

#define RANDI()                 (rand() % (UINT8_MAX + 1))

#define TIME_60HZ_MS            ((1.0 / 60.0) * 1000.0) 
#endif //__CHIP8_UTILS_H__