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

#ifndef BRAND_H
#define BRAND_H

#include <string>
#include <vector>

#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

#include <cereal/cereal.hpp>
#include <sol/forward.hpp>

#include "creature/cr_defs.h"
#include "helpers/registry.h"
#include "magic/resist.h"

// A brand is what an attack carries besides the blow itself: a weapon of
// fire, a spectre's touch, a sword that slays orcs. Which brands exist is
// content - world/brands.lua - and the id is the name it registers under.
using BRAND = std::string;

// The one brand the engine acts on by name: draining reaches back to the
// attacker, which no brand row can express. Everything else a brand does
// is declared in content.
inline constexpr const char* BR_DRAIN_LIFE = "drain_life";

// How a brand's name joins with the others when a weapon carries more
// than one. Elemental and slayer names combine ("Elemental Sword of Slay
// Orcs"); this is naming only, and says nothing about what the brand does.
enum class BrandGroup {
    ELEMENTAL,
    BLACK,
    SLAYER
};

// What a brand does. A brand may do several of these at once, and one
// that does none of them is inert - it still names the weapon.
struct BrandStats {
    BRAND id;

    // The weapon-name template, e.g. "{} of Fire", where {} is the plain
    // name. Empty for a brand that does not rename what carries it.
    std::string templ;
    BrandGroup group = BrandGroup::BLACK;

    // Magic damage against this resistance. Empty for a brand that deals
    // no damage of its own.
    RESISTANCE element;

    // Triple damage against creatures of these classes.
    CreatureClassSet slays;

    // Laid on the victim after a blow that got through, by id. Empty
    // for a brand that inflicts nothing.
    std::string inflicts;

    // What carrying this adds to an item's price.
    int value = 0;
};

extern Registry<BrandStats> brands_db;

// The row a brand id names, or nullptr if content never registered one.
const BrandStats* FindBrand(const BRAND& id);

// The brands one attack carries. Holds ids rather than a bit per brand,
// so content may register as many as it likes and a saved game keeps
// meaning what it meant when it was written.
class BrandSet
{
    public:
        BrandSet() = default;

        // Space-separated ids, e.g. "fire cold demon_slayer". Also takes a
        // single id, which is what most call sites pass.
        BrandSet(const char* ids);
        BrandSet(const std::string& ids);

        bool Has(const BRAND& id) const;
        void Add(const BRAND& id);

        // Everything the other set carries, without duplicates - a
        // creature whose several natural attacks all land at once.
        void Add(const BrandSet& other);
        bool Empty() const { return brands.empty(); }

        // Space-separated, in the order they were added. This is what
        // content sees and what a save file holds.
        std::string toString() const;

        auto begin() const { return brands.begin(); }
        auto end() const { return brands.end(); }

        bool operator==(const BrandSet& o) const { return brands == o.brands; }

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(brands);
        }

    private:
        std::vector<BRAND> brands;
};

// Content-facing builder: Brand.new("fire"):Called("{} of Fire") ...
class BrandBuilder
{
    public:
        explicit BrandBuilder(std::string id);

        BrandBuilder& Called(const std::string& templ);
        BrandBuilder& Group(BrandGroup group);
        BrandBuilder& Element(const RESISTANCE& resist);
        BrandBuilder& Slays(const std::string& prey);
        BrandBuilder& Inflicts(const std::string& modifier);
        BrandBuilder& Value(int value);
        void Register();

    private:
        BrandStats t;
};

// The name a set of brands makes of a plain one: "steel sword of Fire",
// "heap of (12) arrows of Slay Orcs". Which template each brand carries
// and which kind it belongs to is content (world/brands.lua); the way the
// kinds combine into one name is the engine's, and the same for anything
// that can be branded. A set that names nothing gives the plain name
// back, so a poisoned sword is named as a plain sword.
std::string BrandedName(const BrandSet& brands, const std::string& plain, int quantity);

// Complains if no row is registered under this id and returns false.
// Content calls the builders after world/brands.lua has loaded, so an
// unknown id there is a typo rather than an ordering accident.
bool CheckBrandExists(const BRAND& id, const char* where);

void RegisterBrandLua(sol::state_view& lua);

#endif
