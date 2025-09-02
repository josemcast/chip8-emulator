#ifndef __CHIP8_LOG_H__
#define __CHIP8_LOG_H__

#include <stdio.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

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
void add_log(chip8_logtype_t, const char*, ...);

#define CHIP8_TRACELOG(type, msg, ...)       add_log(type, msg, ##__VA_ARGS__);

#ifdef __cplusplus
}
#endif

#endif //__CHIP8_LOG_H__

#ifdef CHIP8_LOG_IMPLEMENTATION
#undef CHIP8_LOG_IMPLEMENTATION
#include <stdarg.h>
#include <stdbool.h>


static chip8_logtype_t current_log_level = CHIP8_LOG_INFO;
static bool log_enabled = 0;
static char log_to_file = 0;
static FILE *fp = NULL;

void init_log(bool enable, chip8_logtype_t ll, const char* filename) {
    
    if (fp != NULL) {return;}

    set_log_level(ll);
    log_enabled = enable;
    if(log_enabled){
        if (filename != NULL){
            fp = fopen(filename, "w");
            log_to_file = 1;
        }
    
        if (NULL == fp){
            //perror("Could not open file for logging...fallbacking to stdout");
            log_to_file = 0;
        }

    }
}

void deinit_log() {
    if (fp != NULL)
        fclose(fp);
}

void set_log_level(chip8_logtype_t ll) {
    current_log_level = ll;
}

void add_log(chip8_logtype_t lt, const char * msg, ...) {
    
    if ((log_enabled != true) || ((fp == NULL) && (log_to_file == 1)))
        return;

    va_list args;
    va_start(args, msg);

    if (log_to_file){
        if (lt > current_log_level){
            va_end(args);
            return;
        }
        char buffer[LOG_BUFFER_MSG_SIZE];
        int bw = vsnprintf(buffer, LOG_BUFFER_MSG_SIZE, msg, args);
        fwrite(buffer, sizeof(char), bw, fp);
    }else {
        if (lt <= current_log_level)
            vprintf(msg, args);
    }
    va_end(args);
}
#endif