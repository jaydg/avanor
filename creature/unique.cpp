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

#include "creature/unique.h"
#include "item/uniquei.h"
#include "game/quest.h"
#include "item/itemf.h"
#include "magic/modifer.h"
#include "creature/xhero.h"
#include "global/xapi.h"
#include "game/setting.h"
#include "item/xtool.h"
#include "creature/skeep_ai.h"
#include "game/game.h"


///////////////////////////////////////////////////////////////////////
// AHK-ULAN
///////////////////////////////////////////////////////////////////////
REGISTER_CLASS(XAhkUlan);

XAhkUlan::XAhkUlan(_CREATURE * cr) : XAnyCreature(cr) { }

int XAhkUlan::Chat(XCreature * chatter, const char* msg)
{
    if (xai->isEnemy(chatter)) {
        msgwin.Add("It is time to die!");
        return 1;
    }

    if (XQuest::quest.ahk_ulan_quest == 0) {
        msgwin.Add("Hello, brave hero.");
        msgwin.Add("Some years ago, some evil wizards destroyed my tower.  Now I wait here gaining strength and planning my revenge. I am searching for 3 parts to an ancient machine. Bring them to me and I will reward you well.");
        XQuest::quest.ahk_ulan_quest = 1;
        return 1;
    }

    if (XQuest::quest.ahk_ulan_quest < 4 || (XQuest::quest.roderick_ordered && !XQuest::quest.roderick_killed)) {
        msgwin.Add("Don't disturb me before completing my quest, puny mortal!");
        return 1;
    }

    if (XQuest::quest.roderick_killed) {
        XQuest::quest.hero_win = 1;
        XHero::EndGame("***WINNER***");
    }

    return 1;
}

void XAhkUlan::Die(XCreature * killer)
{
    XQuest::quest.ahk_ulan_killed = 1;
    XAnyCreature::Die(killer);
}

