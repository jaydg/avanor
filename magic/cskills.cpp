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

#include <cmath>
#include <iostream>
#include <utility>

#include <sol/sol.hpp>

#include "magic/cskills.h"

std::vector<CombatSkillStats> combat_skills;

const CombatSkillStats* FindCombatSkill(const COMBAT_SKILL& cs)
{
    for (const auto& row : combat_skills) {
        if (row.id == cs) {
            return &row;
        }
    }

    return nullptr;
}

// Reads a Lua list of numbers into a bonus row, padded to MAX_LEVEL + 1
// with its own last value so a short row means "and no better after that".
static std::vector<int> ReadRow(const sol::table& rows)
{
    std::vector<int> out;

    for (size_t i = 1; i <= rows.size(); i++) {
        out.push_back(rows[i]);
    }

    if (out.empty()) {
        out.push_back(0);
    }

    while (out.size() <= static_cast<size_t>(XCombatSkills::MAX_LEVEL)) {
        out.push_back(out.back());
    }

    return out;
}

CombatSkillBuilder::CombatSkillBuilder(std::string id)
{
    t.id = std::move(id);
}

CombatSkillBuilder& CombatSkillBuilder::Called(const std::string& name)
{
    t.name = name;
    return *this;
}

CombatSkillBuilder& CombatSkillBuilder::Group(const CombatSkillStats::Group group)
{
    t.group = group;
    return *this;
}

CombatSkillBuilder& CombatSkillBuilder::Role(const CombatRole role)
{
    t.role = role;
    return *this;
}

CombatSkillBuilder& CombatSkillBuilder::Defence(const sol::table& rows)
{
    t.base_dv = ReadRow(rows);
    return *this;
}

CombatSkillBuilder& CombatSkillBuilder::ToHit(const sol::table& rows)
{
    t.base_hit = ReadRow(rows);
    return *this;
}

CombatSkillBuilder& CombatSkillBuilder::Damage(const sol::table& rows)
{
    t.base_dmg = ReadRow(rows);
    return *this;
}

void CombatSkillBuilder::Register()
{
    if (t.id.empty()) {
        std::cerr << "world: a combat skill with no id" << std::endl;
        return;
    }

    if (FindCombatSkill(t.id)) {
        std::cerr << "world: two combat skills both called '" << t.id << "'"
                  << std::endl;
        return;
    }

    if (t.name.empty()) {
        t.name = t.id;
    }

    // A row never stated is a row of zeroes - a shield adds no damage.
    const sol::table nothing;

    if (t.base_dv.empty()) {
        t.base_dv.assign(XCombatSkills::MAX_LEVEL + 1, 0);
    }

    if (t.base_hit.empty()) {
        t.base_hit.assign(XCombatSkills::MAX_LEVEL + 1, 0);
    }

    if (t.base_dmg.empty()) {
        t.base_dmg.assign(XCombatSkills::MAX_LEVEL + 1, 0);
    }

    combat_skills.push_back(std::move(t));
}

int XCombatSkills::GetN(const int level)
{
    //30, 26
    return 10 * std::lround((float)(25.0 * (pow(M_E, level / 4.0) - 1)));
}

const std::string& XCombatSkills::GetName(const COMBAT_SKILL& cs)
{
    static const std::string nothing;
    const CombatSkillStats* row = FindCombatSkill(cs);
    return row ? row->name : nothing;
}

int XCombatSkills::GetLevel(const COMBAT_SKILL& cs) const
{
    const auto it = levels.find(cs);
    return it == levels.end() ? 0 : it->second;
}

int XCombatSkills::GetMarks(const COMBAT_SKILL& cs) const
{
    const auto it = marks_counter.find(cs);

    // Never practised: the whole of the first level still stands between
    // this creature and any progress, same as a fresh XCombatSkills.
    return (it == marks_counter.end() ? GetN(1) : it->second) / 10;
}

// The bonus a row gives at this creature's level in it.
static int BonusAt(const std::vector<int>& row, const int level)
{
    if (row.empty()) {
        return 0;
    }

    const size_t at = static_cast<size_t>(level) < row.size()
        ? static_cast<size_t>(level) : row.size() - 1;

    return row[at];
}

int XCombatSkills::GetDV(const COMBAT_SKILL& cs) const
{
    const CombatSkillStats* row = FindCombatSkill(cs);
    return row ? BonusAt(row->base_dv, GetLevel(cs)) : 0;
}

int XCombatSkills::GetHIT(const COMBAT_SKILL& cs) const
{
    const CombatSkillStats* row = FindCombatSkill(cs);
    return row ? BonusAt(row->base_hit, GetLevel(cs)) : 0;
}

int XCombatSkills::GetDMG(const COMBAT_SKILL& cs) const
{
    const CombatSkillStats* row = FindCombatSkill(cs);
    return row ? BonusAt(row->base_dmg, GetLevel(cs)) : 0;
}

int XCombatSkills::GetUseTime(const COMBAT_SKILL& cs) const
{
    return 1000 - GetLevel(cs) * 30;
}

COMBAT_SKILL XCombatSkills::Best(const CombatRole role) const
{
    if (role == CombatRole::NONE) {
        return CS_NONE;
    }

    COMBAT_SKILL best;
    int best_level = -1;

    for (const auto& row : combat_skills) {
        if (row.role != role) {
            continue;
        }

        // Ties go to whichever content declared first, so the answer is
        // stable rather than depending on map order.
        if (const int lvl = GetLevel(row.id); lvl > best_level) {
            best_level = lvl;
            best = row.id;
        }
    }

    return best;
}

void XCombatSkills::SetLevel(const COMBAT_SKILL& cs, const int level)
{
    if (cs.empty()) {
        return;
    }

    marks_counter[cs] = GetN(level + 1);
    levels[cs] = level;
}

void XCombatSkills::UseSkill(const COMBAT_SKILL& cs, const int time)
{
    if (cs.empty()) {
        return;
    }

    if (marks_counter.find(cs) == marks_counter.end()) {
        marks_counter[cs] = GetN(1);
    }

    marks_counter[cs] -= time * 10;

    if (marks_counter[cs] <= 0) {
        if (levels[cs] < MAX_LEVEL) {
            levels[cs]++;
            marks_counter[cs] += GetN(levels[cs]);
        }
    }
}

void XCombatSkills::RegisterLua(sol::state_view& lua)
{
    // The jobs the engine needs some skill for. It never names a skill of
    // its own; content says which of its skills answers for each.
    lua.new_enum("CombatRole",
        "UNARMED", CombatRole::UNARMED,
        "SHIELD", CombatRole::SHIELD,
        "THROW", CombatRole::THROW
    );

    // Where the skill screen lists one.
    lua.new_enum("CombatGroup",
        "MELEE", CombatSkillStats::Group::MELEE,
        "MISSILE", CombatSkillStats::Group::MISSILE,
        "SHIELD", CombatSkillStats::Group::SHIELD
    );

    lua.new_usertype<CombatSkillBuilder>("CombatSkill",
        sol::constructors<CombatSkillBuilder(std::string)>(),
        "Called", &CombatSkillBuilder::Called,
        "Group", &CombatSkillBuilder::Group,
        "Role", &CombatSkillBuilder::Role,
        "Defence", &CombatSkillBuilder::Defence,
        "ToHit", &CombatSkillBuilder::ToHit,
        "Damage", &CombatSkillBuilder::Damage,
        "Register", &CombatSkillBuilder::Register
    );
}
