#ifndef __LOG_H__
#define __LOG_H__

#include <stdio.h>

typedef enum {
    CHIP8_LOG_INFO,
    CHIP8_LOG_DEBUG,
    CHIP8_LOG_ERROR,
    CHIP8_LOG_COUNT
}log_type_t;

void init_log(log_type_t);
void deinit_log(void); 
void set_log_level(log_type_t);
extern void add_log(log_type_t, const char*, const char*, const char*, size_t);

#define log_info(msg)   add_log(CHIP8_LOG_INFO, msg, __FUNCTION__, __FILE__, __LINE__);
#define log_debug(msg)   add_log(CHIP8_LOG_DEBUG, msg, __FUNCTION__, __FILE__, __LINE__);

#endif //__LOG_H__