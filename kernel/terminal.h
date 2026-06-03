#ifndef TERMINAL_H
#define TERMINAL_H

#include <stddef.h>
#include <stdint.h>

/* VGA Text Mode Configuration */
#define VGA_WIDTH   80
#define VGA_HEIGHT  25
#define MAX_LINES   50
#define MAX_SCREENS 6

/* VGA Color Palette */
enum vga_color {
	VGA_COLOR_BLACK = 0,
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

/* Public API - Terminal Management */
void terminal_initialize(void);
void terminal_setcolor(uint8_t color);

/* Public API - Character and String Output */
void terminal_putchar(char c);
void terminal_writestring(const char* data);
void terminal_write(const char* data, size_t size);

/* Public API - Cursor Movement */
void terminal_move_cursor_left(void);
void terminal_move_cursor_right(void);
void terminal_move_cursor_up(void);
void terminal_move_cursor_down(void);

/* Public API - Screen Management */
void terminal_switch_screen(int screen_num);

/* Public API - VGA Color Functions */
uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg);

#endif
