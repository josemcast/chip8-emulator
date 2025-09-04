#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <chip8.h>
#include <display.h>
#include <keyboard.h>
#include <script.h>
#include <log.h>

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

global_conf_t config;

void keyboard_handler(SDL_Scancode sc)
{
    switch (sc)
    {
        case SDL_SCANCODE_1:
            set_key_pressed(CHIP8_KEYCODE_1);
            break;
        case SDL_SCANCODE_2:
            set_key_pressed(CHIP8_KEYCODE_2);
            break;
        case SDL_SCANCODE_3:
            set_key_pressed(CHIP8_KEYCODE_3);
            break;
        case SDL_SCANCODE_4:
            set_key_pressed(CHIP8_KEYCODE_C);
            break;
        case SDL_SCANCODE_Q:
            set_key_pressed(CHIP8_KEYCODE_4);
            break;
        case SDL_SCANCODE_W:
            set_key_pressed(CHIP8_KEYCODE_5);
            break;
        case SDL_SCANCODE_E:
            set_key_pressed(CHIP8_KEYCODE_6);
            break;
        case SDL_SCANCODE_R:
            set_key_pressed(CHIP8_KEYCODE_D);
            break;
        case SDL_SCANCODE_A:
            set_key_pressed(CHIP8_KEYCODE_7);
            break;
        case SDL_SCANCODE_S:
            set_key_pressed(CHIP8_KEYCODE_8);
            break;
        case SDL_SCANCODE_D:
            set_key_pressed(CHIP8_KEYCODE_9);
            break;
        case SDL_SCANCODE_F:
            set_key_pressed(CHIP8_KEYCODE_E);
            break;
        case SDL_SCANCODE_Z:
            set_key_pressed(CHIP8_KEYCODE_A);
            break;
        case SDL_SCANCODE_X:
            set_key_pressed(CHIP8_KEYCODE_0);
            break;
        case SDL_SCANCODE_C:
            set_key_pressed(CHIP8_KEYCODE_B);
            break;
        case SDL_SCANCODE_V:
            set_key_pressed(CHIP8_KEYCODE_F);
            break;
        default:
            break;
    }
}

void display_handler(uint8_t disp[CHIP8_DISPLAY_HEIGHT][CHIP8_DISPLAY_WIDTH])
{
    for(int i = 0; i<CHIP8_DISPLAY_HEIGHT; ++i){
        for(int j = 0; j < CHIP8_DISPLAY_WIDTH; ++j){
            if (disp[i][j] == 0)
                continue;
            //center CHIP-8 display based on current window dimensions
            float col = config.display_scale_factor*j + (config.window_width / 2) - config.display_scale_factor*(CHIP8_DISPLAY_WIDTH / 2);
            float row = config.display_scale_factor*i;
            for(int dy = 0; dy < config.display_scale_factor; ++dy){
                uint32_t dy_row = row + dy;
                for(int dx = 0; dx < config.display_scale_factor; ++dx)
                    SDL_RenderPoint(renderer, col+dx, dy_row);
            }
        }
    }
}

// CHIP-8 Timers must be updates independently of CPU flow
uint32_t timer_60hz_callback(void *ud, SDL_TimerID id, uint32_t interval) {
    chip8_clock_60hz();
    return TIME_60HZ_MS;
}

int main(int argc, char *argv[]) {

    if (argc < 2) {
        fprintf(stderr, "Usage: chip8 [rom.bin | -s] [config_script] [run_script]\n");
        exit(1);
    }
    
    if (strcmp("-s", argv[1]) == 0) {
            if (argc < 4){
                fprintf(stderr, "missing arguments after -s option\n");
                fprintf(stderr, "Usage: chip8 [rom.bin | -s] [config_script] [run_script]\n");
                exit(1);
            }         
            config.script_mode = true;
    } 

    if(config.script_mode) {
       chip8_script_load_conf(argv[2], &config); 
    } else {
        memcpy((uint8_t *)config.rom_filename, argv[1], strlen(argv[1]));
        config.debug_mode = false;
        config.display_scale_factor = 4; 
        config.window_width = 256;
        config.window_height = 128;
    }

    //fallback to default dimensions if zero
    config.window_width = config.window_width == 0 ? 256 : config.window_width;
    config.window_height = config.window_height == 0 ? 128 : config.window_height;
    config.display_scale_factor = config.display_scale_factor == 0 ? 4 : config.display_scale_factor;
    
    SDL_SetAppMetadata("CHIP-8 Emulator", "1.0", "Emulator");

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("chip8", config.window_width, config.window_height, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    uint8_t buffer[CHIP8_MEMORY_BUFFER];
    FILE *fp = NULL;
    fp = fopen(config.rom_filename, "rb");
    
    if (fp == NULL) {
        perror("Could not open binary");
        exit(1);
    }
    
    size_t bytes = fread(buffer, sizeof(uint8_t), CHIP8_MEMORY_BUFFER, fp);
    fclose(fp);

    chip8_config_t cfg = {
        .rom = buffer,
        .rom_size = bytes,
        .display_handler = display_handler,
        .keyboard_poll = SDL_PumpEvents, //we need a handler to update input before polling for keyboard presses
        .log_enable = true,
        .log_type = config.debug_mode ? CHIP8_LOG_DEBUG:CHIP8_LOG_INFO,
        .log_filename = NULL,
    };

    chip8_init(&cfg);
    SDL_AddTimer(TIME_60HZ_MS, timer_60hz_callback, NULL);

    SDL_Event e;
    while(1) {

        if(SDL_PollEvent(&e) != 0) {
            if(e.type == SDL_EVENT_QUIT)
                break;
            else if(e.type == SDL_EVENT_KEY_UP)
                keyboard_handler(e.key.scancode);
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);  /* black, full alpha */
        SDL_RenderClear(renderer);  /* start with a blank canvas. */
        SDL_SetRenderDrawColor(renderer, 0, 117, 44, SDL_ALPHA_OPAQUE);
    
        SDL_Delay(1);
        chip8_step();
        if(config.script_mode)
            chip8_script_run(argv[3]);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}