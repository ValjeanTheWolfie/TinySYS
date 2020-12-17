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
#define FLAGS_4K_32     0x9     /* 1100b */
#define FLAGS_64        0x2     /* 0010b */

#define SEG_DESC32_INIT(base, limit, accessByte, flag)    \
(                                                         \
    ( TO_UL(limit)      & 0x0000ffff)               |     \
    ((TO_UL(base)       & 0x00ffffff) << 16)        |     \
    ((TO_UL(accessByte) & 0x000000ff) << 40)        |     \
    ((TO_UL(limit)      & 0x000f0000) << (48 - 16)) |     \
    ((TO_UL(flag)       & 0x0000000f) << 52)        |     \
    ((TO_UL(base)       & 0xff000000) << (56 - 24))       \
)

#define SEG_DESC64_INIT(accessByte, flag)                 \
(                                                         \
    ((TO_UL(accessByte) & 0x000000ff) << 40)        |     \
    ((TO_UL(flag)       & 0x0000000f) << 52)              \
)

#define TSS64_DEST_INIT_LOW(base, limit, accessByte, flag)  SEG_DESC32_INIT(base, limit, accessByte, flag)
#define TSS64_DEST_INIT_HIGH(base)  (TO_UL(base) >> 32)

#define GATE_DESC_INIT(selector, offset, accessByte)  \
{                                                     \
    ( TO_UL(offset)     & 0x00ff)                |    \
    ((TO_UL(selector)   & 0x00ff) << 16)         |    \
    ((TO_UL(accessByte) & 0x00ff) << 40)         |    \
    ((TO_UL(offset)     & 0xff00) << (48 - 16)),      \
    TO_UL(offset) >> 32                               \
}





SegDescriptor gtdTable[] = {
    0,
    SEG_DESC64_INIT(ACCESS_BYTE_KERNEL_CODE,   FLAGS_64),
    SEG_DESC64_INIT(ACCESS_BYTE_KERNEL_RWDATA, FLAGS_64),
    SEG_DESC64_INIT(ACCESS_BYTE_KERNEL_RODATA, FLAGS_64),
    SEG_DESC64_INIT(ACCESS_BYTE_USER_CODE,     FLAGS_64),
    SEG_DESC64_INIT(ACCESS_BYTE_USER_RWDATA,   FLAGS_64),
    SEG_DESC64_INIT(ACCESS_BYTE_USER_RODATA,   FLAGS_64),



};
