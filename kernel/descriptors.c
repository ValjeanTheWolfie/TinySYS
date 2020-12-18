#include "descriptors.h"

#define TO_UL(x) ((unsigned long)(x))

#define ACCESS_BYTE_KERNEL_CODE     0x9a      /*  1_00_1_1010b, Ex = 1             */
#define ACCESS_BYTE_KERNEL_RODATA   0x90      /*  1_00_1_0000b, Ex = 0, RW = 0     */
#define ACCESS_BYTE_KERNEL_RWDATA   0x92      /*  1_00_1_0010b, Ex = 0, RW = 1     */
#define ACCESS_BYTE_USER_CODE       0xfa      /*  1_11_1_1010b, Ex = 1             */
#define ACCESS_BYTE_USER_RODATA     0xf0      /*  1_11_1_0000b, Ex = 0, RW = 0     */
#define ACCESS_BYTE_USER_RWDATA     0xf2      /*  1_11_1_0010b, Ex = 0, RW = 1     */

#define ACCESS_BYTE_TSS64           0x89      /*  1_00_0_1001b  */
#define ACCESS_BYTE_TRAP            0x8e      /*  1_00_0_1110b  */
#define ACCESS_BYTE_INTERRUPT       0x8f      /*  1_00_0_1111b  */

#define FLAGS_1B_16     0x0     /* 0000b */
#define FLAGS_1B_32     0x4     /* 0100b */
#define FLAGS_4K_16     0x8     /* 1000b */
#define FLAGS_4K_32     0xc     /* 1100b */
#define FLAGS_64        0x2     /* 0010b */

#define SEG_DESC_32(base, limit, accessByte, flag)        \
(                                                         \
    ( TO_UL(limit)      & 0x0000ffff)               |     \
    ((TO_UL(base)       & 0x00ffffff) << 16)        |     \
    ((TO_UL(accessByte) & 0x000000ff) << 40)        |     \
    ((TO_UL(limit)      & 0x000f0000) << (48 - 16)) |     \
    ((TO_UL(flag)       & 0x0000000f) << 52)        |     \
    ((TO_UL(base)       & 0xff000000) << (56 - 24))       \
)

#define SEG_DESC_64(accessByte, flag)                     \
(                                                         \
    ((TO_UL(accessByte) & 0x000000ff) << 40)        |     \
    ((TO_UL(flag)       & 0x0000000f) << 52)              \
)

#define TSS_DESC_64_LOW(base, limit, accessByte, flag)  SEG_DESC_32(base, limit, accessByte, flag)
#define TSS_DESC_64_HIGH(base)  (TO_UL(base) >> 32)

#define GATE_DESC_64_LOW(selector, offset, accessByte)    \
(                                                         \
    ( TO_UL(offset)     & 0x0000ffff)                |    \
    ((TO_UL(selector)   & 0x000000ff) << 16)         |    \
    ((TO_UL(accessByte) & 0x000000ff) << 40)         |    \
    ((TO_UL(offset)     & 0xffff0000) << (48 - 16))       \
)
#define GATE_DESC_64_HIGH(offset)    (TO_UL(offset) >> 32)





SegDescriptor gtdTable[16] = {
    0,  /* NULL selector */
    SEG_DESC_64(ACCESS_BYTE_KERNEL_CODE,   FLAGS_64),
    SEG_DESC_64(ACCESS_BYTE_KERNEL_RWDATA, FLAGS_64),
    SEG_DESC_64(ACCESS_BYTE_USER_CODE,     FLAGS_64),
    SEG_DESC_64(ACCESS_BYTE_USER_RWDATA,   FLAGS_64),
    SEG_DESC_32(0, 0xfffff, ACCESS_BYTE_KERNEL_CODE,   FLAGS_4K_32),
    SEG_DESC_32(0, 0xfffff, ACCESS_BYTE_KERNEL_RWDATA, FLAGS_4K_32),
    SEG_DESC_32(0, 0xfffff, ACCESS_BYTE_USER_CODE,     FLAGS_4K_32),
    SEG_DESC_32(0, 0xfffff, ACCESS_BYTE_USER_RWDATA,   FLAGS_4K_32),
    0,  /* TSS low 64 */
    0,  /* TSS high 64 */
};

GateDescriptor idtTable[256];
TSS64 tssTable;

DescTablePtr gdtPtr = {sizeof(gtdTable) - 1, gtdTable};
DescTablePtr idtPtr = {sizeof(idtTable) - 1, idtTable};
DescTablePtr tssPtr = {sizeof(tssTable) - 1, &tssTable};

void initTss()
{
    TssDescriptor* pTss = (TssDescriptor *) &gtdTable[SEG_TSS];
    pTss->tssDesc32 = TSS_DESC_64_LOW(&tssTable, 0xfffff, ACCESS_BYTE_TSS64, FLAGS_64);
    pTss->baseHigh = TSS_DESC_64_HIGH(&tssTable);

    __asm__ __volatile__ ("ltr %%ax\n\t"::"a"(Selector(SEG_TSS)));
}

extern void ignore_int();

void initIdt()
{
    int i;
    for(i = 0; i < 256; i++)
    {
        idtTable[i].gate32 = GATE_DESC_64_LOW(Selector(SEG_KERNEL_CODE), ignore_int, ACCESS_BYTE_TRAP);
        idtTable[i].offsetHigh = GATE_DESC_64_HIGH(ignore_int);
    }

    __asm__ __volatile__ ("lidt %0\n\t"::"m"(idtPtr));
}
