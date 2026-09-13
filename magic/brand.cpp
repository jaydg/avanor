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

#include <fmt/format.h>

#include <sol/sol.hpp>

#include "helpers/registry.h"
#include "magic/brand.h"
#include "magic/modifier.h"

Registry<BrandStats> brands_db{"brand"};

const BrandStats* FindBrand(const std::string& id)
{
    return brands_db.Find(id);
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

// The name template of the one brand of this kind in the set. Only asked
// where the count is known to be exactly one.
static std::string TemplateOf(const BrandSet& brands, const BrandGroup group)
{
    for (const BRAND& id : brands) {
        const BrandStats* row = FindBrand(id);

        if (row && row->group == group) {
            return row->templ;
        }
    }

    return std::string();
}

std::string BrandedName(const BrandSet& brands, const std::string& plain, const int quantity)
{
    // How many brands of each kind are on it. Content says which kind a
    // brand belongs to; the way the kinds combine is decided here.
    int ec = 0;
    int bc = 0;
    int sc = 0;

    for (const BRAND& id : brands) {
        const BrandStats* row = FindBrand(id);

        if (!row) {
            continue;
        }

        switch (row->group) {
            case BrandGroup::ELEMENTAL: ec++; break;
            case BrandGroup::BLACK:     bc++; break;
            case BrandGroup::SLAYER:    sc++; break;
        }
    }

    // A black brand suppresses the name altogether - every branch below
    // asks for bc == 0 - so a poisoned sword is named as a plain sword.
    std::string templ;

    if (ec == 1 && bc == 0 && sc == 0) {
        templ = TemplateOf(brands, BrandGroup::ELEMENTAL);
    } else if (ec == 0 && bc == 0 && sc == 1) {
        templ = TemplateOf(brands, BrandGroup::SLAYER);
    } else if (ec >= 1 && bc == 0 && sc == 1) {
        templ = fmt::format("Elemental {}", TemplateOf(brands, BrandGroup::SLAYER));
    } else if (ec >= 1 && bc == 0 && sc > 1) {
        templ = "Elemental {} of Slaying";
    } else if (ec == 0 && bc == 0 && sc > 1) {
        templ = "{} of Slaying";
    } else if (ec > 1 && bc == 0 && sc == 0) {
        templ = "Elemental {}";
    }

    // The template stands around whatever is being named, so the plural
    // goes on before it rather than after: "arrows of Slay Orcs", not
    // "arrow of Slay Orcss".
    const std::string named = quantity == 1 ? plain : plain + "s";
    const std::string full = templ.empty() ? named : fmt::format(templ, named);

    if (quantity == 1) {
        return full;
    }

    return fmt::format("heap of ({}) {}", quantity, full);
}

bool CheckBrandExists(const std::string& id, const char* where)
{
    return brands_db.Exists(id, where);
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

BrandBuilder& BrandBuilder::Slays(const std::string& prey)
{
    if (CheckCreatureClassExists(prey, "a brand")) {
        t.slays.Add(prey);
    }

    return *this;
}

BrandBuilder& BrandBuilder::Inflicts(const std::string& modifier)
{
    // A name nothing knows: say so while the file that wrote it is
    // loading, and lay nothing on.
    if (!IsKnownModifier(modifier)) {
        std::cerr << "world: a brand names a modifier '" << modifier
                  << "', which nothing defines" << std::endl;

        return *this;
    }

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
    brands_db.Add(t);
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
