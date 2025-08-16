#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include <stdint.h>

#define CHIP8_DISPLAY_WIDTH    64
#define CHIP8_DISPLAY_HEIGHT   32

typedef struct{
    uint8_t matrix[CHIP8_DISPLAY_HEIGHT][CHIP8_DISPLAY_WIDTH];
}chip8_display_t;

void init_display(void);
void clear_display(void);
uint8_t set_display(uint8_t, uint8_t, uint8_t);
const chip8_display_t * get_display(void);

#endif //__DISPLAY_H__