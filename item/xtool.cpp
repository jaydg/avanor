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

#include "creature/creature.h"
#include "item/xherb.h"
#include "item/xpotion.h"
#include "item/xtool.h"

//////////////////////////////////////////////////////////////////////
// XCookingSet
//////////////////////////////////////////////////////////////////////

REGISTER_CLASS(XCookingSet);

int CorpseFiltr(XItem * item)
{
    if (item->im & IM_FOOD && item->it == IT_CORPSE) {
        if (((XCorpse*)item)->corpse_flag & CF_COOKED) {
            return 0;
        } else {
            return 1;
        }
    } else {
        return 0;
    }
}

RESULT XCookingSet::onUse(USE_ITEM_STATE uis, XCreature * cr)
{
    switch (uis) {
        case UIS_BEGIN:
            if (cr->isHero()) {
                XItem * corpse = cr->SelectItem(CorpseFiltr);

                if (corpse == NULL) {
                    return FAIL;
                }

                cooked_item = corpse;
                ((XCorpse*)corpse)->roating_stopped = 1;
                use_time = 50 - cr->sk->GetLevel(SKT_COOKING) * 2;

                if (cr->isVisible()) {
                    msgwin.Add(cr->GetNameEx(CRN_T1));
                    msgwin.Add(cr->GetVerb("start"));
                    msgwin.Add("to cook");
                    msgwin.AddLast(corpse->name);
                }

                return CONTINUE;
            }

            break;

        case UIS_CONTINUE:
            use_time--;

            if (use_time <= 0) {
                XCorpse * corpse = (XCorpse*)cooked_item.get();
                corpse->UnCarry();

                if (vRand(100) < cr->sk->GetLevel(SKT_COOKING) * 4 + 30) {
                    corpse->Cook();
                    corpse->food_nutrio *= 2;
                    corpse->weight /= 10;
                    corpse->weight = corpse->weight == 0 ? 1 : corpse->weight;
                    cr->ContainItem(corpse);
                    cr->sk->UseSkill(SKT_COOKING, 3);

                    if (cr->isVisible()) {
                        msgwin.Add(cr->GetNameEx(CRN_T1));
                        msgwin.Add(cr->GetVerb("cook"));
                        msgwin.AddLast(corpse->name);
                    }
                } else {
                    if (cr->isVisible()) {
                        msgwin.Add(cr->GetNameEx(CRN_T1));
                        msgwin.Add(cr->GetVerb("fail"));
                        msgwin.Add("to cook");
                        msgwin.AddLast(corpse->name);
                    }
                }

                cooked_item = NULL;
                return SUCCESS;
            } else {
                return CONTINUE;
            }

            break;

        case UIS_STOP:
            XCorpse * corpse = (XCorpse*)cooked_item.get();
            corpse->roating_stopped = 0;
            cr->contain.Add(corpse);
            cooked_item = NULL;
            break;
    }

    return FAIL;
}

void XCookingSet::Store(XFile * f)
{
    f->Write(&use_time, sizeof(int));
    XItem::Store(f);
}

void XCookingSet::Restore(XFile * f)
{
    f->Read(&use_time, sizeof(int));
    XItem::Restore(f);
}

void XCookingSet::Invalidate()
{
    cooked_item = NULL;
    XItem::Invalidate();
}

//////////////////////////////////////////////////////////////////////
// PickAxe
// see header for detail
//////////////////////////////////////////////////////////////////////

REGISTER_CLASS(XPickAxe);

RESULT XPickAxe::onUse(USE_ITEM_STATE uis, XCreature * cr)
{
    switch (uis) {
        case UIS_BEGIN:
            if (cr->isHero()) {
                XPoint pt;

                if (cr->GetTarget(TR_ATTACK_DIRECTION, &pt)) {
                    tgt_x = cr->x + pt.x;
                    tgt_y = cr->y + pt.y;

                    if (cr->l->map->GetXY(tgt_x, tgt_y) == M_STONEWALL || cr->l->map->GetXY(tgt_x, tgt_y) == M_MAGMA) {
                        rock_resist = 1000;
                        char buf[256];
                        sprintf(buf, "%s starts to dig.", cr->name);
                        msgwin.Add(buf);
                        return CONTINUE;
                    } else {
                        msgwin.Add("You can't dig something other than walls.");
                    }
                }
            }

            break;

        case UIS_CONTINUE:
            rock_resist -= (dice.Throw() + cr->sk->GetLevel(SKT_MINING) * 5 + cr->GetStats(S_STR) / 2);

            if (rock_resist < 0) {
                if (cr->isHero()) {
                    msgwin.Add(cr->GetNameEx(CRN_T1));
                    msgwin.Add(cr->GetVerb("smash"));
                    msgwin.Add("the stone to pieces.");
                    cr->l->map->SetXY(tgt_x, tgt_y, M_STONEFLOOR);
                    cr->sk->UseSkill(SKT_MINING);

                    if (vRand(3) == 0) {
                        msgwin.Add("There was some gold in ore.");
                        XItem * it = new XMoney(vRand(100) + 10);
                        it->Drop(cr->l, tgt_x, tgt_y);
                    }
                }

                return SUCCESS;
            } else {
                return CONTINUE;
            }

            break;

        case UIS_STOP:
            break;

    }

    return FAIL;
}



//////////////////////////////////////////////////////////////////////
// EyeOfRaa
// Artifact
//////////////////////////////////////////////////////////////////////

REGISTER_CLASS(XEyeOfRaa);

RESULT XEyeOfRaa::onUse(USE_ITEM_STATE uis, XCreature * cr)
{
    if (cr->isHero()) {
        RESULT res = XEffect::Make(cr, E_LIGHTNING_BOLT, 30);
    }

    return SUCCESS;
}




//////////////////////////////////////////////////////////////////////
// AlchemySet
// Allow to get potions from roots.
//////////////////////////////////////////////////////////////////////

REGISTER_CLASS(XAlchemySet);

int RootsFiltr(XItem * item)
{
    if (item->im & IM_FOOD && item->it == IT_HERB) {
        return 1;
    } else {
        return 0;
    }
}

RESULT XAlchemySet::onUse(USE_ITEM_STATE uis, XCreature * cr)
{
    XHerb * herb = (XHerb*)cr->SelectItem(RootsFiltr);

    if (herb == NULL) {
        return FAIL;
    }

    POTION_NAME pn = herb->GetTargetPotion();
    POTION_REC * pr = POTION_REC::GetRec(pn);
    int val = cr->sk->GetLevel(SKT_ALCHEMY) * 8 + 30 - pr->alchemy_power * 10;

    if (vRand(100) < val) {
        XPotion * pot = new XPotion(pr->pn);
        char buf[256];
        pot->toString(buf);
        msgwin.Add(cr->name);
        char buf1[256];
        sprintf(buf1, "managed to create a %s.", buf);
        msgwin.Add(buf1);
        cr->CarryItem(pot);
        cr->contain.Add(pot);
        cr->sk->UseSkill(SKT_ALCHEMY);
    } else {
        msgwin.Add(cr->name);
        msgwin.Add("failed to create a potion.");
    }

    herb->Invalidate();
    return SUCCESS;
}
