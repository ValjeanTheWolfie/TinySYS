#include "print.h"

void _start(void)
{
    moveCursor(0, 9);
    setPrintColor(getColorVal(PRINT_RED, PRINT_NONE, 1, 0));
    printf("Now we are in the kernel!!\n");
    
    setPrintColor(getColorVal(PRINT_WHITE, PRINT_BLACK, 0, 0));
    printf("|1234567|1234567|1234567|1234567|1234567|1234567|1234567|1234567|1234567\n");
    printf("\t|\t|1\t|12\t|123\t|1234\t|12345\t|123456\t|1234567\t|12345678\t|123456789\t.\n");

    printf("%s/%s %s\n", "abc", "GFDDS", (const char*)0);
    color_printf(PRINT_YELLOW, PRINT_RED, 1, "%s %s\n", "abc", (const char*)0);

    const char *colors[] = {"BLACK", "BLUE", "GREEN", "CYAN", "RED", "PURPLE", "YELLOW", "WHITE", };
    for(int i = 0; i < 8; i++)
    {
        for(int j = 0; j < 8; j++)
        {
            color_printf(i, j, 1, "%-6.6s", colors[i]);
            color_printf(i, j, 0, "dark");
        }
    }

    showCursor();

    while(1);
}