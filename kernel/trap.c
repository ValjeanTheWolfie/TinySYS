#include "tables.h"
#include "trap.h"
#include "lib.h"
#include "printk.h"

void initTss()
{
    TssDescriptor* pTss = (TssDescriptor *) &gdtTable[SEG_TSS];

    pTss->tssDesc32 = TSS_DESC_64_LOW(&tssTable, 0xfffff, ACCESS_BYTE_TSS64, FLAGS_64);
    pTss->baseHigh = TSS_DESC_64_HIGH(&tssTable);

    tssTable.rsp0 = tssTable.rsp1 = tssTable.rsp2 = tssTable.ist1 = tssTable.ist2 = tssTable.ist3 = tssTable.ist4 = tssTable.ist5 = tssTable.ist6 = tssTable.ist7 = 0xffff800000080000;

    __asm__ __volatile__ ("ltr %%ax\n\t"::"a"(Selector(SEG_TSS)));
}


void initIdt()
{
    int i;
    void* idtEntry[] = {
        /*0 */ idt_ent_divide_error,
        /*1 */ idt_ent_debug_exception,
        /*2 */ idt_ent_nmi,
        /*3 */ idt_ent_breakpoint,
        /*4 */ idt_ent_overflow,
        /*5 */ idt_ent_bound,
        /*6 */ idt_ent_invalid_opcode,
        /*7 */ idt_ent_device_not_available,
        /*8 */ idt_ent_double_fault,
        /*9 */ idt_ent_coprocessor_segment_overrun,
        /*10*/ idt_ent_invalid_tss,
        /*11*/ idt_ent_segment_not_present,
        /*12*/ idt_ent_stack_segment_fault,
        /*13*/ idt_ent_general_protection,
        /*14*/ idt_ent_page_fault,
        /*15*/ NULL,    /* Intel reserved. Do not use */
        /*16*/ idt_ent_x87_fpu_floating_point_error,
        /*17*/ idt_ent_alignment_check,
        /*18*/ idt_ent_machine_check,
        /*19*/ idt_ent_simd_floating_point_exception,
        /*20*/ idt_ent_virtualization_exception,
        /*21*/ idt_ent_control_protection_exception,
        /*22*/ NULL,   /* 22 - 31: Intel reserved. Do not use */
        /*23*/ NULL,   
        /*24*/ NULL,   
        /*25*/ NULL,   
        /*26*/ NULL,   
        /*27*/ NULL,   
        /*28*/ NULL,   
        /*29*/ NULL,   
        /*30*/ NULL,   
        /*31*/ NULL,   
    };

    for(i = 0; i < 32; i++)
    {
        if(NULL != idtEntry[i])
        {
            idtTable[i].gate32 = GATE_DESC_64_LOW(Selector(SEG_KERNEL_CODE), idtEntry[i], ACCESS_BYTE_TRAP, 1);
            idtTable[i].offsetHigh = GATE_DESC_64_HIGH(idtEntry[i]);
        }
        else
        {
            idtTable[i].gate32 = idtTable[i].offsetHigh = 0;
        }
    }

    __asm__ __volatile__ ("lidt %0\n\t"::"m"(idtPtr));
}


void handleIdtCall(int vectorID, unsigned long errorCode, unsigned long rip)
{
    const char* faultName[] = {
        "[0] Divide Error",
        "[1] Debug Exception",
        "[2] NMI",
        "[3] Breakpoint",
        "[4] Overflow",
        "[5] BOUND",
        "[6] Invalid Opcode (Undefined Opcode)",
        "[7] Device Not Available (No Math Coprocessor)",
        "[8] Double Fault",
        "[9]  Coprocessor Segment Overrun (reserved)",
        "[10] Invalid TSS",
        "[11] Segment Not Present",
        "[12] Stack-Segment Fault",
        "[13] General Protection",
        "[14] Page Fault",
        "[15] Intel reserved. Do not use",
        "[16] x87 FPU Floating-Point Error (Math Fault)",
        "[17] Alignment Check",
        "[18] Machine Check",
        "[19] SIMD Floating-Point Exception",
        "[20] Virtualization Exception",
        "[21] Control Protection Exception",
        "[22] Intel reserved. Do not use",
        "[23] Intel reserved. Do not use",
        "[24] Intel reserved. Do not use",
        "[25] Intel reserved. Do not use",
        "[26] Intel reserved. Do not use",
        "[27] Intel reserved. Do not use",
        "[28] Intel reserved. Do not use",
        "[29] Intel reserved. Do not use",
        "[30] Intel reserved. Do not use",
        "[31] Intel reserved. Do not use",
    };

    color_printk(PRINT_RED, PRINT_NONE, 1, "A fault/trap was triggered!!!!\n");
    color_printk(PRINT_RED, PRINT_NONE, 1, "%s\nerrorCode %d, rip = %#018lx\n", faultName[vectorID], errorCode, rip);

    while(1);
}
