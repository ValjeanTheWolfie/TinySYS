#include <stdarg.h>
#include "printk.h"
#include "string.h"
#include "lib.h"

typedef struct
{
    char c;
    unsigned char color;
} ScreenChar;

typedef struct
{
    int xPos, yPos;
    int xRes, yRes;
    int offset;    /* Quick access to xPos + yPos * xRes */
    char shape;
    unsigned char color;
    unsigned char printColor;
} Cursor;


static ScreenChar *const videoMemBase = (ScreenChar *const) 0xffff8000000b8000;
static Cursor cursor = 
{
    .xRes = 80,
    .yRes = 25,
    .shape = '_',
    .printColor = 0x07,  // normal white
    .color = 0x8f,       // bright white and blinking
};
static char buff[4096];

static inline void drawChar(int offset, char c, unsigned char color)
{
    videoMemBase[offset].c = c;
    videoMemBase[offset].color = color;
}

int putstr(const char *str)
{
    const char* p;
    for(p = str; *p; p++) putchar(*p);
    return p - str;
}


void putchar(char c)
{
    switch(c)
    {
        case '\n':
            cursor.yPos++;
            cursor.offset += cursor.xRes;
        /* fall through: treat '\n' as '\r' + '\n' */
        case '\r':
            cursor.offset -= cursor.xPos;
            cursor.xPos = 0;
            break;
        case '\b':
            drawChar(cursor.offset, 0, 0);
            if(cursor.offset > 0)
            {
                cursor.offset--;
            }
            cursor.xPos--;
            if(cursor.xPos < 0)
            {
                if(cursor.yPos > 0) cursor.yPos--;
                cursor.xPos = 0;
            }
            break;
        case '\t':
            cursor.offset += (8 - (cursor.xPos % 8));
            cursor.xPos += (8 - (cursor.xPos % 8));
            break;
        default:
            drawChar(cursor.offset, c, cursor.printColor);
            cursor.offset++; 
            cursor.xPos++;
            break;
    }
    
    if(cursor.xPos >= cursor.xRes)
    {
        cursor.xPos = 0;
        cursor.yPos++;
    }

    if(cursor.yPos >= cursor.yRes)
    {
        scollScreen(1);
    }
}



void scollScreen(int lines)
{
    int charCnt = lines * cursor.xRes;
    int totalCnt = cursor.yRes * cursor.xRes;

    memmove(videoMemBase, videoMemBase + charCnt, (totalCnt - charCnt) * sizeof(ScreenChar));
    memset(videoMemBase + totalCnt - charCnt, 0, charCnt * sizeof(ScreenChar));

    cursor.yPos -= lines;
    cursor.offset -= charCnt;
}


void moveCursor(int xPos, int yPos)
{
    cursor.xPos = xPos;
    cursor.yPos = yPos;
    cursor.offset = xPos + yPos * cursor.xRes;
}

void setCursorRes(int xRes, int yRes)
{
    cursor.xRes = xRes;
    cursor.yRes = yRes;
}

void setCursorShape(char shape, unsigned char color)
{
    cursor.shape = shape;
    cursor.color = color;
}

void showCursor()
{
    drawChar(cursor.offset, cursor.shape, cursor.color);
}

void hideCursor()
{
    drawChar(cursor.offset, 0, 0);
}

void setPrintColor(unsigned char color)
{
    cursor.printColor = color;
}

unsigned char getPrintColor()
{
    return cursor.printColor;
}

unsigned char getColorVal(int forecolor, int background, int bright, int blink)
{
    return ((blink & 1) << 7) 
         | ((background & 0x07) << 4) 
         | ((bright & 1) << 3) 
         | (forecolor & 0x07);
}

#define isdigit(c) (((c) >= '0') && ((c) <= '9'))

static int skip_atoi(const char **s)
{
    int i=0;

    while (isdigit(**s))
        i = i*10 + *((*s)++) - '0';
    return i;
}

#define ZEROPAD    1         /* pad with zero */
#define SIGN       2         /* unsigned/signed long */
#define PLUS       4         /* show plus */
#define SPACE      8         /* space if plus */
#define LEFT       16        /* left justified */
#define SPECIAL    32        /* 0x */
#define LARGE      64        /* use 'ABCDEF' instead of 'abcdef' */

static char * number(char * str, long num, int base, int size, int precision, int type)
{
    char c,sign,tmp[66];
    const char *digits="0123456789abcdefghijklmnopqrstuvwxyz";
    int i;

    if (type & LARGE)
        digits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    if (type & LEFT)
        type &= ~ZEROPAD;
    if (base < 2 || base > 36)
        return 0;
    c = (type & ZEROPAD) ? '0' : ' ';
    sign = 0;
    if (type & SIGN) {
        if (num < 0) {
            sign = '-';
            num = -num;
            size--;
        } else if (type & PLUS) {
            sign = '+';
            size--;
        } else if (type & SPACE) {
            sign = ' ';
            size--;
        }
    }
    if (type & SPECIAL) {
        if (base == 16)
            size -= 2;
        else if (base == 8)
            size--;
    }
    i = 0;
    if (num == 0)
        tmp[i++]='0';
    else while (num != 0)
        tmp[i++] = digits[do_div(num,base)];
    if (i > precision)
        precision = i;
    size -= precision;
    if (!(type&(ZEROPAD+LEFT)))
        while(size-->0)
            *str++ = ' ';
    if (sign)
        *str++ = sign;
    if (type & SPECIAL) {
        if (base==8)
            *str++ = '0';
        else if (base==16) {
            *str++ = '0';
            *str++ = digits[33];
        }
    }
    if (!(type & LEFT))
        while (size-- > 0)
            *str++ = c;
    while (i < precision--)
        *str++ = '0';
    while (i-- > 0)
        *str++ = tmp[i];
    while (size-- > 0)
        *str++ = ' ';
    return str;
}

