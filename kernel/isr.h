#ifndef ISR_H
#define ISR_H

#include <stdint.h>

// Structure describing the stack frame passed by isr_common_stub
struct registers {
   uint32_t ds;                                     // Data segment selector
   uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha
   uint32_t int_no, err_code;                       // Interrupt number and error code
   uint32_t eip, cs, eflags, useresp, ss;           // Pushed automatically by the processor
};
typedef struct registers registers_t;

void isr_handler(registers_t *regs);

#endif
