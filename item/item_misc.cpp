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

#include <sol/sol.hpp>

#include "engine/xlua.h"
#include "item/item_cereal.h"
#include "item/item_misc.h"
#include "item/itemf.h"

REGISTER_CLASS(XPlainItem);
CEREAL_REGISTER_TYPE(XPlainItem);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XItem, XPlainItem);

REGISTER_CLASS(XLuaTool);
CEREAL_REGISTER_TYPE(XLuaTool);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XItem, XLuaTool);

void XLuaTool::OnInvalidate()
{
    // Mirrors what XCookingSet used to do with its corpse: the tool holds
    // the only reference while it works, so letting it go silently would
    // destroy whatever was being worked on.
    if (held) {
        held->Invalidate();
        held = nullptr;
    }

    XItem::OnInvalidate();
}

RESULT XLuaTool::onUse(const ItemUsageState uis, XCreature* cr)
{
    if (use_handler.empty()) {
        return FAIL;
    }

    sol::state_view lua(XLua::State());
    sol::protected_function handler = lua[use_handler];

    if (!handler.valid()) {
        std::cerr << "world: " << name << " wants to be used through '"
                  << use_handler << "', which is not defined" << std::endl;

        return FAIL;
    }

    // this/cr stay void*, like every other handler argument - see
    // XCreature::RegisterLua for why the dispatch never went typed.
    const auto result = handler(static_cast<int>(uis), (void*)this, (void*)cr);

    if (!result.valid()) {
        const sol::error err = result;
        std::cerr << "world: " << use_handler << ": " << err.what() << std::endl;

        return FAIL;
    }

    // A handler that says nothing did its work and is finished; the three
    // other answers have to be spelt out.
    if (result.get_type() == sol::type::none || result.get_type() == sol::type::lua_nil) {
        return SUCCESS;
    }

    return static_cast<RESULT>(result.get<int>());
}

REGISTER_CLASS(XContentWeapon);
CEREAL_REGISTER_TYPE(XContentWeapon);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XItem, XContentWeapon);

REGISTER_CLASS(XContentCap);
CEREAL_REGISTER_TYPE(XContentCap);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XItem, XContentCap);

REGISTER_CLASS(XContentShield);
CEREAL_REGISTER_TYPE(XContentShield);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XItem, XContentShield);

REGISTER_CLASS(XContentCloak);
CEREAL_REGISTER_TYPE(XContentCloak);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XItem, XContentCloak);

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

XItem* XItemStorage::MakeItem(const std::string& id, const ContentItemTemplate& t)
{
    // The base constructor first, exactly as the hand-written classes did:
    // it is what binds a weapon to its war skill and picks the material,
    // neither of which a definition restates.
    XItem* item = nullptr;
    std::string* content_id = nullptr;
    std::string* display_name = nullptr;
    bool* unique = nullptr;
    bool* artifact = nullptr;

    switch (t.base) {
        case ContentItemTemplate::PLAIN: {
            auto* made = new XPlainItem();
            made->kind = t.kind;
            made->bp = t.bp;
            content_id = &made->content_id;
            unique = &made->unique;
            artifact = &made->artifact;
            item = made;
            break;
        }

        case ContentItemTemplate::WEAPON: {
            auto* made = new XContentWeapon(t.base_type);
            content_id = &made->content_id;
            display_name = &made->display_name;
            unique = &made->unique;
            artifact = &made->artifact;
            item = made;
            break;
        }

        case ContentItemTemplate::CAP: {
            auto* made = new XContentCap(t.base_type);
            content_id = &made->content_id;
            display_name = &made->display_name;
            unique = &made->unique;
            artifact = &made->artifact;
            item = made;
            break;
        }

        case ContentItemTemplate::SHIELD: {
            auto* made = new XContentShield(t.base_type);
            content_id = &made->content_id;
            display_name = &made->display_name;
            unique = &made->unique;
            artifact = &made->artifact;
            item = made;
            break;
        }

        case ContentItemTemplate::CLOAK: {
            auto* made = new XContentCloak(t.base_type);
            content_id = &made->content_id;
            display_name = &made->display_name;
            unique = &made->unique;
            artifact = &made->artifact;
            item = made;
            break;
        }

        case ContentItemTemplate::TOOL: {
            auto* made = new XLuaTool();
            made->kind = t.kind;
            made->bp = t.bp;
            made->use_handler = t.use_handler;
            content_id = &made->content_id;
            unique = &made->unique;
            artifact = &made->artifact;
            item = made;
            break;
        }
    }

    *content_id = id;
    *unique = t.unique;
    *artifact = t.artifact;

    // A plain item has no base whose name it could fall back to, so it is
    // simply called what it is called - there is nothing for display_name
    // to add.
    if (display_name) {
        *display_name = t.display_name;
    }

    item->name = t.name;
    item->it = t.it;

    if (t.view != 0) {
        item->view = t.view;
    }

    if (t.color >= 0) {
        item->color = t.color;
    }

    item->value = t.value;
    item->weight = t.weight;
    item->dv = t.dv;
    item->pv = t.pv;
    item->to_hit = t.to_hit;
    item->dice.Setup(t.dice_count, t.dice_sides, t.dice_bonus);
    item->RNG = 0;

    // Every hand-written item class allocates these in its own constructor,
    // and code outside Compare() dereferences them without checking - the
    // value calculation and the description among them.
    item->resistances = std::make_unique<XResistance>(t.resists.c_str());
    item->stats = std::make_unique<XStats>(t.stats.c_str());
    item->aet = t.aet;

    // BasicFill() gives one item in twenty a random "of Strength" style
    // enhancement. A definition is already what it says it is.
    item->special_property = SPP_NONE;

    return item;
}

