#ifndef _PRINT_H_
#define _PRINT_H_

                             // R G B
#define  PRINT_BLACK    0    // 0 0 0  黑
#define  PRINT_NONE     0    // 0 0 0  黑（别名）
#define  PRINT_BLUE     1    // 0 0 1  蓝
#define  PRINT_GREEN    2    // 0 1 0  绿
#define  PRINT_CYAN     3    // 0 1 1  青
#define  PRINT_RED      4    // 1 0 0  红
#define  PRINT_MAGENTA  5    // 1 0 1  紫
#define  PRINT_PURPLE   5    // 1 0 1  紫（别名）
#define  PRINT_YELLOW   6    // 1 1 0  黄
#define  PRINT_WHITE    7    // 1 1 1  白


extern int putstr(const char* str);
extern void putchar(char c);
extern void scollScreen(int lines);
extern void moveCursor(int xPos, int yPos);
extern void setCursorRes(int xRes, int yRes);
extern void showCursor();
extern void hideCursor();
extern void setPrintColor(unsigned char color);
unsigned char getColorVal(int forecolor, int background, int bright, int blink);

extern int printk(const char *fmt, ...);
extern int color_printk(int forecolor, int background, int bright, const char *fmt, ...);
extern int sprintf(char * buf, const char *fmt, ...);



#endif