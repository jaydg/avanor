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

#include <iostream>
#include <utility>

#include "engine/global.h"
#include "helpers/keyword_dice.h"
#include "magic/resist.h"

void XResistance::RegisterLua(sol::state_view& lua)
{
    lua.new_usertype<ResistanceBuilder>("Resistance",
        sol::constructors<ResistanceBuilder(std::string)>(),
        "Called", &ResistanceBuilder::Called,
        "Gained", &ResistanceBuilder::Gained,
        "Lost", &ResistanceBuilder::Lost,
        "Register", &ResistanceBuilder::Register
    );

    // Which resistances this world declares, so that content can walk
    // them: world/modifiers.lua mints one "resistant to X for a while"
    // modifier per row, and a world that adds a resistance to radiation
    // gets its modifier without touching anything here.
    lua["Resistance"]["All"] = [](sol::this_state state) {
        sol::state_view lua(state);
        sol::table all = lua.create_table();

        for (const auto& row : resistances_db) {
            all.add(lua.create_table_with("id", row.id, "name", row.name));
        }

        return all;
    };
}

std::vector<ResistanceStats> resistances_db;

const ResistanceStats* FindResistance(const RESISTANCE& r)
{
    for (const auto& row : resistances_db) {
        if (row.id == r) {
            return &row;
        }
    }

    return nullptr;
}

ResistanceBuilder::ResistanceBuilder(std::string id)
{
    t.id = std::move(id);
}

ResistanceBuilder& ResistanceBuilder::Called(const std::string& name)
{
    t.name = name;
    return *this;
}

ResistanceBuilder& ResistanceBuilder::Gained(const std::string& text)
{
    t.gained = text;
    return *this;
}

ResistanceBuilder& ResistanceBuilder::Lost(const std::string& text)
{
    t.lost = text;
    return *this;
}

void ResistanceBuilder::Register()
{
    if (t.id.empty()) {
        std::cerr << "world: a resistance with no id" << std::endl;
        return;
    }

    if (FindResistance(t.id)) {
        std::cerr << "world: two resistances both called '" << t.id << "'"
                  << std::endl;
        return;
    }

    if (t.name.empty()) {
        t.name = t.id;
    }

    resistances_db.push_back(std::move(t));
}


XResistance::XResistance(const XResistance* xr)
{
    Set(xr);
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

    for (const auto& row : resistances_db) {
        const std::string value = FindParam(str, row.id);

        if (!value.empty()) {
            d.Setup(value);
            SetResistance(row.id, d.Throw());
        }
    }
}

void XResistance::Set(const XResistance* r)
{
    if (r) {
        resistances = r->resistances;
    }
}

void XResistance::Add(const XResistance* r)
{
    if (r) {
        for (const auto& [id, val] : r->resistances) {
            resistances[id] += val;
        }
    }
}

void XResistance::Sub(const XResistance* r)
{
    if (r) {
        for (const auto& [id, val] : r->resistances) {
            resistances[id] -= val;
        }
    }
}

bool XResistance::isEqual(const XResistance* xr) const
{
    // Compared by what each actually carries, in both directions: a
    // resistance absent from one map and zero in the other is the same
    // thing, and neither should count as a difference.
    for (const auto& [id, val] : resistances) {
        if (val != xr->GetResistance(id)) {
            return false;
        }
    }

    for (const auto& [id, val] : xr->resistances) {
        if (val != GetResistance(id)) {
            return false;
        }
    }

    return true;
}

const std::string& XResistance::GetResistanceName(const RESISTANCE& r)
{
    static const std::string nothing;
    const ResistanceStats* row = FindResistance(r);
    return row ? row->name : nothing;
}

const char* resist_level[] = {
    "<QUALITY_POOR>awful",
    "<QUALITY_POOR>bad",
    "<QUALITY_NEUTRAL>none",
    "<QUALITY_FAIR>mediocre",
    "<QUALITY_FAIR>fair",
    "<QUALITY_FAIR>good",
    "<QUALITY_GOOD>excellent",
    "<QUALITY_PERFECT>complete"
};

const char* XResistance::GetResistanceLevel(const RESISTANCE& r) const
{
    const int val = GetResistance(r);

    if (val < -50) {
        return resist_level[0];
    } else if (val < 0) {
        return resist_level[1];
    } else if (val == 0) {
        return resist_level[2];
    } else if (val < 10) {
        return resist_level[3];
    } else if (val < 40) {
        return resist_level[4];
    } else if (val < 80) {
        return resist_level[5];
    } else if (val < 100) {
        return resist_level[6];
    } else {
        return resist_level[7];
    }
}

XResistGenerator::XResistGenerator() = default;

void XResistGenerator::Init(const char* str)
{
    for (auto [keyword, dice]: ParseKeywordDice(str)) {
        resist[keyword].Setup(dice);
    }
}

std::unique_ptr<XResistance> XResistGenerator::Generate()
{
    auto r = std::make_unique<XResistance>();

    for (auto& [id, dice] : resist) {
        r->SetResistance(id, dice.Throw());
    }

    return r;
}
