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

#include "helpers/point.h"
#include "item/itemkind.h"
#include "map/map.h"
#include "map/xanyplace.h"

class XLocation;

class XShop : public XAnyPlace
{
    public:
        // Which side of the shop's rectangle its door sits on. The four sides
        // make the shop build its own room - walls, floor and a door  opening
        // on that side. BUILT_IN means the opposite: the room is already drawn
        // by the map pattern, so the shop builds nothing and is placed there.
        enum class Door {
            UP,
            LEFT,
            DOWN,
            RIGHT,
            BUILT_IN,
        };

    protected:
        XShop() {}
        friend class cereal::access;

    public:
        DECLARE_CREATOR(XShop, XAnyPlace);
        XShop(XRect& _area, ItemKind _kind, XLocation* _loc, Door sd,
              XTileType::Id wall, XTileType::Id floor,
              int _min_value, int _max_value);

        // What this shop is willing to have on its floor, in gold. The
        // generator rolls an item up to a hundred times looking for one
        // inside the range (XItemFactory::CreateAnyItem), so a floor here
        // is a quality floor: GetValue() weights a point of protection six
        // times as heavily as a point of defence, which makes a minimum
        // price the bluntest available way of asking for armour that
        // actually protects. It is also what the customer pays, so raising
        // it stocks a better shop and an unaffordable one in equal measure.
        [[nodiscard]] int MinValue() const { return min_value; }
        [[nodiscard]] int MaxValue() const { return max_value; }

        bool onCreatureEnter(XCreature* cr) override;
        bool onCreatureLeave(XCreature* cr) override;
        bool onCreaturePickItem(XCreature* cr, XItem* item) override;
        bool onCreatureDropItem(XCreature* cr, XItem* item) override;
        bool onCreatureMove(XCreature* cr) override;
        std::string onShowItem(XItem* item) override;
        void SetShopkeeper(XCreature* shopkeeper);

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
            ar(min_value, max_value);

            if constexpr (Archive::is_loading::value) {
                hero_in = 0;
            }
        }

        ItemKind shop_mask;
    protected:
        int min_value = 0;
        int max_value = 10000;
        int hero_in;

        // Not persisted: purely derived from the (already-persisted) map
        // layout, cheap to recompute after a load the same way it was
        // found the first time in a fresh game.
        std::optional<XPoint> door_pos;
        bool door_search_done = false;
};

#endif
