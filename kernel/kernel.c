#include <stddef.h>
#include <stdint.h>
#include "terminal.h"
#include "pic.h"
#include "gdt.h"
#include "idt.h"

/* Check if the compiler thinks you are targeting the wrong operating system. */
#if defined(__linux__)
#error "You are not using a cross-compiler, you will most certainly run into trouble"
#endif

/* This tutorial will only work for the 32-bit ix86 targets. */
#if !defined(__i386__)
#error "This tutorial needs to be compiled with a ix86-elf compiler"
#endif

void kerror(const char* msg)
{
	terminal_setcolor(vga_entry_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK));
	terminal_writestring("\nKERNEL PANIC: ");
	terminal_writestring(msg);

	asm volatile("cli");
	for (;;) {
		asm volatile("hlt");
	}
}

__attribute__((noreturn)) void kernel_main(void)
{
	/* Initialize terminal interface */
	terminal_initialize();

	/* Initialize Global Descriptor Table */
	init_gdt_table();

	/* Initialize Interrupt Descriptor Table */
	pic_remap(0x20, 0x28);
	init_idt_table();

	/* Enable hardware interrupts */
	asm volatile("sti");

	/* Display kernel ready message */
	terminal_writestring("42");

	/* Halt the CPU indefinitely */
	for (;;) {
		asm volatile("hlt");
	}
}
