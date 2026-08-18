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

#include <cassert>
#include <cstring>

#include "engine/global.h"
#include "helpers/keyword_dice.h"
#include "magic/resist.h"
#include "magic/stats.h"

namespace {

struct Keyword {
    const char* key;
    int value;
};

const Keyword keywords[] = {
    {"St", XStats::STR},
    {"Dx", XStats::DEX},
    {"To", XStats::TOU},
    {"Le", XStats::LEN},
    {"Wi", XStats::WIL},
    {"Ma", XStats::MAN},
    {"Pe", XStats::PER},
    {"Ch", XStats::CHR},

    {"white",	XResistance::WHITE},
    {"black",	XResistance::BLACK},
    {"fire",	XResistance::FIRE},
    {"water",	XResistance::WATER},
    {"air",	XResistance::AIR},
    {"earth",	XResistance::EARTH},
    {"acid",	XResistance::ACID},
    {"cold",	XResistance::COLD},
    {"poison",	XResistance::POISON},
    {"disease",	XResistance::DISEASE},
    {"paralyse",	XResistance::PARALYSE},
    {"stun",	XResistance::STUN},
    {"confuse",	XResistance::CONFUSE},
    {"blind",	XResistance::BLIND},
    {"light",	XResistance::LIGHT},
    {"darkness",	XResistance::DARKNESS},
    {"invisible",	XResistance::INVISIBLE},
    {"see_invisible", XResistance::SEE_INVISIBLE},
};

// Exact (case-insensitive).
int KeywordValue(const std::string& keyword)
{
    for (const auto& entry : keywords) {
        if (strcasecmp(entry.key, keyword.c_str()) == 0) {
            return entry.value;
        }
    }

    assert(!"unknown keyword in a keyword/dice string");

    return 0;
}

} // namespace

std::vector<KeywordDice> ParseKeywordDice(const std::string& str)
{
    static constexpr char SEPARATORS[] = " \t:";

    std::vector<std::string> tokens;

    for (std::string::size_type pos = 0;
         (pos = str.find_first_not_of(SEPARATORS, pos)) != std::string::npos; ) {
        const auto end = str.find_first_of(SEPARATORS, pos);
        tokens.push_back(str.substr(pos, end == std::string::npos
            ? std::string::npos
            : end - pos));
        pos = (end == std::string::npos) ? str.size() : end;
    }

    // Tokens alternate keyword, dice. A trailing keyword with no dice is
    // dropped rather than parsed as a pair against whatever follows.
    std::vector<KeywordDice> pairs;
    pairs.reserve(tokens.size() / 2);

    for (std::size_t i = 0; i + 1 < tokens.size(); i += 2) {
        KeywordDice pair;
        pair.keyword_index = KeywordValue(tokens[i]);
        pair.dice.Setup(tokens[i + 1]);
        pairs.push_back(pair);
    }

    return pairs;
}
