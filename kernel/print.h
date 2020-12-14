#ifndef _PRINT_H_
#define _PRINT_H_


extern int putline(const char* str);
extern void putchar(char c);
extern void scollScreen(int lines);
extern void moveCursor(int xPos, int yPos);
extern void setCursorRes(int xRes, int yRes);
extern void showCursor();
extern void hideCursor();
extern void setPrintColor(unsigned char color);

extern int printf(const char *fmt, ...);
extern int sprintf(char * buf, const char *fmt, ...);



#endif