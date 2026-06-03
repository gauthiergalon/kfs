#include <stddef.h>
#include <stdint.h>
#include "terminal.h"
#include "io.h"

#define VGA_MEMORY 0xB8000

/* Virtual Screen Structure */
typedef struct {
	uint16_t buffer[MAX_LINES][VGA_WIDTH];
	size_t cursor_x;
	size_t cursor_y;
	size_t scroll_offset;
	uint8_t color;
	size_t total_lines;
} terminal_screen;

/* Global state */
static terminal_screen screens[MAX_SCREENS];
static int current_screen = 0;
static uint16_t* terminal_buffer = (uint16_t*)VGA_MEMORY;

uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg)
{
	return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color)
{
	return (uint16_t)uc | (uint16_t)color << 8;
}

static void terminal_update_cursor(size_t x, size_t y)
{
	uint16_t pos = y * VGA_WIDTH + x;

	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t)(pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

static terminal_screen* terminal_active_screen(void)
{
	return &screens[current_screen];
}

static void terminal_sync_viewport(terminal_screen* screen)
{
	/* Adjust scroll offset to keep cursor visible */
	if (screen->cursor_y < screen->scroll_offset) {
		screen->scroll_offset = screen->cursor_y;
	} else if (screen->cursor_y >= screen->scroll_offset + VGA_HEIGHT) {
		screen->scroll_offset = screen->cursor_y - VGA_HEIGHT + 1;
	}

	/* Copy screen buffer to VGA memory */
	for (size_t row = 0; row < VGA_HEIGHT; row++) {
		size_t line = screen->scroll_offset + row;

		for (size_t col = 0; col < VGA_WIDTH; col++) {
			if (line < screen->total_lines) {
				terminal_buffer[row * VGA_WIDTH + col] = screen->buffer[line][col];
			} else {
				terminal_buffer[row * VGA_WIDTH + col] = vga_entry(' ', screen->color);
			}
		}
	}

	/* Update hardware cursor position */
	terminal_update_cursor(screen->cursor_x, screen->cursor_y - screen->scroll_offset);
}

static void terminal_finish_frame(terminal_screen* screen)
{
	terminal_sync_viewport(screen);
}

static void terminal_clear_line(terminal_screen* screen, size_t line)
{
	for (size_t col = 0; col < VGA_WIDTH; col++) {
		screen->buffer[line][col] = vga_entry(' ', screen->color);
	}
}

static void terminal_scroll_up(void)
{
	terminal_screen* screen = terminal_active_screen();

	/* Shift lines up if buffer is full, otherwise extend */
	if (screen->total_lines < MAX_LINES) {
		screen->total_lines++;
	} else {
		for (size_t line = 1; line < MAX_LINES; line++) {
			for (size_t col = 0; col < VGA_WIDTH; col++) {
				screen->buffer[line - 1][col] = screen->buffer[line][col];
			}
		}
		terminal_clear_line(screen, MAX_LINES - 1);
	}

	/* Move cursor down */
	if (screen->cursor_y + 1 < MAX_LINES) {
		screen->cursor_y++;
	}
	if (screen->cursor_y >= screen->total_lines) {
		screen->cursor_y = screen->total_lines - 1;
	}

	terminal_sync_viewport(screen);
}

static void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
	terminal_active_screen()->buffer[y][x] = vga_entry(c, color);
}

