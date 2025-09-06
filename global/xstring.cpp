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

#include "xstring.h"

char* x_strchr(char* str, char c)
{
    while (true) {
        if (*str == 0x1F) {
            str += 2;
            continue;
        }

        if (*str == c) {
            return str;
        }

        if (*str++ == 0) {
            return 0;
        }
    }
}

int x_strlen(const char* str)
{
    int count = 0;

    while (*str != 0) {
        if (*str == 0x1F) {
            str += 2;
            continue;
        }

        count++;
        str++;
    }

    return count;
}

int x_strsize(const char* str)
{
    int count = 0;

    while (str[count] != 0)
        if (str[count++] == 0x1F) {
            count++;
        }

    return count;
}

void x_strcpy(char* dst, const char* src)
{
    while (true) {
        if (*src == 0x1F) {
            *dst++ = *src++;
            *dst++ = *src++;
            continue;
        }

        if ((*dst++ = *src++) == 0) {
            return;
        }
    }
}
