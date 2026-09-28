/*
This file is part of "Avanor, the Land of Mystery" roguelike game

Copyright (C) 2000-2006 Vadim Gaidukevich
Copyright (C) 2025,2026 Joachim de Groot

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
#include "map/map.h"
#include "map/map_objects.h"

REGISTER_CLASS(XShop);
CEREAL_REGISTER_TYPE(XShop);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XAnyPlace, XShop);

void XShop::SetShopkeeper(XCreature * shopkeeper)
{
    owner = XCreature::ToWeakPtr(shopkeeper);
}

std::optional<XPoint> XShop::FindDoor()
{
    if (door_search_done) {
        return door_pos;
    }

    door_search_done = true;

    XMap* map = location->map;

    for (int x = area.left - 1; x <= area.right; x++) {
        for (int y = area.top - 1; y <= area.bottom; y++) {
            const bool on_ring = (x == area.left - 1 || x == area.right || y == area.top - 1 || y == area.bottom);

            if (!on_ring) {
                continue;
            }

            if (dynamic_cast<XDoor*>(map->GetSpecial(x, y))) {
                door_pos = XPoint(x, y);
                return door_pos;
            }
        }
    }

    return std::nullopt;
}

// Fills a rectangle of the shop floor, one item to a cell.
//
// A launcher puts its own ammunition on the next cell, and that one is
// made without the shop's value bounds. Missiles likely are below a
// shop's `min_value`, so they could never stock a quarrel at all.
void XShop::Stock(const XRect& where)
{
    // Ammunition owed to the launcher placed on the previous cell.
    XItem* pending = nullptr;

    for (int i = where.left; i < where.right; i++) {
        for (int j = where.top; j < where.bottom; j++) {
            XItem* item = pending;
            pending = nullptr;

            if (!item) {
                item = ICREATE(shop_mask, min_value, max_value);

                if (const ItemType ammo = XItemFactory::MissileFor(item); ammo != IT_NONE) {
                    pending = ICREATEB(ItemKind::MISSILE, ammo, 0, 10000000);
                }
            }

            item->Drop(location, i, j);
        }
    }

    // A launcher on the very last cell has no next cell to put its
    // ammunition on. It goes down beside the bow rather than being
    // thrown away - a cell holds as many items as are dropped on it.
    if (pending) {
        pending->Drop(location, where.right - 1, where.bottom - 1);
    }
}

XShop::XShop(XRect& _area, ItemKind _kind, XLocation* _loc, Door sd,
             const XTileType::Id wall, const XTileType::Id floor,
             const int _min_value, const int _max_value,
             const std::string& handler)
    : XAnyPlace(_area, _loc, handler)
{
    shop_mask = _kind;
    min_value = _min_value;
    max_value = _max_value;

    if (sd != Door::BUILT_IN) {
        int dx = 0;
        int dy = 0;

        switch (sd) {
            case Door::DOWN :
                dx = (area.left + area.right) / 2;
                dy = area.bottom - 1;
                break;

            case Door::UP :
                dx = (area.left + area.right) / 2;
                dy = area.top;
                break;

            case Door::LEFT:
                dx = area.left;
                dy = (area.top + area.bottom) / 2;
                break;

            case Door::RIGHT:
                dx = area.right - 1;
                dy = (area.top + area.bottom) / 2;
                break;

            default:
                assert(0);
        }

        location->map->CreateRoom(area.left, area.top, area.Width(), area.Height(),
            dx, dy, floor, wall);

        Stock(XRect(area.left + 1, area.top + 1, area.right - 1, area.bottom - 1));
    } else {
        Stock(area);
    }

    hero_in = 0;
}

bool XShop::onCreaturePickItem(XCreature* cr, XItem* item)
{
    if (auto o = owner.lock()) {
        return dynamic_cast<XShopKeeperAI *>(o->xai.get())->onAnyonePickItem(cr, item);
    }

    return true;
}

bool XShop::onCreatureEnter(XCreature* cr)
{
    if (cr->isHero()) {
        for (int i = area.left + 1; i < area.right - 1; i++)
            for (int j = area.top + 1; j < area.bottom - 1; j++) {
                XItemList* ilist = location->map->GetItemList(i, j);

                for (auto it: *ilist) {
                    it->Identify();
                }
            }
    }

    if (auto o = owner.lock()) {
        dynamic_cast<XShopKeeperAI *>(o->xai.get())->onCreatureEnterShop(cr);
    }

    // And then the shop's own script, if it was given one - a shop is a place
    // like any other, and this is the same MOVE_IN that EventPlace() delivers.
    XAnyPlace::onCreatureEnter(cr);

    return true;
}

bool XShop::onCreatureLeave(XCreature* cr)
{
    if (auto o = owner.lock()) {
        dynamic_cast<XShopKeeperAI *>(o->xai.get())->onCreatureLeaveShop(cr);
    }

    return true;
}

bool XShop::onCreatureDropItem(XCreature* cr, XItem* item)
{
    if (auto o = owner.lock()) {
        return dynamic_cast<XShopKeeperAI *>(o->xai.get())->onAnyoneDropItem(cr, item);
    }

    return true;
}

bool XShop::onCreatureMove(XCreature * /*cr*/)
{
    return true;
}

std::string XShop::onShowItem(XItem* item)
{
    auto desc = item->toSentence();

    if (owner.lock()) {
        // Spaced off the name, the way a part-eaten ration's "{3/4}" is -
        // without it the two ran together as "ration{1gp}".
        desc.append(fmt::format(" {{{}gp}}", item->quantity * item->GetValue()));
    }

    return desc;
}