void terminal_putchar(char c)
{
	terminal_screen* screen = terminal_active_screen();
	size_t line;

	/* Handle backspace */
	if (c == '\b') {
		if (screen->cursor_x > 0) {
			screen->cursor_x--;
		} else if (screen->cursor_y > 0) {
			screen->cursor_y--;
			line = screen->cursor_y;
			/* Find end of previous line */
			for (size_t col = VGA_WIDTH; col > 0; col--) {
				if (screen->buffer[line][col - 1] != vga_entry(' ', screen->color)) {
					screen->cursor_x = col - 1;
					break;
				}
			}
		}
		screen->buffer[screen->cursor_y][screen->cursor_x] = vga_entry(' ', screen->color);
		return;
	}

	/* Handle newline */
	if (c == '\n') {
		screen->cursor_x = 0;
		if (screen->cursor_y + 1 >= screen->total_lines) {
			terminal_scroll_up();
		} else {
			screen->cursor_y++;
		}
		return;
	}

	/* Handle carriage return */
	if (c == '\r') {
		screen->cursor_x = 0;
		return;
	}

	/* Write character */
	if (screen->cursor_y >= MAX_LINES) {
		terminal_scroll_up();
	}
	if (screen->cursor_y >= screen->total_lines) {
		screen->total_lines = screen->cursor_y + 1;
	}

	terminal_putentryat(c, screen->color, screen->cursor_x, screen->cursor_y);

	/* Handle line wrapping */
	if (++screen->cursor_x == VGA_WIDTH) {
		screen->cursor_x = 0;
		screen->cursor_y++;
		if (screen->cursor_y >= MAX_LINES) {
			terminal_scroll_up();
		}
		if (screen->cursor_y >= screen->total_lines) {
			screen->total_lines = screen->cursor_y + 1;
		}
	}
}

void terminal_write(const char* data, size_t size)
{
	for (size_t i = 0; i < size; i++) {
		terminal_putchar(data[i]);
	}
	terminal_finish_frame(terminal_active_screen());
}

static size_t strlen(const char* str)
{
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

void terminal_writestring(const char* data)
{
	terminal_write(data, strlen(data));
}

void terminal_setcolor(uint8_t color)
{
	terminal_active_screen()->color = color;
}

void terminal_move_cursor_left(void)
{
	terminal_screen* screen = terminal_active_screen();

	if (screen->cursor_x > 0) {
		screen->cursor_x--;
	}
	else if (screen->cursor_x == 0 && screen->cursor_y > 0) {
		screen->cursor_y--;
		screen->cursor_x = VGA_WIDTH - 1;
	}

	terminal_sync_viewport(screen);
}

void terminal_move_cursor_right(void)
{
	terminal_screen* screen = terminal_active_screen();

	if (screen->cursor_x + 1 < VGA_WIDTH) {
		screen->cursor_x++;
	}
	else if (screen->cursor_x + 1 == VGA_WIDTH && screen->cursor_y + 1 < screen->total_lines) {
		screen->cursor_y++;
		screen->cursor_x = 0;
	}

	terminal_sync_viewport(screen);
}

void terminal_move_cursor_up(void)
{
	terminal_screen* screen = terminal_active_screen();

	if (screen->cursor_y > 0) {
		screen->cursor_y--;
	}
	terminal_sync_viewport(screen);
}

void terminal_move_cursor_down(void)
{
	terminal_screen* screen = terminal_active_screen();

	if (screen->cursor_y + 1 < screen->total_lines) {
		screen->cursor_y++;
	}
	terminal_sync_viewport(screen);
}

void terminal_switch_screen(int screen_num)
{
	if (screen_num < 0 || screen_num >= MAX_SCREENS) {
		return;
	}

	current_screen = screen_num;
	terminal_sync_viewport(terminal_active_screen());
}

void terminal_initialize(void)
{
	uint8_t default_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	enum vga_color text_colors[] = {
		VGA_COLOR_LIGHT_GREY,
		VGA_COLOR_LIGHT_BLUE,
		VGA_COLOR_LIGHT_GREEN,
		VGA_COLOR_LIGHT_CYAN,
		VGA_COLOR_LIGHT_RED,
		VGA_COLOR_LIGHT_MAGENTA
	};

	/* Clear VGA memory */
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			terminal_buffer[y * VGA_WIDTH + x] = vga_entry(' ', default_color);
		}
	}

	/* Initialize virtual screens */
	for (int i = 0; i < MAX_SCREENS; i++) {
		screens[i].cursor_x = 0;
		screens[i].cursor_y = 0;
		screens[i].scroll_offset = 0;
		screens[i].color = vga_entry_color(text_colors[i], VGA_COLOR_BLACK);
		screens[i].total_lines = 0;

		/* Fill buffer with empty spaces */
		for (size_t line = 0; line < MAX_LINES; line++) {
			terminal_clear_line(&screens[i], line);
		}
	}

	current_screen = 0;
	terminal_update_cursor(0, 0);
}
