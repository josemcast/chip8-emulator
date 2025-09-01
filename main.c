#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <log.h>

#define BIN_BUFFER_SIZE         0xFFF

#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include <chip8.h>
#include <display.h>
#include <keyboard.h>
#include <utilities.h>


static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
bool keyboardStates[SDL_SCANCODE_COUNT] = {0};

#define WINDOW_WIDTH 256
#define WINDOW_HEIGHT 128

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
            set_key_pressed(CHIP8_KEYCODE_0)    ;
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
}

uint32_t timer_60hz_callback(void *ud, SDL_TimerID id, uint32_t interval) {
    chip8_clock_60hz();
    return TIME_60HZ_MS;
}

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    bool debug_mode = false;
    bool script_mode = false;
    if (argc < 2) {
        fprintf(stderr, "Usage: chip8 [-s] [rom.bin]\n");
        exit(1);
    }
    
    if (strcmp("-s", argv[1]) == 0) {
            script_mode = true;
    } 

    SDL_SetAppMetadata("CHIP-8 Emulator", "1.0", "Emulator");

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("chip8", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    const char *rom = NULL;

    if(script_mode) {
        lua_State *L = luaL_newstate();
        luaL_openlibs(L);

        if(luaL_dofile(L, "config.lua") != LUA_OK) {
            fprintf(stderr,"Could not load config file: %s\n", lua_tostring(L, -1));
            lua_close(L);
            exit(1);
        }

        if(!lua_istable(L, -1)){
            fprintf(stderr, "[config.lua] return value is not a table\n");
            lua_close(L);
            exit(1);
        }

        lua_pushstring(L, "rom_filename");
        lua_gettable(L, -2);
    
        if (lua_isstring(L, -1) & !lua_isnumber(L, -1))
            rom = lua_tostring(L, -1);    
        
        if (rom[0]== '\0'){
            fprintf(stderr, "[config.lua] rom filename not valid\n");
            lua_close(L);
            exit(1);
        }

        lua_pop(L, 1);
        
        lua_pushstring(L, "debug_mode");
        lua_gettable(L, -2);
        debug_mode = lua_isboolean(L, -1) ? lua_toboolean(L, -1): false;

        lua_close(L);
    } else {
        rom = argv[1];
        debug_mode = false;
    }

    uint8_t buffer[BIN_BUFFER_SIZE];
    FILE *fp = NULL;
    fp = fopen(rom, "rb");
    
    if (fp == NULL) {
        perror("Could not open binary");
        exit(1);
    }
    
    size_t bytes = fread(buffer, sizeof(uint8_t), BIN_BUFFER_SIZE, fp);
    fclose(fp);

    chip8_config_t cfg = {
        .rom = buffer,
        .rom_size = bytes,
        .display_handler = display_handler,
        .keyboard_handler = NULL,
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
    }else if (event->type == SDL_EVENT_KEY_UP){
        keyboard_handler(event->key.scancode);
    }
    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);  /* black, full alpha */
    SDL_RenderClear(renderer);  /* start with a blank canvas. */
    SDL_SetRenderDrawColor(renderer, 0, 117, 44, SDL_ALPHA_OPAQUE);
    
    SDL_Delay(1);
    chip8_step();
    
    SDL_RenderPresent(renderer);
    
    return SDL_APP_CONTINUE;  /* carry on with the program! */
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    /* SDL will clean up the window/renderer for us. */
}