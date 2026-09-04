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

#include "creature/creature.h"
#include "game/game.h"
#include "helpers/msgwin.h"
#include "item/item_cereal.h"
#include "item/xherb.h"
#include "item/xpotion.h"
#include "magic/modifier.h"

#define HERBS_COUNT 18

PlantDefinition herbs[] = {
    {"valeriana root",	"valeriana",	"sedative",	xGREEN,	HT_HERB,	PN_NONE,	0, false},
    {"stellaria leave",	"stellaria",	"strange",	xGREEN,	HT_HERB,	PN_NONE,	0, false},
    {"trifolium leave",	"trifolium",	"grassy",	xGREEN,	HT_HERB,	PN_NONE,	0, false},
    {"trifolium flower",	"trifolium",	"sweet",	xGREEN,	HT_HERB,	PN_NONE,	0, false},
    {"urtica leave",	"urtica",	"bitter",	xGREEN,	HT_HERB,	PN_NONE,	0, false},
    {"convallaria flower",	"convallaria",	"sweet",	xGREEN,	HT_HERB,	PN_NONE,	0, false},
    {"tussilago farfara leave",	"tussilago farfara",	"bitter",	xGREEN,	HT_HERB,	PN_NONE,	0, false},
    {"melissa leave",	"melissa",	"delicate",	xGREEN,	HT_HERB,	PN_NONE,	0, false},
    {"mentha leave",	"mentha",	"mint",	xGREEN,	HT_HERB,	PN_NONE,	0, false},
    {"taraxacum flower",	"taraxacum",	"sweet",	xGREEN,	HT_HERB,	PN_NONE,	0, false},
    {"paeonia root",	"paeonia",	"bitter",	xGREEN,	HT_HERB,	PN_NONE,	0, false},
    {"plantago leave",	"plantago",	"bitter",	xGREEN,	HT_HERB,	PN_NONE,	0, false},
    {"chamomilla flower",	"chamomilla",	"sweet",	xGREEN,	HT_HERB,	PN_NONE,	0, false},

    {"red mushroom",	"red mushroom",	"bitter",	xRED,	HT_MUSHROOM,	PN_NONE,	0, false},
    {"green mushroom",	"green mushroom",	"bitter",	xGREEN,	HT_MUSHROOM,	PN_NONE,	0, false},
    {"blue mushroom",	"blue mushroom",	"bitter",	xBLUE,	HT_MUSHROOM,	PN_NONE,	0, false},
    {"yellow mushroom",	"yellow mushroom",	"bitter",	xYELLOW,	HT_MUSHROOM,	PN_NONE,	0, false},
    {"white mushroom",	"white mushroom",	"bitter",	xWHITE,	HT_MUSHROOM,	PN_NONE,	0, false},
};

void PlantDefinition::Create()
{
    for (int i = 0; i < HERBS_COUNT; i++) {
        while (1) {
            PotionDescription * pr = PotionDescription::GetRec(PotionDescription::GetAnyPotion());

            // check if there was such potion already
            bool continue_flag = false;

            for (int j = 0; j < i; j++) {
                if (herbs[j].pn == pr->pn) {
                    continue_flag = true;
                    break;
                }
            }

            if (continue_flag) {
                continue;
            }

            if (herbs[i].herb_type == HT_HERB) {
                if (pr->alchemy_power == 1) {
                    herbs[i].pn = pr->pn;
                    herbs[i].difficulty = vRand(4) + 1;
                    break;
                } else if (pr->alchemy_power == 2) {
                    herbs[i].pn = pr->pn;
                    herbs[i].difficulty = vRand(4) + 4;
                    break;
                }
            } else {
                if (pr->alchemy_power == 3) {
                    herbs[i].pn = pr->pn;
                    herbs[i].difficulty = vRand(4) + 7;
                    break;
                }
            }
        }
    }
}

void PlantDefinition::SaveTable(cereal::JSONOutputArchive& ar)
{
    for (int i = 0; i < HERBS_COUNT; i++) {
        ar(herbs[i].pn, herbs[i].difficulty, herbs[i].identified);
    }
}

void PlantDefinition::LoadTable(cereal::JSONInputArchive& ar)
{
    for (int i = 0; i < HERBS_COUNT; i++) {
        ar(herbs[i].pn, herbs[i].difficulty, herbs[i].identified);
    }
}

REGISTER_CLASS(XHerb);
CEREAL_REGISTER_TYPE(XHerb);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XItem, XHerb);
// XHerb()'s only no-args constructor is an assert(0) guard - real
// instances always come from XHerb(int), so route Cereal's load-time
// construction through the DUMMY_STRUCT idiom instead of that assert.
CEREAL_LOAD_VIA_DUMMY_CONSTRUCT(XHerb, serialize);

XHerb::XHerb(int _herb_index) : XAnyFood(), herb_index(_herb_index)
{
    weight = 3;
    food_nutrio = 15;
    name = herbs[herb_index].herb_name;
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
    return herbs[herb_index].pn;
}

std::string XHerb::postEat(XCreature * /*eater*/)
{
    return herbs[herb_index].post_eat;
}

std::string XHerb::toString()
{
    if (herbs[herb_index].identified) {
        name = herbs[herb_index].herb_name;
    } else {
        if (herbs[herb_index].herb_type == HT_HERB) {
            name = "unknown herb";
        } else {
            name = "unknown mushroom";
        }
    }

    return XAnyFood::toString();
}

void XHerb::Identify()
{
    herbs[herb_index].identified = true;
}

bool XHerb::isIdentified()
{
    return herbs[herb_index].identified;
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

const PlantDefinition& XPlant::Species() const
{
    return herbs[herb_index];
}

PlantDefinition& XPlant::Species()
{
    return herbs[herb_index];
}

bool XPlant::PlaceAt(XLocation* location, const int _x, const int _y)
{
    if (!XMapObject::PlaceAt(location, _x, _y)) {
        return false;
    }

    do {
        herb_index = vRand(HERBS_COUNT);
    } while (herbs[herb_index].herb_type != SpeciesType());

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
        // herb_index read below even when the cell was its last owner.
        Invalidate();
    }

    return new XHerb(herb_index);
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
    // herb_index read below even when the cell was its last owner.
    Invalidate();
    XHerb * it = new XHerb(herb_index);
    it->Identify();
    return it;
}
