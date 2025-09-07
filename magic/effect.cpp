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

#include "magic/effect.h"
#include "helpers/dice.h"
#include "creature/creature.h"
#include "magic/modifer.h"
#include "creature/xhero.h"
#include "game/game.h"

EFFECT_REQ XEffect::GetReq(EFFECT effect)
{
    switch (effect) {
        case E_NONE:
        case E_CURE_LIGHT_WOUNDS:
        case E_CURE_SERIOUS_WOUNDS:
        case E_CURE_CRITICAL_WOUNDS:
        case E_CURE_MORTAL_WOUNDS:
        case E_HEAL:
        case E_HEROISM:
        case E_POWER:
        case E_RESTORATION:
        case E_SUMMON_MONSTER:
        case E_CREATE_ITEM:
        case E_CURE_POISON:
        case E_CURE_DISEASE:
        case E_BLINK:
        case E_SELF_KNOWLEDGE:
        case E_SEE_INVISIBLE:
        case E_ACID_RESISTANCE:
        case E_FIRE_RESISTANCE:
        case E_COLD_RESISTANCE:
        case E_POISON_RESISTANCE:
            return ER_NONE;
            break;

        case E_BURNING_HANDS:
        case E_ICE_TOUCH:
        case E_DRAIN_LIFE:
            return ER_DIRECTION;
            break;

        case E_IDENTIFY:
            return ER_ITEM;
            break;

        case E_MAGIC_ARROW:
        case E_FIRE_BOLT:
        case E_ICE_BOLT:
        case E_LIGHTNING_BOLT:
        case E_ACID_BOLT:
        case E_TELEPORT:
            return ER_TARGET;
            break;

        default:
            return ER_NONE;
    }
}

int XEffect::GetRange(EFFECT effect, int power)
{
    switch (effect) {
        case E_MAGIC_ARROW:
            return power / 4 + 2;

        case E_FIRE_BOLT:
            return power / 6 + 2;

        case E_ICE_BOLT:
            return power / 6 + 2;

        case E_LIGHTNING_BOLT:
            return power / 7 + 2;

        case E_ACID_BOLT:
            return power / 8 + 2;

        case E_TELEPORT:
            return 1;

        default:
            return 0;
    }
}

int XEffect::Heal(XCreature * caster, int X, int Y, int Z)
{
    XDice d(X, Y, Z);

    if (caster->onHeal(d.S)) {
        if (caster->isVisible()) {
            msgwin.Add(caster->GetNameEx(CRN_T1));
            msgwin.Add(caster->GetVerb("feel"));

            if (caster->GetMaxHP() == caster->_HP) {
                msgwin.Add("completly healed.");
            } else if (d.S < 8) {
                msgwin.Add("slightly healed.");
            } else if (d.S < 15) {
                msgwin.Add("somewhat healed.");
            } else if (d.S < 100) {
                msgwin.Add("healed.");
            } else {
                msgwin.Add("greatly healed.");
            }
        }

        return 1;
    } else {
        return 0;
    }
}

int XEffect::Cure(XCreature * caster, int X, int Y, int Z)
{
    XDice d(X, Y, Z);

    if (caster->md->Add(MOD_WOUND, -d.S, caster)) {
        if (caster->isVisible()) {
            msgwin.Add(caster->GetNameEx(CRN_T1));

            if (caster->md->Get(MOD_WOUND)) {
                msgwin.Add("cured.");
            } else {
                msgwin.Add("relieved.");
            }
        }

        return 1;
    }

    return 0;
}

int XEffect::Mana(XCreature * caster, int X, int Y, int Z)
{
    XDice d(X, Y, Z);

    if (caster->onRestorePP(d.S)) {
        if (caster->isVisible()) {
            char buf[256];
            sprintf(buf, "The power flows through %s body.", caster->GetNameEx(CRN_T4));
            msgwin.Add(buf);
        }

        return 1;
    }

    return 0;
}


