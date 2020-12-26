#include "tables.h"
#include "gates.h"
#include "lib.h"
#include "printk.h"
#include "entry.h"

IdtInfo idtEntryInfo[] = {
    {idt_ent_0 , ACCESS_BYTE_TRAP_KERNEL, "[0] Divide Error",                               },
    {idt_ent_1 , ACCESS_BYTE_TRAP_KERNEL, "[1] Debug Exception",                            },
    {idt_ent_2 , ACCESS_BYTE_INTERRUPT  , "[2] NMI",                                        },
    {idt_ent_3 , ACCESS_BYTE_TRAP_USER  , "[3] Breakpoint",                                 },
    {idt_ent_4 , ACCESS_BYTE_TRAP_USER  , "[4] Overflow",                                   },
    {idt_ent_5 , ACCESS_BYTE_TRAP_USER  , "[5] BOUND",                                      },
    {idt_ent_6 , ACCESS_BYTE_TRAP_KERNEL, "[6] Invalid Opcode (Undefined Opcode)",          },
    {idt_ent_7 , ACCESS_BYTE_TRAP_KERNEL, "[7] Device Not Available (No Math Coprocessor)", },
    {idt_ent_8 , ACCESS_BYTE_TRAP_KERNEL, "[8] Double Fault",                               },
    {idt_ent_9 , ACCESS_BYTE_TRAP_KERNEL, "[9] Coprocessor Segment Overrun (reserved)",     },
    {idt_ent_10, ACCESS_BYTE_TRAP_KERNEL, "[10] Invalid TSS",                               },
    {idt_ent_11, ACCESS_BYTE_TRAP_KERNEL, "[11] Segment Not Present",                       },
    {idt_ent_12, ACCESS_BYTE_TRAP_KERNEL, "[12] Stack-Segment Fault",                       },
    {idt_ent_13, ACCESS_BYTE_TRAP_KERNEL, "[13] General Protection",                        },
    {idt_ent_14, ACCESS_BYTE_TRAP_KERNEL, "[14] Page Fault",                                },
    {                                     /* [15] Intel reserved. Do not use              */},
    {idt_ent_16, ACCESS_BYTE_TRAP_KERNEL, "[16] x87 FPU Floating-Point Error (Math Fault)", },
    {idt_ent_17, ACCESS_BYTE_TRAP_KERNEL, "[17] Alignment Check",                           },
    {idt_ent_18, ACCESS_BYTE_TRAP_KERNEL, "[18] Machine Check",                             },
    {idt_ent_19, ACCESS_BYTE_TRAP_KERNEL, "[19] SIMD Floating-Point Exception",             },
    {idt_ent_20, ACCESS_BYTE_TRAP_KERNEL, "[20] Virtualization Exception",                  },
    {idt_ent_21, ACCESS_BYTE_TRAP_KERNEL, "[21] Control Protection Exception",              },
    {                                     /* [22] Intel reserved. Do not use              */},
    {                                     /* [23] Intel reserved. Do not use              */},
    {                                     /* [24] Intel reserved. Do not use              */},
    {                                     /* [25] Intel reserved. Do not use              */},
    {                                     /* [26] Intel reserved. Do not use              */},
    {                                     /* [27] Intel reserved. Do not use              */},
    {                                     /* [28] Intel reserved. Do not use              */},
    {                                     /* [29] Intel reserved. Do not use              */},
    {                                     /* [30] Intel reserved. Do not use              */},
    {                                     /* [31] Intel reserved. Do not use              */},
    {idt_ent_0x20, ACCESS_BYTE_INTERRUPT, "8259A PIC Master: IR0 (Timer)"                    },
    {idt_ent_0x21, ACCESS_BYTE_INTERRUPT, "8259A PIC Master: IR1 (Keyboard)"                 },
    {idt_ent_0x22, ACCESS_BYTE_INTERRUPT, "8259A PIC Master: IR2 (Slave 8259A chip)"         },
    {idt_ent_0x23, ACCESS_BYTE_INTERRUPT, "8259A PIC Master: IR3 "                           },
    {idt_ent_0x24, ACCESS_BYTE_INTERRUPT, "8259A PIC Master: IR4 "                           },
    {idt_ent_0x25, ACCESS_BYTE_INTERRUPT, "8259A PIC Master: IR5 "                           },
    {idt_ent_0x26, ACCESS_BYTE_INTERRUPT, "8259A PIC Master: IR6 (Floppy Drive)"             },
    {idt_ent_0x27, ACCESS_BYTE_INTERRUPT, "8259A PIC Master: IR7 "                           },
    {idt_ent_0x28, ACCESS_BYTE_INTERRUPT, "8259A PIC Slave : IR0 (CMOS RTC clock)"           },
    {idt_ent_0x29, ACCESS_BYTE_INTERRUPT, "8259A PIC Slave : IR1 "                           },
    {idt_ent_0x2a, ACCESS_BYTE_INTERRUPT, "8259A PIC Slave : IR2 "                           },
    {idt_ent_0x2b, ACCESS_BYTE_INTERRUPT, "8259A PIC Slave : IR3 "                           },
    {idt_ent_0x2c, ACCESS_BYTE_INTERRUPT, "8259A PIC Slave : IR4 (PS/2 mouse)"               },
    {idt_ent_0x2d, ACCESS_BYTE_INTERRUPT, "8259A PIC Slave : IR5 "                           },
    {idt_ent_0x2e, ACCESS_BYTE_INTERRUPT, "8259A PIC Slave : IR6 "                           },
    {idt_ent_0x2f, ACCESS_BYTE_INTERRUPT, "8259A PIC Slave : IR7 "                           },
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

    for(i = 0; i < sizeof(idtEntryInfo) / sizeof(IdtInfo); i++)
    {
        if(NULL != idtEntryInfo[i].entranceAddr)
        {
            idtTable[i].gate32 = GATE_DESC_64_LOW(Selector(SEG_KERNEL_CODE), idtEntryInfo[i].entranceAddr, idtEntryInfo[i].accessByte, 1);
            idtTable[i].offsetHigh = GATE_DESC_64_HIGH(idtEntryInfo[i].entranceAddr);
        }
        else
        {
            idtTable[i].gate32 = idtTable[i].offsetHigh = 0;
        }
    }

    __asm__ __volatile__ ("lidt %0\n\t"::"m"(idtPtr));
}

