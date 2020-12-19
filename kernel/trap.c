#include "tables.h"

void initTss()
{
    TssDescriptor* pTss = (TssDescriptor *) &gdtTable[SEG_TSS];

    pTss->tssDesc32 = TSS_DESC_64_LOW(&tssTable, 0xfffff, ACCESS_BYTE_TSS64, FLAGS_64);
    pTss->baseHigh = TSS_DESC_64_HIGH(&tssTable);

    tssTable.rsp0 = tssTable.rsp1 = tssTable.rsp2 = tssTable.ist1 = tssTable.ist2 = tssTable.ist3 = tssTable.ist4 = tssTable.ist5 = tssTable.ist6 = tssTable.ist7 = 0xffff800000080000;

    __asm__ __volatile__ ("ltr %%ax\n\t"::"a"(Selector(SEG_TSS)));
}

extern void ignore_int();

void initIdt()
{
    int i;
    for(i = 0; i < 256; i++)
    {
        idtTable[i].gate32 = GATE_DESC_64_LOW(Selector(SEG_KERNEL_CODE), ignore_int, ACCESS_BYTE_TRAP, 1);
        idtTable[i].offsetHigh = GATE_DESC_64_HIGH(ignore_int);
    }

    __asm__ __volatile__ ("lidt %0\n\t"::"m"(idtPtr));
}