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

#ifndef KEYWORD_DICE_H
#define KEYWORD_DICE_H

#include <string>
#include <string>
#include <vector>

#include "helpers/dice.h"

// One "keyword dice" entry parsed out of a data-table string.
struct KeywordDice {
    // The word as it was written - "St", "fire". Each caller resolves it
    // in its own vocabulary: stats have a fixed set, resistances are
    // whatever world/resistances.lua declared, and neither has to know
    // about the other.
    std::string keyword;
    XDice dice;
};

// Parses "keyword dice keyword dice ...", separated by spaces or colons -
// "St 2d2 Dx 2d4 To 1d1" and "fire:5d5-80 cold:3d10" are both accepted.
// A keyword this parser does not recognise is skipped, with a complaint -
// the caller sees only words that named something.
//
// Repeated keywords come back as separate entries for the caller to
// accumulate, so "St 1d1+2 St 1d1" is equivalent to "St 2d1+2".
std::vector<KeywordDice> ParseKeywordDice(const std::string& str);

// The stat a keyword names, or -1 for a word that is not a stat.
int StatKeyword(const std::string& keyword);

#endif
