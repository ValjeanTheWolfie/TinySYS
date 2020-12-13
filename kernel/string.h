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

static inline unsigned long strlen(const char *s)
{
    long d0;
    register unsigned long res;
    __asm__ __volatile__
    (
        "repne scasb \n\t"
        "not %0 \n\t"
        "dec %0 \n\t"
        :"=c"(res), "=&D"(d0)
        :"1"(s),"a"(0), "0"((long)-1)
    );

    return res;
}

static inline char* strcpy(char *dest, const char *src)
{
    long d0, d1, d2;
    __asm__ __volatile__
	(
        "1: lodsb \n\t"
        "stosb \n\t"
        "testb %%al,%%al \n\t"
        "jne 1b"
        :"=&S"(d0), "=&D"(d1), "=&a"(d2)
        :"0"(src),"1"(dest) 
        :"memory"
	);
    return dest;
}

static inline char* strncpy(char * dest, const char *src, unsigned long length)
{
    long d0, d1, d2, d3;
    __asm__ __volatile__
	(
        "1: dec %2 \n\t"
        "js 2f \n\t"
        "lodsb \n\t"
        "stosb \n\t"
        "testb %%al, %%al \n\t"
        "jne 1b \n\t"
        "rep \n\t"
        "stosb\n"
        "2:"
        :"=&S"(d0), "=&D"(d1), "=&c"(d2), "=&a"(d3)
        :"0"(src),"1"(dest),"2"(length) : "memory"
	);

    return dest;
}

static inline char* strcat(char *dest, const char *src)
{
    long d0, d1, d2, d3;
    __asm__ __volatile__
	(
        "repne scasb \n\t"
        "dec %1\n"
        "1: \n\t"
        "lodsb \n\t"
        "stosb \n\t"
        "testb %%al, %%al \n\t"
        "jne 1b \n\t"
        :"=&S"(d0), "=&D"(d1), "=&a"(d2), "=&c"(d3)
        :"0"(src), "1"(dest), "2"(0), "3"(0xffffffffffffffffUL)
        :"memory"
	);

    return dest;
}

static inline char* strncat(char *dest, const char *src, unsigned long length)
{
    long d0, d1, d2, d3;
    __asm__ __volatile__
	(
        "repne \n\t"
        "scasb \n\t"
        "dec %1 \n\t"
        "mov %4, %3\n"
        "1: dec %3 \n\t"
        "js 2f \n\t"
        "lodsb \n\t"
        "stosb \n\t"
        "testb %%al, %%al \n\t"
        "jne 1b\n"
        "2: xor %2, %2 \n\t"
        "stosb"
        :"=&S"(d0), "=&D"(d1), "=&a"(d2), "=&c"(d3)
        :"g"(length), "0"(src), "1"(dest), "2"(0), "3"(0xffffffff)
        :"memory"
	);
	
    return dest;
}

static inline int strcmp(const char *str1,const char *str2)
{
    long d0, d1;
    register int res;
    __asm__ __volatile__
    (
        "1: lodsb \n\t"
        "scasb \n\t"
        "jne 2f \n\t"
        "testb %%al, %%al \n\t"
        "jne 1b \n\t"
        "xorl %%eax, %%eax \n\t"
        "jmp 3f\n"
        "2: sbbl %%eax, %%eax \n\t"
        "orb $1,%%al\n"
        "3:"
        :"=a"(res), "=&S"(d0), "=&D"(d1)
        :"1"(str1), "2"(str2)
    );
	
    return res;
}

static inline int strncmp(const char *str1,const char *str2, unsigned long count)
{
    register int res;
    int d0, d1, d2;
    __asm__ __volatile__
    (
        "1: dec %3 \n\t"
        "js 2f \n\t"
        "lodsb \n\t"
        "scasb \n\t"
        "jne 3f \n\t"
        "testb %%al, %%al \n\t"
        "jne 1b\n"
        "2: xorl %%eax, %%eax \n\t"
        "jmp 4f\n"
        "3: sbbl %%eax, %%eax \n\t"
        "orb $1, %%al\n"
        "4:"
        :"=a"(res), "=&S"(d0), "=&D"(d1), "=&c"(d2)
        :"1"(str1),"2"(str2),"3"(count)
    );

    return res;
}





#endif