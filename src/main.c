#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>


#include <log.h>


#ifdef MAJOR_REV
uint8_t major = MAJOR_REV;
#else
uint8_t major = 0;
#endif

#ifdef MINOR_REV
uint8_t minor = MINOR_REV;
#else
uint8_t minor = 3;
#endif

#define BIN_BUFFER_SIZE         256

#define RL_SCREEN_HEIGHT        128
#define RL_SCREEN_WIDTH         256

#include <raylib.h>
#include <chip8.h>
#include <display.h>
#include <utilities.h>

int main(int argc, char *argv[]) {
    printf("chip8 Emulator v%d.%d\n\n", major, minor);

    InitWindow(RL_SCREEN_WIDTH, RL_SCREEN_HEIGHT, "CHIP8-Emulator");
    SetTargetFPS(60);

    uint8_t buffer[BIN_BUFFER_SIZE];
    FILE *fp = fopen("testbin/ibm_logo.ch8", "rb");
    if (fp == NULL)
        return 1;
    
    size_t bytes = fread(buffer, sizeof(uint8_t), BIN_BUFFER_SIZE, fp);
    fclose(fp);

    chip8_init(false, CHIP8_LOG_INFO, buffer, bytes);
    
    //chip8_dump_memory();
    //printf("Result: %d\n", chip8_run(bytecodes));

    chip8_display_t  *display;

    // chip8_vm_t *state;
    while(!WindowShouldClose()){
        //state = chip8_get_state();
        chip8_step();
        display = get_screen();  
        BeginDrawing();
        ClearBackground(RAYWHITE);
        for(int i = 0; i<CHIP8_SCREEN_HEIGHT; ++i){
            for(int j = 0; j < CHIP8_SCREEN_WIDTH; ++j){
                if(display->matrix[i][j] == 1){ 
                    //scale 4X: from 64 x 32  to 256 x 128
                    uint32_t col = 4*j;
                    uint32_t row = 4*i;
                    for(int dy = 0; dy < 4; ++dy){
                        uint32_t dy_row = row + dy;
                        DrawPixel(col, dy_row, BLACK);
                        DrawPixel(col + 1, dy_row, BLACK);
                        DrawPixel(col + 2, dy_row, BLACK);
                        DrawPixel(col + 3, dy_row, BLACK);    
                    }
                }
            }
        }   
    EndDrawing();
    }
    return 0;
}