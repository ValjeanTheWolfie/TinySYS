#include "print.h"

void _start(void)
{
    setCursorRes(80, 25);
    moveCursor(0, 9);
    setPrintColor(0x0c);

    printf("Now we are in the kernel!!\n");
    setPrintColor(0x07);
    printf("ab123\r123\b\b\bPrint test pass!!\n");
    showCursor();

    printf("%04d, %o, 0x%3X, %p", 625, 64, 255, (void*) 0x7c00);

    while(1);
}