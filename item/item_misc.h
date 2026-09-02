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

#ifndef ITEM_MISC_H
#define ITEM_MISC_H

#include <string>
#include <unordered_map>

#include <cereal/types/set.hpp>

#include "item/itemkind.h"
#include "item/xanyfood.h"

// A food defined by content rather than by a C++ class.
struct FoodTemplate {
    std::string name;
    char view;
    int color;

    // Distinct per food, and load-bearing: XAnyFood::Compare() stacks two
    // foods when their nutrition and their ItemType match, so two different
    // trophies sharing a type would merge into one "heap of (2) rat tails".
    ItemType it;

    int value;
    int weight;
    int food_nutrio;
    int consume_nutrio;

    // How well it sits: FT_BESTFOOD through FT_VOMIT shift how much
    // satiation eating it actually yields (XAnyFood::onEat).
    FOOD_TYPE food_type;

    // Weight in the draw when the game asks for a food and does not say
    // which (XItemFactory::CreateItem). Zero - the default - keeps a food
    // out of that draw entirely, which is what a quest trophy wants: a
    // shop should never stock a rat tail.
    int probability;
};

// The foods world/ has defined, by the id it named them. Consulted by
// CreateObjectByName() before the C++ class factory, so CreateObject()
// takes a Lua food id exactly as it takes a class name.
class XFoodStorage
{
    public:
        static std::unordered_map<std::string, FoodTemplate> food_storage;

        // nullptr when no content defined this id - the caller falls back
        // to XClassFactory.
        static XItem* Create(const std::string& id);

        // A food picked by probability from what content defined, for the
        // paths that ask for "some food" rather than a particular one.
        // nullptr if nothing was defined with a probability at all.
        static XItem* CreateRandom();
};

// Fluent builder, the item-side twin of MonsterBuilder:
//
//   Food.new("rat_tail")
//       :View("rat tail", '%', xColor.xBROWN)
//       :Basic(ItemType.RATTAIL, 100, 3)
//       :Nutrition(10, 10)
//       :Register()
//
// :Taste() and :Random() are optional - a food that says neither is
// ordinary fare that the game will never hand out on its own.
class FoodBuilder
{
    public:
        explicit FoodBuilder(std::string id);

        FoodBuilder& View(const std::string& name, char view, int color);
        FoodBuilder& Basic(ItemType it, int value, int weight);
        FoodBuilder& Nutrition(int food_nutrio, int consume_nutrio);
        FoodBuilder& Taste(FOOD_TYPE food_type);
        FoodBuilder& Random(int probability);

        void Register();

    private:
        std::string id;
        FoodTemplate t{};
};

class XChest : public XItem
{
    public:
        XItemList contain;
        DECLARE_CREATOR(XChest, XItem);
        XChest()
        {
            assert(0);
        }

        XChest(int item_count, ItemKind kind_mask, int low_v, int high_v);
        // Chests are not copyable
        XChest(XChest*) = delete;

        int Compare(XObject * /*o*/) override
        {
            return -1;
        }

        std::string toString() override;
    protected:
        // teardown hook, called by XObject::Invalidate()
        void OnInvalidate() override;
    public:

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XItem>(this));
            ar(contain);
        }
};

class XAncientMachinePart : public XItem
{
    public:
        DECLARE_CREATOR(XAncientMachinePart, XItem);
        XAncientMachinePart()
        {
            color = xDARKGRAY;
            view = ']';
            name = "ancient machine part";
            value = 1000;
            weight = 15;
            kind = ItemKind::TOOL;
            bp = BP_OTHER;
            it = ItemType::ANCIENTMACHINEPART;
        }

        XAncientMachinePart(XAncientMachinePart * copy) : XItem(copy) {}

        XItem* MakeCopy() override
        {
            return new XAncientMachinePart(this);
        }

        std::string toString() override
        {
            return GetFullName();
        }

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XItem>(this));
        }
};

#endif
