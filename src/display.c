#include <stdio.h>
#include <stdint.h>

#include <display.h>

static chip8_display_t display;

void init_display(void){
    for(int i = 0; i < CHIP8_DISPLAY_HEIGHT; ++i){
        for(int j = 0; j < CHIP8_DISPLAY_WIDTH; ++j){
            display.matrix[i][j] = 0;
        }
    }
}

void clear_display(void){
    init_display();
}

uint8_t set_display(uint8_t vx, uint8_t vy, uint8_t byte){
    vx = (vx % CHIP8_DISPLAY_WIDTH);
    vy = (vy % CHIP8_DISPLAY_HEIGHT);
    uint8_t collision = 0;

    for(int i = 7; ((i >= 0) && (vx < CHIP8_DISPLAY_WIDTH)); --i)
    {
        if((byte >> i) & 0x1){
            if(display.matrix[vy][vx] == 1){
                display.matrix[vy][vx] = 0;
                collision = 1;
            }else {
                display.matrix[vy][vx] = 1;
            }
        }
        vx++;
    }

    return collision;
}

const chip8_display_t *get_display(void)
{
    return &display;
}