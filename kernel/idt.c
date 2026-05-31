#include "idt.h"

// Declarations for all ISR stubs generated in assembly
extern void isr0(); extern void isr1(); extern void isr2(); extern void isr3();
extern void isr4(); extern void isr5(); extern void isr6(); extern void isr7();
extern void isr8(); extern void isr9(); extern void isr10(); extern void isr11();
extern void isr12(); extern void isr13(); extern void isr14(); extern void isr15();
extern void isr16(); extern void isr17(); extern void isr18(); extern void isr19();
extern void isr20(); extern void isr21(); extern void isr22(); extern void isr23();
extern void isr24(); extern void isr25(); extern void isr26(); extern void isr27();
extern void isr28(); extern void isr29(); extern void isr30(); extern void isr31();

// IDT with 256 entries
struct InterruptDescriptor32 idt[256];

extern void setIdt(uint32_t limit, uint32_t base);

void set_idt_descriptor(uint8_t vector, void *isr, uint8_t flags) 
{
    struct InterruptDescriptor32 *descriptor = &idt[vector];
    
    uint32_t offset = (uint32_t)isr;
    
    descriptor->offset_1 = offset & 0xFFFF;
    descriptor->selector = 0x08; // 0x08 corresponds to the kernel code segment in our GDT
    descriptor->zero = 0;
    descriptor->type_attributes = flags;
    descriptor->offset_2 = (offset >> 16) & 0xFFFF;
}

void init_idt_table(void) 
{
    // Bind the 32 CPU exceptions to our ISR stubs
    set_idt_descriptor(0, isr0, 0x8E);
    set_idt_descriptor(1, isr1, 0x8E);
    set_idt_descriptor(2, isr2, 0x8E);
    set_idt_descriptor(3, isr3, 0x8E);
    set_idt_descriptor(4, isr4, 0x8E);
    set_idt_descriptor(5, isr5, 0x8E);
    set_idt_descriptor(6, isr6, 0x8E);
    set_idt_descriptor(7, isr7, 0x8E);
    set_idt_descriptor(8, isr8, 0x8E);
    set_idt_descriptor(9, isr9, 0x8E);
    set_idt_descriptor(10, isr10, 0x8E);
    set_idt_descriptor(11, isr11, 0x8E);
    set_idt_descriptor(12, isr12, 0x8E);
    set_idt_descriptor(13, isr13, 0x8E);
    set_idt_descriptor(14, isr14, 0x8E);
    set_idt_descriptor(15, isr15, 0x8E);
    set_idt_descriptor(16, isr16, 0x8E);
    set_idt_descriptor(17, isr17, 0x8E);
    set_idt_descriptor(18, isr18, 0x8E);
    set_idt_descriptor(19, isr19, 0x8E);
    set_idt_descriptor(20, isr20, 0x8E);
    set_idt_descriptor(21, isr21, 0x8E);
    set_idt_descriptor(22, isr22, 0x8E);
    set_idt_descriptor(23, isr23, 0x8E);
    set_idt_descriptor(24, isr24, 0x8E);
    set_idt_descriptor(25, isr25, 0x8E);
    set_idt_descriptor(26, isr26, 0x8E);
    set_idt_descriptor(27, isr27, 0x8E);
    set_idt_descriptor(28, isr28, 0x8E);
    set_idt_descriptor(29, isr29, 0x8E);
    set_idt_descriptor(30, isr30, 0x8E);
    set_idt_descriptor(31, isr31, 0x8E);

    // Initialize hardware IRQs (offset by 32 thanks to the PIC)
    set_idt_descriptor(32, irq0, 0x8E); // Timer
    set_idt_descriptor(33, irq1, 0x8E); // Keyboard
    set_idt_descriptor(34, irq2, 0x8E); // Cascade
    set_idt_descriptor(35, irq3, 0x8E); // COM2
    set_idt_descriptor(36, irq4, 0x8E); // COM1
    set_idt_descriptor(37, irq5, 0x8E); // LPT2
    set_idt_descriptor(38, irq6, 0x8E); // Floppy disk
    set_idt_descriptor(39, irq7, 0x8E); // LPT1
    set_idt_descriptor(40, irq8, 0x8E); // CMOS real-time clock
    set_idt_descriptor(41, irq9, 0x8E); // Free
    set_idt_descriptor(42, irq10, 0x8E); // Free
    set_idt_descriptor(43, irq11, 0x8E); // Free
    set_idt_descriptor(44, irq12, 0x8E); // PS2 Mouse
    set_idt_descriptor(45, irq13, 0x8E); // FPU
    set_idt_descriptor(46, irq14, 0x8E); // Primary ATA Hard Disk
    set_idt_descriptor(47, irq15, 0x8E); // Secondary ATA Hard Disk
    
    // Load the IDT via assembly
    setIdt(sizeof(idt) - 1, (uint32_t)&idt);
}