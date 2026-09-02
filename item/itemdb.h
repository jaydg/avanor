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

#ifndef ITEMDB_H
#define ITEMDB_H

#include <string>
#include <vector>

#include "magic/wskills.h"
#include "item/itemdef.h"
#include "item/itemkind.h"
#include "magic/attack_effect_type.h"

#define DB_PROP_SZ	15 // number of materials!
#define ENH_DB_SZ	20 // number of special powers ("of Strength")

// One sort of item: what it is called, what it is made of, what it does,
// and how often the game hands one out. The dice fields are the strings
// XDice takes ("1d3+2", "" for none).
struct ItemTemplate {
    ItemType it;
    std::string name;
    char view;
    std::string dv;
    std::string pv;
    std::string hit;
    std::string dice;
    std::string z; //random z to dice;
    std::string r;
    ITEM_SET iset;

    // Which war skill wielding one exercises.
    XWarSkills::Type wt;

    // For a missile: the skill of the weapon that fires it, so an arrow
    // asks for a bow and a quarrel for a crossbow. OTHER means nothing
    // launches it - a shuriken is thrown and nothing else. Anything at all
    // can be thrown, which is why this says nothing about throwing.
    XWarSkills::Type launcher;

    int value;
    int valume;
    int probability;
    ITEM_QUALITY iq;
};

// One kind's worth of templates - every sort of sword, or every sort of
// boot - and how likely each is when the game asks for one of that kind
// without saying which.
//
// The rows are filled from world/items/ as those scripts load.
// The nine pools, one per kind of ordinary item, filled from world/items/
// as those scripts load. Declared here so the builder can reach them; each
// is defined beside the class that draws from it.
class XItemBasicStructure;

extern XItemBasicStructure gi_weapon;
extern XItemBasicStructure gi_missilew;
extern XItemBasicStructure gi_missile;
extern XItemBasicStructure gi_armour;
extern XItemBasicStructure gi_shield;
extern XItemBasicStructure gi_cap;
extern XItemBasicStructure gi_cloaks;
extern XItemBasicStructure gi_boots;
extern XItemBasicStructure gi_gloves;

class XItemBasicStructure
{
    public:
        void Add(const ItemTemplate& row)
        {
            rows.push_back(row);

            // Re-derived rather than accumulated: push_back may reallocate,
            // and pFirstItem has to follow the storage.
            pFirstItem = rows.data();
            total_item = static_cast<int>(rows.size());
            total_prob = 0;

            for (const auto& r : rows) {
                total_prob += r.probability;
            }
        }

        void Clear()
        {
            rows.clear();
            pFirstItem = nullptr;
            total_item = 0;
            total_prob = 0;
        }

        int total_prob{0};
        int total_item{0};
        ItemTemplate* pFirstItem{nullptr};

    private:
        std::vector<ItemTemplate> rows;
};

struct ENHANCE_STRUCT {
    const char* name;
    int color; //if 0 don't change a color
    const char* dv;
    const char* pv;
    const char* hit;
    const char* dice;
    const char* z; //random z to dice;
    const char* rng;
    int val; // value
    ItemKind kind;
    AttackEffectType brt;
    CAN_FLAG res_flag;
    SPECIAL_PROPERTY spp;
    const char* s; //stats
    const char* r; //resists
};


extern ItemMaterial item_prop[DB_PROP_SZ];
extern ENHANCE_STRUCT ienh_db[ENH_DB_SZ];


// Fluent builder for one row of one of those pools:
//
//   Template.new(ItemKind.WEAPON, ItemType.LONGSWORD)
//       :View("long sword", '|')
//       :Made(ItemSet.OBSIMETAL, ItemQuality.FAIR)
//       :Skill(XWarSkills.SWORD)
//       :Worth(18, 10)
//       :Combat("", "2d4", "")
//       :Chance(60)
//       :Register()
//
// Everything not stated is empty or zero, so a row says only what it has:
// a cap names no damage dice, a sword no protection.
class TemplateBuilder
{
    public:
        TemplateBuilder(ItemKind kind, ItemType it);

        TemplateBuilder& View(const std::string& name, const std::string& view);
        TemplateBuilder& Made(ITEM_SET iset, ITEM_QUALITY iq);
        TemplateBuilder& Skill(XWarSkills::Type wt);
        TemplateBuilder& Launcher(XWarSkills::Type wt);
        TemplateBuilder& Worth(int value, int weight);
        TemplateBuilder& Armour(const std::string& dv, const std::string& pv);
        TemplateBuilder& Combat(const std::string& hit, const std::string& dice,
            const std::string& extra);
        TemplateBuilder& Range(const std::string& range);
        TemplateBuilder& Chance(int probability);

        void Register();

    private:
        ItemKind kind;
        ItemTemplate t{};
};

#endif
