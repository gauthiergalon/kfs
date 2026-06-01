#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>

void keyboard_handler(void);
void keyboard_set_alt_state(uint8_t pressed);

#endif
