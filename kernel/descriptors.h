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







#endif