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

#ifndef __SKEEP_AI_H
#define __SKEEP_AI_H

#include "std_ai.h"
#include "rect.h"
#include "shop.h"

class XCreature;

struct SHOP_DEBT {
    XPtr<XCreature> debtor;
    double debtor_sum;
    double debtor_add_value;
    int turn_count; //after some turns after debtor leave a shop - debtors can't by nothing
    int debtor_leave_shop;
    XList<XItem*> item_list; //picked but not payed items;
};

class XShopKeeperAI : public XStandardAI
{
    protected:
        XShopKeeperAI() {}

    public:
        DECLARE_CREATOR(XShopKeeperAI, XStandardAI);
        XShopKeeperAI(XCreature * shoopkeeper, XShop * _shop);
        virtual void Invalidate();
        virtual void Move();
        virtual void onCreatureEnterShop(XCreature * customer);
        virtual void onCreatureLeaveShop(XCreature * customer);
        virtual int onAnyonePickItem(XCreature * customer, XItem * item);
        virtual int onAnyoneDropItem(XCreature * customer, XItem * item);
        virtual int onGiveItem(XCreature * giver, XItem * item);
        virtual void Store(XFile * f);
        virtual void Restore(XFile * f);
        void SetShop(XShop * _shop)
        {
            shop = _shop;
        }

        const XPtr<XShop> &GetShop()
        {
            return shop;
        }

        SHOP_DEBT debt;
    protected:
        XPtr<XShop> shop;
};

#endif
