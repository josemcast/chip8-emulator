#include <keyboard.h>

static chip8_keyboard_handler kb_handler;
static chip8_keyboard_poll kb_poll;

void init_keyboard(chip8_keyboard_handler keyboard_handler, chip8_keyboard_poll keyboard_poll){
    kb_handler = keyboard_handler;
    kb_poll = keyboard_poll;
}

void poll_keyboard(void) {
    kb_poll();
}

bool is_keycode_pressed(chip8_keyboard_key_t keycode){
    return kb_handler(keycode);
}