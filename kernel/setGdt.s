.section .data
gdtr:
    .word 0
    .long 0

.section .text
.global setGdt

setGdt:
    mov 4(%esp), %eax
    mov %ax, gdtr
    mov 8(%esp), %eax
    mov %eax, gdtr+2
    lgdt gdtr
    ret

