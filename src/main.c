#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include <log.h>

#define BIN_BUFFER_SIZE         256

#define RL_SCREEN_HEIGHT        128
#define RL_SCREEN_WIDTH         256

#include <raylib.h>
#include <chip8.h>
#include <display.h>
#include <utilities.h>

int main(int argc, char *argv[]) {

    bool debug_mode = 0;
    if (argc > 1)   {
        if (TextIsEqual("-c", argv[1]))
            debug_mode = 1;
    }

    if (!debug_mode){
       InitWindow(RL_SCREEN_WIDTH, RL_SCREEN_HEIGHT, "CHIP8-Emulator");
        SetTargetFPS(60);
    }

    //Load binary from testbin folder - default IMB logo
    uint8_t buffer[BIN_BUFFER_SIZE];
    FILE *fp = NULL;
    if(debug_mode)
        fp = fopen("testbin/debug.ch8", "rb");
    else    
        fp = fopen("testbin/ibm_logo.ch8", "rb");
    
    if (fp == NULL)
        return 1;
    
    size_t bytes = fread(buffer, sizeof(uint8_t), BIN_BUFFER_SIZE, fp);
    fclose(fp);

    //chip8_dump_memory();

    if(debug_mode){
        chip8_init(true, CHIP8_LOG_DEBUG, buffer, bytes);
        printf("Result: %d\n", chip8_run());
    }else {
        chip8_init(true, CHIP8_LOG_DEBUG, buffer, bytes);
        chip8_display_t  *display;
        while(!WindowShouldClose()){
            chip8_step();
            display = get_screen(); //Get current display state from emulator 
            
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
    }
    return 0;
}