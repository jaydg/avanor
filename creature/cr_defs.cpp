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

#include <iostream>

#include <sol/sol.hpp>

#include "helpers/registry.h"
#include "creature/cr_defs.h"

namespace {

Registry<CreatureClassStats> classes_db{"creature class"};

}

CreatureClassSet::CreatureClassSet(std::initializer_list<CREATURE_CLASS> ids)
{
    for (const auto& id : ids) {
        Add(id);
    }
}

CreatureClassSet::CreatureClassSet(std::vector<CREATURE_CLASS> ids)
{
    for (auto& id : ids) {
        Add(id);
    }
}

bool CreatureClassSet::Has(const CREATURE_CLASS& id) const
{
    for (const auto& c : classes) {
        if (c == id) {
            return true;
        }
    }

    return false;
}

void CreatureClassSet::Add(const CREATURE_CLASS& id)
{
    if (!id.empty() && !Has(id)) {
        classes.push_back(id);
    }
}

void CreatureClassSet::Add(const CreatureClassSet& other)
{
    for (const auto& c : other.classes) {
        Add(c);
    }
}

void CreatureClassSet::Remove(const CREATURE_CLASS& id)
{
    for (auto it = classes.begin(); it != classes.end(); ++it) {
        if (*it == id) {
            classes.erase(it);
            return;
        }
    }
}

std::string CreatureClassSet::toString() const
{
    std::string out;

    for (const auto& c : classes) {
        if (!out.empty()) {
            out.append(", ");
        }

        out.append(c);
    }

    return out;
}

const CreatureClassStats* FindCreatureClass(const std::string& id)
{
    return classes_db.Find(id);
}

const std::vector<CreatureClassStats>& AllCreatureClasses()
{
    return classes_db.All();
}

CreatureClassSet DefaultEnemies()
{
    CreatureClassSet out;

    for (const auto& row : classes_db) {
        if (row.enemy_by_default) {
            out.Add(row.id);
        }
    }

    return out;
}

CreatureClassSet GuardsEnemies()
{
    CreatureClassSet out = DefaultEnemies();

    for (const auto& row : classes_db) {
        if (row.folk) {
            out.Remove(row.id);
        }
    }

    return out;
}

CreatureClassBuilder::CreatureClassBuilder(CREATURE_CLASS id)
{
    t.id = std::move(id);
}

CreatureClassBuilder& CreatureClassBuilder::NoCorpse()
{
    t.leaves_corpse = false;
    return *this;
}

CreatureClassBuilder& CreatureClassBuilder::Slain(const std::string& verb)
{
    t.slain_verb = verb;
    return *this;
}

CreatureClassBuilder& CreatureClassBuilder::Enemy()
{
    t.enemy_by_default = true;
    return *this;
}

CreatureClassBuilder& CreatureClassBuilder::Folk()
{
    t.folk = true;
    return *this;
}

void CreatureClassBuilder::Register()
{
    if (t.slain_verb.empty()) {
        std::cerr << "world: the creature class '" << t.id
                  << "' is slain by no verb at all" << std::endl;
        return;
    }

    classes_db.Add(t);
}

// Complains if no row is registered under this id and returns false.
// Content declares the classes before anything names one, so an unknown
// id is a typo rather than an ordering accident.
bool CheckCreatureClassExists(const std::string& id, const char* where)
{
    return classes_db.Exists(id, where);
}

void RegisterCreatureClassLua(sol::state_view& lua)
{
    lua.new_usertype<CreatureClassBuilder>("CreatureClass",
        sol::constructors<CreatureClassBuilder(CREATURE_CLASS)>(),
        "NoCorpse", &CreatureClassBuilder::NoCorpse,
        "Slain", &CreatureClassBuilder::Slain,
        "Enemy", &CreatureClassBuilder::Enemy,
        "Folk", &CreatureClassBuilder::Folk,
        "Register", &CreatureClassBuilder::Register
    );
}

void RegisterCrDefsEnums(sol::state_view& lua)
{
    RegisterCreatureClassLua(lua);
}
