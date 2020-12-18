#include "printk.h"
#include "descriptors.h"

void main(void)
{
    moveCursor(0, 9);
    color_printk(PRINT_RED, PRINT_NONE, 0, "Now we are in the kernel!!\n");
    
    showCursor();

    initTss();

    while(1);
}