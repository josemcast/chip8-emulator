#include <stdarg.h>
#include <stdbool.h>
#include <log.h>


static log_type_t current_log_level = CHIP8_LOG_INFO;
static bool log_enabled = 0;
static char log_to_file = 0;
static FILE *fp = NULL;

void init_log(bool enable, log_type_t ll) {
    
    if (fp != NULL) {return;}

    set_log_level(ll);
    log_enabled = enable;
    if(log_enabled){
        if (log_to_file)
            fp = fopen("chip8_run.log", "w");
    
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

void set_log_level(log_type_t ll) {
    current_log_level = ll;
}

void add_log(log_type_t lt, const char * msg, ...) {
    
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