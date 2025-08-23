#ifndef __CHIP8_KEYBOARD_H__
#define __CHIP8_KEYBOARD_H__

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    CHIP8_KEYCODE_0,
    CHIP8_KEYCODE_1,
    CHIP8_KEYCODE_2,
    CHIP8_KEYCODE_3,
    CHIP8_KEYCODE_4,
    CHIP8_KEYCODE_5,
    CHIP8_KEYCODE_6,
    CHIP8_KEYCODE_7,
    CHIP8_KEYCODE_8,
    CHIP8_KEYCODE_9,
    CHIP8_KEYCODE_A,
    CHIP8_KEYCODE_B,
    CHIP8_KEYCODE_C,
    CHIP8_KEYCODE_D,
    CHIP8_KEYCODE_E,
    CHIP8_KEYCODE_F,
    CHIP8_KEYCODE_COUNT
}chip8_keyboard_key_t;

//receive a keycode representing one of the keys in the chip8 hex keyboard and returns
// if the key is pressed
typedef bool (*chip8_keyboard_handler)(chip8_keyboard_key_t);

//since we are out of the update loop of our graphics lib, we need a way to update the input events
// independent of the end of the frame
typedef void (*chip8_keyboard_poll)(void);

void init_keyboard(chip8_keyboard_handler, chip8_keyboard_poll);
void poll_keyboard(void);
void set_key_pressed(chip8_keyboard_key_t);
bool is_keycode_pressed(chip8_keyboard_key_t);
#endif // __CHIP8_KEYBOARD_H__