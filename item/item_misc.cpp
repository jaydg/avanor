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

#include "item/item_misc.h"
#include "item/itemf.h"

REGISTER_CLASS(XBatWing);
REGISTER_CLASS(XRatTail);
REGISTER_CLASS(XBone);
REGISTER_CLASS(XAncientMachinePart);
REGISTER_CLASS(XChest);

XChest::XChest(int item_count, ITEM_MASK imask, int low_v, int high_v)
{
    color = xLIGHTGRAY;
    view = '~';
    it = IT_CHEST;
    name = "chest";
    value = 50;
    weight = 100;
    im = IM_CHEST;
    bp = BP_OTHER;

    for (int i = 0; i < item_count; i++) {
        XItem * it = ICREATE(imask, low_v, high_v);
        weight += it->weight;
        contain.insert(it);
    }
}

void XChest::toString(char* buf)
{
    sprintf(buf, name.c_str());

    if (contain.empty()) {
        strcat(buf, "{empty}");
    }
}

void XChest::Store(XFile * f)
{
    XItem::Store(f);

    // FIXME: Implement when porting saving/restoring to Cereal
    // contain.StoreList(f);
}

void XChest::Restore(XFile * f)
{
    XItem::Restore(f);

    // FIXME: Implement when porting saving/restoring to Cereal
    // contain.RestoreList(f);
}
