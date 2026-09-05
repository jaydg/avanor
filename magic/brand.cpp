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
#include <sstream>

#include <sol/sol.hpp>

#include "magic/brand.h"

std::vector<BrandStats> brands_db;

const BrandStats* FindBrand(const BRAND& id)
{
    for (const auto& row : brands_db) {
        if (row.id == id) {
            return &row;
        }
    }

    return nullptr;
}

BrandSet::BrandSet(const char* ids)
    : BrandSet(std::string(ids ? ids : ""))
{
}

BrandSet::BrandSet(const std::string& ids)
{
    std::istringstream in(ids);
    std::string id;

    while (in >> id) {
        Add(id);
    }
}

bool BrandSet::Has(const BRAND& id) const
{
    for (const auto& b : brands) {
        if (b == id) {
            return true;
        }
    }

    return false;
}

void BrandSet::Add(const BRAND& id)
{
    if (!id.empty() && !Has(id)) {
        brands.push_back(id);
    }
}

void BrandSet::Add(const BrandSet& other)
{
    for (const auto& b : other.brands) {
        Add(b);
    }
}

std::string BrandSet::toString() const
{
    std::string out;

    for (const auto& b : brands) {
        if (!out.empty()) {
            out.append(" ");
        }

        out.append(b);
    }

    return out;
}

bool CheckBrandExists(const BRAND& id, const char* where)
{
    if (FindBrand(id)) {
        return true;
    }

    std::cerr << "world: " << where << " asks for a brand '" << id
              << "' that world/brands.lua does not declare - ignored"
              << std::endl;

    return false;
}

BrandBuilder::BrandBuilder(std::string id)
{
    t.id = std::move(id);
}

BrandBuilder& BrandBuilder::Called(const std::string& templ)
{
    t.templ = templ;
    return *this;
}

BrandBuilder& BrandBuilder::Group(const BrandGroup group)
{
    t.group = group;
    return *this;
}

BrandBuilder& BrandBuilder::Element(const RESISTANCE& resist)
{
    t.element = resist;
    return *this;
}

BrandBuilder& BrandBuilder::Slays(const CreatureClass prey)
{
    t.slays = prey;
    return *this;
}

BrandBuilder& BrandBuilder::Inflicts(const int modifier)
{
    t.inflicts = modifier;
    return *this;
}

BrandBuilder& BrandBuilder::Value(const int value)
{
    t.value = value;
    return *this;
}

void BrandBuilder::Register()
{
    brands_db.push_back(t);
}

void RegisterBrandLua(sol::state_view& lua)
{
    lua.new_enum("BrandGroup",
        "ELEMENTAL", BrandGroup::ELEMENTAL,
        "BLACK", BrandGroup::BLACK,
        "SLAYER", BrandGroup::SLAYER
    );

    lua.new_usertype<BrandBuilder>("Brand",
        sol::constructors<BrandBuilder(std::string)>(),
        "Called", &BrandBuilder::Called,
        "Group", &BrandBuilder::Group,
        "Element", &BrandBuilder::Element,
        "Slays", &BrandBuilder::Slays,
        "Inflicts", &BrandBuilder::Inflicts,
        "Value", &BrandBuilder::Value,
        "Register", &BrandBuilder::Register
    );
}
