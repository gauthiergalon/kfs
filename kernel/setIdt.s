.section .data
idtr:
    .word 0
    .long 0

.section .text
.global setIdt

setIdt:
    mov 4(%esp), %eax
    mov %ax, idtr
    mov 8(%esp), %eax
    mov %eax, idtr+2
    lidt idtr
    ret
