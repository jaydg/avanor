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
#include <cereal/types/polymorphic.hpp>

#include "creature/shopkeeper.h"
#include "creature/skeep_ai.h"
#include "item/xamulet.h"
#include "item/xenhance.h"
#include "item/xring.h"

REGISTER_CLASS(XShopkeeper);
CEREAL_REGISTER_TYPE(XShopkeeper);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XCreature, XShopkeeper);

XShopkeeper::XShopkeeper(CreatureTemplate * cr) : XAnyCreature(cr)
{
    auto am = new XAmulet(XEnhance::SEEINVISIBLE);
    XBodyPart * bp = GetBodyPart(BP_NECK);
    bp->Wear(am);

    auto rn = new XRing(XEnhance::ACIDRESIST);
    bp = GetBodyPart(BP_RING, 0);
    bp->Wear(rn);

    rn = new XRing(XEnhance::SLAYING);
    bp = GetBodyPart(BP_RING, 1);
    bp->Wear(rn);
}

void XShopkeeper::SetShop(char* _name, XShop * shop)
{
    name = _name;
    xai = std::make_unique<XShopKeeperAI>(this, shop);
}

void XShopkeeper::Die(XCreature * killer)
{
    dynamic_cast<XShopKeeperAI*>(xai.get())->GetShop()->SetShopkeeper(nullptr);
    XAnyCreature::Die(killer);
}

std::string XShopkeeper::StdAnswer()
{
    auto ai = dynamic_cast<XShopKeeperAI*>(xai.get());

    if (!ai->debt.unpaid_items.empty()) {
        return "Don't forget to pay for the items you have taken!";
    }

    if (ai->debt.debtor_sum > 0) {
        return fmt::format("Remember that you owe me money. Don't touch anything else before you pay me {} gp!",
            static_cast<int>(ai->debt.debtor_sum));
    } else {
        switch (vRand(5)) {
            case 0 :
                return "I have excellent quality goods for sale!";

            case 1 :
                return "I'm sure you will find everything you need here in my shop.";

            case 2 :
                return "You'd better look at the goods instead of talking.";

            case 3 :
                return "Don't even try to steal anything.";

            default:
                return "Please buy something!";
        }
    }
}
