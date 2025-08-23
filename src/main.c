#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <log.h>

#define BIN_BUFFER_SIZE         256

#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <chip8.h>
#include <display.h>
#include <keyboard.h>
#include <utilities.h>


static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;

#define WINDOW_WIDTH 256
#define WINDOW_HEIGHT 128

static SDL_Scancode keyboard_map[CHIP8_KEYCODE_COUNT] = {
    SDL_SCANCODE_1,
    SDL_SCANCODE_2,
    SDL_SCANCODE_3,
    SDL_SCANCODE_4,
    SDL_SCANCODE_Q,
    SDL_SCANCODE_W,
    SDL_SCANCODE_E,
    SDL_SCANCODE_R,
    SDL_SCANCODE_A,
    SDL_SCANCODE_S,
    SDL_SCANCODE_D,
    SDL_SCANCODE_F,
    SDL_SCANCODE_Z,
    SDL_SCANCODE_X,
    SDL_SCANCODE_C,
    SDL_SCANCODE_V,
};

bool keyboard_handler(chip8_keyboard_key_t key)
{
    return SDL_GetKeyboardState(NULL)[keyboard_map[key]];
}

void display_handler(uint8_t disp[CHIP8_DISPLAY_HEIGHT][CHIP8_DISPLAY_WIDTH])
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);  /* black, full alpha */
    SDL_RenderClear(renderer);  /* start with a blank canvas. */
    SDL_SetRenderDrawColor(renderer, 0, 117, 44, SDL_ALPHA_OPAQUE);
    
    const uint8_t scale_factor = 4; //scale 4X: from 64 x 32  to 256 x 128
    for(int i = 0; i<CHIP8_DISPLAY_HEIGHT; ++i){
        for(int j = 0; j < CHIP8_DISPLAY_WIDTH; ++j){
            if (disp[i][j] == 0)
                continue;
            float col = scale_factor*j + (WINDOW_WIDTH / 2) - scale_factor*(CHIP8_DISPLAY_WIDTH / 2);
            float row = scale_factor*i;
            for(int dy = 0; dy < scale_factor; ++dy){
                uint32_t dy_row = row + dy;
                SDL_RenderPoint(renderer, col, dy_row);
                SDL_RenderPoint(renderer, col + 1, dy_row);
                SDL_RenderPoint(renderer, col + 2, dy_row);
                SDL_RenderPoint(renderer, col + 3, dy_row);
            }
        }
    }

    SDL_RenderPresent(renderer);
}

uint32_t timer_60hz_callback(void *ud, SDL_TimerID id, uint32_t interval) {
    CHIP8_TRACELOG(CHIP8_LOG_DEBUG, "INTERVAL: %d\n", interval);
    chip8_clock_60hz();
    return TIME_60HZ_MS;
}

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    bool debug_mode = false;
    if (argc > 1)   {
        if (strcmp("-d", argv[1]) == 0)
            debug_mode = true;
    }

    SDL_SetAppMetadata("CHIP-8 Emulator", "1.0", "Emulator");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("chip8", WINDOW_WIDTH, WINDOW_HEIGHT, 0, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

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
        .keyboard_poll = SDL_PumpEvents, //we need a handler to update input before polling for keyboard presses
        .log_enable = true,
        .log_type = debug_mode ? CHIP8_LOG_DEBUG:CHIP8_LOG_INFO,
        .log_filename = NULL,
    };

    chip8_init(&cfg);
    SDL_AddTimer(TIME_60HZ_MS, timer_60hz_callback, NULL);
    // if(debug_mode)
    //     chip8_dump_memory();

    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }
    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    /* let's move all our points a little for a new frame. */
    /* as you can see from this, rendering draws over whatever was drawn before it. */
    chip8_step();
    
    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    /* SDL will clean up the window/renderer for us. */
}