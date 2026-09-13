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

#include <algorithm>
#include <iostream>
#include <sol/sol.hpp>

#include <fmt/format.h>

#include "item/item_cereal.h"
#include "helpers/msgwin.h"
#include "item/xanyfood.h"

REGISTER_CLASS(XAnyFood);
CEREAL_REGISTER_TYPE(XAnyFood);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XItem, XAnyFood);

XAnyFood::XAnyFood()
{
    food_nutrio = 100;
    consume_nutrio = 100;
    view = '%';
    color = xBROWN;
    kind = ItemKind::FOOD;
    consumed_food = 0;

    // Until something says otherwise, it is the ordinary sort - which is
    // what FT_NORMALFOOD used to mean, now that content names it.
    food_type = OrdinaryTaste();
}

bool XAnyFood::Compare(XObject* o)
{
    assert(dynamic_cast<XAnyFood*>(o));

    return ((XAnyFood*)o)->consume_nutrio == consume_nutrio
        && ((XAnyFood*)o)->consumed_food == consumed_food
        && ((XAnyFood*)o)->food_nutrio == food_nutrio
        && ((XAnyFood*)o)->it == it;
}

std::string XAnyFood::toString()
{
    std::string str;

    if (quantity == 1) {
        str = name;
    } else {
        str = fmt::format("heap of ({}) {}s", quantity, name);
    }

    double rel = static_cast<double>(consumed_food) / food_nutrio;

    if (rel == 0) {
    } else if (rel < 0.25) {
        str.append(" {3/4}");
    } else if (rel < 0.50) {
        str.append(" {1/2}");
    } else if (rel < 0.75) {
        str.append(" {1/4}");
    } else {
        str.append(" {less than 1/4}");
    }

    return str;
}

RESULT XAnyFood::onEat(XCreature * eater)
{
    int eated_peace = consume_nutrio;

    if (food_nutrio < consume_nutrio) {
        eated_peace = food_nutrio;
    }

    eater->nutrio += eated_peace * 10;
    consumed_food += eated_peace;

    if (eater->isHero()) {
        msgwin.Add(fmt::format("You eat the {}.",name));
    } else if (eater->isVisible()) {
        msgwin.Add(fmt::format("{} {} {}.",
            eater->GetNameEx(CRN_T1),
            eater->GetVerb("eat"),
            name));
    }

    if (consumed_food >= food_nutrio) {
        if (eater->isHero()) {
            msgwin.Add(fmt::format("You find that the {} is {}.",
                name, postEat(eater)));
        } else if (eater->isVisible()) {
            msgwin.Add(fmt::format("{} {} that {} is {}.",
                eater->GetNameEx(CRN_T1),
                eater->GetVerb("find"),
                name,
                postEat(eater)));
        }

        UnCarry();
        Invalidate();
        return SUCCESS;
    } else {
        return CONTINUE;
    }
}

namespace {

std::vector<TasteStats> tastes_db;

}

const TasteStats* FindTaste(const TASTE& id)
{
    for (const auto& row : tastes_db) {
        if (row.id == id) {
            return &row;
        }
    }

    return nullptr;
}

const std::vector<TasteStats>& AllTastes()
{
    return tastes_db;
}

std::string TasteWord(const TASTE& id)
{
    const TasteStats* row = FindTaste(id);
    return row ? row->text : "not bad";
}

int TasteIndex(const TASTE& id)
{
    for (size_t i = 0; i < tastes_db.size(); i++) {
        if (tastes_db[i].id == id) {
            return static_cast<int>(i);
        }
    }

    return -1;
}

const TASTE& TasteAt(const int index)
{
    static const TASTE none;

    if (tastes_db.empty()) {
        return none;
    }

    const int last = static_cast<int>(tastes_db.size()) - 1;

    return tastes_db[std::clamp(index, 0, last)].id;
}

int OrdinaryTasteIndex()
{
    for (size_t i = 0; i < tastes_db.size(); i++) {
        if (tastes_db[i].ordinary) {
            return static_cast<int>(i);
        }
    }

    return -1;
}

const TASTE& OrdinaryTaste()
{
    return TasteAt(OrdinaryTasteIndex());
}

TasteBuilder::TasteBuilder(TASTE id)
{
    t.id = std::move(id);
}

TasteBuilder& TasteBuilder::Called(const std::string& text)
{
    t.text = text;
    return *this;
}

TasteBuilder& TasteBuilder::Ordinary()
{
    t.ordinary = true;
    return *this;
}

void TasteBuilder::Register()
{
    if (t.id.empty()) {
        std::cerr << "world: a taste with no id" << std::endl;
        return;
    }

    if (FindTaste(t.id)) {
        std::cerr << "world: two tastes both called '" << t.id << "'" << std::endl;
        return;
    }

    if (t.text.empty()) {
        std::cerr << "world: the taste '" << t.id << "' says nothing" << std::endl;
        return;
    }

    if (t.ordinary && OrdinaryTasteIndex() >= 0) {
        std::cerr << "world: the taste '" << t.id << "' is the ordinary one, and so is '"
                  << tastes_db[OrdinaryTasteIndex()].id << "'" << std::endl;
    }

    tastes_db.push_back(t);
}

void RegisterTasteLua(sol::state_view& lua)
{
    lua.new_usertype<TasteBuilder>("Taste",
        sol::constructors<TasteBuilder(TASTE)>(),
        "Called", &TasteBuilder::Called,
        "Ordinary", &TasteBuilder::Ordinary,
        "Register", &TasteBuilder::Register
    );
}

// Complains if no row is registered under this id and returns false.
bool CheckTasteExists(const TASTE& id, const char* where)
{
    if (FindTaste(id)) {
        return true;
    }

    std::cerr << "world: " << where << " names a taste '" << id
              << "', which world/tastes.lua does not declare" << std::endl;

    return false;
}

std::string XAnyFood::postEat(XCreature *eater)
{
    return TasteWord(TasteForCreature(eater));
}

// The same food is not the same meal to every stomach. Which taste each
// of these lands on is a step along the order world/tastes.lua declares -
// no taste is named here.
TASTE XAnyFood::TasteForCreature(XCreature * creature)
{
    const int here = TasteIndex(food_type);

    if (here < 0) {
        return food_type;
    }

    switch (creature->food_feeling) {
        // Everything tastes a little worse than it is, and the worst
        // there is cannot get worse - TasteAt() stops at the end.
        case FF_SENSITIVE :
            return TasteAt(here + 1);

        // Nothing tastes worse than ordinary to a stomach that takes
        // anything - and, oddly but as it has always been, nothing tastes
        // better than ordinary either.
        case FF_TOLERANT : {
            const int ordinary = OrdinaryTasteIndex();

            if (ordinary < 0) {
                return food_type;
            }

            return here > ordinary ? TasteAt(here - 1) : TasteAt(ordinary);
        }

        case FF_NORMAL :
            return food_type;

        default:
            assert(0);
            break;
    }

    return food_type;
}

