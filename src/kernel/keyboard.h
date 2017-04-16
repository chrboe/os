#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "util.h"
#include "console.h"
#include "errors.h"

#include <stdint.h>

uint8_t init_keyboard();
void keyboard_command(uint8_t command);
uint8_t scancode_to_ascii(uint8_t scancode);

#endif /* !KEYBOARD_H */
