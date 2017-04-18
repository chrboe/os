#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "util.h"
#include "console.h"
#include "errors.h"

#include <stdint.h>

#define KEY_LCTRL 0x1D
#define KEY_LALT 0x38

/* TODO: find better solution for multi part keys */
#define KEY_DELETE1 0xE0
#define KEY_DELETE2 0x53

uint8_t init_keyboard();
void keyboard_command(uint8_t command);
uint8_t scancode_to_ascii(uint8_t scancode);
void process_keyboard(uint8_t scancode);
uint8_t is_key_pressed(uint8_t scancode);

#endif /* !KEYBOARD_H */
