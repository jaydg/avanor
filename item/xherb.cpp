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
#include <map>
#include <vector>

#include <cereal/types/map.hpp>
#include <cereal/types/string.hpp>

#include "helpers/registry.h"
#include "creature/creature.h"
#include "helpers/msgwin.h"
#include "item/item_cereal.h"
#include "item/xherb.h"
#include "item/xpotion.h"
#include "magic/modifier.h"

// Filled from world/items/herbs.lua as that script loads.
Registry<PlantDefinition> herbs{"plant"};

PlantDefinition* PlantDefinition::Find(const std::string& id)
{
    for (auto& row : herbs) {
        if (row.id == id) {
            return &row;
        }
    }

    return nullptr;
}

Registry<PlantKindStats> plant_kinds_db{"plant kind"};

const PlantKindStats* FindPlantKind(const std::string& id)
{
    return plant_kinds_db.Find(id);
}

const PLANT_KIND& DefaultPlantKind()
{
    static const PLANT_KIND nothing;

    return plant_kinds_db.empty() ? nothing : plant_kinds_db.front().id;
}

PlantKindBuilder::PlantKindBuilder(std::string id)
{
    t.id = std::move(id);
}

PlantKindBuilder& PlantKindBuilder::Unknown(const std::string& name)
{
    t.unknown_name = name;
    return *this;
}

PlantKindBuilder& PlantKindBuilder::Distils(const int alchemy_power,
    const std::string& difficulty)
{
    t.grades.push_back({alchemy_power, difficulty});
    return *this;
}

void PlantKindBuilder::Register()
{
    if (t.unknown_name.empty()) {
        t.unknown_name = "unknown " + t.id;
    }

    plant_kinds_db.Add(t);
}

std::string PlantDefinition::RandomOfType(const PLANT_KIND& kind)
{
    std::vector<const PlantDefinition*> of_type;

    for (const auto& row : herbs) {
        if (row.kind == kind) {
            of_type.push_back(&row);
        }
    }

    if (of_type.empty()) {
        std::cerr << "world: no plant of that kind is defined" << std::endl;

        return {};
    }

    return of_type[vRand(static_cast<int>(of_type.size()))]->id;
}

PlantBuilder::PlantBuilder(std::string id) : id(std::move(id)) {}

PlantBuilder& PlantBuilder::Called(const std::string& n)
{
    herb_name = n;
    return *this;
}

PlantBuilder& PlantBuilder::Growing(const std::string& n)
{
    bush_name = n;
    return *this;
}

PlantBuilder& PlantBuilder::Kind(const std::string& k)
{
    kind = k;
    return *this;
}

PlantBuilder& PlantBuilder::Taste(const std::string& t)
{
    post_eat = t;
    return *this;
}

PlantBuilder& PlantBuilder::Looks(const int colour)
{
    color = colour;
    return *this;
}

void PlantBuilder::Register()
{
    // The kinds are declared above the species that belong to them, so a
    // species naming one that does not exist is a typo. It would
    // otherwise distil into nothing and read as "unknown plant" for ever.
    if (!kind.empty() && !FindPlantKind(kind)) {
        std::cerr << "world: the plant '" << id << "' is of kind '" << kind
                  << "', which world/items/herbs.lua does not declare" << std::endl;
        return;
    }

    PlantDefinition row;
    row.id = id;
    row.herb_name = herb_name.empty() ? id : herb_name;

    // A mushroom is its own plant: there is no bush it grows on, so it
    // stands under its own name.
    row.bush_name = bush_name.empty() ? row.herb_name : bush_name;
    row.post_eat = post_eat;
    row.color = color;
    row.kind = kind.empty() ? DefaultPlantKind() : kind;

    herbs.Add(std::move(row));
}

