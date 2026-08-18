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

#include <sol/sol.hpp>

#include "creature/creature.h"
#include "creature/xhero.h"
#include "game/game.h"
#include "helpers/dice.h"
#include "helpers/msgwin.h"
#include "item/itemf.h"
#include "magic/effect.h"
#include "magic/modifier.h"

void XEffect::RegisterLua(sol::state_view& lua)
{
    lua.new_enum("XEffect",
        "CURE_LIGHT_WOUNDS", XEffect::CURE_LIGHT_WOUNDS,
        "CURE_SERIOUS_WOUNDS", XEffect::CURE_SERIOUS_WOUNDS,
        "CURE_CRITICAL_WOUNDS", XEffect::CURE_CRITICAL_WOUNDS,
        "CURE_MORTAL_WOUNDS", XEffect::CURE_MORTAL_WOUNDS,
        "HEAL", XEffect::HEAL,
        "ULTRAHEAL", XEffect::ULTRAHEAL,
        "POWER", XEffect::POWER,
        "ULTRAPOWER", XEffect::ULTRAPOWER,
        "RESTORATION", XEffect::RESTORATION,
        "CURE_POISON", XEffect::CURE_POISON,
        "CURE_DISEASE", XEffect::CURE_DISEASE,
        "BURNING_HANDS", XEffect::BURNING_HANDS,
        "ICE_TOUCH", XEffect::ICE_TOUCH,
        "DRAIN_LIFE", XEffect::DRAIN_LIFE,
        "MAGIC_ARROW", XEffect::MAGIC_ARROW,
        "FIRE_BOLT", XEffect::FIRE_BOLT,
        "ICE_BOLT", XEffect::ICE_BOLT,
        "LIGHTNING_BOLT", XEffect::LIGHTNING_BOLT,
        "ACID_BOLT", XEffect::ACID_BOLT,
        "HEROISM", XEffect::HEROISM,
        "IDENTIFY", XEffect::IDENTIFY,
        "GREAT_IDENTIFY", XEffect::GREAT_IDENTIFY,
        "SUMMON_MONSTER", XEffect::SUMMON_MONSTER,
        "CREATE_ITEM", XEffect::CREATE_ITEM,
        "BLINK", XEffect::BLINK,
        "TELEPORT", XEffect::TELEPORT,
        "SELF_KNOWLEDGE", XEffect::SELF_KNOWLEDGE,
        "SEE_INVISIBLE", XEffect::SEE_INVISIBLE,
        "ACID_RESISTANCE", XEffect::ACID_RESISTANCE,
        "FIRE_RESISTANCE", XEffect::FIRE_RESISTANCE,
        "COLD_RESISTANCE", XEffect::COLD_RESISTANCE,
        "POISON_RESISTANCE", XEffect::POISON_RESISTANCE
    );
}

EFFECT_REQ XEffect::GetReq(XEffect::Id effect)
{
    switch (effect) {
        case XEffect::NONE:
        case XEffect::CURE_LIGHT_WOUNDS:
        case XEffect::CURE_SERIOUS_WOUNDS:
        case XEffect::CURE_CRITICAL_WOUNDS:
        case XEffect::CURE_MORTAL_WOUNDS:
        case XEffect::HEAL:
        case XEffect::HEROISM:
        case XEffect::POWER:
        case XEffect::RESTORATION:
        case XEffect::SUMMON_MONSTER:
        case XEffect::CREATE_ITEM:
        case XEffect::CURE_POISON:
        case XEffect::CURE_DISEASE:
        case XEffect::BLINK:
        case XEffect::SELF_KNOWLEDGE:
        case XEffect::SEE_INVISIBLE:
        case XEffect::ACID_RESISTANCE:
        case XEffect::FIRE_RESISTANCE:
        case XEffect::COLD_RESISTANCE:
        case XEffect::POISON_RESISTANCE:
            return ER_NONE;

        case XEffect::BURNING_HANDS:
        case XEffect::ICE_TOUCH:
        case XEffect::DRAIN_LIFE:
            return ER_DIRECTION;

        case XEffect::IDENTIFY:
            return ER_ITEM;

        case XEffect::MAGIC_ARROW:
        case XEffect::FIRE_BOLT:
        case XEffect::ICE_BOLT:
        case XEffect::LIGHTNING_BOLT:
        case XEffect::ACID_BOLT:
        case XEffect::TELEPORT:
            return ER_TARGET;

        default:
            return ER_NONE;
    }
}

