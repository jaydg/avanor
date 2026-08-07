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

#include <cereal/archives/json.hpp>
#include <cereal/types/polymorphic.hpp>

#include "creature/creature.h"
#include "creature/skeep_ai.h"
#include "game/shop.h"
#include "item/itemf.h"

REGISTER_CLASS(XShop);
CEREAL_REGISTER_TYPE(XShop);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XAnyPlace, XShop);

void XShop::SetShopkeeper(XCreature * shopkeeper)
{
    owner = XCreature::ToWeakPtr(shopkeeper);
}

XShop::XShop(XRect& _area, ITEM_MASK _im, XLocation * _loc, SHOP_DOOR sd)
    : XAnyPlace(_area, _loc)
{
    shop_mask = _im;

    if (sd != SHOP_BUILD_IN) {
        int dx = 0;
        int dy = 0;

        switch (sd) {
            case SHOP_DOOR_DOWN :
                dx = (area.left + area.right) / 2;
                dy = area.bottom - 1;
                break;

            case SHOP_DOOR_UP :
                dx = (area.left + area.right) / 2;
                dy = area.top;
                break;

            case SHOP_DOOR_LEFT:
                dx = area.left;
                dy = (area.top + area.bottom) / 2;
                break;

            case SHOP_DOOR_RIGHT:
                dx = area.right - 1;
                dy = (area.top + area.bottom) / 2;
                break;

            default:
                assert(0);
        }

        location->map->CreateRoom(area.left, area.top, area.Width(), area.Height(),
            dx, dy, XTileType::STONE_FLOOR, XTileType::STONE_WALL);

        for (int i = area.left + 1; i < area.right - 1; i++)
            for (int j = area.top + 1; j < area.bottom - 1; j++) {
                XItem * item = ICREATEA(shop_mask);
                item->Drop(location, i, j);
            }

    } else {
        for (int i = area.left; i < area.right; i++)
            for (int j = area.top; j < area.bottom; j++) {
                XItem * item = ICREATEA(shop_mask);
                item->Drop(location, i, j);
            }
    }

    hero_in = 0;
}

int XShop::onCreaturePickItem(XCreature * cr, XItem * item)
{
    if (auto o = owner.lock()) {
        return dynamic_cast<XShopKeeperAI *>(o->xai.get())->onAnyonePickItem(cr, item);
    }

    return 1;
}

int XShop::onCreatureEnter(XCreature * cr)
{
    if (cr->isHero()) {
        for (int i = area.left + 1; i < area.right - 1; i++)
            for (int j = area.top + 1; j < area.bottom - 1; j++) {
                XItemList* ilist = location->map->GetItemList(i, j);

                for (auto it: *ilist) {
                    it->Identify(1);
                }
            }
    }

    if (auto o = owner.lock()) {
        dynamic_cast<XShopKeeperAI *>(o->xai.get())->onCreatureEnterShop(cr);
    }

    return 1;
};

int XShop::onCreatureLeave(XCreature * cr)
{
    if (auto o = owner.lock()) {
        dynamic_cast<XShopKeeperAI *>(o->xai.get())->onCreatureLeaveShop(cr);
    }

    return 1;
};

int XShop::onCreatureDropItem(XCreature * cr, XItem * item)
{
    if (auto o = owner.lock()) {
        return dynamic_cast<XShopKeeperAI *>(o->xai.get())->onAnyoneDropItem(cr, item);
    }

    return 1;
}

int XShop::onCreatureMove(XCreature * cr)
{
    return 1;
}

std::string XShop::onShowItem(XItem* item)
{
    auto desc = item->toString();

    if (owner.lock()) {
        desc.append(fmt::format("{{{}gp}}", item->quantity * item->GetValue()));
    }

    return desc;
}
