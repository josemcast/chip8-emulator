#ifndef __UTILS_H__
#define __UTILS_H__

#define BIG_END16U(value)       (((value >> 8) & 0xFF) | ((value & 0xFF) << 8))

#define ARRAY_LEN(arr, type)    (sizeof(arr) / sizeof(type))   

#endif //__UTILS_H__