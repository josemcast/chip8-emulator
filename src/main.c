#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <log.h>

#define BIN_BUFFER_SIZE         256

#define RL_SCREEN_HEIGHT        128
#define RL_SCREEN_WIDTH         256

#include "raylib.h"
#include <chip8.h>
#include <display.h>
#include <keyboard.h>
#include <utilities.h>

static KeyboardKey keyboard_map[CHIP8_KEYCODE_COUNT] = {
    KEY_ONE,
    KEY_TWO,
    KEY_THREE,
    KEY_FOUR,
    KEY_Q,
    KEY_W,
    KEY_E,
    KEY_R,
    KEY_A,
    KEY_S,
    KEY_D,
    KEY_F,
    KEY_Z,
    KEY_X,
    KEY_C,
    KEY_V,
};

bool keyboard_handler(chip8_keyboard_key_t key)
{
    return IsKeyPressed(keyboard_map[key]);
}

void display_handler(uint8_t disp[CHIP8_DISPLAY_HEIGHT][CHIP8_DISPLAY_WIDTH])
{
    ClearBackground(RAYWHITE);
    const uint8_t scale_factor = 4; //scale 4X: from 64 x 32  to 256 x 128
    for(int i = 0; i<CHIP8_DISPLAY_HEIGHT; ++i){
        for(int j = 0; j < CHIP8_DISPLAY_WIDTH; ++j){
            Color pixel_color = disp[i][j] == 1 ? DARKGREEN:BLACK;
            uint32_t col = scale_factor*j + (RL_SCREEN_WIDTH / 2) - scale_factor*(CHIP8_DISPLAY_WIDTH / 2);
            uint32_t row = scale_factor*i;
            for(int dy = 0; dy < scale_factor; ++dy){
                uint32_t dy_row = row + dy;
                DrawPixel(col, dy_row, pixel_color);
                DrawPixel(col + 1, dy_row, pixel_color);
                DrawPixel(col + 2, dy_row, pixel_color);
                DrawPixel(col + 3, dy_row, pixel_color);    
            }
        }
    }
}

int main(int argc, char *argv[]) {

    bool debug_mode = false;
    if (argc > 1)   {
        if (TextIsEqual("-d", argv[1]))
            debug_mode = true;
    }

    InitWindow(RL_SCREEN_WIDTH, RL_SCREEN_HEIGHT, "CHIP-8 Emulator");
    SetTargetFPS(60);

    //Load binary from testbin folder - default IMB logo
    uint8_t buffer[BIN_BUFFER_SIZE];
    FILE *fp = NULL;
    if(debug_mode)
        fp = fopen("testbin/debug.ch8", "rb");
    else    
        fp = fopen("testbin/ibm_logo.ch8", "rb");
    
    if (fp == NULL) {return 1;}
    
    size_t bytes = fread(buffer, sizeof(uint8_t), BIN_BUFFER_SIZE, fp);
    fclose(fp);

    chip8_config_t cfg = {
        .rom = buffer,
        .rom_size = bytes,
        .display_handler = display_handler,
        .keyboard_handler = keyboard_handler,
        .keyboard_poll = PollInputEvents,
        .log_enable = true,
        .log_type = debug_mode ? CHIP8_LOG_DEBUG:CHIP8_LOG_INFO,
        .log_filename = NULL,
    };

    chip8_init(&cfg);

    while(!WindowShouldClose()){
        BeginDrawing();
            chip8_step();
        EndDrawing();
    }
    return 0;
}