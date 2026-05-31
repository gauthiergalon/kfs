#include "isr.h"
#include "kernel.h"
#include "keyboard.h"
#include "pic.h"

/* Table of standard OSDev exception messages */
const char *exception_messages[32] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

void isr_handler(registers_t *regs)
{
    /* If the interrupt is an exception (0-31), it's a kernel panic */
    if (regs->int_no < 32) {
        kerror(exception_messages[regs->int_no]);
    }
    /* Hardware Interrupts (IRQ, 32+) */
    else {
        /* Keyboard (IRQ1) is mapped to interrupt 33 */
        if (regs->int_no == 33) {
            keyboard_handler();
        }

        /* Send End Of Interrupt signal to the PICs */
        /* (int_no - 32) to get the IRQ number (0-15) */
        pic_send_eoi(regs->int_no - 32);
    }
}
