#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdlib.h>

#define BIG_END16U(value)       (((value >> 8) & 0xFF) | ((value & 0xFF) << 8))

#define ARRAY_LEN(arr, type)    (sizeof(arr) / sizeof(type))   

#define RANDI()                 (rand() % (UINT8_MAX + 1))

#endif //__UTILS_H__