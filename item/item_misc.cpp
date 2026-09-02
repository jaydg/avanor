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
#include <utility>

#include "item/item_cereal.h"
#include "item/item_misc.h"
#include "item/itemf.h"

REGISTER_CLASS(XPlainItem);
CEREAL_REGISTER_TYPE(XPlainItem);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XItem, XPlainItem);

std::unordered_map<std::string, XItemStorage::Entry> XItemStorage::items;

XItem* XItemStorage::Create(const std::string& id)
{
    const auto rec = items.find(id);

    if (rec == items.end()) {
        return nullptr;
    }

    return rec->second.make();
}

XItem* XItemStorage::CreateRandom(const ItemKind kind)
{
    int total = 0;

    for (const auto& [id, e] : items) {
        if (e.kind == kind) {
            total += e.probability;
        }
    }

    if (total == 0) {
        return nullptr;
    }

    long roll = vRand(total);

    for (const auto& [id, e] : items) {
        if (e.kind != kind) {
            continue;
        }

        roll -= e.probability;

        if (roll < 0) {
            return e.make();
        }
    }

    return nullptr;
}

XItem* XItemStorage::MakeFood(const FoodTemplate& t)
{
    // A plain XAnyFood, configured. It needs no subclass and no cereal
    // registration of its own: XAnyFood is already a registered concrete
    // type, and every field set here is serialized instance state.
    auto* food = new XAnyFood();

    food->name = t.name;
    food->view = t.view;
    food->color = t.color;
    food->it = t.it;
    food->value = t.value;
    food->weight = t.weight;
    food->food_nutrio = t.food_nutrio;
    food->consume_nutrio = t.consume_nutrio;
    food->food_type = t.food_type;

    return food;
}

ItemBuilder::ItemBuilder(std::string id) : id(std::move(id)), probability(0)
{
    t.view = ']';
    t.color = xLIGHTGRAY;
    t.it = ItemType::UNKNOWN;
    t.kind = ItemKind::TOOL;
    t.bp = BP_OTHER;
}

ItemBuilder& ItemBuilder::View(const std::string& name, const char view, const int color)
{
    t.name = name;
    t.view = view;
    t.color = color;
    return *this;
}

ItemBuilder& ItemBuilder::Basic(const ItemType it, const ItemKind kind, const int value, const int weight)
{
    t.it = it;
    t.kind = kind;
    t.value = value;
    t.weight = weight;
    return *this;
}

ItemBuilder& ItemBuilder::Random(const int probability)
{
    this->probability = probability;
    return *this;
}

void ItemBuilder::Register()
{
    if (XItemStorage::items.count(id)) {
        std::cerr << "world: item '" << id << "' is defined twice" << std::endl;
    }

    if (t.name.empty()) {
        std::cerr << "world: item '" << id << "' has no name" << std::endl;
    }

    // Same reasoning as MonsterBuilder::Register(): a Lua constant the
    // engine never registered arrives here as 0 rather than as an error.
    if (t.it == ItemType::UNKNOWN) {
        std::cerr << "world: item '" << id << "' has no ItemType" << std::endl;
    }

    const PlainItemTemplate captured = t;

    XItemStorage::items[id] = {
        [captured] {
            auto* item = new XPlainItem();

            item->name = captured.name;
            item->view = captured.view;
            item->color = captured.color;
            item->it = captured.it;
            item->kind = captured.kind;
            item->bp = captured.bp;
            item->value = captured.value;
            item->weight = captured.weight;

            // Every hand-written item class allocates these in its own
            // constructor (XPickAxe, XEyeOfRaa, ...), and code outside
            // Compare() dereferences them without checking - the value
            // calculation and the description among them. A content item
            // is an item like any other, so it gets them too.
            item->stats = std::make_unique<XStats>();
            item->resistances = std::make_unique<XResistance>();

            return static_cast<XItem*>(item);
        },
        t.kind,
        probability
    };
}

FoodBuilder::FoodBuilder(std::string id) : id(std::move(id))
{
    // XAnyFood's own constructor defaults (a brown '%', ItemKind::FOOD)
    // still apply to what Create() news up; these are only the fields a
    // definition is expected to state for itself.
    t.view = '%';
    t.color = xBROWN;
    t.it = ItemType::UNKNOWN;
    t.food_type = FT_NORMALFOOD;
    t.probability = 0;
}

FoodBuilder& FoodBuilder::View(const std::string& name, const char view, const int color)
{
    t.name = name;
    t.view = view;
    t.color = color;
    return *this;
}

FoodBuilder& FoodBuilder::Basic(const ItemType it, const int value, const int weight)
{
    t.it = it;
    t.value = value;
    t.weight = weight;
    return *this;
}

FoodBuilder& FoodBuilder::Nutrition(const int food_nutrio, const int consume_nutrio)
{
    t.food_nutrio = food_nutrio;
    t.consume_nutrio = consume_nutrio;
    return *this;
}

FoodBuilder& FoodBuilder::Taste(const FOOD_TYPE food_type)
{
    t.food_type = food_type;
    return *this;
}

FoodBuilder& FoodBuilder::Random(const int probability)
{
    t.probability = probability;
    return *this;
}

void FoodBuilder::Register()
{
    // Same reasoning as MonsterBuilder::Register(): a Lua constant the
    // engine never registered arrives here as 0 rather than as an error,
    // and the food looks fine until something asks it what it is. An
    // unnamed food, or one that never said what ItemType it is, is a
    // definition that says nothing.
    if (XItemStorage::items.count(id)) {
        std::cerr << "world: food '" << id << "' is defined twice" << std::endl;
    }

    if (t.name.empty()) {
        std::cerr << "world: food '" << id << "' has no name" << std::endl;
    }

    // ItemType is what XAnyFood::Compare() stacks on, so two foods sharing
    // one would merge in the pack however differently they are described.
    if (t.it == ItemType::UNKNOWN) {
        std::cerr << "world: food '" << id << "' has no ItemType" << std::endl;
    }

    const FoodTemplate captured = t;

    XItemStorage::items[id] = {
        [captured] { return XItemStorage::MakeFood(captured); },
        ItemKind::FOOD,
        t.probability
    };
}

REGISTER_CLASS(XChest);
CEREAL_REGISTER_TYPE(XChest);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XItem, XChest);
// XChest()'s only no-args constructor is an assert(0) guard - real
// instances always come from XChest(int, ItemKind, int, int), so
// route Cereal's load-time construction through the DUMMY_STRUCT idiom
// instead of that assert.
CEREAL_LOAD_VIA_DUMMY_CONSTRUCT(XChest, serialize);

XChest::XChest(int item_count, ItemKind kind_mask, int low_v, int high_v)
{
    color = xLIGHTGRAY;
    view = '~';
    it = ItemType::CHEST;
    name = "chest";
    value = 50;
    weight = 100;
    kind = ItemKind::CHEST;
    bp = BP_OTHER;

    for (int i = 0; i < item_count; i++) {
        XItem * it = ICREATE(kind_mask, low_v, high_v);
        weight += it->weight;
        contain.insert(XItem::Own(it));
    }
}

std::string XChest::toString()
{
    auto str = name;

    if (contain.empty()) {
        str.append("{{empty}}");
    }

    return str;
}

void XChest::OnInvalidate()
{
    contain.InvalidateAll();

    XItem::OnInvalidate();
}

