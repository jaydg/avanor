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

#include "engine/global.h"
#include "helpers/keyword_dice.h"
#include "magic/resist.h"

void XResistance::RegisterLua(sol::state_view& lua)
{
    lua.new_enum("XResistance",
        "WHITE", XResistance::WHITE,
        "BLACK", XResistance::BLACK,
        "FIRE", XResistance::FIRE,
        "WATER", XResistance::WATER,
        "AIR", XResistance::AIR,
        "EARTH", XResistance::EARTH,
        "ACID", XResistance::ACID,
        "COLD", XResistance::COLD,
        "POISON", XResistance::POISON,
        "DISEASE", XResistance::DISEASE,
        "PARALYSE", XResistance::PARALYSE,
        "STUN", XResistance::STUN,
        "CONFUSE", XResistance::CONFUSE,
        "BLIND", XResistance::BLIND,
        "LIGHT", XResistance::LIGHT,
        "DARKNESS", XResistance::DARKNESS,
        "INVISIBLE", XResistance::INVISIBLE,
        "SEE_INVISIBLE", XResistance::SEE_INVISIBLE
    );
}

RESIST_REC resists_data[] = {
    {"unknown",	FLU_NONE},
    {"white",	FLU_CREATURE},
    {"black",	FLU_CREATURE},
    {"fire",	FLU_ALL},
    {"water",	FLU_ALL},
    {"air",	FLU_ALL},
    {"earth",	FLU_ALL},
    {"acid",	FLU_ALL},
    {"cold",	FLU_ALL},
    {"poison",	FLU_CREATURE},
    {"disease",	FLU_CREATURE},
    {"paralyse",	FLU_CREATURE},
    {"stun",	FLU_CREATURE},
    {"confuse",	FLU_CREATURE},
    {"blind",	FLU_CREATURE},
    {"light",	FLU_CREATURE},
    {"darkness",	FLU_CREATURE},
    {"invisible",	FLU_CREATURE},
    {"see_invisible",	FLU_CREATURE},
    "eof",	FLU_NONE
};

XResistance::XResistance(const XResistance* xr)
{
    Set(xr);
}

XResistance::XResistance()
{
    for (int i = XResistance::WHITE; i < XResistance::COUNT; i++) {
        SetResistance(static_cast<XResistance::Id>(i), 0);
    }
}

namespace {

// The value token following `param` in a "name value name value" string:
// FindParam("fire 1d3 cold 2d2", "cold") is "2d2", and absent names give
// an empty string.
//
// This is all that remained of XStringProc::GetParam(), which wrote into
// a caller-supplied char[256] with no bound on the token length.
std::string FindParam(const std::string& str, const std::string& param)
{
    for (auto pos = str.find(param); pos != std::string::npos; pos = str.find(param, pos + 1)) {
        // Must start a word, so "invisible" doesn't match inside
        // "see_invisible" - the same test the old GetParam() made.
        if (pos != 0 && str[pos - 1] != ' ') {
            continue;
        }

        // Exactly one separator sits between a name and its value.
        const auto value = pos + param.size() + 1;

        if (value >= str.size()) {
            return {};
        }

        auto end = value;

        while (end < str.size() && static_cast<unsigned char>(str[end]) > ' ') {
            end++;
        }

        return str.substr(value, end - value);
    }

    return {};
}

} // namespace

XResistance::XResistance(const char* str1)
{
    const std::string str = str1 ? str1 : "";
    XDice d;

    for (int i = XResistance::WHITE; i < XResistance::COUNT; i++) {
        const std::string value = FindParam(str, resists_data[i + 1].name);

        if (value.empty()) {
            SetResistance(static_cast<XResistance::Id>(i), 0);
        } else {
            d.Setup(value);
            SetResistance(static_cast<XResistance::Id>(i), d.Throw());
        }
    }
}

void XResistance::Set(const XResistance* r)
{
    if (r)
        for (int i = XResistance::WHITE; i < XResistance::COUNT; i++) {
            SetResistance(static_cast<XResistance::Id>(i), r->GetResistance(static_cast<XResistance::Id>(i)));
        }
}

void XResistance::Add(const XResistance* r)
{
    if (r)
        for (int i = XResistance::WHITE; i < XResistance::COUNT; i++) {
            resistances[i] += r->resistances[i];
        }
}

void XResistance::Sub(const XResistance* r)
{
    if (r)
        for (int i = XResistance::WHITE; i < XResistance::COUNT; i++) {
            resistances[i] -= r->resistances[i];
        }
};

bool XResistance::isEqual(const XResistance* xr) const
{
    for (int i = XResistance::WHITE; i < XResistance::COUNT; i++)
        if (resistances[i] != xr->resistances[i]) {
            return false;
        }

    return true;
}

const char* resist_name[] = {
    "White magic",
    "Black magic",
    "Fire magic",
    "Water magic",
    "Air magic",
    "Earth magic",
    "Acid",
    "Cold",
    "Poison",
    "Disease",
    "Paralyzation",
    "Stun",
    "Confusion",
    "Blindness",
    "Light",
    "Darkness",
    "Invisible",
    "See Invisible"
};

const char* XResistance::GetResistanceName(const XResistance::Id r)
{
    return resist_name[r];
}

const char* resist_level[] = {
    MSG_LIGHTRED "awful",
    MSG_LIGHTRED "bad",
    MSG_LIGHTGRAY "none",
    MSG_LIGHTGREEN "mediocre",
    MSG_LIGHTGREEN "fair",
    MSG_LIGHTGREEN "good",
    MSG_YELLOW "excellent",
    MSG_WHITE "complete"
};

const char* XResistance::GetResistanceLevel(const XResistance::Id r) const
{
    if (resistances[r] < -50) {
        return resist_level[0];
    } else if (resistances[r] < 0) {
        return resist_level[1];
    } else if (resistances[r] == 0) {
        return resist_level[2];
    } else if (resistances[r] < 10) {
        return resist_level[3];
    } else if (resistances[r] < 40) {
        return resist_level[4];
    } else if (resistances[r] < 80) {
        return resist_level[5];
    } else if (resistances[r] < 100) {
        return resist_level[6];
    } else {
        return resist_level[7];
    }
}

XResistGenerator::XResistGenerator()
{
    for (int i = XResistance::WHITE; i < XResistance::COUNT; i++) {
        resist[i].Setup(0, 0, 0);
    }
}

void XResistGenerator::Init(const char* str)
{
    for (auto [keyword_index, dice]: ParseKeywordDice(str)) {
        resist[keyword_index].Setup(dice);
    }
}

std::unique_ptr<XResistance> XResistGenerator::Generate()
{
    auto r = std::make_unique<XResistance>();

    for (int i = XResistance::WHITE; i < XResistance::COUNT; i++) {
        r->SetResistance(static_cast<XResistance::Id>(i), resist[i].Throw());
    }

    return r;
}
