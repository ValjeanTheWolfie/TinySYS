#ifndef _DESCRIPTORS_H_
#define _DESCRIPTORS_H_


typedef unsigned long SegDescriptor;

typedef struct _TssDescriptor
{
    unsigned long tssDesc32;
    unsigned long baseHigh;
} TssDescriptor;

typedef struct _GateDescriptor
{
    unsigned long gate32;
    unsigned long offsetHigh;
} GateDescriptor;

typedef struct _DescTablePtr 
{
    unsigned short limit;
    void *address;
} __attribute__((packed)) DescTablePtr;

typedef struct _TSS64
{
    unsigned int reserved0;
    unsigned long rsp0, rsp1, rsp2;
    unsigned int reserved1[2];
    unsigned long ist1, ist2, ist3, ist4, ist5, ist6, ist7;
    unsigned int reserved2[2];
    unsigned short reserved3;
    unsigned short bitmapAddr;
} __attribute__((packed)) TSS64;

extern SegDescriptor gtdTable[];
extern TSS64 tssTable;
extern GateDescriptor idtTable[256];

enum SegDescIndex
{
    SEG_NULL = 0,
    SEG_KERNEL_CODE,
    SEG_KERNEL_RWDATA,
    SEG_USER_CODE,
    SEG_USER_RWDATA,
    SEG_KERNEL_CODE_32,
    SEG_KERNEL_RWDATA_32,
    SEG_USER_CODE_32,
    SEG_USER_RWDATA_32,
    SEG_TSS,
    SEG_RESERVED0,
};

#define Selector(segDescInd) ((unsigned short)(segDescInd) << 3)


#define GDT_SIZE  16
#define IDT_SIZE  256


#define TO_UL(x) ((unsigned long)(x))

#define ACCESS_BYTE_KERNEL_CODE     0x9a      /*  1_00_1_1010b, Ex = 1             */
#define ACCESS_BYTE_KERNEL_RODATA   0x90      /*  1_00_1_0000b, Ex = 0, RW = 0     */
#define ACCESS_BYTE_KERNEL_RWDATA   0x92      /*  1_00_1_0010b, Ex = 0, RW = 1     */
#define ACCESS_BYTE_USER_CODE       0xfa      /*  1_11_1_1010b, Ex = 1             */
#define ACCESS_BYTE_USER_RODATA     0xf0      /*  1_11_1_0000b, Ex = 0, RW = 0     */
#define ACCESS_BYTE_USER_RWDATA     0xf2      /*  1_11_1_0010b, Ex = 0, RW = 1     */

#define ACCESS_BYTE_TSS64           0x89      /*  1_00_0_1001b  */
#define ACCESS_BYTE_TRAP            0x8e      /*  1_00_0_1110b  */
#define ACCESS_BYTE_INT_KERNEL      0x8f      /*  1_00_0_1111b  */
#define ACCESS_BYTE_INT_USER        0xef      /*  1_11_0_1111b  */

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

#define GATE_DESC_64_LOW(selector, offset, accessByte, ist)    \
(                                                              \
    ( TO_UL(offset)     & 0x0000ffff)                |         \
    ((TO_UL(selector)   & 0x000000ff) << 16)         |         \
    ((TO_UL(ist)        & 0x00000007) << 32)         |         \
    ((TO_UL(accessByte) & 0x000000ff) << 40)         |         \
    ((TO_UL(offset)     & 0xffff0000) << (48 - 16))            \
)
#define GATE_DESC_64_HIGH(offset)    (TO_UL(offset) >> 32)





#endif