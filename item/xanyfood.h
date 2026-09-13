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

#ifndef XANYFOOD_H
#define XANYFOOD_H

#include <string>
#include <vector>

#include <cereal/types/base_class.hpp>
#include <cereal/types/string.hpp>

#include <sol/forward.hpp>

#include "item/item.h"

// How well something sits, from delicious down to outright sick-making.
// Which tastes there are, what an eater calls them, and what order they
// run in is content (world/tastes.lua). The engine only ever steps along
// that order - a delicate stomach finds everything a little worse than it
// is - and never names a taste of its own.
using TASTE = std::string;

struct TasteStats {
    TASTE id;

    // What the eater says of it: "You find that the apple is delicious."
    std::string text;

    // Which one a stomach that takes anything settles on: it finds
    // nothing worse than this, and nothing better either.
    bool ordinary = false;
};

const TasteStats* FindTaste(const TASTE& id);
const std::vector<TasteStats>& AllTastes();

// What an eater says of this taste. A taste nothing declares is "not
// bad", which is what an unreadable food type used to come out as.
std::string TasteWord(const TASTE& id);

// Where a taste sits in the order content declared, and what sits at a
// place in it. The index is clamped, so stepping off either end simply
// stays there; an unknown taste has no place and answers -1.
int TasteIndex(const TASTE& id);
const TASTE& TasteAt(int index);

// The place of the one marked :Ordinary(), or -1 if content marked none.
int OrdinaryTasteIndex();

// That one by id - what anything nobody has said a word about tastes of.
const TASTE& OrdinaryTaste();

class TasteBuilder
{
    public:
        explicit TasteBuilder(TASTE id);

        TasteBuilder& Called(const std::string& text);
        TasteBuilder& Ordinary();
        void Register();

    private:
        TasteStats t;
};

void RegisterTasteLua(sol::state_view& lua);

// Complains if no row is registered under this id and returns false.
bool CheckTasteExists(const TASTE& id, const char* where);

class XAnyFood : public XItem
{
    public:
        DECLARE_CREATOR(XAnyFood, XItem);
        XAnyFood();
        // Every member the class holds, food_type included: MakeCopy()
        // is how a stack is split (drop 3 of 7 rations, sell 2 of 5),
        // and the part that leaves the stack is the same food as the
        // part that stays.
        XAnyFood(XAnyFood * food) : XItem((XItem*)food), food_nutrio(food->food_nutrio),
            consumed_food(food->consumed_food), food_type(food->food_type),
            consume_nutrio(food->consume_nutrio) {}

        XItem* MakeCopy() override
        {
            return new XAnyFood(this);
        }

        std::string toString() override;
        virtual RESULT onEat(XCreature * eater); // eat a peace from food
        [[nodiscard]] virtual std::string postEat(XCreature *eater);
        bool Compare(XObject* o) override;

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XItem>(this));
            ar(food_nutrio, consume_nutrio, consumed_food, food_type);
        }

        int food_nutrio;
        int consumed_food;  // how much is eated
        TASTE TasteForCreature(XCreature * creature);

        // What it tastes like. Content states it with Food.new():Taste()
        // when it is worth stating - elvish waybread does, bat wings and
        // rat tails and bones do not - and everything else is plain food.
        // Needs a value here rather than in the constructor, because
        // nothing but this declaration is common to every way a food comes
        // into existence.
        TASTE food_type;
    protected:
        // Content-defined foods (world/items.lua) are plain XAnyFood
        // instances configured after construction rather than subclasses
        // configured in a constructor, so the one place that does that
        // configuring needs the same reach a subclass had.
        friend class XItemStorage;

        int consume_nutrio; // part of food eated for one turn

};

#endif
