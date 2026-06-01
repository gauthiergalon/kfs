#include "keyboard.h"
#include "io.h"
#include "terminal.h"

/* Minimal US keyboard scancode set 1 map */
unsigned char keyboard_map[128] =
{
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8',
  '9', '0', '-', '=', '\b',
  '\t',
  'q', 'w', 'e', 'r',
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',
 '\'', '`',   0,
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',
  'm', ',', '.', '/',   0,
  '*',
    0,
  ' ',
    0,
    0, 0, 0, 0, 0, 0, 0, 0,
    0,
    0,
    0,
    0,
    0,
  '-',
    0,
    0,
    0,
  '+',
    0,
    0,
    0,
    0,
    0,
    0, 0, 0,
    0,
    0,
};

static uint8_t alt_pressed = 0;
static uint8_t escape_next = 0;

void keyboard_handler(void)
{
    unsigned char scancode = inb(0x60);

    if (scancode == 0xE0) {
        escape_next = 1;
        return;
    }

    if (scancode == 0x38 && !escape_next) {
        alt_pressed = 1;
        escape_next = 0;
        return;
    }
    if (scancode == 0xB8) {
        alt_pressed = 0;
        escape_next = 0;
        return;
    }

    if (alt_pressed && (scancode >= 0x3B && scancode <= 0x40)) {
        int screen_num = scancode - 0x3B;
        terminal_switch_screen(screen_num);
        escape_next = 0;
        return;
    }

    if (scancode == 0x48 || scancode == 0x50 || scancode == 0x4B || scancode == 0x4D) {
        if (scancode == 0x48)
            terminal_move_cursor_up();
        else if (scancode == 0x50)
            terminal_move_cursor_down();
        else if (scancode == 0x4B)
            terminal_move_cursor_left();
        else if (scancode == 0x4D)
            terminal_move_cursor_right();
        escape_next = 0;
        return;
    }

    if (!(scancode & 0x80) && !escape_next) {
        char c = keyboard_map[scancode];
        if (c != 0) {
            terminal_putchar(c);
        }
    }
    escape_next = 0;
}
