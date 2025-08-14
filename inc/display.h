#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include <stdint.h>

#define CHIP8_SCREEN_WIDTH    64
#define CHIP8_SCREEN_HEIGHT   32

typedef struct{
    uint8_t matrix[CHIP8_SCREEN_HEIGHT][CHIP8_SCREEN_WIDTH];
}chip8_display_t;

void init_screen(void);
void clear_screen(void);
uint8_t set_screen(uint8_t, uint8_t, uint8_t);
chip8_display_t * get_screen(void);

#endif //__DISPLAY_H__