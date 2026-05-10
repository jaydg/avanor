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

#include <fmt/format.h>

#include "creature/creature.h"
#include "helpers/msgwin.h"
#include "item/xanyfood.h"

REGISTER_CLASS(XAnyFood);

XAnyFood::XAnyFood()
{
    food_nutrio = 100;
    consume_nutrio = 100;
    view = '%';
    color = xBROWN;
    im = IM_FOOD;
    consumed_food = 0;
}

int XAnyFood::Compare(XObject * o)
{
    assert(dynamic_cast<XAnyFood*>(o));

    if (((XAnyFood*)o)->consume_nutrio == consume_nutrio
        && ((XAnyFood*)o)->consumed_food == consumed_food
        && ((XAnyFood*)o)->food_nutrio == food_nutrio
        && ((XAnyFood*)o)->it == it) {
        return 0;
    } else {
        return 1;
    }
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

const char* food_type_feeling[] = {
    "delicious", "very tasty", "tasty",
    "tasteless", "taste like a slops", "vomit"
};

const char* XAnyFood::postEat(XCreature * eater)
{
    FOOD_TYPE ft = FoodTypeForCreature(eater);

    if (ft > 1 && ft < 8) {
        return food_type_feeling[ft - 2];
    } else {
        return "not bad";
    }
}

FOOD_TYPE XAnyFood::FoodTypeForCreature(XCreature * creature)
{
    switch (creature->food_feeling) {
        case FF_SENSITIVE :
            if (food_type == FT_VOMIT) {
                return FT_VOMIT;
            } else {
                return (FOOD_TYPE)(food_type + 1);
            }

            break;

        case FF_TOLERANT :
            if (food_type > FT_NORMALFOOD) {
                return (FOOD_TYPE)(food_type - 1);
            } else {
                return FT_NORMALFOOD;
            }

            break;

        case FF_NORMAL :
            return food_type;
            break;

        default:
            assert(0);
            break;
    }

    return food_type;
}

void XAnyFood::Store(XFile * f)
{
    XItem::Store(f);
    f->Write(&food_nutrio, sizeof(int));
    f->Write(&consume_nutrio, sizeof(int));
    f->Write(&consumed_food, sizeof(int));
    f->Write(&food_type, sizeof(FOOD_TYPE));
}

void XAnyFood::Restore(XFile * f)
{
    XItem::Restore(f);
    f->Read(&food_nutrio, sizeof(int));
    f->Read(&consume_nutrio, sizeof(int));
    f->Read(&consumed_food, sizeof(int));
    f->Read(&food_type, sizeof(FOOD_TYPE));
}
