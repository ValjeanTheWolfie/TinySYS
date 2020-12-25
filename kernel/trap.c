#include "tables.h"
#include "trap.h"
#include "lib.h"
#include "printk.h"

IdtEntryInfo idtEntry[] = {
    {/*0 */ idt_ent_0 , "[0] Divide Error",                               ACCESS_BYTE_TRAP_KERNEL,                  },
    {/*1 */ idt_ent_1 , "[1] Debug Exception",                            ACCESS_BYTE_TRAP_KERNEL,                  },
    {/*2 */ idt_ent_2 , "[2] NMI",                                        ACCESS_BYTE_INTERRUPT  ,                  },
    {/*3 */ idt_ent_3 , "[3] Breakpoint",                                 ACCESS_BYTE_TRAP_USER  ,                  },
    {/*4 */ idt_ent_4 , "[4] Overflow",                                   ACCESS_BYTE_TRAP_USER  ,                  },
    {/*5 */ idt_ent_5 , "[5] BOUND",                                      ACCESS_BYTE_TRAP_USER  ,                  },
    {/*6 */ idt_ent_6 , "[6] Invalid Opcode (Undefined Opcode)",          ACCESS_BYTE_TRAP_KERNEL,                  },
    {/*7 */ idt_ent_7 , "[7] Device Not Available (No Math Coprocessor)", ACCESS_BYTE_TRAP_KERNEL,                  },
    {/*8 */ idt_ent_8 , "[8] Double Fault",                               ACCESS_BYTE_TRAP_KERNEL,                  },
    {/*9 */ idt_ent_9 , "[9] Coprocessor Segment Overrun (reserved)",     ACCESS_BYTE_TRAP_KERNEL,                  },
    {/*10*/ idt_ent_10, "[10] Invalid TSS",                               ACCESS_BYTE_TRAP_KERNEL,                  },
    {/*11*/ idt_ent_11, "[11] Segment Not Present",                       ACCESS_BYTE_TRAP_KERNEL,                  },
    {/*12*/ idt_ent_12, "[12] Stack-Segment Fault",                       ACCESS_BYTE_TRAP_KERNEL,                  },
    {/*13*/ idt_ent_13, "[13] General Protection",                        ACCESS_BYTE_TRAP_KERNEL,                  },
    {/*14*/ idt_ent_14, "[14] Page Fault",                                ACCESS_BYTE_TRAP_KERNEL,                  },
    {/*15*/ NULL      , "[15] Intel reserved. Do not use",                ACCESS_BYTE_TRAP_KERNEL,                  },
    {/*16*/ idt_ent_16, "[16] x87 FPU Floating-Point Error (Math Fault)", ACCESS_BYTE_TRAP_KERNEL,                  },
    {/*17*/ idt_ent_17, "[17] Alignment Check",                           ACCESS_BYTE_TRAP_KERNEL,                  },
    {/*18*/ idt_ent_18, "[18] Machine Check",                             ACCESS_BYTE_TRAP_KERNEL,                  },
    {/*19*/ idt_ent_19, "[19] SIMD Floating-Point Exception",             ACCESS_BYTE_TRAP_KERNEL,                  },
    {/*20*/ idt_ent_20, "[20] Virtualization Exception",                  ACCESS_BYTE_TRAP_KERNEL,                  },
    {/*21*/ idt_ent_21, "[21] Control Protection Exception",              ACCESS_BYTE_TRAP_KERNEL,                  },
    {/*22*/         /*  "[22] Intel reserved. Do not use",            */                                            },
    {/*23*/         /*  "[23] Intel reserved. Do not use",            */                                            },
    {/*24*/         /*  "[24] Intel reserved. Do not use",            */                                            },
    {/*25*/         /*  "[25] Intel reserved. Do not use",            */                                            },
    {/*26*/         /*  "[26] Intel reserved. Do not use",            */                                            },
    {/*27*/         /*  "[27] Intel reserved. Do not use",            */                                            },
    {/*28*/         /*  "[28] Intel reserved. Do not use",            */                                            },
    {/*29*/         /*  "[29] Intel reserved. Do not use",            */                                            },
    {/*30*/         /*  "[30] Intel reserved. Do not use",            */                                            },
    {/*31*/         /*  "[31] Intel reserved. Do not use",            */                                            },
};

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

    for(i = 0; i < 32; i++)
    {
        if(NULL != idtEntry[i].entryAddr)
        {
            idtTable[i].gate32 = GATE_DESC_64_LOW(Selector(SEG_KERNEL_CODE), idtEntry[i].entryAddr, idtEntry[i].accessByte, 1);
            idtTable[i].offsetHigh = GATE_DESC_64_HIGH(idtEntry[i].entryAddr);
        }
        else
        {
            idtTable[i].gate32 = idtTable[i].offsetHigh = 0;
        }
    }

    __asm__ __volatile__ ("lidt %0\n\t"::"m"(idtPtr));
}



