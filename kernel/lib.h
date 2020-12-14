#ifndef _LIB_H_
#define _LIB_H_

#define do_div(n,base) ({ \
    int __res; \
    __res = ((unsigned long) n) % (unsigned) base; \
    n = ((unsigned long) n) / (unsigned) base; \
    __res; })

#endif