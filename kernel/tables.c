#include "descriptors.h"

SegDescriptor gdtTable[GDT_SIZE] = {
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

GateDescriptor idtTable[IDT_SIZE];

TSS64 tssTable;


DescTablePtr gdtPtr = {sizeof(gdtTable) - 1, gdtTable};
DescTablePtr idtPtr = {sizeof(idtTable) - 1, idtTable};