int XEffect::Touch(EFFECT_DATA * pData, int X, int Y, int Z, xColor col, BRAND_TYPE brt, const char* msg)
{
    XCreature * target = pData->l->map->GetMonster(pData->target_x, pData->target_y);

    if (pData->l->map->GetVisible(pData->target_x, pData->target_y) && __animation_flag) {
        pData->l->map->Put(pData->caller);
        pData->l->map->PutChar(pData->target_x, pData->target_y, '*', col);
        vRefresh();
        vDelay(__animation_flag);
    }

    if (target) {
        XDice d(X, Y, Z);

        DAMAGE_DATA_EX dd;
        dd.damage	= d.S;
        dd.attacker	= pData->caller;
        dd.attack_name	= msg;
        dd.attack_HIT	= 1000;
        dd.attack_brand	= brt;
        dd.flags	= DF_MAGIC_BOLT;
        target->InflictDamage(&dd);
        return 1;
    } else {
        return 0;
    }
}


int XEffect::Bolt(EFFECT_DATA * pData, int X, int Y, int Z, xColor col, BRAND_TYPE brt, const char* msg)
{
    MF_DATA mfd;
    mfd.arrow_type = MFT_BALL;
    mfd.arrow_color = col;
    mfd.l = pData->l;
    mfd.sx = pData->call_x;
    mfd.sy = pData->call_y;
    mfd.ex = pData->target_x;
    mfd.ey = pData->target_y;
    mfd.to_hit = 1000;
    mfd.max_range = GetRange(pData->effect, pData->power);
    MF_RESULT res = XCreature::MissileFlight(&mfd);

    XCreature * target;

    if (target = pData->l->map->GetMonster(mfd.pt.x, mfd.pt.y)) {
        XDice d(X, Y, Z);
        DAMAGE_DATA_EX dd;
        dd.damage	= d.S;
        dd.attacker	= pData->caller;
        dd.attack_name	= msg;
        dd.attack_HIT	= 1000;
        dd.attack_brand	= brt;
        dd.flags	= DF_MAGIC_BOLT;
        target->InflictDamage(&dd);
        return 1;
    }

    return 0;
}

RESULT XEffect::Make(XCreature * caster, EFFECT effect, int power)
{
    EFFECT_DATA ed;
    ed.caller	= caster;
    ed.l	= caster->l;
    ed.effect	= effect;
    ed.power	= power;
    ed.call_x	= caster->x;
    ed.call_y	= caster->y;

    if (GetReq(effect) == ER_DIRECTION) {
        XPoint pt;

        if (caster->GetTarget(TR_ATTACK_DIRECTION, &pt) == SUCCESS) {
            ed.target_x = pt.x + caster->x;
            ed.target_y = pt.y + caster->y;
        } else {
            return ABORT;
        }
    } else if (GetReq(effect) == ER_TARGET) {
        XPoint pt;

        if (caster->GetTarget(TR_ATTACK_TARGET, &pt, GetRange(ed.effect, ed.power)) == SUCCESS) {
            ed.target_x = pt.x;
            ed.target_y = pt.y;
        } else {
            return ABORT;
        }
    }

    if (Make(&ed)) {
        return SUCCESS;
    } else {
        return CONTINUE;
    }
}

