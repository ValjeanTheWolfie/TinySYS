#include "printk.h"
#include "trap.h"

void main(void)
{
    int i, j, k;
    moveCursor(0, 9);
    color_printk(PRINT_RED, PRINT_NONE, 0, "Now we are in the kernel!!\n");
    
    showCursor();

    initIdt();
    initTss();

    i = 1, j = 0, k = 10;
    printk("i = %d, j = %d, k = %d\n", i, j, k);
    k = i / j;
    printk("After i / j: i = %d, j = %d, k = %d\n", i, j, k);

    i = 0, j = 0, k = 10;
    printk("i = %d, j = %d, k = %d\n", i, j, k);
    k = i / j;
    printk("After i / j: i = %d, j = %d, k = %d\n", i, j, k);


    while(1);
}