#define DIE()    printk("System halt!\n"); showCursor();die();

void handleIdtCall(int vectorID, unsigned long errorCode, unsigned long rip)
{
    if(vectorID <= 21)
    {
        color_printk(PRINT_RED, PRINT_NONE, 1, "\nA fault/trap has been triggered:\n");
        color_printk(PRINT_YELLOW, PRINT_NONE, 1, "%s\n", idtEntry[vectorID].printInfo);
        color_printk(PRINT_YELLOW, PRINT_NONE, 1, "At rip = %#018lx\n", rip);
    }


    switch(vectorID) {
        case 0 ... 7:
        case 9:
        case 16:
        case 18 ... 20:
            DIE();
            break;
        case 8:
        case 17:
            /* Zero Error Code */
            color_printk(PRINT_YELLOW, PRINT_NONE, 1, "ErrorCode = %lu\n", errorCode);
            DIE()
            break;
        case 10 ... 13:
            commonErrorCodeInfo(errorCode);
            DIE()
            break;
        case 14:
            handlePageFault(errorCode);
            break;
        case 21:
            controlProtectionExceptionInfo(errorCode);
            DIE();
            break;
        default:
            color_printk(PRINT_RED, PRINT_NONE, 1, "Unknown interrupt or trap!!!!\n");
            color_printk(PRINT_RED, PRINT_NONE, 1, "Vector ID = %d\n", vectorID);
            DIE()
            break;
    }

}

void commonErrorCodeInfo(unsigned long errorCode)
{
    printk("Error code info:\n");

    if(errorCode & 0x01) /* EXT */
    {
        printk("The exception occurred during delivery of an event external to the program, such as an interrupt or an earlier exception.\n");
    }
    else
    {
        printk("The exception occurred during delivery of a software interrupt (INT n, INT3, or INTO).\n");
    }

    if(errorCode & 0x02) /* IDT */
    {
        printk("The index portion of the error code refers to a gate descriptor in the IDT.\n");
    }
    else
    {
        printk("The index refers to a descriptor in the GDT or the current LDT.\n");
        if(errorCode & 0x04) /* TI */
        {
            printk("The index portion of the error code refers to a segment or gate descriptor in the LDT.\n");
        }
        else
        {
            printk("The index refers to a descriptor in the current GDT.\n");
        }
    }
    
    printk("Selector index: %d\n", errorCode >> 3);
}

void handlePageFault(unsigned long errorCode)
{
    unsigned long cr2;
    __asm__ __volatile__ ("movq %%cr2, %0 \n\t":"=r"(cr2)::"memory");

    printk("%s\n", (errorCode & (1 <<  0)) ? "The fault was caused by a non-present page." : "The fault was caused by a page-level protection violation.");
    printk("%s\n", (errorCode & (1 <<  1)) ? "The access causing the fault was a read." : "The access causing the fault was a write.");
    printk("%s\n", (errorCode & (1 <<  2)) ? "A supervisor-mode access caused the fault." : "A user-mode access caused the fault.");
    printk("%s\n", (errorCode & (1 <<  3)) ? "The fault was not caused by reserved bit violation." : "The fault was caused by a reserved bit set to 1 in some paging-structure entry.");
    printk("%s\n", (errorCode & (1 <<  4)) ? "The fault was not caused by an instruction fetch." : "The fault was caused by an instruction fetch.");
    printk("%s\n", (errorCode & (1 <<  5)) ? "The fault was not caused by protection keys." : "There was a protection-key violation.");
    printk("%s\n", (errorCode & (1 <<  6)) ? "The fault was not caused by a shadow-stack access." : "The fault was caused by a shadow-stack access.");
    printk("%s\n", (errorCode & (1 << 15)) ? "The fault is not related to SGX." : "The fault resulted from violation of SGX-specific access-control requirements.");

    printk("CR2: %#018lx\n", cr2);

    DIE();
}

void controlProtectionExceptionInfo(unsigned long errorCode)
{
    const char *CPEC[256] = {"", "NEAR-RET", "FAR-RET/IRET", "ENDBRANCH", "RSTORSSP", "SETSSBSY",};
 
    printk("CPEC = %s\n", CPEC[errorCode & ((1 << 15) - 1)]);
    if(errorCode & (1 << 15)) printk("the #CP occurred during enclave execution\n");
}