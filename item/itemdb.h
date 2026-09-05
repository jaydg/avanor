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

#include "magic/cskills.h"
#include "item/itemdef.h"
#include "item/itemkind.h"
#include "magic/attack_effect_type.h"

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
    COMBAT_SKILL wt;

    // What one does to somebody: an axe hacks, a mace smashes, and most
    // things simply hit. Left unsaid it is "hit", which is why only the
    // weapons with a word of their own name one.
    std::string verb;

    // For a missile: the skill of the weapon that fires it, so an arrow
    // asks for a bow and a quarrel for a crossbow. OTHER means nothing
    // launches it - a shuriken is thrown and nothing else. Anything at all
    // can be thrown, which is why this says nothing about throwing.
    COMBAT_SKILL launcher;

    int value;
    int valume;
    int probability;
    ITEM_QUALITY iq;

    // The Lua function that is called once the engine has built the item and
    // before anyone sees it - so content can rename it, sharpen it, poison it.
    // Empty means the plain item is the finished item, which is what almost
    // every row wants.
    std::string on_create;

    // The Lua function that is called when a creature is being outfitted
    // with one of these, with the item and the creature. For anything that
    // needs a second thing to make sense at all: a bow is no use without
    // arrows. Empty for almost everything, which needs only itself.
    std::string on_outfit;
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

        // The row describing that sort of item, or nullptr if this pool
        // holds no such thing.
        [[nodiscard]] const ItemTemplate* Find(const ItemType& it) const
        {
            for (const auto& r : rows) {
                if (r.it == it) {
                    return &r;
                }
            }

            return nullptr;
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

// One way a piece of armour can turn out better than plain - "a mithril
// helmet of Strength". Filled from world/items/armour_enchantments.lua as
// that script loads.
//
// Six dice fields (dv/pv/hit/dice/z/rng) and a res_flag went with the old
// table: every row left all seven empty or unread, so they described
// nothing. They can come back the day a row wants them.
struct ENHANCE_STRUCT {
    std::string id;

    // How the enchanted item reads. A format string, because the words go
    // on either side depending: "{} of Strength", but "clean {}".
    std::string name;

    // Its colour, or 0 to keep the colour its material gave it.
    int color{0};

    // How rare: the higher this is, the fewer come out enchanted this way.
    // Not the weight in a draw - the threshold a roll has to beat.
    int rarity{0};

    // How many places it takes in the draw. Two rows of the old table were
    // byte-identical, which doubled that outcome's chance; saying so once
    // is the same thing, said honestly.
    int weight{1};

    // Which sorts of item it can appear on.
    ItemKind kind{ItemKind::UNKNOWN};

    AttackEffectType brt{AttackEffectType::NONE};

    // Carried onto the item by SpecialFill(). SPP_SLOWDIGESTION is acted
    // on (XCreature::DecNutrio halves the wearer's hunger); the others are
    // declared here and by the materials table but nothing reads them yet.
    SPECIAL_PROPERTY spp{SPP_NONE};

    std::string s; //stats
    std::string r; //resists
};

// Fluent builder for one of those:
//
//   ArmourEnchantment.new("strength")
//       :Called("{} of Strength")
//       :Rarity(1)
//       :Fits(ItemKind.ARMOUR)
//       :Stats("St:1d4")
//       :Register()
class ArmourEnchantmentBuilder
{
    public:
        explicit ArmourEnchantmentBuilder(std::string id);

        ArmourEnchantmentBuilder& Called(const std::string& name);
        ArmourEnchantmentBuilder& Looks(int color);
        ArmourEnchantmentBuilder& Rarity(int rarity);
        ArmourEnchantmentBuilder& Weight(int weight);
        ArmourEnchantmentBuilder& Fits(ItemKind kind);
        ArmourEnchantmentBuilder& Brand(AttackEffectType brt);
        ArmourEnchantmentBuilder& Property(SPECIAL_PROPERTY spp);
        ArmourEnchantmentBuilder& Stats(const std::string& s);
        ArmourEnchantmentBuilder& Resist(const std::string& r);

        void Register();

    private:
        ENHANCE_STRUCT t;
};

// The row with this id, or nullptr for one nothing defines.
const ENHANCE_STRUCT* FindArmourEnchantment(const std::string& id);

// One drawn at random, weighted, or empty if nothing is defined.
std::string RandomArmourEnchantment();


extern std::vector<ItemMaterial> item_prop;

// The material with this id, or nullptr for one nothing defines.
const ItemMaterial* FindMaterial(const std::string& id);

// Fluent builder for one material:
//
//   Material.new("mithril")
//       :Called("mithril")
//       :Looks(xColor.xLIGHTCYAN)
//       :Sets(ItemSet.MITHRIL)
//       :Chance(5)
//       :Quality(ItemQuality.GOOD)
//       :Body(11, 100)
//       :Armour("1d3+6", "2d3+3")
//       :Combat("2d4+4", "0d1", "2d4+3")
//       :Resist("poison:0d0+10")
//       :Register()
class MaterialBuilder
{
    public:
        explicit MaterialBuilder(std::string id);

        MaterialBuilder& Called(const std::string& name);
        MaterialBuilder& Looks(int color);
        MaterialBuilder& Sets(unsigned int iflag);
        MaterialBuilder& Chance(int probability);
        MaterialBuilder& Quality(ITEM_QUALITY iq);
        MaterialBuilder& Body(int density, int value);
        MaterialBuilder& Armour(const std::string& dv, const std::string& pv);
        MaterialBuilder& Combat(const std::string& hit, const std::string& dice,
            const std::string& extra);
        MaterialBuilder& Resist(const std::string& r);
        MaterialBuilder& Property(SPECIAL_PROPERTY sp);

        void Register();

    private:
        ItemMaterial t;
};
extern std::vector<ENHANCE_STRUCT> ienh_db;


// The pool a kind of item is drawn from, or nullptr for a kind that keeps
// no table of its own (potions, scrolls and the rest build themselves).
XItemBasicStructure* PoolFor(ItemKind kind);

// Fluent builder for one row of one of those pools:
//
//   Template.new(ItemKind.WEAPON, ItemType.LONGSWORD)
//       :View("long sword", '|')
//       :Made(ItemSet.OBSIMETAL, ItemQuality.FAIR)
//       :Skill(XCombatSkills.SWORD)
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
        TemplateBuilder& Skill(COMBAT_SKILL wt);
        TemplateBuilder& Verb(const std::string& verb);
        TemplateBuilder& Launcher(COMBAT_SKILL wt);
        TemplateBuilder& Worth(int value, int weight);
        TemplateBuilder& Armour(const std::string& dv, const std::string& pv);
        TemplateBuilder& Combat(const std::string& hit, const std::string& dice,
            const std::string& extra);
        TemplateBuilder& Range(const std::string& range);
        TemplateBuilder& Chance(int probability);
        TemplateBuilder& OnCreate(const std::string& handler);
        TemplateBuilder& OnOutfit(const std::string& handler);

        void Register();

    private:
        ItemKind kind;
        ItemTemplate t{};
};

#endif
