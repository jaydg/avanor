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

#include "creature/creature.h"
#include "game/game.h"
#include "helpers/msgwin.h"
#include "item/item_cereal.h"
#include "item/xherb.h"
#include "item/xpotion.h"
#include "magic/modifier.h"

// Filled from world/items/herbs.lua as that script loads.
std::vector<PlantDefinition> herbs;

PlantDefinition* PlantDefinition::Find(const std::string& id)
{
    for (auto& row : herbs) {
        if (row.id == id) {
            return &row;
        }
    }

    return nullptr;
}

std::string PlantDefinition::RandomOfType(const HERB_TYPE type)
{
    std::vector<const PlantDefinition*> of_type;

    for (const auto& row : herbs) {
        if (row.herb_type == type) {
            of_type.push_back(&row);
        }
    }

    if (of_type.empty()) {
        std::cerr << "world: no plant of that kind is defined" << std::endl;

        return {};
    }

    return of_type[vRand(static_cast<int>(of_type.size()))]->id;
}

HerbBuilder::HerbBuilder(std::string id) : id(std::move(id)) {}

HerbBuilder& HerbBuilder::Called(const std::string& n)
{
    herb_name = n;
    return *this;
}

HerbBuilder& HerbBuilder::Growing(const std::string& n)
{
    bush_name = n;
    return *this;
}

HerbBuilder& HerbBuilder::Mushroom()
{
    herb_type = HT_MUSHROOM;
    return *this;
}

HerbBuilder& HerbBuilder::Taste(const std::string& t)
{
    post_eat = t;
    return *this;
}

HerbBuilder& HerbBuilder::Looks(const int colour)
{
    color = colour;
    return *this;
}

