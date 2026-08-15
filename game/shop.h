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

#ifndef XSHOP_H
#define XSHOP_H

#include <optional>

#include <cereal/types/base_class.hpp>

#include "game/location.h"
#include "helpers/point.h"
#include "item/itemkind.h"
#include "map/xanyplace.h"

class XShop : public XAnyPlace
{
    protected:
        XShop() {}
        friend class cereal::access;

    public:
        DECLARE_CREATOR(XShop, XAnyPlace);
        XShop(XRect& _area, ItemKind _kind, XLocation * _loc, SHOP_DOOR sd = SHOP_DOOR_UP);

        int onCreatureEnter(XCreature * cr) override;
        int onCreatureLeave(XCreature * cr) override;
        int onCreaturePickItem(XCreature * cr, XItem * item) override;
        int onCreatureDropItem(XCreature * cr, XItem * item) override;
        int onCreatureMove(XCreature * cr) override;
        std::string onShowItem(XItem* item) override;
        void SetShopkeeper(XCreature * shopkeeper);

        // Shops have no formal notion of "the door".
        // Every shop is hand-drawn as a walled room with a single opening one
        // tile outside the area, so find it by scanning the ring immediately
        // surrounding the area for the XDoor object.
        // Lazily found and cached on first call (not at construction time
        // - the door's XDoor object may not exist yet at that point,
        // depending on where in the map pattern it was drawn relative to
        // this shop; by the time anything actually needs the door, world
        // generation has long since finished). Returns nullopt if none is
        // found (e.g. an open-plan shop with no walls at all).
        std::optional<XPoint> FindDoor();

        // hero_in is transient per-visit UI state (whether the hero is
        // currently standing inside), always false for a freshly loaded
        // game - not persisted.
        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XAnyPlace>(this));
            ar(shop_mask);

            if constexpr (Archive::is_loading::value) {
                hero_in = 0;
            }
        }

        ItemKind shop_mask;
    protected:
        int hero_in;

        // Not persisted: purely derived from the (already-persisted) map
        // layout, cheap to recompute after a load the same way it was
        // found the first time in a fresh game.
        std::optional<XPoint> door_pos;
        bool door_search_done = false;
};

#endif
