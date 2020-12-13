#ifndef _STRING_H_
#define _STRING_H_

static inline void* memcpy(void * dest, const void * src, unsigned long size)
{
    long d0, d1, d2;
    register void *tmp = (void *)dest;
    __asm__ __volatile__ 
    (
        "cld \n\t"
        "0: shr $1,%%rcx \n\t"
        "jnc 1f \n\t"
        "movsb \n\t"
        "1: shr $1, %%rcx \n\t"
        "jnc 2f \n\t"
        "movsw \n\t"
        "2: shr $1, %%rcx \n\t"
        "jnc 3f \n\t"
        "movsl \n\t"
        "3: rep movsq \n\t"
        :"=&c"(d0), "=&D"(d1), "=&S"(d2)
        :"0"(size), "1"((long) tmp), "2"((long) src)
        :"memory"
    );

    return dest;
}

static inline void* memmove(void *dest, const void *src, unsigned long size)
{
    long d0, d1, d2;
    register void *tmp = (void *) dest;
    if (dest < src)
        __asm__ __volatile__ 
        (
            "cld \n\t"
            "rep movsb \n\t"
            :"=&c"(d0), "=&S"(d1), "=&D"(d2)
            :"0"(size), "1"(src), "2"(tmp)
            :"memory"
        );
    else
        __asm__ __volatile__ 
        (
            "std \n\t"
            "rep movsb \n\t"
            "cld \n\t"
            :"=&c"(d0), "=&S"(d1), "=&D"(d2)
            :"0"(size), "1"(size - 1 + (const char *)src), "2"(size - 1 + (char *)tmp)
            :"memory"
        );
    
    return dest;
}

static inline void* memset(void * dest,unsigned char val,unsigned long size)
{
    long d0, d1, d2;
    register void *tmp = (void *) dest;
    __asm__ __volatile__ 
    (
        "cld \n\t"
        "0: shr $1,%%rcx \n\t"
        "jnc 1f \n\t"
        "stosb \n\t"
        "1: shr $1, %%rcx \n\t"
        "jnc 2f \n\t"
        "stosw \n\t"
        "2: shr $1, %%rcx \n\t"
        "jnc 3f \n\t"
        "stosl \n\t"
        "3: rep stosq \n\t"
        :"=&c"(d0), "=&D"(d1), "=&a"(d2)
        :"0"(size), "1"(tmp), "2"(val * 0x0101010101010101UL)
        :"memory"
    );

    return dest;
}




#endif