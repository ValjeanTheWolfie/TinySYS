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

ignore_int_msg: 
    .asciz "An fault or interrupt was occured and ignored.\n"
.globl ignore_int
ignore_int:
    push %rax
    push %rbx
    push %rcx
    push %rdx
    push %rsi
    push %rdi
    push %rbp
    push %r8
    push %r9
    push %r10
    push %r11
    push %r12
    push %r13
    push %r14
    push %r15
    mov %ds, %rax
    push %rax
    mov %es, %rax
    push %rax

    xor %rax, %rax
    lea ignore_int_msg(%rip), %rdi
    call printk

1: jmp 1b

    pop %rax
    mov %rax, %es
    pop %rax
    mov %rax, %ds
    pop %r15
    pop %r14
    pop %r13
    pop %r12
    pop %r11
    pop %r10
    pop %r9
    pop %r8
    pop %rbp
    pop %rdi
    pop %rsi
    pop %rdx
    pop %rcx
    pop %rbx
    pop %rax
    iretq
