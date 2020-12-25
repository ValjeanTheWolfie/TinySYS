#ifndef _LIB_H_
#define _LIB_H_

#ifndef NULL
#define NULL ((void *) 0)
#endif

#ifndef _BOOL_TYPE_
#define _BOOL_TYPE_
typedef unsigned char bool;
#endif

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif


static inline void die(void)
{
    while(1)
    {
        __asm__ __volatile__ ("hlt \n\t");
    }
}

#define do_div(n,base) ({ \
    int __res; \
    __res = ((unsigned long) n) % (unsigned) base; \
    n = ((unsigned long) n) / (unsigned) base; \
    __res; })

#endif