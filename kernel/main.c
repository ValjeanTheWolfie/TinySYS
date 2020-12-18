#include "printk.h"
#include "descriptors.h"

void main(void)
{
    int i, j, k;
    moveCursor(0, 9);
    color_printk(PRINT_RED, PRINT_NONE, 0, "Now we are in the kernel!!\n");
    
    showCursor();

    initIdt();
    initTss();

    j = 0;
    i /= j;

    while(1);
}