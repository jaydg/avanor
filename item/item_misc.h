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

#include <functional>
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

// An item with no behaviour of its own: a name, a look and some numbers.
// The carrier that content-defined plain items are instances of, the way
// XAnyFood is the carrier for content-defined foods. It exists because
// XItem::toString() asserts - every leaf is expected to say how it reads -
// and because cereal needs one concrete registered type to restore into.
class XPlainItem : public XItem
{
    public:
        DECLARE_CREATOR(XPlainItem, XItem);
        XPlainItem() = default;
        explicit XPlainItem(XPlainItem* copy) : XItem(copy) {}

        XItem* MakeCopy() override
        {
            return new XPlainItem(this);
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

// Everything world/ has defined for itself, by the id it named it.
// Consulted by CreateObjectByName() before the C++ class factory, so
// CreateObject() takes a content id exactly as it takes a class name.
//
// Entries hold a maker rather than one struct of every field any kind of
// item might need: a food and a plain item have almost nothing in common
// beyond a name, and the builders each know how to make their own.
class XItemStorage
{
    public:
        struct Entry {
            std::function<XItem*()> make;

            // What kind of thing it is, and its weight in the draw when
            // the game asks for one of that kind without saying which.
            // Zero probability - the default - keeps it out of that draw,
            // which is what a quest item wants: no shop should stock a
            // part of an ancient machine.
            ItemKind kind;
            int probability;
        };

        static std::unordered_map<std::string, Entry> items;

        // nullptr when no content defined this id - the caller falls back
        // to XClassFactory.
        static XItem* Create(const std::string& id);

        // One item of that kind, drawn by probability. nullptr if content
        // defined nothing drawable of that kind.
        static XItem* CreateRandom(ItemKind kind);

        // Builds a configured XAnyFood. A named function rather than
        // something inside the builder's lambda, so that XAnyFood can
        // befriend it - consume_nutrio is protected, and this is what used
        // to be a subclass constructor.
        static XItem* MakeFood(const FoodTemplate& t);
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
// A plain item: no behaviour, just what it is called, how it looks and
// what it is worth.
struct PlainItemTemplate {
    std::string name;
    char view;
    int color;
    ItemType it;
    ItemKind kind;
    BODY_PART bp;
    int value;
    int weight;
};

// Fluent builder for those:
//
//   Item.new("ancient_machine_part")
//       :View("ancient machine part", ']', xColor.xDARKGRAY)
//       :Basic(ItemType.ANCIENTMACHINEPART, ItemKind.TOOL, 1000, 15)
//       :Register()
class ItemBuilder
{
    public:
        explicit ItemBuilder(std::string id);

        ItemBuilder& View(const std::string& name, char view, int color);
        ItemBuilder& Basic(ItemType it, ItemKind kind, int value, int weight);
        ItemBuilder& Random(int probability);

        void Register();

    private:
        std::string id;
        int probability;
        PlainItemTemplate t{};
};

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

#endif
