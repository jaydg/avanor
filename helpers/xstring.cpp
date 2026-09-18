/*
This file is part of "Avanor, the Land of Mystery" roguelike game

Copyright (C) 2000-2006 Vadim Gaidukevich
Copyright (C) 2025,2026 Joachim de Groot

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

#include <string>

#include "engine/global.h"
#include "helpers/xstring.h"

int x_strlen(const char* str)
{
    // Measured after the roles written into it become escape bytes, so
    // that "<LABEL>Name:" is five characters wide and not thirteen.
    const std::string expanded = ExpandMarkup(str ? str : "");

    str = expanded.c_str();

    int count = 0;

    while (*str != 0) {
        if (*str == 0x1F) {
            str += 2;
            continue;
        }

        if (*str == RGB_ESCAPE) {
            str += RGB_ESCAPE_LENGTH;
            continue;
        }

        count++;
        str++;
    }

    return count;
}
