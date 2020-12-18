#ifndef _DESCRIPTORS_H_
#define _DESCRIPTORS_H_


typedef unsigned long SegDescriptor;

typedef struct {
    unsigned long tssDesc32;
    unsigned long baseHigh;
} TssDescriptor;

typedef struct {
    unsigned long gate32;
    unsigned long offsetHigh;
} GateDescriptor;

typedef struct 
{
    unsigned short limit;
    void *address;
} __attribute__((packed)) DescTablePtr;

typedef struct
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


extern void initTss();
extern void initIdt();

#endif