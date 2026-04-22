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

#include "game/location.h"
#include "map/xanyplace.h"

class XShop : public XAnyPlace
{
    protected:
        XShop() {}

    public:
        DECLARE_CREATOR(XShop, XAnyPlace);
        XShop(XRect& _area, ITEM_MASK _im, XLocation * _loc, SHOP_DOOR sd = SHOP_DOOR_UP);

        virtual int onCreatureEnter(XCreature * cr);
        virtual int onCreatureLeave(XCreature * cr);
        virtual int onCreaturePickItem(XCreature * cr, XItem * item);
        virtual int onCreatureDropItem(XCreature * cr, XItem * item);
        virtual int onCreatureMove(XCreature * cr);
        virtual void onShowItem(XItem * item, char* buf);
        void SetShopkeeper(XCreature * shopkeeper)
        {
            owner = shopkeeper;
        }

        virtual void Store(XFile * f);
        virtual void Restore(XFile * f);

        ITEM_MASK shop_mask;
    protected:
        int hero_in;
};

#endif
