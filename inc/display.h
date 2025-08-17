#ifndef __CHIP8_DISPLAY_H__
#define __CHIP8_DISPLAY_H__

#include <stdint.h>

#define CHIP8_DISPLAY_WIDTH    64
#define CHIP8_DISPLAY_HEIGHT   32

//Callback to graphics library used by show_display()
typedef void (*chip8_display_handler)(uint8_t [CHIP8_DISPLAY_HEIGHT][CHIP8_DISPLAY_WIDTH]);

void init_display(chip8_display_handler);
void clear_display(void);
uint8_t set_display(uint8_t, uint8_t, uint8_t);
void show_display(void);

#endif //__DISPLAY_H__