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

XItem* XItemFactory::CreateAnyItem(ItemKind kind, ITEM_TYPE _it, int low_v, int hi_v)
{
    int n = 100;

    while (n-- > 0) {
        XItem * it = XItemFactory::CreateItem(kind, _it);

        if (it->GetValue() >= low_v && it->GetValue() <= hi_v) {
            return it;
        } else {
            it->Invalidate();
        }
    }

    return XItemFactory::CreateItem(kind, _it);;
}

XItem* XItemFactory::CreateItem(ItemKind kind, ITEM_TYPE it)
{
    ItemKind picked_kind = static_cast<ItemKind>(vGetRandomBit(static_cast<unsigned int>(kind)));

    switch (picked_kind) {
        case ItemKind::IM_HAT:
            return new XCap(it);
            break;

        case ItemKind::IM_NECK:
            return new XAmulet();
            break;

        case ItemKind::IM_RING:
            return new XRing();
            break;

        case ItemKind::IM_BODY:
            return new XArmor(it);
            break;

        case ItemKind::IM_CLOAK:
            return new XCloak(it);
            break;

        case ItemKind::IM_WEAPON:
            return new XWeapon(it);
            break;

        case ItemKind::IM_SHIELD:
            return new XShield(it);
            break;

        case ItemKind::IM_GLOVES:
            return new XGloves(it);
            break;

        case ItemKind::IM_BOOTS:
            return new XBoots(it);
            break;

        case ItemKind::IM_MISSILEW:
            return new XMissileWeapon(it);

        case ItemKind::IM_MISSILE:
            return new XMissile(it);

        case ItemKind::IM_WAND:
        case ItemKind::IM_TOOL:
        case ItemKind::IM_GEM:
        case ItemKind::IM_FOOD:
            return new XRation(it);
            break;

        case ItemKind::IM_MONEY:
            return new XMoney(vRand(40) + 1);
            break;

        case ItemKind::IM_POTION:
            return new XPotion();
            break;

        case ItemKind::IM_SCROLL:
            return new XScroll();
            break;

        case ItemKind::IM_BOOK:
            return new XBook();
            break;
            return new XCap();
            break;

        default:
            return new XMoney(vRand(40) + 1);
            break;
    }

    assert(0);
    return nullptr;
}