void HerbBuilder::Register()
{
    if (id.empty()) {
        std::cerr << "world: a plant with no id" << std::endl;
        return;
    }

    if (PlantDefinition::Find(id)) {
        std::cerr << "world: two plants both called '" << id << "'" << std::endl;
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
    row.herb_type = herb_type;

    herbs.push_back(std::move(row));
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

            if (row.herb_type == HT_HERB) {
                if (pr->alchemy_power == 1) {
                    row.pn = pr->pn;
                    row.difficulty = vRand(4) + 1;
                } else if (pr->alchemy_power == 2) {
                    row.pn = pr->pn;
                    row.difficulty = vRand(4) + 4;
                }
            } else if (pr->alchemy_power == 3) {
                row.pn = pr->pn;
                row.difficulty = vRand(4) + 7;
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
    } else if (!row || row->herb_type == HT_HERB) {
        name = "unknown herb";
    } else {
        name = "unknown mushroom";
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

////////////////////////////////////////////////////////////
// XHerbBush
////////////////////////////////////////////////////////////

REGISTER_CLASS(XHerbBush);
CEREAL_REGISTER_TYPE(XHerbBush);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XMapObject, XPlant);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XPlant, XHerbBush);

XHerbBush::XHerbBush(int _x, int _y, XLocation * _l)
{
    const bool placed = PlaceAt(_l, _x, _y);
    assert(placed);
}

// A species that is always there, so a plant whose id content removed
// still has something to describe itself with rather than dereferencing
// nothing.
static PlantDefinition unknown_species;

const PlantDefinition& XPlant::Species() const
{
    const PlantDefinition* row = PlantDefinition::Find(species);
    return row ? *row : unknown_species;
}

PlantDefinition& XPlant::Species()
{
    PlantDefinition* row = PlantDefinition::Find(species);
    return row ? *row : unknown_species;
}

bool XPlant::PlaceAt(XLocation* location, const int _x, const int _y)
{
    if (!XMapObject::PlaceAt(location, _x, _y)) {
        return false;
    }

    species = PlantDefinition::RandomOfType(SpeciesType());

    ttm = FirstRunDelay();
    view = SpeciesView();
    color = Species().color;
    name = Species().herb_name;

    Game.Scheduler.Add(this);

    return true;
}

bool XHerbBush::PlaceAt(XLocation* location, const int _x, const int _y)
{
    if (!XPlant::PlaceAt(location, _x, _y)) {
        return false;
    }

    herb_strength = 1;

    return true;
}

const std::string XHerbBush::GetName(XCreature *viewer)
{
    XSkill * xsk = viewer->sk->GetSkill(XSkill::Skill::HERBALISM);
    int val = 0;

    if (xsk) {
        val += xsk->GetLevel();
    }

    PlantDefinition * herb_data = &Species();

    const char* size_name = "";

    switch (herb_strength) {
        case 1:
            size_name = "small";
            break;

        case 2:
            size_name = "medium";
            break;

        case 3:
            size_name = "large";
            break;
    }

    if (herb_data->difficulty > val && !herb_data->identified) {
        return fmt::format("{} bush of unknown herbs", size_name);
    }

    herb_data->identified = true;

    return fmt::format("{} bush of {}", size_name, herb_data->bush_name);
}

int XHerbBush::CountNeighbours(int x, int y)
{
    int N = 0;

    for (int i = x - 1; i <= x + 1; i++)
        for (int j = y - 1; j <= y + 1; j++) {
            if (i == x && j == y) {
                continue;
            }

            XMapObject * obj = l->map->GetSpecial(i, j);
            XHerbBush * bush = dynamic_cast<XHerbBush *>(obj);

            if (!bush || !bush->isValid()) {
                continue;
            }

            N++;
        }

    return N;
}

bool XHerbBush::Run()
{
    assert(isValid());

    int N = CountNeighbours(x, y);

    if (N < 2 || N > 3) {
        if (--herb_strength <= 0) {
            // Invalidate() handles the map-cell eviction itself (see
            // XMapObject::Invalidate()).
            Invalidate();
            return false;
        }
    }

    if (N == 3 && herb_strength < 3) {
        herb_strength++;
    }

    for (int i = x - 1; i <= x + 1; i++)
        for (int j = y - 1; j <= y + 1; j++) {
            XMapObject * obj = l->map->GetSpecial(i, j);

            if (obj != 0 || !XTileType::isFertile(l->map->GetXY(i, j))) {
                continue;
            }

            if (CountNeighbours(i, j) != 3) {
                continue;
            }

            new XHerbBush(i, j, l);
        }

    if (ttm <= 0 && isValid()) {
        ttm += vRand(BASE_HERB_REFRESH);
    }

    return true;
}

XObject* XHerbBush::Pick(XCreature * picker)
{
    picker->sk->UseSkill(XSkill::Skill::HERBALISM);

    if (--herb_strength <= 0) {
        // Invalidate() evicts this bush from its map cell itself; the
        // deferred-release graveyard keeps the object alive through the
        // species read below even when the cell was its last owner.
        Invalidate();
    }

    return new XHerb(species);
}

////////////////////////////////////////////////////////////
// XMushSpawn
////////////////////////////////////////////////////////////

REGISTER_CLASS(XMushSpawn);
CEREAL_REGISTER_TYPE(XMushSpawn);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XPlant, XMushSpawn);

XMushSpawn::XMushSpawn(int _x, int _y, XLocation * _l)
{
    const bool placed = PlaceAt(_l, _x, _y);
    assert(placed);
}

const std::string XMushSpawn::GetName(XCreature * /*viewer*/)
{
    PlantDefinition * herb_data = &Species();
    return herb_data->bush_name;
}

bool XMushSpawn::Run()
{
    assert(isValid());

    if (ttm <= 0 && isValid()) {
        if (isVisible()) {
            msgwin.Add("Suddenly mushroom dissapered in the small cloud of spores.");
        }

        // Invalidate() handles the map-cell eviction itself (see
        // XMapObject::Invalidate()).
        Invalidate();
        return false;
    }

    return true;
}

XObject* XMushSpawn::Pick(XCreature * picker)
{
    picker->sk->UseSkill(XSkill::Skill::HERBALISM);

    // Invalidate() evicts this spawn from its map cell itself; the
    // deferred-release graveyard keeps the object alive through the
    // species read below even when the cell was its last owner.
    Invalidate();
    XHerb * it = new XHerb(species);
    it->Identify();
    return it;
}