// Deals each species the potion it distils into, and how hard that is -
// fresh every game. A herb yields one of the two easiest grades of potion,
// a mushroom one of the hardest, and no two species yield the same potion.
void PlantDefinition::Create()
{
    for (auto& row : herbs) {
        row.pn.clear();
        row.difficulty = 0;
    }

    for (auto& row : herbs) {
        // Bounded rather than a bare loop: if content defines more species
        // than there are potions of the grades they want, the old code
        // spun here forever.
        for (int attempt = 0; attempt < 1000 && row.pn.empty(); attempt++) {
            const PotionDescription* pr =
                PotionDescription::GetRec(PotionDescription::GetAnyPotion());

            if (!pr) {
                break;
            }

            bool taken = false;

            for (const auto& other : herbs) {
                if (&other != &row && other.pn == pr->pn) {
                    taken = true;
                    break;
                }
            }

            if (taken) {
                continue;
            }

            // Which grades of potion this sort of plant yields, and how
            // hard each is to recognise, is content - see the plant kinds
            // at the head of world/items/herbs.lua.
            const PlantKindStats* kind = FindPlantKind(row.kind);

            if (!kind) {
                break;
            }

            for (const auto& grade : kind->grades) {
                if (grade.alchemy_power != pr->alchemy_power) {
                    continue;
                }

                XDice d;
                d.Setup(grade.difficulty);
                row.pn = pr->pn;
                row.difficulty = d.Throw();
                break;
            }
        }

        if (row.pn.empty()) {
            std::cerr << "world: no potion left for '" << row.id
                      << "' to distil into" << std::endl;
        }
    }
}

// What this game dealt each species and what the player has worked out,
// keyed by id so content may add or reorder species without spoiling a
// save. Everything else about a plant is content and comes back from
// world/items/herbs.lua on the next load.
struct PlantMemory {
    PotionName pn;
    int difficulty{0};
    bool identified{false};

    template<class Archive>
    void serialize(Archive& ar)
    {
        ar(pn, difficulty, identified);
    }
};

void PlantDefinition::SaveTable(cereal::JSONOutputArchive& ar)
{
    std::map<std::string, PlantMemory> learned;

    for (const auto& row : herbs) {
        learned[row.id] = PlantMemory{row.pn, row.difficulty, row.identified};
    }

    ar(learned);
}

void PlantDefinition::LoadTable(cereal::JSONInputArchive& ar)
{
    std::map<std::string, PlantMemory> learned;
    ar(learned);

    for (auto& row : herbs) {
        if (const auto it = learned.find(row.id); it != learned.end()) {
            row.pn = it->second.pn;
            row.difficulty = it->second.difficulty;
            row.identified = it->second.identified;
        }
    }
}

REGISTER_CLASS(XHerb);
CEREAL_REGISTER_TYPE(XHerb);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XItem, XHerb);
// XHerb()'s only no-args constructor is an assert(0) guard - real
// instances always come from XHerb(int), so route Cereal's load-time
// construction through the DUMMY_STRUCT idiom instead of that assert.
CEREAL_LOAD_VIA_DUMMY_CONSTRUCT(XHerb, serialize);

XHerb::XHerb(std::string _species) : XAnyFood(), species(std::move(_species))
{
    weight = 3;
    food_nutrio = 15;

    if (const PlantDefinition* row = PlantDefinition::Find(species)) {
        name = row->herb_name;
    } else {
        std::cerr << "world: nothing defines a plant '" << species << "'"
                  << std::endl;
    }

    it = IT_HERB;
}

RESULT XHerb::onEat(XCreature * eater)
{
    if (XAnyFood::onEat(eater)) {
        return SUCCESS;
    }

    return FAIL;
}

PotionName XHerb::GetTargetPotion()
{
    const PlantDefinition* row = PlantDefinition::Find(species);
    return row ? row->pn : PN_NONE;
}

std::string XHerb::postEat(XCreature * /*eater*/)
{
    const PlantDefinition* row = PlantDefinition::Find(species);
    return row ? row->post_eat : std::string();
}

std::string XHerb::toString()
{
    const PlantDefinition* row = PlantDefinition::Find(species);

    if (row && row->identified) {
        name = row->herb_name;
    } else {
        const PlantKindStats* kind = row ? FindPlantKind(row->kind) : nullptr;
        name = kind ? kind->unknown_name : "unknown plant";
    }

    return XAnyFood::toString();
}

void XHerb::Identify()
{
    if (PlantDefinition* row = PlantDefinition::Find(species)) {
        row->identified = true;
    }
}

bool XHerb::isIdentified()
{
    const PlantDefinition* row = PlantDefinition::Find(species);
    return row && row->identified;
}
