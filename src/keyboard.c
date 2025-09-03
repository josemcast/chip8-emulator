#include <keyboard.h>
#include <string.h>

static chip8_keyboard_poll kb_poll;
static bool keyboard[CHIP8_KEYCODE_COUNT];

void init_keyboard(chip8_keyboard_poll keyboard_poll){
    memset(keyboard, 0, CHIP8_KEYCODE_COUNT);    
    kb_poll = keyboard_poll;
}

void poll_keyboard(void) {
    kb_poll();
}

void set_key_pressed(chip8_keyboard_key_t key) {
    keyboard[key] = true;
}

bool is_keycode_pressed(chip8_keyboard_key_t key){
    if(keyboard[key] == true) {
        keyboard[key] = false;
        return true;
    }

    return false;
}