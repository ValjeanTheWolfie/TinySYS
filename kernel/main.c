#include "printk.h"
#include "gates.h"
#include "lib.h"

void main(void)
{
    int i, j, k;
    moveCursor(0, 9);
    color_printk(PRINT_RED, PRINT_NONE, 0, "Now we are in the kernel!!\n");
    
    initIdt();
    initTss();
    initInterrupt();


    die();
}