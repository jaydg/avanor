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

#include <cereal/types/base_class.hpp>

#include "item/item.h"

enum FOOD_TYPE {
    FT_BESTFOOD     = 0x00000002, // feel very nice, +50% to food satiation
    FT_GOODFOOD     = 0x00000003, // feel nice, +25% to food satiation
    FT_NORMALFOOD   = 0x00000004, // ok
    FT_BADFOOD      = 0x00000005, // -25% to food satiation
    FT_VERYBADFOOD  = 0x00000006, // -50% to food satiation
    FT_VOMIT        = 0x00000007, // -XXX from total satiation
};

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
        FOOD_TYPE FoodTypeForCreature(XCreature * creature);

        // What it tastes like. Content states it with Food.new():Taste()
        // when it is worth stating - elvish waybread does, bat wings and
        // rat tails and bones do not - and everything else is plain food.
        // Needs a value here rather than in the constructor, because
        // nothing but this declaration is common to every way a food comes
        // into existence.
        FOOD_TYPE food_type = FT_NORMALFOOD;
    protected:
        // Content-defined foods (world/items.lua) are plain XAnyFood
        // instances configured after construction rather than subclasses
        // configured in a constructor, so the one place that does that
        // configuring needs the same reach a subclass had.
        friend class XItemStorage;

        int consume_nutrio; // part of food eated for one turn

};

#endif
