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

#include "creature/skeep_ai.h"
#include "creature/unique.h"
#include "game/game.h"
#include "game/quest.h"
#include "game/setting.h"
#include "helpers/msgwin.h"
#include "item/uniquei.h"
#include "magic/modifier.h"


///////////////////////////////////////////////////////////////////////
// Beelzevile
///////////////////////////////////////////////////////////////////////
REGISTER_CLASS(XBeelzvile);
CEREAL_REGISTER_TYPE(XBeelzvile);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XCreature, XBeelzvile);

XBeelzvile::XBeelzvile(CreatureTemplate * cr) : XAnyCreature(cr)
{
    if (XSettings::isDemo) {
        XCreature::main_creature = this;
    }
}

void XBeelzvile::NewMove()
{
    if (XSettings::isDemo) {
        l->map->Center(x, y);
        l->map->Put(this);
        PutStatus();
        vRefresh();
        msgwin.ClrMsg();
    }

    XAnyCreature::NewMove();
}

void XBeelzvile::Move()
{
    if (XSettings::isDemo) {
        HideOldView();
        ShowNewView();
    }

    XAnyCreature::Move();
}

///////////////////////////////////////////////////////////////////////
// RODERICK, King of Avanor
///////////////////////////////////////////////////////////////////////

REGISTER_CLASS(XRoderick);
CEREAL_REGISTER_TYPE(XRoderick);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XCreature, XRoderick);
XRoderick::XRoderick(CreatureTemplate * cr) : XAnyCreature(cr)
{
    XPotion * pt = new XPotion(PN_HEALING);
    ContainItem(pt);

    pt = new XPotion(PN_HEALING);
    ContainItem(pt);

    pt = new XPotion(PN_HEALING);
    ContainItem(pt);

    pt = new XPotion(PN_HEALING);
    ContainItem(pt);

    XItem * it = new XAvanorCrown();
    ContainItem(it);

    it = new XAvanorScepter();
    ContainItem(it);
}

///////////////////////////////////////////////////////////////////////
// HIGHT PRIEST
///////////////////////////////////////////////////////////////////////

REGISTER_CLASS(XHighPriest);
CEREAL_REGISTER_TYPE(XHighPriest);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XCreature, XHighPriest);
XHighPriest::XHighPriest(CreatureTemplate * cr) : XAnyCreature(cr)
{
    XPotion * pt = new XPotion(PN_HEALING);
    ContainItem(pt);

    pt = new XPotion(PN_HEALING);
    ContainItem(pt);

    pt = new XPotion(PN_HEALING);
    ContainItem(pt);

    pt = new XPotion(PN_HEALING);
    ContainItem(pt);

    XItem * it = new XAvanorMitre();
    ContainItem(it);
}

///////////////////////////////////////////////////////////////////////
// ROTMOTH
///////////////////////////////////////////////////////////////////////

REGISTER_CLASS(XRotmoth);
CEREAL_REGISTER_TYPE(XRotmoth);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XCreature, XRotmoth);
CEREAL_REGISTER_TYPE(XRotmothAI);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XStandardAI, XRotmothAI);
XRotmoth::XRotmoth(CreatureTemplate * cr) : XAnyCreature(cr)
{
    xai = std::make_unique<XRotmothAI>(this);
    xai->SetEnemyClass(CreatureClass::ALL ^ (CreatureClass::HUMAN | CreatureClass::HUMANOID));
    xai->SetAIFlag(XStandardAI::RANDOM_MOVE);

}

void XRotmothAI::onWasAttacked(XCreature * attacker)
{
    if (attacker->isHero()) {
        if (auto girl = XQuest::quest.GetCreatureRef("kidnapped_girl")) {
            XStandardAI::onWasAttacked(girl);
            AddPersonalEnemy(attacker);
            return;
        }
    }

    XStandardAI::onWasAttacked(attacker);
}

///////////////////////////////////////////////////////////////////////
// GIANA
///////////////////////////////////////////////////////////////////////

REGISTER_CLASS(XGiana);
CEREAL_REGISTER_TYPE(XGiana);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XCreature, XGiana);
XGiana::XGiana(CreatureTemplate * cr) : XAnyCreature(cr)
{
}

void XGiana::FirstStep(int _x, int _y, XLocation * _l)
{
    XAnyCreature::FirstStep(_x, _y, _l);

    // Registering self as the tracked kidnapped_girl needs shared_from_this(),
    // which throws until this creature has been placed on the map for the
    // first time (the constructor runs before that, so it can't be done
    // there). FirstStep() runs on every move, not just the first one, so
    // guard against re-registering on every subsequent step.
    if (!XQuest::quest.GetCreatureRef("kidnapped_girl")) {
        XQuest::quest.SetCreatureRef("kidnapped_girl", this);
    }
}

///////////////////////////////////////////////////////////////////////
// SHOPKEEPER
///////////////////////////////////////////////////////////////////////

REGISTER_CLASS(XShopkeeper);
CEREAL_REGISTER_TYPE(XShopkeeper);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XCreature, XShopkeeper);

XShopkeeper::XShopkeeper(CreatureTemplate * cr) : XAnyCreature(cr)
{
    // Wear() puts the item in the inventory itself now (see
    // XBodyPart::Wear()), so the explicit CarryItem() calls that used to
    // precede these are redundant (and would have double-counted weight).
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
