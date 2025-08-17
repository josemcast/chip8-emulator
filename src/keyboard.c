#include <keyboard.h>

static chip8_keyboard_handler kb_handler;

void init_keyboard(chip8_keyboard_handler keyboard_handler){
    kb_handler = keyboard_handler;
}

bool is_keycode_pressed(chip8_keyboard_key_t keycode){
    return kb_handler(keycode);
}