int XEffect::GetRange(XEffect::Id effect, int power)
{
    switch (effect) {
        case XEffect::MAGIC_ARROW:
            return power / 4 + 2;

        case XEffect::FIRE_BOLT:
        case XEffect::ICE_BOLT:
            return power / 6 + 2;

        case XEffect::LIGHTNING_BOLT:
            return power / 7 + 2;

        case XEffect::ACID_BOLT:
            return power / 8 + 2;

        case XEffect::TELEPORT:
            return 1;

        default:
            return 0;
    }
}

int XEffect::Heal(XCreature * caster, int X, int Y, int Z)
{
    XDice d(X, Y, Z);

    if (caster->onHeal(d.GetResult())) {
        if (caster->isVisible()) {
            msgwin.Add(caster->GetNameEx(CRN_T1));
            msgwin.Add(caster->GetVerb("feel"));

            if (caster->GetMaxHP() == caster->HP) {
                msgwin.Add("completly healed.");
            } else if (d.GetResult() < 8) {
                msgwin.Add("slightly healed.");
            } else if (d.GetResult()< 15) {
                msgwin.Add("somewhat healed.");
            } else if (d.GetResult()< 100) {
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

    if (caster->md->Add(MOD_WOUND, -d.GetResult(), caster)) {
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

    if (caster->onRestorePP(d.GetResult())) {
        if (caster->isVisible()) {
            std::string str;
            str = fmt::format("The power flows through {} body.", caster->GetNameEx(CRN_T4));
            msgwin.Add(str);
        }

        return 1;
    }

    return 0;
}

int XEffect::Touch(const EFFECT_DATA* pData, int X, int Y, int Z, xColor col, AttackEffectType brt, const char* msg)
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

        DAMAGE_DATA_EX dd{};
        dd.damage = d.GetResult();
        dd.attacker = pData->caller;
        dd.attack_name = msg;
        dd.attack_HIT = 1000;
        dd.attack_effect = brt;
        dd.flags = DF_MAGIC_BOLT;
        target->InflictDamage(&dd);
        return 1;
    } else {
        return 0;
    }
}


int XEffect::Bolt(const EFFECT_DATA* pData, int X, int Y, int Z, xColor col, AttackEffectType brt, const char* msg)
{
    MF_DATA mfd{};
    mfd.arrow_type = MFT_BALL;
    mfd.arrow_color = col;
    mfd.l = pData->l;
    mfd.sx = pData->call_x;
    mfd.sy = pData->call_y;
    mfd.ex = pData->target_x;
    mfd.ey = pData->target_y;
    mfd.to_hit = 1000;
    mfd.max_range = GetRange(pData->effect, pData->power);

    // Actually launch the bolt. MissileFlight() walks it from (sx,sy)
    // towards (ex,ey), draws it, lets the victim try to avoid it, and
    // reports where it came to rest in mfd.pt - which is exactly what
    // the lookup below needs.
    if (XCreature::MissileFlight(&mfd) != MF_HIT) {
        return 0;
    }

    if (XCreature* target = pData->l->map->GetMonster(mfd.pt.x, mfd.pt.y)) {
        XDice d(X, Y, Z);
        DAMAGE_DATA_EX dd{};
        dd.damage = d.GetResult();
        dd.attacker = pData->caller;
        dd.attack_name = msg;
        dd.attack_HIT = 1000;
        dd.attack_effect = brt;
        dd.flags = DF_MAGIC_BOLT;
        target->InflictDamage(&dd);
        return 1;
    }

    return 0;
}

RESULT XEffect::Make(XCreature * caster, XEffect::Id effect, int power)
{
    EFFECT_DATA ed{};
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

int XEffect::Make(const EFFECT_DATA* pData)
{
    switch (pData->effect) {
        // healing and restoration
        case XEffect::CURE_LIGHT_WOUNDS:
            return Heal(pData->caller, 1, pData->power / 2, 3) ||
                Cure(pData->caller, 1, pData->power / 10, 1);

        case XEffect::CURE_SERIOUS_WOUNDS:
            return Heal(pData->caller, 1, pData->power, 5) ||
                Cure(pData->caller, 1, pData->power / 5, 2);

        case XEffect::CURE_CRITICAL_WOUNDS:
            return Heal(pData->caller, 2, pData->power, 5) ||
                Cure(pData->caller, 1, pData->power / 2, 3);

        case XEffect::CURE_MORTAL_WOUNDS:
            return Heal(pData->caller, 3, pData->power, 10) ||
                Cure(pData->caller, 3, pData->power, 10);

        case XEffect::HEAL:
            return Heal(pData->caller, 5, pData->power, 20) ||
                Cure(pData->caller, 5, pData->power, 20);

        case XEffect::POWER:
            return Mana(pData->caller, 3, pData->power, 20);

        case XEffect::RESTORATION:
            return Heal(pData->caller, 5, pData->power, 20) ||
                Cure(pData->caller, 5, pData->power, 20) ||
                    Mana(pData->caller, 5, pData->power, 20);

        case XEffect::ULTRAHEAL:
            return Heal(pData->caller, 7, pData->power, 20) ||
                Cure(pData->caller, 7, pData->power, 20);

        case XEffect::ULTRAPOWER:
            return Mana(pData->caller, 5, pData->power, 20);


        case XEffect::CURE_POISON: {
            XDice d(1, pData->power, 5);
            pData->caller->md->Add(MOD_POISON, -d.GetResult(), pData->caller);
        }
        break;

        case XEffect::CURE_DISEASE: {
            XDice d(1, pData->power, 3);
            pData->caller->md->Add(MOD_DISEASE, -d.GetResult(), pData->caller);
        }
        break;

        // combat - touch
        case XEffect::BURNING_HANDS:
            return Touch(pData, 1, pData->power, 5, xRED, AttackEffectType::FIRE, "the ball of fire");

        case XEffect::ICE_TOUCH:
            return Touch(pData, 1, pData->power, 7, xWHITE, AttackEffectType::COLD, "the cone of ice");

        case XEffect::DRAIN_LIFE:
            return Touch(pData, 1, pData->power, 9, xDARKGRAY, AttackEffectType::DRAIN_LIFE, "the black sphere");

        // combat - bolts
        case XEffect::MAGIC_ARROW:
            return Bolt(pData, 1, pData->power / 2, 0, xBROWN, AttackEffectType::EARTH, "the small arrow");

        case XEffect::FIRE_BOLT:
            return Bolt(pData, 1, pData->power, 3, xRED, AttackEffectType::FIRE, "the small ball of fire");

        case XEffect::ICE_BOLT:
            return Bolt(pData, 1, pData->power, 5, xWHITE, AttackEffectType::COLD, "the small cone of ice");

        case XEffect::LIGHTNING_BOLT:
            return Bolt(pData, 2, pData->power, 10, xLIGHTBLUE, AttackEffectType::LIGHTNING, "the bright spark");

        case XEffect::ACID_BOLT:
            return Bolt(pData, 3, pData->power, 15, xGREEN, AttackEffectType::ACID, "the small ball of viscous liquid");

        // Misc	modifiers
        case XEffect::HEROISM: {
            XDice d(2, pData->power, 5);
            pData->caller->md->Add(MOD_HEROISM, d.GetResult(), pData->caller);
        }
        break;

        case XEffect::SELF_KNOWLEDGE:
            if (pData->caller->isHero()) {
                dynamic_cast<XHero *>(pData->caller)->ShowResistance();
            }
            break;

        case XEffect::IDENTIFY: {
            if (auto it = pData->caller->onIdentifyItem()) {
                if (it->isIdentified()) {
                    if (pData->caller->isVisible()) {
                        msgwin.Add(fmt::format(
                            "{} learns nothing new about their items.",
                            pData->caller->name));
                    }
                } else {
                    it->Identify(1);

                    if (pData->caller->isVisible()) {
                        msgwin.Add(fmt::format(
                            "{} identifies the item. It was {}.",
                            pData->caller->name,
                            it->toString()));
                    }
                }

                return 1;
            }

            return 0;
        }

        case XEffect::GREAT_IDENTIFY: {
            for (auto i : pData->target->contain) {
                i->Identify(1);
            }

            for (auto& bp: pData->target->components) {
                if (bp->Item()) {
                    bp->Item()->Identify(1);
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

        case XEffect::SUMMON_MONSTER: {
            int flg = 0;
            int tx = 0;
            int ty = 0;

            for (int i = 0; i < 20; i++) {
                tx = pData->caller->x + static_cast<int>(vRand(3)) - 1;
                ty = pData->caller->y + static_cast<int>(vRand(3)) - 1;

                if (pData->l->map->XGetMovability(tx, ty) == 0) {
                    flg = 1;
                    break;
                }
            }

            if (flg) {
                XCreature* cr = pData->l->NewCreature(CreatureClass::UNDEAD);

                if (!cr) {
                    return 0;
                }

                // NewCreature() already placed cr via its own FirstStep(),
                // whose birth path makes the map cell cr's only strong
                // owner (the scheduler only keeps a weak_ptr - see
                // XScheduler::Add()). Without this, cr->LastStep() below
                // drops that sole strong reference, and the following
                // FirstStep() re-registers cr under a brand-new, separate
                // control block - the same use-after-free class fixed for
                // the hero-swap case in creature/xhero.cpp (see
                // cr_keepalive there).
                auto cr_keepalive = std::static_pointer_cast<XCreature>(cr->shared_from_this());
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

        case XEffect::CREATE_ITEM: {
            XItem * item = ICREATEA(ItemKind::ITEM);
            pData->caller->DropItem(item);

            if (pData->caller->isVisible()) {
                msgwin.Add(pData->caller->name);
                msgwin.Add("creates an item.");
            }

            return 1;
        };

        case XEffect::BLINK: {
            XRect rect(pData->caller->x - 5, pData->caller->y - 5, pData->caller->x + 5, pData->caller->y + 5);

            // Nowhere within five tiles to put it - the summon fizzles
            const auto pt_opt = pData->l->GetFreeXY(&rect);

            if (!pt_opt) {
                return 0;
            }

            const XPoint pt = *pt_opt;

            if (pData->caller->isVisible()) {
                msgwin.Add(pData->caller->name);
                msgwin.Add("has suddenly disappered.");
            }

            // See the SUMMON_MONSTER case above for why this keepalive is
            // needed - pData->caller isn't necessarily the creature the
            // scheduler currently holds a strong reference to.
            auto caller_keepalive = std::static_pointer_cast<XCreature>(pData->caller->shared_from_this());
            pData->caller->LastStep();
            pData->caller->FirstStep(pt.x, pt.y, pData->l);

            if (pData->caller->isVisible()) {
                msgwin.Add(pData->caller->name);
                msgwin.Add("has suddenly appeared.");
            }

            return 1;
        };

        case XEffect::TELEPORT: {
            const auto pt_opt = pData->l->GetFreeXY();

            if (!pt_opt) {
                return 0;
            }

            const XPoint pt = *pt_opt;

            if (!pData->target->isHero() && pData->target->isVisible()) {
                msgwin.Add(pData->target->name);
                msgwin.Add("has suddenly disappered.");
            }

            // See the SUMMON_MONSTER case above - pData->target is whoever
            // the spell/trap is acting on, not necessarily the creature
            // currently taking its own turn, so it isn't otherwise
            // guaranteed to have a strong reference keeping it alive here.
            auto target_keepalive = std::static_pointer_cast<XCreature>(pData->target->shared_from_this());
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

        case XEffect::SEE_INVISIBLE:
            return pData->caller->md->Add(MOD_SEE_INVISIBLE, pData->power, pData->caller);

        case XEffect::ACID_RESISTANCE:
            return pData->caller->md->Add(MOD_ACID_RESISTANCE, pData->power, pData->caller);

        case XEffect::FIRE_RESISTANCE:
            return pData->caller->md->Add(MOD_FIRE_RESISTANCE, pData->power, pData->caller);

        case XEffect::POISON_RESISTANCE:
            return pData->caller->md->Add(MOD_POISON_RESISTANCE, pData->power, pData->caller);

        case XEffect::COLD_RESISTANCE:
            return pData->caller->md->Add(MOD_COLD_RESISTANCE, pData->power, pData->caller);

        default:
            assert(0);
    }

    return 0;
}
