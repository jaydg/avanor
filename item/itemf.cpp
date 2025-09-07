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

#include "engine/xapi.h"
#include "item/itemf.h"

XItem* XItemFactory::CreateAnyItem(ITEM_MASK im, ITEM_TYPE _it, int low_v, int hi_v)
{
    int n = 100;

    while (n-- > 0) {
        XItem * it = XItemFactory::CreateItem(im, _it);

        if (it->GetValue() >= low_v && it->GetValue() <= hi_v) {
            return it;
        } else {
            it->Invalidate();
        }
    }

    return XItemFactory::CreateItem(im, _it);;
}

XItem* XItemFactory::CreateItem(ITEM_MASK im, ITEM_TYPE it)
{
    ITEM_MASK nim = (ITEM_MASK)vGetRandomBit(im);

    switch (nim) {
        case IM_HAT:
            return new XCap(it);
            break;

        case IM_NECK:
            return new XAmulet();
            break;

        case IM_RING:
            return new XRing();
            break;

        case IM_BODY:
            return new XArmor(it);
            break;

        case IM_CLOAK:
            return new XCloak(it);
            break;

        case IM_WEAPON:
            return new XWeapon(it);
            break;

        case IM_SHIELD:
            return new XShield(it);
            break;

        case IM_GLOVES:
            return new XGloves(it);
            break;

        case IM_BOOTS:
            return new XBoots(it);
            break;

        case IM_MISSILEW:
            return new XMissileWeapon(it);

        case IM_MISSILE:
            return new XMissile(it);

        case IM_WAND:
        case IM_HERB:
        case IM_TOOL:
        case IM_GEM:
        case IM_FOOD:
            return new XRation(it);
            break;

        case IM_MONEY:
            return new XMoney(vRand(40) + 1);
            break;

        case IM_POTION:
            return new XPotion();
            break;

        case IM_SCROLL:
            return new XScroll();
            break;

        case IM_BOOK:
            return new XBook();
            break;
            return new XCap();
            break;

        default:
            return new XMoney(vRand(40) + 1);
            break;
    }

    assert(0);
    return NULL;
}
