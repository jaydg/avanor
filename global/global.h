/*
This file is part of "Avanor, the Land of Mystery" roguelike game
Home page: http://www.avanor.com/
Copyright (C) 2000-2003 Vadim Gaidukevich

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef __GLOBAL_H
#define __GLOBAL_H

#ifdef WIN32
#  define XWIN32
#else
#  ifdef GO32
#     define XDOS
#  else
#     define XLINUX
#  endif
#endif
//#define XDOS

// Directory for common data files (game manual, hiscore, ...)
#ifndef DATA_DIR
#  define DATA_DIR "./"
#endif

// Directory for private data files (user settings, saved games, ...)
#ifndef HOME_DIR
#  ifdef XLINUX
#    define HOME_DIR "~/.avanor/"
#  else
#    define HOME_DIR "./"
#  endif
#endif

#include <stdlib.h>
#include <stdio.h>

#ifdef XDOS
#  include <dos.h>
#  include <conio.h>
#endif // WIN32

#ifdef XWIN32
#  include <conio.h>
#endif


#ifdef XLINUX
#  include <curses.h>
#  include <string.h>
#endif


#include <math.h>
#include <assert.h>

extern long size_x;
extern long size_y;

extern unsigned long total_cr;
extern unsigned long total_it;
extern unsigned long cr_kiled;
extern unsigned long cr_died;

//global flags (users preferences)
extern int __animation_flag; //0 - none, other - delay miliseconds

#ifndef M_E
	#define M_E 2.7182818284590452354
#endif
#ifndef M_PI
	#define M_PI 3.1415926535897932384
#endif

enum xColor {
xBLACK = 0,
xBLUE, xGREEN, xCYAN, xRED, xMAGENTA, xBROWN, xLIGHTGRAY, xDARKGRAY,
xLIGHTBLUE, xLIGHTGREEN, xLIGHTCYAN, xLIGHTRED, xLIGHTMAGENTA, xYELLOW,
xWHITE = 15
};

#define MSG_BLACK          "\x1F\x00"
#define MSG_BLUE           "\x1F\x01"
#define MSG_GREEN          "\x1F\x02"
#define MSG_CYAN           "\x1F\x03"
#define MSG_RED            "\x1F\x04"
#define MSG_MAGENTA        "\x1F\x05"
#define MSG_BROWN          "\x1F\x06"
#define MSG_LIGHTGRAY      "\x1F\x07"
#define MSG_DARKGRAY       "\x1F\x08"
#define MSG_LIGHTBLUE      "\x1F\x09"
#define MSG_LIGHTGREEN     "\x1F\x0A"
#define MSG_LIGHTCYAN      "\x1F\x0B"
#define MSG_LIGHTRED       "\x1F\x0C"
#define MSG_LIGHTMAGENTA   "\x1F\x0D"
#define MSG_YELLOW         "\x1F\x0E"
#define MSG_WHITE          "\x1F\x0F"

//next table helps to convert dynamic xCOLOR to text const char *
//that allows to create construction such next
//vPutS(MSG_YELLOW "yellow" SCOLOR(vRand(15)) "random color");

#define SCOLOR(x) color_convert_table[x]
extern const char* color_convert_table[];



//typedef char V_BUFFER[80 * 25 * 2];
struct V_BUFFER
{
	char * buffer;
	V_BUFFER();
	~V_BUFFER();
};

extern int tri_table[120][10][2]; //120 a*3, 10 - r, 2 (0 - x, 1 - y)

#undef KEY_EXTENDED_CODE
#undef KEY_ESC
#undef KEY_UP
#undef KEY_DOWN
#undef KEY_LEFT
#undef KEY_RIGHT
#undef KEY_HOME
#undef KEY_END
#undef KEY_PGUP
#undef KEY_PGDOWN
#undef KEY_CENTER

#undef KEY_ENTER
#undef KEY_BACKSPACE

#undef KEY_CTRL_Z
#undef KEY_UNKNOWN

#define KEY_EXTENDED_CODE 0x8000
#define KEY_ESC           (27)
#define KEY_UP            (KEY_EXTENDED_CODE | 72)
#define KEY_DOWN          (KEY_EXTENDED_CODE | 80)
#define KEY_LEFT          (KEY_EXTENDED_CODE | 75)
#define KEY_RIGHT         (KEY_EXTENDED_CODE | 77)
#define KEY_HOME          (KEY_EXTENDED_CODE | 71)
#define KEY_END           (KEY_EXTENDED_CODE | 79)
#define KEY_PGUP          (KEY_EXTENDED_CODE | 73)
#define KEY_PGDOWN        (KEY_EXTENDED_CODE | 81)
#define KEY_CENTER        (KEY_EXTENDED_CODE | 76)

#define KEY_ENTER         13
#define KEY_BACKSPACE     9

#define KEY_CTRL_Z		  26
#define KEY_CTRL_T		  20
#define KEY_CTRL_O		  15
#define KEY_CTRL_D		  4

#define KEY_UNKNOWN       0xFFFF

void vClrScr();
void vInit();
void vFinit();
void vRefresh();
void vPutCh(int x, int y, char ch);
void vPutCh(int x, int y, char ch, int attr);
char vTestCh(int x, int y);
void vPutS(const char * s);
void vFPutS(FILE * f, const char * s);
int vGetS(char * s, int buffer_size);
void vClrEol();
void vGotoXY(int x, int y);
void vXGotoXY(int x, int y);
void vGetCursorPos(int * x, int * y);
void vSetAttr(int color);
void vDelay(int n);
int  vKbhit();
int  vGetch();
int  vXGetch(char * ch_buf);
void vStore(V_BUFFER * buf);
void vRestore(V_BUFFER * buf);
void vHideCursor();

void vRandSeed(unsigned long seed);
long vRand();
long vRand(unsigned long n);

char *vMakePath(char *prefix, char *filename);

#endif
