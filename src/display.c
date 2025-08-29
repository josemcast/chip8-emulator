#include <stdio.h>
#include <stdint.h>

#include <display.h>

static uint8_t matrix[CHIP8_DISPLAY_HEIGHT][CHIP8_DISPLAY_WIDTH];
static chip8_display_handler internal_handler;

void init_display(chip8_display_handler handler){
    internal_handler = handler;
    clear_display();
}

void clear_display(void){
    for(int i = 0; i < CHIP8_DISPLAY_HEIGHT; ++i){
        for(int j = 0; j < CHIP8_DISPLAY_WIDTH; ++j){
            matrix[i][j] = 0;
        }
    }
}

uint8_t set_display(uint8_t vx, uint8_t vy, uint8_t byte){
    vx = (vx % CHIP8_DISPLAY_WIDTH);
    vy = (vy % CHIP8_DISPLAY_HEIGHT);
    uint8_t collision = 0;

    for(int i = 7; ((i >= 0) && (vx < CHIP8_DISPLAY_WIDTH)); --i)
    {
        if((byte >> i) & 0x1){
            if(matrix[vy][vx] == 1){
                matrix[vy][vx] = 0;
                collision = 1;
            }else {
                matrix[vy][vx] = 1;
            }
        }
        vx++;
    }
    return collision;
}

void show_display(){
    internal_handler(matrix);
}