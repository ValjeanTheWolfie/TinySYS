#include "printk.h"
#include "trap.h"

void main(void)
{
    int i, j, k;
    moveCursor(0, 9);
    color_printk(PRINT_RED, PRINT_NONE, 0, "Now we are in the kernel!!\n");
    
    initIdt();
    initTss();

    i = 1, j = 0, k = 10;
    printk("i = %d, j = %d, k = %d, calculating k = i / j\n", i, j, k);
    k = i / j;

    /* 
    i = * (int*) 0x1000000;
    */


    while(1);
}