#include <log.h>

static log_type_t current_log_level = CHIP8_LOG_INFO;
static char log_to_file = 1;
static FILE *fp = NULL;

void init_log(log_type_t ll) {
    
    if (fp != NULL)
        return;

    fp = fopen("chip8_run.log", "w");
    if (NULL == fp){
        perror("Could not open file for logging...fallbacking to stdout");
        log_to_file = 0;
    }

    set_log_level(ll);
}

void deinit_log() {
    if (fp != NULL)
        fclose(fp);
}

void set_log_level(log_type_t ll) {
    current_log_level = ll;
}

void add_log(log_type_t lt, const char * msg, const char* func, const char *file, size_t line) {
    
    if (fp == NULL)
        return;
    
    if (log_to_file){
        if (lt > current_log_level)
            return;
        char buffer[256];
        int bw = sprintf(buffer, "[%s:%lu]->%s: %s\n", file, line, func, msg);
        fwrite(buffer, sizeof(char), bw, fp);
    }else {
        if (lt <= current_log_level)
            printf("[%s:%lu]->%s: %s\n", file, line, func, msg);
    }
}