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

#include "item/xration.h"

REGISTER_CLASS(XRation);

struct _SIMPLE_RATION {
    ITEM_TYPE it;
    FOOD_TYPE food_type;
    int color;
    const char* name;
    const char* nutrio;
    int value;
    int weight;
};

const int ration_db_sz = 4;

_SIMPLE_RATION rations_db[ration_db_sz] = {
    {IT_LARGERATION,	FT_NORMALFOOD,	xLIGHTGRAY,	"large ration", "0d0+400",	5,	100},
    {IT_RATION,	FT_NORMALFOOD,	xBROWN,	"ration", "0d0+250",	3,	70},
    {IT_SMALLRATION,	FT_NORMALFOOD,	xBROWN,	"small ration", "0d0+150",	1,	40},
    {IT_ELVISHWAYBREAD,	FT_GOODFOOD,	xYELLOW,	"elvish waybread", "0d0+700",	15,	15},

};

XRation::XRation(ITEM_TYPE _it) : XAnyFood()
{
    _SIMPLE_RATION * ration = nullptr;

    if (_it == IT_RANDOM) {
        int rn = vRand() % ration_db_sz;
        ration = &rations_db[rn];
    } else {
        for (int i = 0; i < ration_db_sz; i++) {
            if (rations_db[i].it == _it) {
                ration = &rations_db[i];
                break;
            }
        }
    }

    assert(ration);
    it = ration->it;
    name = ration->name;
    XDice d(ration->nutrio);
    food_nutrio = d.S;
    value = ration->value;
    weight = ration->weight;
    consume_nutrio = (food_nutrio * 20) / weight;
    food_type = ration->food_type;
}

RESULT XRation::onEat(XCreature * eater)
{
    return XAnyFood::onEat(eater);
}
