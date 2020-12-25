#define ENTRY(name)   \
.global name;         \
name##:

#define IDT_ENTRY_WITH_ERRCODE(vectorID, gateName)  \
ENTRY(gateName)                                     \
    pushq $##vectorID;                              \
    jmp push_all;

#define IDT_ENTRY(vectorID, gateName)    \
ENTRY(gateName)                          \
    pushq $0;                            \
    pushq $##vectorID;                   \
    jmp push_all;

IDT_ENTRY             (0 , idt_ent_0 )
IDT_ENTRY             (1 , idt_ent_1 )
IDT_ENTRY             (2 , idt_ent_2 )
IDT_ENTRY             (3 , idt_ent_3 )
IDT_ENTRY             (4 , idt_ent_4 )
IDT_ENTRY             (5 , idt_ent_5 )
IDT_ENTRY             (6 , idt_ent_6 )
IDT_ENTRY             (7 , idt_ent_7 )
IDT_ENTRY_WITH_ERRCODE(8 , idt_ent_8 )
IDT_ENTRY             (9 , idt_ent_9 )
IDT_ENTRY_WITH_ERRCODE(10, idt_ent_10)
IDT_ENTRY_WITH_ERRCODE(11, idt_ent_11)
IDT_ENTRY_WITH_ERRCODE(12, idt_ent_12)
IDT_ENTRY_WITH_ERRCODE(13, idt_ent_13)
IDT_ENTRY_WITH_ERRCODE(14, idt_ent_14)
/* IDT_ENTRY          (15, idt_ent_15)(intel reserved. do not use.) */
IDT_ENTRY             (16, idt_ent_16)
IDT_ENTRY_WITH_ERRCODE(17, idt_ent_17)
IDT_ENTRY             (18, idt_ent_18)
IDT_ENTRY             (19, idt_ent_19)
IDT_ENTRY             (20, idt_ent_20)
IDT_ENTRY_WITH_ERRCODE(21, idt_ent_21)


/* 0x00 - 0x80 : rax, rbx, rcx, rdx, rsi, rdi, rbp, r8, r9, r10, r11, r12, r13, r14, r15 */
ERR_CODE = 0x88
VEC_ID   = 0x90
RIP      = 0x98
CS       = 0xa0
RFLAGS   = 0xa8
OLDRSP   = 0xb0
OLDSS    = 0xb8

push_all:
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
    mov %ds, %rcx
    push %rcx
    mov %es, %rcx
    push %rcx

    mov VEC_ID(%rsp), %rsi
    mov ERR_CODE(%rsp), %rdi
    mov RIP(%rsp), %rdx
    call handleIdtCall

pop_all:
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
    addq $0x10, %rsp
    iretq





ENTRY(ignore_int_and_halt)
    xor %rax, %rax
    lea ignore_int_and_halt_msg(%rip), %rdi
    call printk
1:
    hlt
    jmp 1b
ignore_int_and_halt_msg: 
    .asciz "An fault or interrupt has occured. System halt.\n"
