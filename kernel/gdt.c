#include <stdint.h>
#include "gdt.h"
#include "kernel.h"

// Array to store the GDT entries (3 entries * 8 bytes each = 24 bytes)
uint8_t *gdt_table = (uint8_t*)0x00000800;

extern void setGdt(uint32_t limit, uint32_t base);
extern void reloadSegments(void);

void encode_gdt_entry(uint8_t *target, struct GDT source)
{
    // Check the limit to make sure that it can be encoded
    if (source.limit > 0xFFFFF) { kerror("GDT cannot encode limits larger than 0xFFFFF"); }
    
    // Encode the limit
    target[0] = source.limit & 0xFF;
    target[1] = (source.limit >> 8) & 0xFF;
    target[6] = (source.limit >> 16) & 0x0F;
    
    // Encode the base
    target[2] = source.base & 0xFF;
    target[3] = (source.base >> 8) & 0xFF;
    target[4] = (source.base >> 16) & 0xFF;
    target[7] = (source.base >> 24) & 0xFF;
    
    // Encode the access byte
    target[5] = source.access_byte;
    
    // Encode the flags
    target[6] |= (source.flags << 4);
}

void init_gdt_table()
{
	// Null Descriptor
	encode_gdt_entry((uint8_t*)&gdt_table[0], (struct GDT){0, 0, 0, 0});
	
	// Kernel Mode Code Segment
	encode_gdt_entry((uint8_t*)&gdt_table[8], (struct GDT){
		.base = 0,
		.limit = 0xFFFFF,
		.access_byte = 0x9A,
		.flags = 0xC
	});
	
	// Kernel Mode Data Segment
	encode_gdt_entry((uint8_t*)&gdt_table[16], (struct GDT){
		.base = 0,
		.limit = 0xFFFFF,
		.access_byte = 0x92,
		.flags = 0xC
	});

	// User Mode Code Segment
	encode_gdt_entry((uint8_t*)&gdt_table[24], (struct GDT){
		.base = 0,
		.limit = 0xFFFFF,
		.access_byte = 0xFA,
		.flags = 0xC
	});

	// User Mode Data Segment
	encode_gdt_entry((uint8_t*)&gdt_table[32], (struct GDT){
		.base = 0,
		.limit = 0xFFFFF,
		.access_byte = 0xF2,
		.flags = 0xC
	});

	// User Mode Stack Segment
	encode_gdt_entry((uint8_t*)&gdt_table[40], (struct GDT){
		.base = 0,
		.limit = 0xFFFFF,
		.access_byte = 0xF2,
		.flags = 0xC
	});

	setGdt(7 * 8 - 1, (uint32_t)gdt_table);

	reloadSegments();
}