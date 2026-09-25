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

#ifndef XSTRING_H
#define XSTRING_H

#include <string>
#include <vector>

int x_strlen(const char* str);

// Breaks prose into lines no wider than `width` as it will be drawn,
// splitting at spaces. Measured with x_strlen(), so a word carrying a
// role - "<WARNING>careful" - is counted at the width it draws and not
// the width it is written.
//
// A word wider than the whole line is left on one of its own rather than
// broken: prose does not contain such words, and running on is the lesser
// wrong when something else does.
std::vector<std::string> WrapText(const std::string& text, int width);

#endif