void initInterrupt()
{
    /* Master ICW*/
    io_out8(0x20, 0x11);
    io_out8(0x21, 0x20);
    io_out8(0x21, 0x04);
    io_out8(0x21, 0x01);

    /* Slave ICW*/
    io_out8(0xa0, 0x11);
    io_out8(0xa1, 0x28);
    io_out8(0xa1, 0x02);
    io_out8(0xa1, 0x01);

    /* OCW */
    io_out8(0x21, 0xfd);
    io_out8(0xa1, 0xff);

    sti();
}



void notifyAndDie()
{
    printk("System halt!\n"); 
    showCursor();
    die();
}

void handleIdtCall(int vectorID, unsigned long errorCode, unsigned long rip)
{
    if(vectorID < 32)
    {
        handleFault(vectorID, errorCode, rip);
    }
    else
    {
        handleInterrupt(vectorID);
    }
}

void handleFault(int vectorID, unsigned long errorCode, unsigned long rip)
{
    if(NULL != idtEntryInfo[vectorID].entranceAddr)
    {
        color_printk(PRINT_RED, PRINT_NONE, 1, "\nA fault/trap has been triggered:\n");
        color_printk(PRINT_YELLOW, PRINT_NONE, 1, "%s\n", idtEntryInfo[vectorID].printInfo);
        color_printk(PRINT_YELLOW, PRINT_NONE, 1, "At rip = %#018lx\n", rip);
    }

    switch(vectorID) {
        case 0 ... 7:
        case 9:
        case 16:
        case 18 ... 20:
            notifyAndDie();
            break;
        case 8:
        case 17:
            /* Zero Error Code */
            color_printk(PRINT_YELLOW, PRINT_NONE, 1, "ErrorCode = %lu\n", errorCode);
            notifyAndDie();
            break;
        case 10 ... 13:
            commonErrorCodeInfo(errorCode);
            notifyAndDie();
            break;
        case 14:
            handlePageFault(errorCode);
            break;
        case 21:
            controlProtectionExceptionInfo(errorCode);
            notifyAndDie();
            break;
        default:
            color_printk(PRINT_RED, PRINT_NONE, 1, "Error! Reserved fault!!!!\n");
            color_printk(PRINT_RED, PRINT_NONE, 1, "Vector ID = %d\n", vectorID);
            notifyAndDie();
            break;
    }

}



void handleInterrupt(int vectorID)
{
    int keycode;
    keycode = io_in8(0x60);
    color_printk(PRINT_GREEN, PRINT_BLACK, 1, "%s, Key code = %#04x\n", idtEntryInfo[vectorID].printInfo, keycode);
    io_out8(0x20, 0x20);

}


void commonErrorCodeInfo(unsigned long errorCode)
{
    printk("Error code info:\n");

    if(errorCode & 0x01) /* EXT */
    {
        printk("EXT = 1: During an external event, e.g. interrupt/earlier exception.\n");
    }
    else
    {
        printk("EXT = 0: During a software interrupt (INT n, INT3, or INTO).\n");
    }

    if(errorCode & 0x02) /* IDT */
    {
        printk("IDT = 1: refers to a gate descriptor in the IDT.\n");
    }
    else
    {
        printk("IDT = 0: Refers to a descriptor in the GDT or the current LDT.\n");
        if(errorCode & 0x04) /* TI */
        {
            printk("TI = 1:  Refers to a segment or gate descriptor in the LDT.\n");
        }
        else
        {
            printk("TI = 0:  Refers to a descriptor in the current GDT.\n");
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

    notifyAndDie();
}

void controlProtectionExceptionInfo(unsigned long errorCode)
{
    const char *CPEC[256] = {"", "NEAR-RET", "FAR-RET/IRET", "ENDBRANCH", "RSTORSSP", "SETSSBSY",};
 
    printk("CPEC = %s\n", CPEC[errorCode & ((1 << 15) - 1)]);
    if(errorCode & (1 << 15)) printk("the #CP occurred during enclave execution\n");
}
