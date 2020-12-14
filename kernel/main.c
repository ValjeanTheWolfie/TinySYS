#include "print.h"

void _start(void)
{
    setCursorRes(80, 25);
    moveCursor(0, 9);
    setPrintColor(0x0c);

    putline("Now we are in the kernel!!\n");
    setPrintColor(0x07);
    putline("ab123\r123\b\b\bPrint test pass!!\n");

    for(int i = 0x2000000; i; i--);
    scollScreen(3);
    putline("3 lines scolled!");
    showCursor();

    while(1);
}