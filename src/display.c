#include <stdio.h>
#include <stdint.h>

#include <display.h>

static chip8_display_t screen;

void init_screen(void){
    for(int i = 0; i < CHIP8_SCREEN_HEIGHT; ++i){
        for(int j = 0; j < CHIP8_SCREEN_WIDTH; ++j){
            screen.matrix[i][j] = 0;
        }
    }
}

void clear_screen(void){
    init_screen();
}

uint8_t set_screen(uint8_t vx, uint8_t vy, uint8_t byte){
    vx = (vx % CHIP8_SCREEN_WIDTH);
    vy = (vy % CHIP8_SCREEN_HEIGHT);
    uint8_t collision = 0;

    for(int i = 7; ((i >= 0) && (vx < CHIP8_SCREEN_WIDTH)); --i)
    {
        if((byte >> i) & 0x1){
            if(screen.matrix[vy][vx] == 1){
                screen.matrix[vy][vx] = 0;
                collision = 1;
            }else {
                screen.matrix[vy][vx] = 1;
            }
        }
        vx++;
    }

    return collision;
}

chip8_display_t *get_screen(void)
{
    return &screen;
}