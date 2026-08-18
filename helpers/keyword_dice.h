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

#ifndef KEYWORD_DICE_H
#define KEYWORD_DICE_H

#include <string>
#include <vector>

#include "helpers/dice.h"

// One "keyword dice" entry parsed out of a data-table string.
struct KeywordDice {
    int keyword_index{};
    XDice dice;
};

// Parses "keyword dice keyword dice ...", separated by spaces or colons -
// "St 2d2 Dx 2d4 To 1d1" and "fire:5d5-80 cold:3d10" are both accepted.
// Each keyword is a stat or resistance name and resolves to its
// XStats::Id / XResistance::Id value.
//
// Repeated keywords come back as separate entries for the caller to
// accumulate, so "St 1d1+2 St 1d1" is equivalent to "St 2d1+2".
std::vector<KeywordDice> ParseKeywordDice(const std::string& str);

#endif
