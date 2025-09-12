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

#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>
#include <cstring>

#include "engine/global.h"

unsigned long total_cr = 0;
unsigned long total_it = 0;
unsigned long cr_kiled = 0;
unsigned long cr_died = 0;

const char* color_convert_table[] = {
    "\x1F\x00", "\x1F\x01", "\x1F\x02", "\x1F\x03",
    "\x1F\x04", "\x1F\x05", "\x1F\x06", "\x1F\x07",
    "\x1F\x08", "\x1F\x09", "\x1F\x0A", "\x1F\x0B",
    "\x1F\x0C", "\x1F\x0D", "\x1F\x0E", "\x1F\x0F",
};

int __animation_flag = 100;

#ifndef XWIN32
    char* vscreen;
#endif

long size_x = 80;
long size_y = 25;

int cursor_pos_x = 0;
int cursor_pos_y = 0;
int current_attr = 7;

#ifdef XWIN32
    #include <windows.h>
    HANDLE hStdout;
    HANDLE hStdin;
    CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
    CONSOLE_CURSOR_INFO ccInfo;
    CHAR_INFO* vscreenw;
#endif

void vInit()
{
#ifdef XLINUX
    mkdir(vMakePath(HOME_DIR, ""), 0755);
#endif

#ifdef XWIN32
    hStdin = GetStdHandle(STD_INPUT_HANDLE);
    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    GetConsoleCursorInfo(hStdout, &ccInfo);

    GetConsoleScreenBufferInfo(hStdout, &csbiInfo);
    size_x = csbiInfo.srWindow.Right - csbiInfo.srWindow.Left + 1;
    size_y = csbiInfo.srWindow.Bottom - csbiInfo.srWindow.Top + 1;

    if (size_x != csbiInfo.dwSize.X || size_y != csbiInfo.dwSize.Y) {
        COORD coord = { (short)size_x, (short)size_y };

        if (!SetConsoleScreenBufferSize(hStdout, coord)) {
            assert(false);
        }
    }

    vscreenw = new CHAR_INFO[size_x * size_y];

#endif //XWIN32

#ifdef XLINUX
    initscr();
    cbreak();
    noecho();
    nonl();
    raw();
    start_color();

    init_pair(0, COLOR_BLACK, COLOR_BLACK);
    init_pair(xBLUE, COLOR_BLUE, COLOR_BLACK);
    init_pair(xGREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(xRED, COLOR_RED, COLOR_BLACK);
    init_pair(xMAGENTA, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(xCYAN, COLOR_CYAN, COLOR_BLACK);
    init_pair(xBROWN, COLOR_YELLOW, COLOR_BLACK);
    init_pair(xLIGHTGRAY, COLOR_WHITE, COLOR_BLACK);

    init_pair(xLIGHTBLUE, COLOR_BLUE, COLOR_BLACK);
    init_pair(xLIGHTGREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(xLIGHTRED, COLOR_RED, COLOR_BLACK);
    init_pair(xLIGHTMAGENTA, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(xLIGHTCYAN, COLOR_CYAN, COLOR_BLACK);
    init_pair(xYELLOW, COLOR_YELLOW, COLOR_BLACK);
    init_pair(xWHITE, COLOR_WHITE, COLOR_BLACK);

    size_x = COLS;
    size_y = LINES;

#endif //XLINUX

#ifndef XWIN32
    vscreen = new char[size_x * size_y * 2];
#endif

    vClrScr();
}

void vClrScr()
{
#ifdef XWIN32
    CHAR_INFO blank_char = { ' ', 7 };
    int screenbuf_size = size_x * size_y;

    for (int i = 0; i < screenbuf_size; i++) {
        vscreenw[i] = blank_char;
    }
#else
    for (int i = 0; i < size_x * size_y * 2; i += 2) {
        vscreen[i] = 32;
        vscreen[i + 1] = 7;
    }
#endif
}

void vFinit()
{
#ifdef XWIN32
    SetConsoleScreenBufferSize(hStdout, csbiInfo.dwSize);
    SetConsoleWindowInfo(hStdout, true, &csbiInfo.srWindow);
    SetConsoleCursorInfo(hStdout, &ccInfo);
    delete[] vscreenw;
#endif
#ifdef XLINUX
    endwin();
#endif
#ifndef XWIN32
    delete[] vscreen;
#endif
}

void vRefresh()
{
#ifdef XWIN32
    COORD buffer_size = { (short)size_x, (short)size_y };
    COORD buffer_coord = { 0, 0 };
    SMALL_RECT write_region = { 0, 0, size_x - 1, size_y - 1 };

    WriteConsoleOutput(hStdout, vscreenw, buffer_size, buffer_coord, &write_region);
#endif

#ifdef XLINUX
    move(0, 0);

    for (int i = 0; i < size_x * size_y; i++) {
        char ch2 = vscreen[i * 2 + 1];
        char ch1 = vscreen[i * 2];

        if (ch1 < ' ') {
            ch1 = ' ';
        }

        if (ch2 > 7) {
            addch(ch1 | (ch2 << 8) | A_BOLD);
        } else {
            addch(ch1 | (ch2 << 8));
        }
    }

    refresh();
#endif
}

void vPutCh(int x, int y, char ch, int attr)
{
    assert(x >= 0 && y >= 0 && x <= size_x && y <= size_y);
#ifdef XWIN32
    CHAR_INFO tmp = { ch, attr };
    vscreenw[x + y * size_x] = tmp;
#else
    vscreen[x * 2 + y * size_x * 2] = ch;
    vscreen[x * 2 + y * size_x * 2 + 1] = attr;
#endif
};

char vTestCh(int x, int y)
{
#ifdef XWIN32
    return vscreenw[x + y * size_x].Char.AsciiChar;
#else
    return vscreen[x * 2 + y * size_x * 2];
#endif
}

void vPutCh(int x, int y, char ch)
{
#ifdef XWIN32
    vscreenw[x + y * size_x].Char.AsciiChar = ch;
#else
    vscreen[x * 2 + y * size_x * 2] = ch;
#endif
};

void vGotoXY(int x, int y)
{
    cursor_pos_x = x;
    cursor_pos_y = y;
}

void vGetCursorPos(int* x, int* y)
{
    *x = cursor_pos_x;
    *y = cursor_pos_y;
}

int vGetS(char* s, int buffer_size)
{
    int cx;
    int cy;
    vGetCursorPos(&cx, &cy);
    int buffer_pos = 0;
    char ch;
    strcpy(s, "");

    do {
        vXGotoXY(cx + strlen(s), cy);
        ch = vGetch();

        s[buffer_pos] = ch;

        if (ch == 13 || ch == 27) {
            s[buffer_pos] = 0;

            if (ch == 13) {
                return 1;
            } else {
                return 0;
            }
        }

        if (ch == 8 && buffer_pos > 0) {
            s[buffer_pos - 1] = ' ';
            s[buffer_pos] = 0;
            vGotoXY(cx, cy);
            vPutS(s);
            buffer_pos--;
        } else if (ch != 8) {
            buffer_pos++;
        }

        s[buffer_pos] = 0;
        vGotoXY(cx, cy);
        vPutS(s);
        vRefresh();
    } while (buffer_pos < buffer_size - 1);

    return 1;
}

void vDelay(int n)
{
#ifdef XWIN32

#endif
}

int vKbhit()
{
#ifdef XLINUX
    timeout(0);
    int ch = getch();
    timeout(-1);

    if (ch == ERR) {
        return 0;
    }

    ungetch(ch);
    return 1;
#else
    return kbhit();
#endif
}

int vGetch()
{
#ifdef XLINUX
    int ch = getch();

    if (ch != 27) {
        return ch;
    }

    timeout(0);
    ch = getch();
    timeout(-1);

    if (ch == ERR) {
        return KEY_ESC;
    }

    if (ch != '[') {
        ungetch(ch);
        return KEY_ESC;
    }

    switch (getch()) {
        case 'A':
            return KEY_UP;

        case 'B':
            return KEY_DOWN;

        case 'C':
            return KEY_RIGHT;

        case 'D':
            return KEY_LEFT;

        case 'G':
            return KEY_CENTER;

        case '1':
            return getch() == '~' ? KEY_HOME : KEY_UNKNOWN;

        case '4':
            return getch() == '~' ? KEY_END : KEY_UNKNOWN;

        case '5':
            return getch() == '~' ? KEY_PGUP : KEY_UNKNOWN;

        case '6':
            return getch() == '~' ? KEY_PGDOWN : KEY_UNKNOWN;
    }

    return KEY_UNKNOWN;
#else
    int ch = getch();

    if (ch == 0 || ch == 224) {
        ch = KEY_EXTENDED_CODE | getch();
    }

    return ch;
#endif
}

int vXGetch(char* ch_buf)
{
    int slen = strlen(ch_buf);

    while (1) {
        int ch = vGetch();

        for (int i = 0; i < slen; i++) {
            if (ch == ch_buf[i]) {
                return ch;
            }
        }

        if (ch == KEY_ESC) {
            return 0;
        }
    }
}

void vXGotoXY(int x, int y)
{
    cursor_pos_x = x;
    cursor_pos_y = y;

#ifdef XWIN32
    CONSOLE_CURSOR_INFO ConsoleCursorInfo = ccInfo;
    ConsoleCursorInfo.bVisible = TRUE;
    SetConsoleCursorInfo(hStdout, &ConsoleCursorInfo);

    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(hStdout, coord);
#endif

#ifdef XLINUX
    move(y, x);
#endif
}

void vHideCursor()
{
#ifdef XWIN32
    //	CONSOLE_CURSOR_INFO ConsoleCursorInfo = ccInfo;
    //	ConsoleCursorInfo.bVisible = FALSE;
    //	SetConsoleCursorInfo(hStdout, &ConsoleCursorInfo);
#endif

#ifdef XLINUX
    vXGotoXY(size_x, size_y);
#endif
}

void vSetAttr(int color)
{
    current_attr = color;
}

void vClrEol()
{
    while (cursor_pos_x < size_x) {
        vPutCh(cursor_pos_x, cursor_pos_y, ' ');
        cursor_pos_x++;
    }
}

// (31, x) - change attr
void vPutS(const char* s)
{
    if (cursor_pos_y >= size_y) {
        cursor_pos_x = cursor_pos_y = 0;
    }

    while (true) {
        char ch = *s++;

        switch (ch) {
            case 0 :
                return;

            case 31 :
                vSetAttr(*s++);
                break;

            case 13 :
            case '\n' :
                cursor_pos_x = 0;

                if (++cursor_pos_y >= size_y) {
                    cursor_pos_y = 0;
                }

                break;

            default : {
                if (cursor_pos_x >= size_x) {
                    cursor_pos_x = 0;

                    if (++cursor_pos_y >= size_y) {
                        cursor_pos_y = 0;
                    }
                }

                vPutCh(cursor_pos_x++, cursor_pos_y, ch, current_attr);
            }
        }
    }
}

void vFPutS(FILE * f, const char* s)
{
    int pos = 0;

    while (s[pos]) {
        if (s[pos] == 31) {
            pos += 2;
            continue;
        }

        fprintf(f, "%c", s[pos]);
        pos++;
    }
}

static unsigned long randx;

void vRandSeed(unsigned long seed)
{
    randx = seed;
}

#define X_RAND_A   0x000343FDUL
#define X_RAND_B   0x00269EC3UL
#define X_RAND_MAX 0x7FFFFFFFUL

long vRand()
{
    randx = randx * X_RAND_A + X_RAND_B;
    unsigned long randhigh = randx & 0x7FFF0000;
    randx = randx * X_RAND_A + X_RAND_B;
    unsigned long randlow = randx >> 16;
    return randhigh | randlow;
}

long vRand(unsigned long n)
{
    assert(n > 0);

    if (n > 0x8000) {
        assert(n <= X_RAND_MAX + 1);
        randx = randx * X_RAND_A + X_RAND_B;
        unsigned long randhigh = randx & 0x7FFF0000;
        randx = randx * X_RAND_A + X_RAND_B;
        unsigned long randlow = randx >> 16;
        return (randhigh | randlow) % n;
    } else {
        randx = randx * X_RAND_A + X_RAND_B;
        return (randx >> 16) % n;
    }
}

V_BUFFER::V_BUFFER()
{
#ifdef XWIN32
    buffer = new char[size_x * size_y * sizeof(CHAR_INFO)];
#else
    buffer = new char[size_x * size_y * 2];
#endif
}

V_BUFFER::~V_BUFFER()
{
    delete[] buffer;
}

void vStore(V_BUFFER * buf)
{
#ifdef XWIN32
    memcpy(buf->buffer, vscreenw, size_x * size_y * sizeof(CHAR_INFO));
#else
    memcpy(buf->buffer, vscreen, size_x * size_y * 2);
#endif
}

void vRestore(V_BUFFER * buf)
{
#ifdef XWIN32
    memcpy(vscreenw, buf->buffer, size_x * size_y * sizeof(CHAR_INFO));
#else
    memcpy(vscreen, buf->buffer, size_x * size_y * 2);
#endif
}

static char path_buffer[1024];

char* vMakePath(const char* prefix, const char* filename)
{
#ifdef XLINUX

    if (prefix[0] == '~') {
        sprintf(path_buffer, "%s%s%s", getenv("HOME"), prefix + 1, filename);
    } else
#endif
    {
        sprintf(path_buffer, "%s%s", prefix, filename);
    }

    return path_buffer;
}
