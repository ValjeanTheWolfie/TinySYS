#ifndef _TABLES_H_
#define _TABLES_H_

#include "descriptors.h"

extern SegDescriptor gdtTable[GDT_SIZE];
extern GateDescriptor idtTable[IDT_SIZE];
extern TSS64 tssTable;
extern DescTablePtr gdtPtr;
extern DescTablePtr idtPtr;

#endif