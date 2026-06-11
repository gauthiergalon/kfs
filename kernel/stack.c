#include "../lib/printk.h"
#include <stdint.h>

static inline uint32_t get_esp(void)
{
    uint32_t esp;
    asm volatile("mov %%esp, %0" : "=r"(esp));
    return esp;
}

void print_stack(void)
{
    uint32_t *esp = (uint32_t *)get_esp();

    printk("ESP = 0x%x\n", (uint32_t)esp);

    for (int i = 0; i < 32; i++)
    {
        printk("[%d] 0x%x : 0x%x\n",
               i,
               (uint32_t)(esp + i),
               esp[i]);
    }
}