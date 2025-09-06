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

#include "itemf.h"

_CREATURE people_db[] = {
    {
        "farmer", 'p', xBROWN,
        "1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d100+1200",
        "head neck body hand hand boots",
        "St 2d3 Dx 2d3 To 1d3 Le 2d3 Wi 1d3 Ma 1d2 Pe 1d6 Ch 1d3",
        "",
        "1d1", "1d1-1", "1d2", "1d2", "2d3+1", "2d2",
        CRL_VL,
    },

    {
        "bandit", 'p', xLIGHTGRAY,
        "1d10+95", "0d0+1000", "0d0+1000", CS_NORMAL, "1d100+1200",
        "head neck cloak body hand hand gloves boots",
        "St 1d4+10 Dx 1d4+10 To 1d4+10 Le 1d4+10 Wi 1d4+10 Ma 1d4+10 Pe 1d4+10 Ch 1d4+10",
        "",
        "1d4", "1d1-1", "1d4", "1d3", "1d4+10", "1d4+10",
        CRL_LOW,
    }

};

REGISTER_CLASS(XPeople);

XPeople::XPeople(CREATURE_SUBCLASS subclass, CREATURE_LEVEL crl, CREATURE_EXP cre)
    : XAnyCreature(people_db, 2, subclass, crl, cre)
{
}

void XPeople::Equip(int low_value, int high_value)
{
    low_value = low_value > 0 ? low_value : 100;
    high_value = high_value > 0 ? high_value : 400;
    ITEM_MASK tem_im = (ITEM_MASK)(IM_SCROLL | IM_POTION);
    int item_count = vRand() % 2;

    XBodyPart * hand_1 = GetBodyPart(BP_HAND, 0);
    XBodyPart * hand_2 = GetBodyPart(BP_HAND, 1);

    XAnyCreature::StdEquip(5, (ITEM_MASK)(IM_WEAPON | IM_SHIELD), item_count, tem_im, low_value, high_value);

    switch (creature_subclass) {
        case CRS_BANDIT:
            XItem * item;
            item = ICREATEB(IM_WEAPON, IT_DAGGER, low_value, high_value);
            hand_1->Wear(item);
            item = ICREATEB(IM_WEAPON, IT_DAGGER, low_value, high_value);
            hand_2->Wear(item);
            wsk->SetLevel(WSK_DAGGER, 7);
            break;

    }
}