int XEffect::Make(EFFECT_DATA * pData)
{
    int flag = 0;
    char buf[256];

    switch (pData->effect) {
        //healing and restoration
        case E_CURE_LIGHT_WOUNDS:
            return Heal(pData->caller, 1, pData->power / 2, 3) || Cure(pData->caller, 1, pData->power / 10, 1);
            break;

        case E_CURE_SERIOUS_WOUNDS:
            return Heal(pData->caller, 1, pData->power, 5) || Cure(pData->caller, 1, pData->power / 5, 2);
            break;

        case E_CURE_CRITICAL_WOUNDS:
            return Heal(pData->caller, 2, pData->power, 5) || Cure(pData->caller, 1, pData->power / 2, 3);
            break;

        case E_CURE_MORTAL_WOUNDS:
            return Heal(pData->caller, 3, pData->power, 10) || Cure(pData->caller, 3, pData->power, 10);
            break;

        case E_HEAL:
            return Heal(pData->caller, 5, pData->power, 20) || Cure(pData->caller, 5, pData->power, 20);
            break;

        case E_POWER:
            return Mana(pData->caller, 3, pData->power, 20);
            break;

        case E_RESTORATION:
            return Heal(pData->caller, 5, pData->power, 20) || Cure(pData->caller, 5, pData->power, 20) || Mana(pData->caller, 5, pData->power, 20);
            break;

        case E_ULTRAHEAL:
            return Heal(pData->caller, 7, pData->power, 20) || Cure(pData->caller, 7, pData->power, 20);
            break;

        case E_ULTRAPOWER:
            return Mana(pData->caller, 5, pData->power, 20);
            break;


        case E_CURE_POISON: {
            XDice d(1, pData->power, 5);
            pData->caller->md->Add(MOD_POISON, -d.S, pData->caller);
        }
        break;

        case E_CURE_DISEASE: {
            XDice d(1, pData->power, 3);
            pData->caller->md->Add(MOD_DISEASE, -d.S, pData->caller);
        }
        break;

        //combat - touch
        case E_BURNING_HANDS:
            return Touch(pData, 1, pData->power, 5, xRED, BR_FIRE, "the ball of fire");
            break;

        case E_ICE_TOUCH:
            return Touch(pData, 1, pData->power, 7, xWHITE, BR_COLD, "the cone of ice");
            break;

        case E_DRAIN_LIFE:
            return Touch(pData, 1, pData->power, 9, xDARKGRAY, BR_DRAIN_LIFE, "the black sphere");
            break;

        //combat - bolts
        case E_MAGIC_ARROW:
            return Bolt(pData, 1, pData->power / 2, 0, xBROWN, BR_EARTH, "the small arrow");
            break;

        case E_FIRE_BOLT:
            return Bolt(pData, 1, pData->power, 3, xRED, BR_FIRE, "the small ball of fire");
            break;

        case E_ICE_BOLT:
            return Bolt(pData, 1, pData->power, 5, xWHITE, BR_COLD, "the small cone of ice");
            break;

        case E_LIGHTNING_BOLT:
            return Bolt(pData, 2, pData->power, 10, xLIGHTBLUE, BR_LIGHTNING, "the bright spark");
            break;

        case E_ACID_BOLT:
            return Bolt(pData, 3, pData->power, 15, xGREEN, BR_ACID, "the small ball of viscous liquid");
            break;

        // Misc	modifers
        case E_HEROISM: {
            XDice d(2, pData->power, 5);
            pData->caller->md->Add(MOD_HEROISM, d.S, pData->caller);
        }
        break;

        case E_SELF_KNOWLEDGE:
            if (pData->caller->im & IM_HERO) {
                ((XHero*)pData->caller)->ShowResistance();
            }
            break;

        case E_IDENTIFY: {
            XItem * it = pData->caller->onIdentifyItem();

            if (it) {
                if (it->isIdentifed()) {
                    if (pData->caller->isVisible()) {
                        msgwin.Add(pData->caller->name);
                        msgwin.Add("learns nothing new about their items.");
                    }
                } else {
                    it->Identify(1);

                    if (pData->caller->isVisible()) {
                        msgwin.Add(pData->caller->name);
                        msgwin.Add("identifies item.");
                        char buf1[256];
                        it->toString(buf1);
                        char buf2[256];
                        sprintf(buf2, "It was %s.", buf1);
                        msgwin.Add(buf2);
                    }
                }

                return 1;
            }

            return 0;
        }
        break;

        case E_GREAT_IDENTIFY: {
            for (it_iterator i = pData->target->contain.begin(); i != pData->target->contain.end(); ++i) {
                i->Identify(1);
            }

            XList<XBodyPart*>::iterator it;

            for (it = pData->target->components.begin(); it != pData->target->components.end(); it++) {
                if ((*it)->Item()) {
                    (*it)->Item()->Identify(1);
                }
            }

            msgwin.Add(pData->caller->GetNameEx(CRN_T1));
            msgwin.Add(pData->caller->GetVerb("mumble"));
            msgwin.Add("arcane formula.");

            msgwin.Add(pData->target->GetNameEx(CRN_T1));
            msgwin.Add(pData->target->GetVerb("know"));
            msgwin.Add("about all items in the backpack.");

            return 1;
        }
        break;

        case E_SUMMON_MONSTER: {
            int flg = 0;
            int tx;
            int ty;

            for (int i = 0; i < 20; i++) {
                tx = pData->caller->x + vRand(3) - 1;
                ty = pData->caller->y + vRand(3) - 1;

                if (pData->l->map->XGetMovability(tx, ty) == 0) {
                    flg = 1;
                    break;
                }
            }

            if (flg) {
                XCreature * cr = pData->l->NewCreature(CR_UNDEAD);
                cr->LastStep();
                cr->FirstStep(tx, ty, pData->l);

                if (pData->l->map->GetVisible(pData->caller->x, pData->caller->y)) {
                    msgwin.Add(pData->caller->GetNameEx(CRN_T1));
                    msgwin.Add(pData->caller->GetVerb("summon"));
                    msgwin.Add("a monster.");
                }

                return 1;
            } else {
                return 0;
            }
        }
        break;

        case E_CREATE_ITEM: {
            XItem * item = ICREATEA(IM_ITEM);
            pData->caller->DropItem(item);

            if (pData->caller->isVisible()) {
                msgwin.Add(pData->caller->name);
                msgwin.Add("creates an item.");
            }

            return 1;
        };

        case E_BLINK: {
            XPoint pt;
            XRect rect(pData->caller->x - 5, pData->caller->y - 5, pData->caller->x + 5, pData->caller->y + 5);
            pData->l->GetFreeXY(&pt, &rect);

            if (pData->caller->isVisible()) {
                msgwin.Add(pData->caller->name);
                msgwin.Add("has suddenly disappered.");
            }

            pData->caller->LastStep();
            pData->caller->FirstStep(pt.x, pt.y, pData->l);

            if (pData->caller->isVisible()) {
                msgwin.Add(pData->caller->name);
                msgwin.Add("has suddenly appeared.");
            }

            return 1;
        };

        case E_TELEPORT: {
            XPoint pt;
            pData->l->GetFreeXY(&pt);

            if (!pData->target->isHero() && pData->target->isVisible()) {
                msgwin.Add(pData->target->name);
                msgwin.Add("has suddenly disappered.");
            }

            pData->target->LastStep();
            pData->target->FirstStep(pt.x, pt.y, pData->l);

            if (!pData->target->isHero() && pData->target->isVisible()) {
                msgwin.Add(pData->target->name);
                msgwin.Add("has suddenly appeared.");
            }

            if (pData->target->isHero()) {
                msgwin.Add("You have teleported away!");
            }

            return 1;
        };

        case E_SEE_INVISIBLE:
            return pData->caller->md->Add(MOD_SEE_INVISIBLE, pData->power, pData->caller);
            break;

        case E_ACID_RESISTANCE:
            return pData->caller->md->Add(MOD_ACID_RESISTANCE, pData->power, pData->caller);
            break;

        case E_FIRE_RESISTANCE:
            return pData->caller->md->Add(MOD_FIRE_RESISTANCE, pData->power, pData->caller);
            break;

        case E_POISON_RESISTANCE:
            return pData->caller->md->Add(MOD_POISON_RESISTANCE, pData->power, pData->caller);
            break;

        case E_COLD_RESISTANCE:
            return pData->caller->md->Add(MOD_COLD_RESISTANCE, pData->power, pData->caller);
            break;
    }

    return 0;
}
