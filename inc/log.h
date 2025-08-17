#ifndef __CHIP8_LOG_H__
#define __CHIP8_LOG_H__

#include <stdio.h>

#define LOG_BUFFER_MSG_SIZE 256

typedef enum {
    CHIP8_LOG_ERROR,
    CHIP8_LOG_INFO,
    CHIP8_LOG_DEBUG,
    CHIP8_LOG_COUNT
}chip8_logtype_t;

void init_log(bool, chip8_logtype_t, const char*);
void deinit_log(void); 
void set_log_level(chip8_logtype_t);
extern void add_log(chip8_logtype_t
, const char*, ...);

#define CHIP8_TRACELOG(type, msg, ...)       add_log(type, msg, ##__VA_ARGS__);

#endif //__CHIP8_LOG_H__