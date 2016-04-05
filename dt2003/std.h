#ifndef _STD_H
#define _STD_H

#include <windows.h>
#include <assert.h>
#include <stdio.h>

#define WINWD 640
#define WINHT 480

#define WINSIZE (WINWD*WINHT)
#define WINBYTESIZE (WINSIZE*4)

extern unsigned int screen[WINSIZE];

void box(int x1, int y1, int x2, int y2, unsigned int colour, unsigned int *pixels);

//#include "font.h"

#endif //_STD_H