int vsprintf(char *buf, const char *fmt, va_list args)
{
    int len;
    unsigned long num;
    int i, base;
    char * str;
    const char *s;

    int flags;             /* flags to number() */

    int field_width;       /* width of output field */
    int precision;         /* min. # of digits for integers; max
                              number of chars for from string */
    int qualifier;         /* 'h', 'l', or 'L' for integer fields */
    

    
    for (str=buf ; *fmt ; ++fmt) {
        if (*fmt != '%') {
            *str++ = *fmt;
            continue;
        }
            
        /* process flags */
        flags = 0;
        repeat:
            ++fmt;        /* this also skips first '%' */
            switch (*fmt) {
                case '-': flags |= LEFT; goto repeat;
                case '+': flags |= PLUS; goto repeat;
                case ' ': flags |= SPACE; goto repeat;
                case '#': flags |= SPECIAL; goto repeat;
                case '0': flags |= ZEROPAD; goto repeat;
                }
        
        /* get field width */
        field_width = -1;
        if (isdigit(*fmt))
            field_width = skip_atoi(&fmt);
        else if (*fmt == '*') {
            ++fmt;
            /* it's the next argument */
            field_width = va_arg(args, int);
            if (field_width < 0) {
                field_width = -field_width;
                flags |= LEFT;
            }
        }

        /* get the precision */
        precision = -1;
        if (*fmt == '.') {
            ++fmt;    
            if (isdigit(*fmt))
                precision = skip_atoi(&fmt);
            else if (*fmt == '*') {
                ++fmt;
                /* it's the next argument */
                precision = va_arg(args, int);
            }
            if (precision < 0)
                precision = 0;
        }

        /* get the conversion qualifier */
        qualifier = -1;
        if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L' || *fmt =='Z') {
            qualifier = *fmt;
            ++fmt;
        }

        /* default base */
        base = 10;

        switch (*fmt) {
        case 'c':
            if (!(flags & LEFT))
                while (--field_width > 0)
                    *str++ = ' ';
            *str++ = (unsigned char) va_arg(args, int);
            while (--field_width > 0)
                *str++ = ' ';
            continue;

        case 's':
            s = va_arg(args, char *);
            if (!s)
                s = "<NULL>";

            len = strnlen(s, precision);

            if (!(flags & LEFT))
                while (len < field_width--)
                    *str++ = ' ';
            for (i = 0; i < len; ++i)
                *str++ = *s++;
            while (len < field_width--)
                *str++ = ' ';
            continue;

        case 'p':
            if (field_width == -1) {
                field_width = 2*sizeof(void *);
                flags |= ZEROPAD;
            }
            str = number(str,
                (unsigned long) va_arg(args, void *), 16,
                field_width, precision, flags);
            continue;


        case 'n':
            if (qualifier == 'l') {
                long * ip = va_arg(args, long *);
                *ip = (str - buf);
            } else {
                int * ip = va_arg(args, int *);
                *ip = (str - buf);
            }
            continue;

        case '%':
            *str++ = '%';
            continue;

        /* integer number formats - set up the flags and "break" */
        case 'o':
            base = 8;
            break;

        case 'X':
            flags |= LARGE;
        case 'x':
            base = 16;
            break;

        case 'd':
        case 'i':
            flags |= SIGN;
        case 'u':
            break;

        default:
            *str++ = '%';
            if (*fmt)
                *str++ = *fmt;
            else
                --fmt;
            continue;
        }
        if (qualifier == 'L')
            num = va_arg(args, long);
        else if (qualifier == 'l') {
            num = va_arg(args, unsigned long);
            if (flags & SIGN)
                num = (signed long) num;
        } else if (qualifier == 'h') {
            num = (unsigned short) va_arg(args, int);
            if (flags & SIGN)
                num = (signed short) num;
        } else {
            num = va_arg(args, unsigned int);
            if (flags & SIGN)
                num = (signed int) num;
        }
        str = number(str, num, base, field_width, precision, flags);
    }
    *str = '\0';
    return str-buf;
}


int printk(const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    vsprintf(buff, fmt, args);
    va_end(args);

    return putstr(buff);
}

int color_printk(int forecolor, int background, int bright, const char *fmt, ...)
{
    va_list args;
    unsigned char backup = cursor.printColor;
    int i;
    cursor.printColor = forecolor | (background << 4) | (bright << 3);

    va_start(args, fmt);
    vsprintf(buff, fmt, args);
    i = putstr(buff);
    va_end(args);

    cursor.printColor = backup;
    return i;
}


int sprintf(char * buf, const char *fmt, ...)
{
    va_list args;
    int i;

    va_start(args, fmt);
    i = vsprintf(buf, fmt, args);
    va_end(args);
    return i;
}