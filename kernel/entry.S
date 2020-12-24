#define ENTRY(name)   \
.global name;         \
name##:

#define IDT_ENTRY_WITH_ERRCODE(vectorID, gateName)  \
ENTRY(gateName)                                     \
    pushq $vectorID## ;                              \
    jmp push_all;

#define IDT_ENTRY(vectorID, gateName)    \
ENTRY(gateName)                          \
    pushq $0;                            \
    pushq $vectorID## ;                   \
    jmp push_all;

IDT_ENTRY(0 , idt_ent_divide_error                    )
IDT_ENTRY(1 , idt_ent_debug_exception                 )
IDT_ENTRY(2 , idt_ent_nmi                             )
IDT_ENTRY(3 , idt_ent_breakpoint                      )
IDT_ENTRY(4 , idt_ent_overflow                        )
IDT_ENTRY(5 , idt_ent_bound                           )
IDT_ENTRY(6 , idt_ent_invalid_opcode                  )
IDT_ENTRY(7 , idt_ent_device_not_available            )
IDT_ENTRY_WITH_ERRCODE(8 , idt_ent_double_fault       )
IDT_ENTRY(9 , idt_ent_coprocessor_segment_overrun     )
IDT_ENTRY_WITH_ERRCODE(10, idt_ent_invalid_tss        )
IDT_ENTRY_WITH_ERRCODE(11, idt_ent_segment_not_present)
IDT_ENTRY_WITH_ERRCODE(12, idt_ent_stack_segment_fault)
IDT_ENTRY_WITH_ERRCODE(13, idt_ent_general_protection )
IDT_ENTRY_WITH_ERRCODE(14, idt_ent_page_fault         )
/* IDT_ENTRY(15, (intel reserved. do not use.) */
IDT_ENTRY(16, idt_ent_x87_fpu_floating_point_error    )
IDT_ENTRY_WITH_ERRCODE(17, idt_ent_alignment_check    )
IDT_ENTRY(18, idt_ent_machine_check                   )
IDT_ENTRY(19, idt_ent_simd_floating_point_exception   )
IDT_ENTRY(20, idt_ent_virtualization_exception        )
IDT_ENTRY_WITH_ERRCODE(21, idt_ent_control_protection_exception)


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
