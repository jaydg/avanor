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

#include <algorithm>
#include <iostream>

#include <cereal/archives/json.hpp>

#include "helpers/msgwin.h"
#include "magic/modifier.h"
#include "magic/modifiers.h"
#include "magic/skills.h"

namespace {

std::vector<ModifierStats> modifiers_db;

}

const ModifierStats* FindModifier(const MODIFIER& id)
{
    for (const auto& row : modifiers_db) {
        if (row.id == id) {
            return &row;
        }
    }

    return nullptr;
}

const std::vector<ModifierStats>& AllModifiers()
{
    return modifiers_db;
}

ModifierBuilder::ModifierBuilder(MODIFIER id)
{
    t.id = std::move(id);
}

ModifierBuilder& ModifierBuilder::Called(const std::string& text)
{
    t.names.push_back({0, text});
    return *this;
}

ModifierBuilder& ModifierBuilder::Severity(const int upto, const std::string& text)
{
    t.names.push_back({upto, text});
    return *this;
}

ModifierBuilder& ModifierBuilder::OnSet(const std::string& text)
{
    t.set_msg = text;
    return *this;
}

ModifierBuilder& ModifierBuilder::OnRemove(const std::string& text)
{
    t.remove_msg = text;
    return *this;
}

ModifierBuilder& ModifierBuilder::OnChange(const std::string& grows, const std::string& fades)
{
    t.grows_msg = grows;
    t.fades_msg = fades;
    return *this;
}

ModifierBuilder& ModifierBuilder::Applies(const std::string& text)
{
    t.apply_msg = text;
    return *this;
}

// What shifts while it is on: :While{DV = -5, HIT = -10, Slower = 300,
// Stats = {[XStats.STR] = -3}, Resistance = {fire = 40}}. Whatever is
// named here is applied when it takes hold and taken back when it goes,
// so no modifier has to remember to undo itself.
ModifierBuilder& ModifierBuilder::While(const sol::table& shifts)
{
    t.dv = shifts.get_or("DV", 0);
    t.hit = shifts.get_or("HIT", 0);
    t.slower = shifts.get_or("Slower", 0);

    if (const sol::optional<sol::table> stats = shifts["Stats"]) {
        for (const auto& [key, value] : *stats) {
            t.stats.emplace_back(static_cast<XStats::Id>(key.as<int>()), value.as<int>());
        }
    }

    if (const sol::optional<sol::table> resists = shifts["Resistance"]) {
        for (const auto& [key, value] : *resists) {
            t.resistances.emplace_back(key.as<std::string>(), value.as<int>());
        }
    }

    return *this;
}

ModifierBuilder& ModifierBuilder::Scale(const int scale)
{
    t.scale = scale;
    return *this;
}

ModifierBuilder& ModifierBuilder::ResistedBy(const std::string& resist)
{
    t.resisted_by = resist;
    return *this;
}

ModifierBuilder& ModifierBuilder::Engine(const std::string& which)
{
    t.engine = which;
    return *this;
}

void ModifierBuilder::Register()
{
    if (t.id.empty()) {
        std::cerr << "world: a modifier with no id" << std::endl;
        return;
    }

    if (FindModifier(t.id)) {
        std::cerr << "world: two modifiers both called '" << t.id << "'" << std::endl;
        return;
    }

    if (!t.resisted_by.empty() && !FindResistance(t.resisted_by)) {
        std::cerr << "world: the modifier '" << t.id << "' is resisted by '"
                  << t.resisted_by << "', which world/resistances.lua does not declare"
                  << std::endl;
    }

    for (const auto& [resist, amount] : t.resistances) {
        if (!FindResistance(resist)) {
            std::cerr << "world: the modifier '" << t.id << "' grants '" << resist
                      << "', which world/resistances.lua does not declare" << std::endl;
        }
    }

    modifiers_db.push_back(t);
}

void RegisterModifierLua(sol::state_view& lua)
{
    lua.new_usertype<ModifierBuilder>("Modifier",
        sol::constructors<ModifierBuilder(MODIFIER)>(),
        "Called", &ModifierBuilder::Called,
        "Severity", &ModifierBuilder::Severity,
        "OnSet", &ModifierBuilder::OnSet,
        "OnRemove", &ModifierBuilder::OnRemove,
        "OnChange", &ModifierBuilder::OnChange,
        "Applies", &ModifierBuilder::Applies,
        "While", &ModifierBuilder::While,
        "Scale", &ModifierBuilder::Scale,
        "ResistedBy", &ModifierBuilder::ResistedBy,
        "Engine", &ModifierBuilder::Engine,
        "Register", &ModifierBuilder::Register
    );
}

XBasicModifier::XBasicModifier(const MODIFIER& mt, int _val, XCreature * _cr)
{
    mdt = mt;
    val = _val;
    setter = XCreature::ToWeakPtr(_cr);
}

