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

#include <sol/sol.hpp>

#include "helpers/strproc.h"
#include "magic/stats.h"

void RegisterStatsEnum(sol::state_view& lua)
{
    lua.new_enum("STATS",
        "S_STR", S_STR,
        "S_DEX", S_DEX,
        "S_TOU", S_TOU,
        "S_LEN", S_LEN,
        "S_WIL", S_WIL,
        "S_MAN", S_MAN,
        "S_PER", S_PER,
        "S_CHR", S_CHR,
        "S_EOF", S_EOF
    );
}

const char* stats_str[] = {
    "St",
    "Dx",
    "To",
    "Le",
    "Wi",
    "Ma",
    "Pe",
    "Ch",
    "EOF"
};

const char* stats_full_str[] = {
    "Strength",
    "Dexterity",
    "Toughness",
    "Learning",
    "Willpower",
    "Mana",
    "Perception",
    "Charisma",
    "EOF"
};

XStats::XStats()
{
    for (int i = S_STR; i < S_EOF; i++) {
        stats[i] = 0;
    }
}

XStats::XStats(const XStats* xs)
{
    if (xs)
        for (int i = S_STR; i < S_EOF; i++) {
            stats[i] = xs->Get(static_cast<STATS>(i));
        }
}

XStats::XStats(const char* str)
{
    for (int i = S_STR; i < S_EOF; i++) {
        stats[i] = 0;
    }

    Set(str);
}

void XStats::Set(const char* str)
{
    XStringProcEx xsp(str);

    for (auto [keyword_index, dice]: xsp.GetPairsList()) {
        stats[keyword_index] += dice.Throw() * 100;
    }
}

const char* XStats::GetFullName(STATS s)
{
    return stats_full_str[s];
}

void XStats::Add(const XStats* s)
{
    for (int i = S_STR; i < S_EOF; i++) {
        stats[i] += s->stats[i];
    }
}

void XStats::Sub(const XStats* s)
{
    for (int i = S_STR; i < S_EOF; i++) {
        stats[i] -= s->stats[i];
    }
}

void XStats::Set(const XStats* s)
{
    for (int i = S_STR; i < S_EOF; i++) {
        stats[i] = s->stats[i];
    }
}

bool XStats::isEqual(const XStats* s) const
{
    for (int i = S_STR; i < S_EOF; i++)
        if (stats[i] != s->stats[i]) {
            return false;
        }

    return true;
}

XStatsGenerator::XStatsGenerator()
{
    for (int i = S_STR; i < S_EOF; i++) {
        stats[i].Setup(0, 0, 0);
    }
}

void XStatsGenerator::Init(const char* str)
{
    XStringProcEx xsp(str);

    for (auto [keyword_index, dice]: xsp.GetPairsList()) {
        stats[keyword_index].Setup(dice);
    }
}

std::unique_ptr<XStats> XStatsGenerator::Generate()
{
    auto s = std::make_unique<XStats>();

    for (int i = S_STR; i < S_EOF; i++) {
        s->SetStat(static_cast<STATS>(i), stats[i].Throw());
    }

    return s;
}
