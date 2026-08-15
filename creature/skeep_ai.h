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

#ifndef SKEEP_AI_H
#define SKEEP_AI_H

#include <vector>

#include <cereal/types/base_class.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>

#include "creature/std_ai.h"
#include "game/shop.h"

class XCreature;

struct SHOP_DEBT {
    std::weak_ptr<XCreature> debtor;
    double debtor_sum;
    double debtor_add_value;
    int turn_count; //after some turns after debtor leave a shop - debtors can't by nothing
    int debtor_leave_shop;
    // picked but unpaid items - each a fresh XItem::MakeCopy(), owned
    // here (not a reference into the debtor's own contain), so this
    // needs to be a real owning shared_ptr like every other item
    // reference in the codebase, not a raw pointer. See XItem::Own().
    std::vector<std::shared_ptr<XItem>> unpaid_items;
};

class XShopKeeperAI : public XStandardAI
{
    public:
        XShopKeeperAI() = delete;
        XShopKeeperAI(XCreature * shoopkeeper, XShop * _shop);
        virtual void Invalidate();
        void Move() override;
        virtual void onCreatureEnterShop(XCreature * customer);
        virtual void onCreatureLeaveShop(XCreature * customer);
        virtual int onAnyonePickItem(XCreature * customer, XItem * item);
        virtual int onAnyoneDropItem(XCreature * customer, XItem * item);
        int onGiveItem(XCreature * giver, XItem * item) override;
        void SetShop(XShop * _shop)
        {
            shop = _shop;
        }

        XShop* GetShop()
        {
            return shop;
        }

        SHOP_DEBT debt;

        // `shop` is deliberately not persisted here (see the comment on
        // it below) - XShop/XAnyPlace aren't part of the shared_ptr
        // graph yet, so there's no Cereal-trackable identity to resolve
        // this raw cross-reference against until the location/map-grid
        // step of this migration addresses it.
        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XStandardAI>(this));
            ar(debt.debtor, debt.debtor_sum, debt.debtor_add_value, debt.turn_count, debt.debtor_leave_shop, debt.unpaid_items);
        }

    protected:
        void SetDebtor(XCreature * cr);
        // Raw, not weak: XShop has never been migrated to shared_ptr
        // ownership (it's the sole XAnyPlace subclass, still owned via
        // XLocation::places[]'s unique_ptr). A shopkeeper's shop is a
        // permanent 1:1 pairing set once at location generation and never
        // reassigned; shop and shopkeeper are always torn down together in
        // the same location-teardown pass (see XLocation::Invalidate()),
        // never independently, so this is always safe to dereference for
        // as long as the shopkeeper itself is alive.
        XShop* shop = nullptr;

        // Not persisted: guides a one-time narrowing of the inherited
        // guard_area down to a small patch hugging the door, so the shopkeeper
        // is never far enough from it to lose a race to block it. Deliberately
        // left false after a load - cheap to redo on the shopkeeper's first
        // post-load turn.
        bool home_area_set = false;
};

// XShopKeeperAI() is deleted - real construction always takes an
// owning XCreature* and an XShop*, so both get null placeholders here.
// Must live here, not in skeep_ai.cpp - see the identical reasoning on
// XStandardAI's own CEREAL_LOAD_VIA_PLACEHOLDER_CONSTRUCT in std_ai.h.
CEREAL_LOAD_VIA_PLACEHOLDER_CONSTRUCT(XShopKeeperAI, serialize, nullptr, nullptr);

#endif