int XAhkUlan::onGiveItem(XCreature * giver, XItem * item)
{
    if (item->it == IT_ANCIENTMACHINEPART) {
        ContainItem(item);
        XQuest::quest.ahk_ulan_quest += item->quantity;

        if (XQuest::quest.ahk_ulan_quest >= 4) {
            msgwin.Add("Very nice job, servant!");
            msgwin.Add("And now, my last request: kill Roderick, for he is only one who can stop me now.");
            XQuest::quest.roderick_ordered = 1;
        } else {
            msgwin.Add("You are a loyal servant!");
        }

        return 1;
    } else {
        msgwin.Add("Are you jeering at me?");
        Sacrifice(item);
        return 1;
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////
// Beelzevile
///////////////////////////////////////////////////////////////////////
REGISTER_CLASS(XBeelzvile);

XBeelzvile::XBeelzvile(_CREATURE * cr) : XAnyCreature(cr)
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
// Gefeon, great master of Fire
///////////////////////////////////////////////////////////////////////
REGISTER_CLASS(XGefeon);
XGefeon::XGefeon(_CREATURE * cr) : XAnyCreature(cr) { }

int XGefeon::Chat(XCreature * chatter, const char* msg)
{
    if (XQuest::quest.ahk_ulan_ordered == 0 && XQuest::quest.ahk_ulan_killed == 0) {
        msgwin.Add("Ahk-Ulan, the evil wizard and master of black magic, lives in the dungeon beneath the ruins of his magic tower. The ruins are to the south-east of town. He causes great evil, and he should be eliminated.");
        XQuest::quest.ahk_ulan_ordered = 1;
        return 1;
    }

    if (XQuest::quest.ahk_ulan_ordered == 1 && XQuest::quest.ahk_ulan_killed == 0) {
        msgwin.Add("And how is Ahk-Ulan? Still alive? That is very bad.");
        return 1;
    }

    if (XQuest::quest.ahk_ulan_killed == 1) {
        if (XQuest::quest.roderick_killed == 1) { // Kill all the competition and become king/queen.
            msgwin.Add("Well, you killed the pretender and the King, I guess that makes you the new ruler!");
        } else {
            msgwin.Add("You did a great thing! You truly are the best!");
        }

        XQuest::quest.hero_win = 1;

        XHero::EndGame("***WINNER***");
    }

    return 1;
}

///////////////////////////////////////////////////////////////////////
// RODERICK, King of Avanor
///////////////////////////////////////////////////////////////////////

REGISTER_CLASS(XRoderick);
XRoderick::XRoderick(_CREATURE * cr) : XAnyCreature(cr)
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

int XRoderick::Chat(XCreature * chatter, const char* msg)
{
    XItem * it1 = chatter->GetBodyPart(BP_HAND, 0)->Item();
    XItem * it2 = chatter->GetBodyPart(BP_HAND, 1)->Item();

    if (xai->isEnemy(chatter)) {
        msgwin.Add("No mercy!");
    } else if ((it1 && it1->xguid == XAvanorDefender::avanordefender_guid) || (it2 && it2->xguid == XAvanorDefender::avanordefender_guid)) {
        msgwin.Add(
            "I recognize that sword in your hand. "
            "You have looted the tomb of my ancestors! "
            "Guards! Seize the traitor!");
        xai->AddPersonalEnemy(chatter);
        xai->SetGroupEnemy(chatter);
    } else {
        msgwin.Add("Hello, brave hero.");

        if (XQuest::quest.roderick_quest2 == 0) {
            msgwin.Add(
                "I have heard that my family crypt has been occupied by a group of undead. "
                "Clear the crypt and I will reward you. It lies to the south-west of the city.");
            XQuest::quest.roderick_quest2 = 1;
        } else if (XQuest::quest.roderick_quest2 == 1) {
            if (Game.locations[L_UNDEADS_TOMB1]->GetCreatureCount(CR_UNDEAD) == 0) {
                msgwin.Add(
                    "Thank you for destroying the evil in our crypt. "
                    "Please accept these coins and my gratitude for a job well done.");
                XQuest::quest.roderick_quest2 = 2;
                chatter->MoneyOp(1000);
            } else {
                msgwin.Add("You still have not cleansed my ancestor's crypt.");
            }
        } else if (XQuest::quest.roderick_quest == 0) {
            msgwin.Add("Some years ago one of my trusted servants stole a powerful artifact, the 'Eye of Raa' from me. He tried to hide it from me in one of the caves far south from here, but people say that he was killed while hiding it.  Could you return this artifact to me?");
            XQuest::quest.roderick_quest = 1;
        } else if (XQuest::quest.roderick_quest == 1) {
            msgwin.Add("Please, return the 'Eye of Raa' to me.");
        }
    }

    return 1;
}

void XRoderick::Die(XCreature * killer)
{
    XQuest::quest.roderick_killed = 1;
    XAnyCreature::Die(killer);
}

int XRoderick::onGiveItem(XCreature * giver, XItem * item)
{
    if (item->it == IT_EYEOFRAA) {
        msgwin.Add("Thank you for your great help. The citizens of Avanor never forget your exploits!");
        XQuest::quest.roderick_quest = 2;
        ContainItem(item);
        return 1;
    } else {
        msgwin.Add("I don't need this");
    }

    return 0;
}

///////////////////////////////////////////////////////////////////////
// HIGHT PRIEST
///////////////////////////////////////////////////////////////////////

REGISTER_CLASS(XHighPriest);
XHighPriest::XHighPriest(_CREATURE * cr) : XAnyCreature(cr)
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

int XHighPriest::Chat(XCreature * chatter, const char* msg)
{
    if (xai->isEnemy(chatter)) {
        msgwin.Add("Defiler, you must be punished!");
    } else {
        msgwin.Add("Blessings on you.");
    }

    return 1;
}

void XHighPriest::Die(XCreature * killer)
{
    char buf[80];

    if (killer->isHero()) {
        sprintf(buf, "%s will not be pleased about this...", religion.GetDeityName(D_LIFE));
        msgwin.Add(buf);
    } else {
        sprintf(buf, "%s seems to be trying to anger %s...", killer->name, religion.GetDeityName(D_LIFE));
        msgwin.Add(buf);
    }

    killer->religion.life_act -= 50;
    XAnyCreature::Die(killer);
}

int XHighPriest::onGiveItem(XCreature * giver, XItem * item)
{
    int val = giver->sk->GetLevel(SKT_RELIGION);
    char buf[120];

    msgwin.Add("Thank you for your charitable donation!");
    sprintf(buf, "%s prays to %s.", name, religion.GetDeityName(D_LIFE));
    msgwin.Add(buf);

    if (1 /*TODO if hero can see...*/) {
        item->toString(buf);
        msgwin.Add(buf);
        msgwin.Add("disappears in a bright light.");
    }

    int sacrifice_value;

    if (item->im & IM_MONEY) {
        sacrifice_value = (int)(sqrt((float)item->quantity) + 1) * (val / 4 + 1);
    } else {
        sacrifice_value = (int)((sqrt((float)item->GetValue()) * item->quantity) + 1) * (val / 4 + 1);
    }

    giver->sk->UseSkill(SKT_RELIGION, 3);

    giver->religion.life_act += 2 * sacrifice_value;

    item->UnCarry();
    item->Invalidate();
    return 1;
}

///////////////////////////////////////////////////////////////////////
// ROTMOTH
///////////////////////////////////////////////////////////////////////

REGISTER_CLASS(XRotmoth);
REGISTER_CLASS(XRotmothAI);
XRotmoth::XRotmoth(_CREATURE * cr) : XAnyCreature(cr)
{
    xai->Invalidate();
    xai = new XRotmothAI(this);
    xai->SetEnemyClass((CREATURE_CLASS)(CR_ALL ^ (CR_HUMAN | CR_HUMANOID)));
    xai->SetAIFlag(AIF_RANDOM_MOVE);

}

int XRotmoth::Chat(XCreature * chatter, const char* msg)
{
    if (xai->isEnemy(chatter)) {
        msgwin.Add("You will be rewarded for your stupidness!");
    } else {
        if (XQuest::quest.rotmoth_status == 0) {
            if (XQuest::quest.kidnapped_girl.get() && isCreatureVisible(XQuest::quest.kidnapped_girl)) {
                msgwin.Add("I hope you've brings 100 gold coins, otherwise this girl will die.");

                if (chatter->MoneyOp(0) >= 100) {
                    msgwin.Add("Pay him?");

                    if (chatter->GetTarget(TR_YES_NO)) {
                        chatter->MoneyOp(-100);
                        MoneyOp(100);

                        if (chatter->creature_person_type & CPT_HE) {
                            msgwin.Add("Thank you, boy!");
                        } else {
                            msgwin.Add("Thank you, girl!");
                        }

                        XQuest::quest.kidnapped_girl->xai->companion = chatter;
                        XQuest::quest.rotmoth_status = 1;
                    }
                }
            } else {
                msgwin.Add("I dont know what you are asking about.");
            }
        } else {
            msgwin.Add("Run away quickly before I changed my mind!");
        }
    }

    return 1;
}

void XRotmothAI::onWasAttacked(XCreature * attacker)
{
    if (attacker->isHero()) {
        if (XQuest::quest.kidnapped_girl) {
            XStandardAI::onWasAttacked(XQuest::quest.kidnapped_girl);
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
XGiana::XGiana(_CREATURE * cr) : XAnyCreature(cr)
{
    XQuest::quest.kidnapped_girl = this;
}

int XGiana::Chat(XCreature * chatter, const char* msg)
{
    if (xai->isEnemy(chatter)) {
        msgwin.Add("Don't touch me!");
    } else {
        if (XQuest::quest.rotmoth_status < 2) {
            msgwin.Add("Please, save me.");
        }
    }

    return 1;
}

///////////////////////////////////////////////////////////////////////
// BANDIT
///////////////////////////////////////////////////////////////////////
REGISTER_CLASS(XBandit);
REGISTER_CLASS(XBanditAI);

XBandit::XBandit(_CREATURE * cr) : XAnyCreature(cr)
{
    xai->Invalidate();
    xai = new XBanditAI(this);
    xai->SetAIFlag(AIF_GUARD_AREA);
    xai->SetAIFlag(AIF_PROTECT_AREA);
    xai->SetEnemyClass(CR_NONE);
    XBodyPart * cloak = GetBodyPart(BP_CLOAK, 0);

    if (cloak->Item()) {
        cloak->UnWear()->Invalidate();
    }

    cloak->Wear(new XForestBrotherCloak());
}

int XBanditAI::isEnemy(XCreature * cr)
{
    if (isPersonalEnemy(cr)) {
        return 1;
    }

    XBodyPart * bp = cr->GetBodyPart(BP_CLOAK);

    if (bp && bp->Item() && bp->Item()->it == IT_FORESTBROTHERCLOAK) {
        return 0;
    }

    return XStandardAI::isEnemy(cr);
}

///////////////////////////////////////////////////////////////////////
// SHOPKEEPER
///////////////////////////////////////////////////////////////////////

REGISTER_CLASS(XShopkeeper);

XShopkeeper::XShopkeeper(_CREATURE * cr) : XAnyCreature(cr)
{
    XAmulet * am = new XAmulet(ENH_SEEINVISIBLE);
    CarryItem(am);
    XBodyPart * bp = GetBodyPart(BP_NECK);
    bp->Wear(am);

    XRing * rn = new XRing(ENH_ACIDRESIST);
    CarryItem(rn);
    bp = GetBodyPart(BP_RING, 0);
    bp->Wear(rn);

    rn = new XRing(ENH_SLAYING);
    CarryItem(rn);
    bp = GetBodyPart(BP_RING, 1);
    bp->Wear(rn);
}

void XShopkeeper::SetShop(char* _name, XShop * shop)
{
    strcpy(name, _name);
    xai->Invalidate();
    xai = new XShopKeeperAI(this, shop);
}

void XShopkeeper::Die(XCreature * killer)
{
    (((XShopKeeperAI*)xai)->GetShop())->SetShopkeeper(NULL);
    XAnyCreature::Die(killer);
}

const char* XShopkeeper::StdAnswer()
{
    assert(xai && dynamic_cast<XShopKeeperAI*>(xai));
    XShopKeeperAI * ai = static_cast<XShopKeeperAI*>(xai);

    if (!ai->debt.item_list.empty()) {
        sprintf(static_buffer, "Don't forget to pay for the items you have taken!", ai->debt.debtor_sum);
        return static_buffer;
    }

    if (ai->debt.debtor_sum > 0) {
        sprintf(static_buffer, "Remember that you owe me money. Don't touch anything else before you pay me %d gp!", (int)ai->debt.debtor_sum);
        return static_buffer;
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
                return "Please buy an item!";
        }
    }
}