const ModifierStats* XBasicModifier::Row() const
{
    return FindModifier(mdt);
}

// Nothing at all is said for a modifier whose row says nothing.
static const std::string no_text;

const std::string& XBasicModifier::SetMsg() const
{
    const ModifierStats* row = Row();
    return row ? row->set_msg : no_text;
}

const std::string& XBasicModifier::RemoveMsg() const
{
    const ModifierStats* row = Row();
    return row ? row->remove_msg : no_text;
}

const std::string& XBasicModifier::ChangeMsg(const int change) const
{
    const ModifierStats* row = Row();

    if (!row) {
        return no_text;
    }

    return change > 0 ? row->grows_msg : row->fades_msg;
}

const std::string& XBasicModifier::ApplyMsg() const
{
    const ModifierStats* row = Row();
    return row ? row->apply_msg : no_text;
}

// What the status line calls it at this depth: the first band whose
// ceiling the value has not reached, or the last one, which stands for
// "and anything above".
std::string XBasicModifier::GetDisplayName(const int xval) const
{
    const ModifierStats* row = Row();

    if (!row || row->names.empty() || xval <= 0) {
        return "";
    }

    for (const auto& band : row->names) {
        if (band.upto == 0 || xval < band.upto) {
            return band.text;
        }
    }

    return row->names.back().text;
}

void XBasicModifier::onSet(XCreature * owner)
{
    const ModifierStats* row = Row();

    if (!row) {
        return;
    }

    owner->added_DV += row->dv;
    owner->added_HIT += row->hit;
    owner->ttmb += row->slower;

    for (const auto& [stat, amount] : row->stats) {
        owner->added_stats.Modify(stat, amount);
    }

    for (const auto& [resist, amount] : row->resistances) {
        owner->added_resists.ChangeResistance(resist, amount);
    }
}

void XBasicModifier::onRemove(XCreature * owner)
{
    const ModifierStats* row = Row();

    if (!row) {
        return;
    }

    owner->added_DV -= row->dv;
    owner->added_HIT -= row->hit;
    owner->ttmb -= row->slower;

    for (const auto& [stat, amount] : row->stats) {
        owner->added_stats.Modify(stat, -amount);
    }

    for (const auto& [resist, amount] : row->resistances) {
        owner->added_resists.ChangeResistance(resist, -amount);
    }
}

// The handful whose behaviour turn by turn cannot be written as numbers:
// a wound that bleeds and that first aid closes, a poison that flares, a
// confusion that walks you into walls, an illness that rots a stat away.
// Content names one of these with :Engine(); everything else about the
// modifier is content's own words.
static void RunEngine(const std::string& which, XBasicModifier* mod, XCreature* owner)
{
    if (which == "bleed") {
        mod->val -= (owner->GetStats(XStats::TOU) / 10
            + owner->sk->GetLevel(XSkill::Skill::FIRST_AID));
        owner->sk->UseSkill(XSkill::Skill::FIRST_AID);

        if (mod->val > 0) {
            owner->HP -= mod->val;

            if (owner->isHero()) {
                msgwin.Add(mod->ApplyMsg());
            }
        }

        return;
    }

    if (which == "poison") {
        if (vRand() % 3 == 0) {
            owner->HP -= vRand() % 4;

            if (owner->isHero()) {
                msgwin.Add(mod->ApplyMsg());
            }
        }

        mod->val -= owner->sk->GetLevel(XSkill::Skill::FIRST_AID);
        return;
    }

    if (which == "stagger") {
        owner->nx = owner->x + vRand() % 3 - 1;
        owner->ny = owner->y + vRand() % 3 - 1;

        if (owner->isHero()) {
            msgwin.Add(mod->ApplyMsg());
        }

        return;
    }

    if (which == "hold") {
        owner->nx = owner->x;
        owner->ny = owner->y;
        return;
    }

    if (which == "rot_body") {
        switch (vRand(300)) {
            case 0:
                owner->GainAttr(XStats::STR, -1);
                break;

            case 1:
                owner->GainAttr(XStats::DEX, -1);
                break;

            case 2:
                owner->GainAttr(XStats::TOU, -1);
                break;
        }

        return;
    }

    if (which == "rot_strength") {
        if (vRand(100) == 0) {
            owner->GainAttr(XStats::STR, -1);
        }

        return;
    }

    std::cerr << "world: a modifier runs '" << which
              << "', which the engine does not know how to do" << std::endl;
}

MODIFIER_RESULT XBasicModifier::Run(XCreature * owner)
{
    if (const ModifierStats* row = Row(); row && !row->engine.empty()) {
        RunEngine(row->engine, this, owner);
    }

    if (owner->HP <= 0) {
        owner->Die(setter.lock().get());
        return MR_DIE;
    }

    val--;
    return val > 0 ? MR_OK : MR_REMOVE;
}
