#include "print.h"
#include "string.h"

typedef struct
{
    char c;
    unsigned char color;
} ScreenChar;

typedef struct
{
    int xPos, yPos;
    int xRes, yRes;
    ScreenChar *videoMem;
    unsigned char printColor;
} Cursor;


static ScreenChar *const videoMemBase = (ScreenChar *const) 0xffff8000000b8000;
static Cursor cursor;


int putline(const char *str)
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
            cursor.videoMem += cursor.xRes;
        /* fall through: treat '\n' as '\r' + '\n' */
        case '\r':
            cursor.videoMem -= cursor.xPos;
            cursor.xPos = 0;
            break;
        case '\b':
            cursor.videoMem->c = 0;
            cursor.videoMem->color = 0;
            if(cursor.videoMem > videoMemBase)
            {
                cursor.videoMem--;
            }
            cursor.xPos--;
            if(cursor.xPos < 0)
            {
                if(cursor.yPos > 0) cursor.yPos--;
                cursor.xPos = 0;
            }
            break;
        default:
            cursor.videoMem->c = c;
            cursor.videoMem->color = cursor.printColor;
            cursor.videoMem++; 
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
    cursor.videoMem -= charCnt;
}


void moveCursor(int xPos, int yPos)
{
    cursor.xPos = xPos;
    cursor.yPos = yPos;
    cursor.videoMem = videoMemBase + xPos + yPos * cursor.xRes;
}

void setCursorRes(int xRes, int yRes)
{
    cursor.xRes = xRes;
    cursor.yRes = yRes;
}

void showCursor()
{
    cursor.videoMem->color = 0x8f; // bright white and blinking
    cursor.videoMem->c = '_';
}

void hideCursor()
{
    cursor.videoMem->color = 0;
    cursor.videoMem->c = 0;
}

void setPrintColor(unsigned char color)
{
    cursor.printColor = color;
}