.section    .text
.globl _start
_start:
    /* Load new GDT table */
    lgdt gdtPtr(%rip)     /* gdtPtr is in descriptors.c  */
    mov $0x10, %ax        /* Selector(SEG_KERNEL_RWDATA) */
    mov %ax, %ds 
    mov %ax, %es 
    mov %ax, %fs 
    mov %ax, %gs 
    lea 1f(%rip), %rax 
    pushq $0x08           /* Selector(SEG_KERNEL_CODE)  */
    pushq %rax 
    lretq 
1:
    mov $0xffff800000080000, %rsp

    jmp main