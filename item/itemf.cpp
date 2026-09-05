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
#include "item/item_misc.h"
#include "item/xamulet.h"
#include "item/xarmor.h"
#include "item/xbook.h"
#include "item/xboots.h"
#include "item/xcap.h"
#include "item/xcloak.h"
#include "item/xgloves.h"
#include "item/xmissile.h"
#include "item/xmissileweapon.h"
#include "item/xmoney.h"
#include "item/xpotion.h"
#include "item/xring.h"
#include "item/xscroll.h"
#include "item/xshield.h"
#include "item/xweapon.h"

XItem* XItemFactory::CreateAnyItem(ItemKind kind, ItemType _it, int low_v, int hi_v)
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

XItem* XItemFactory::CreateItem(ItemKind kind, ItemType it)
{
    ItemKind picked_kind = static_cast<ItemKind>(vGetRandomBit(static_cast<unsigned int>(kind)));

    switch (picked_kind) {
        case ItemKind::HAT:
            return new XCap(it);
            break;

        case ItemKind::NECK:
            return new XAmulet();
            break;

        case ItemKind::RING:
            return new XRing();
            break;

        case ItemKind::BODY:
            return new XArmor(it);
            break;

        case ItemKind::CLOAK:
            return new XCloak(it);
            break;

        case ItemKind::WEAPON:
            return new XWeapon(it);
            break;

        case ItemKind::SHIELD:
            return new XShield(it);
            break;

        case ItemKind::GLOVES:
            return new XGloves(it);
            break;

        case ItemKind::BOOTS:
            return new XBoots(it);
            break;

        case ItemKind::MISSILEW:
            return new XMissileWeapon(it);

        case ItemKind::MISSILE:
            return new XMissile(it);

        // FIXME:
        // Long-standing oddity kept as it was: asking for a random wand,
        // tool or gem has always produced food. Only the source has moved -
        // the rations are defined in world/items.lua now, and are picked by
        // the probabilities stated there.
        case ItemKind::WAND:
        case ItemKind::TOOL:
        case ItemKind::GEM:
        case ItemKind::FOOD:
            if (XItem* food = XItemStorage::CreateRandom(ItemKind::FOOD)) {
                return food;
            }
            break;

        case ItemKind::MONEY:
            return new XMoney(vRand(40) + 1);
            break;

        case ItemKind::POTION:
            return new XPotion();
            break;

        case ItemKind::SCROLL:
            return new XScroll();
            break;

        case ItemKind::BOOK:
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
