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

#define nop()        __asm__ __volatile__ ("nop \n\t")
#define sti()        __asm__ __volatile__ ("sti \n\t":::"memory")
#define cli()        __asm__ __volatile__ ("cli \n\t":::"memory")
#define io_mfence()  __asm__ __volatile__ ("mfence \n\t":::"memory")

static inline void die(void)
{
    while(1)
    {
        __asm__ __volatile__ ("hlt \n\t");
    }
}

static inline void io_out8(unsigned short port, unsigned char value)
{
    __asm__ __volatile__ (
        "outb %0, %%dx \n\t"
        "mfence \n\t"
        :
        :"a"(value), "d"(port)
        :"memory"
    );
}

static inline void io_out32(unsigned short port, unsigned int value)
{
    __asm__ __volatile__ (
        "outl %0, %%dx \n\t"
        "mfence \n\t"
        :
        :"a"(value), "d"(port)
        :"memory"
    );
}

static inline unsigned char io_in8(unsigned short port)
{
    register unsigned char ret;
    __asm__ __volatile__ (
        "inb %%dx, %0 \n\t"
        "mfence \n\t"
        :"=a"(ret)
        :"d"(port)
        :"memory"
    );

    return ret;
}

static inline unsigned char io_in32(unsigned short port)
{
    register unsigned char ret;
    __asm__ __volatile__ (
        "inl %%dx, %0 \n\t"
        "mfence \n\t"
        :"=a"(ret)
        :"d"(port)
        :"memory"
    );

    return ret;
}












#define do_div(n,base) ({ \
    int __res; \
    __res = ((unsigned long) n) % (unsigned) base; \
    n = ((unsigned long) n) / (unsigned) base; \
    __res; })

#endif