ItemBuilder::ItemBuilder(std::string id) : id(std::move(id)), probability(0)
{
    t.base = ContentItemTemplate::PLAIN;
    t.base_type = IT_NONE;
    t.it = IT_NONE;
    t.kind = ItemKind::TOOL;
    t.bp = BP_OTHER;
    t.view = 0;
    t.color = -1;
    t.aet = BrandSet();
    t.unique = false;
    t.artifact = false;
}

ItemBuilder& ItemBuilder::Plain(const ItemType it, const ItemKind kind)
{
    t.base = ContentItemTemplate::PLAIN;
    t.it = it;
    t.kind = kind;
    return *this;
}

ItemBuilder& ItemBuilder::Weapon(const ItemType base_type)
{
    t.base = ContentItemTemplate::WEAPON;
    t.base_type = base_type;
    t.it = base_type;
    t.kind = ItemKind::WEAPON;
    return *this;
}

ItemBuilder& ItemBuilder::Cap(const ItemType base_type)
{
    t.base = ContentItemTemplate::CAP;
    t.base_type = base_type;
    t.it = base_type;
    t.kind = ItemKind::HAT;
    return *this;
}

ItemBuilder& ItemBuilder::Shield(const ItemType base_type)
{
    t.base = ContentItemTemplate::SHIELD;
    t.base_type = base_type;
    t.it = base_type;
    t.kind = ItemKind::SHIELD;
    return *this;
}

ItemBuilder& ItemBuilder::Cloak(const ItemType base_type)
{
    t.base = ContentItemTemplate::CLOAK;
    t.base_type = base_type;
    t.it = base_type;
    t.kind = ItemKind::CLOAK;
    return *this;
}

ItemBuilder& ItemBuilder::Tool(const ItemType it)
{
    t.base = ContentItemTemplate::TOOL;
    t.it = it;
    t.kind = ItemKind::TOOL;
    t.bp = BP_TOOL;
    return *this;
}

ItemBuilder& ItemBuilder::View(const std::string& name, sol::optional<std::string> view,
    sol::optional<int> color)
{
    t.name = name;
    t.view = (view && !view->empty()) ? (*view)[0] : 0;
    t.color = color.value_or(-1);
    return *this;
}

ItemBuilder& ItemBuilder::Wear(const int bodypart)
{
    t.bp = static_cast<BODY_PART>(bodypart);
    return *this;
}

ItemBuilder& ItemBuilder::Type(const ItemType it)
{
    t.it = it;
    return *this;
}

ItemBuilder& ItemBuilder::Basic(const int value, const int weight)
{
    t.value = value;
    t.weight = weight;
    return *this;
}

ItemBuilder& ItemBuilder::Armour(const int dv, const int pv)
{
    t.dv = dv;
    t.pv = pv;
    return *this;
}

ItemBuilder& ItemBuilder::Combat(const int to_hit, const int count, const int sides, const int bonus)
{
    t.to_hit = to_hit;
    t.dice_count = count;
    t.dice_sides = sides;
    t.dice_bonus = bonus;
    return *this;
}

ItemBuilder& ItemBuilder::Stats(const std::string& stats)
{
    t.stats = stats;
    return *this;
}

ItemBuilder& ItemBuilder::Resist(const std::string& resists)
{
    t.resists = resists;
    return *this;
}

// Called once per brand: :Brand("fire"):Brand("cold"). Adding rather than
// replacing, so each brand a weapon carries is stated on its own.
ItemBuilder& ItemBuilder::Brand(const std::string& aet)
{
    if (CheckBrandExists(aet, "an item definition")) {
        t.aet.Add(aet);
    }

    return *this;
}

ItemBuilder& ItemBuilder::Called(const std::string& display_name)
{
    t.display_name = display_name;
    return *this;
}

ItemBuilder& ItemBuilder::Unique()
{
    t.unique = true;
    return *this;
}

ItemBuilder& ItemBuilder::Artifact()
{
    t.artifact = true;
    return *this;
}

ItemBuilder& ItemBuilder::Use(const std::string& handler)
{
    t.use_handler = handler;
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
    // engine never registered arrives here as 0 rather than as an error,
    // and the item looks fine until something asks it what it is.
    if (t.it == IT_NONE) {
        std::cerr << "world: item '" << id << "' never said what it is" << std::endl;
    }

    const ContentItemTemplate captured = t;
    const std::string captured_id = id;

    XItemStorage::items[id] = {
        [captured_id, captured] { return XItemStorage::MakeItem(captured_id, captured); },
        t.kind,
        probability
    };
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

FoodBuilder::FoodBuilder(std::string id) : id(std::move(id))
{
    // XAnyFood's own constructor defaults (a brown '%', ItemKind::FOOD)
    // still apply to what MakeFood() news up; these are only the fields a
    // definition is expected to state for itself.
    t.view = '%';
    t.color = xBROWN;
    t.it = IT_NONE;
    t.food_type = OrdinaryTaste();
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

FoodBuilder& FoodBuilder::Taste(const std::string& food_type)
{
    if (!CheckTasteExists(food_type, "a food")) {
        return *this;
    }

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
    if (t.it == IT_NONE) {
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
    it = IT_CHEST;
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

