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
#include <sol/sol.hpp>

#include "creature/anycr.h"
#include "game/game.h"
#include "helpers/msgwin.h"
#include "item/item_cereal.h"
#include "helpers/keyword_dice.h"
#include "item/xcorpse.h"

#include "creature/anycr.h"
#include "helpers/keyword_dice.h"
#include "item/xcorpse.h"
#include "magic/modifier.h"
#include "magic/modifiers.h"

void RegisterCorpseEffectEnum(sol::state_view& lua)
{
    // What is left of this enum: the mechanisms that name nothing. What
    // a corpse does to a stat or a resistance is said with
    // :CorpseStat("To", 1) / :CorpseResist("fire", 1) instead, so that a
    // world may confer any resistance it declares, not five chosen ones.
    lua.new_enum("CorpseEffectType",
        "STOMACH", XCorpse::EffectType::STOMACH,
        "VOMIT", XCorpse::EffectType::VOMIT,
        "SATIATION", XCorpse::EffectType::SATIATION
    );
}

REGISTER_CLASS(XCorpse);
CEREAL_REGISTER_TYPE(XCorpse);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XItem, XCorpse);

XCorpse::XCorpse(XCreature * corpse_owner, CORPSE_FLAG cf)
{
    kind = ItemKind::FOOD;
    view = '%';
    color = corpse_owner->color;

    weight = corpse_owner->weight / 2;

    food_nutrio = (int)(weight / log((weight + 7.0) / 5.0));
    food_nutrio = food_nutrio == 0 ? 1 : food_nutrio;

    value = food_nutrio / 10;
    value = value == 0 ? 1 : value;

    consume_nutrio = food_nutrio / (vRand(5) + 1);
    consume_nutrio = consume_nutrio == 0 ? 1 : consume_nutrio;

    ttmb = 1000;
    ttm = 1000;
    it = IT_CORPSE;
    name = fmt::format("{} corpse", corpse_owner->name);

    corpse_flag = cf;
    time_of_roating = 0;
    roating_stopped = 0;
    pCorpseData = &XCreatureStorage::GetCreatureData(corpse_owner->creature_name)->pCorpseData;
    cn = corpse_owner->creature_name;
    Game.Scheduler.Add(this);
}

RESULT XCorpse::onEat(XCreature * eater)
{
    // Prevent the corpse from being destroyed under us: XAnyFood::onEat()
    // below Invalidate()s it once it has been eaten up, which can drop
    // the last reference to it, while the effect handling further down
    // still reads our own members. A real strong reference held for the
    // whole call.
    // Every corpse is shared_ptr-owned from construction onwards (its
    // constructor registers with the scheduler, which routes through
    // XItem::Own()), so the only way this comes back empty is a corpse
    // that has already been invalidated - rotted away while something
    // still held a reference to it. There is nothing left to eat, and
    // reading its members below would be reading a dead object.
    const auto keepalive = ToWeakPtr(this).lock();

    if (!keepalive) {
        return FAIL;
    }

    RESULT flag = XAnyFood::onEat(eater);

    if (flag == SUCCESS) {
        for (auto it: pCorpseData->effect) {
            switch (it.type) {
                case EffectType::STAT: {
                    const int stat = StatKeyword(it.target);

                    if (stat >= 0) {
                        eater->GainAttr(static_cast<XStats::Id>(stat), it.value);
                    }
                }
                break;

                case EffectType::RESIST:
                    eater->GainResist(it.target, it.value);
                    break;

                case EffectType::MODIFIER:
                    // Whatever was wrong with it catches up with the eater
                    // somewhere in the next hundred turns.
                    eater->md->Add(it.modifier, it.value, eater, eater, vRand(100));
                    break;

                case EffectType::VOMIT:
                    if (eater->isHero()) {
                        msgwin.Add("You vomit!");

                        if (eater->nutrio > 1000) {
                            eater->nutrio = 1000;
                        }
                    }

                    break;

                case EffectType::STOMACH:
                    if (eater->isHero()) {
                        if (it.value < 0) {
                            msgwin.Add("You stomach shrinks from pain!");
                            eater->nutrio_speed++;
                        } else {
                            msgwin.Add("You stomach rumbles peacefully!");

                            if (eater->nutrio_speed > 1) { // 1 is the minimum rate of food processing
                                eater->nutrio_speed--;
                            }
                        }
                    }

                    break;

                // EffectType::SATIATION is declared but never carried out, and
                // nothing creates one - a corpse effect of a type this
                // does not know is simply not applied.
                default:
                    break;
            }
        }
    }

    return flag;
}

std::string XCorpse::postEat(XCreature * /*eater*/)
{
    return "tasty";
}

bool XCorpse::Run()
{
    CORPSE_CONDITION cc = GetCondition();
    auto owner_sp = owner.lock();

    if (corpse_flag == CF_RAW) {
        time_of_roating++;

        if (!owner_sp) {
            time_of_roating += 3;
        }
    }

    if (cc != GetCondition()) {
        if (owner_sp && owner_sp->isHero()) {
            msgwin.Add("Something in your backpack seems to rotting.");
        } else if (l && isInVisibleArea()) {
            msgwin.Add("Something seems to rotting.");
        }
    } else if (time_of_roating > pCorpseData->roating_time) {
        if (owner_sp) {
            if (owner_sp->isHero()) {
                msgwin.Add("Suddenly your equipment weighs less.");
            }

            owner_sp->UnCarryItem(this);
        } else if (l && isInVisibleArea()) {
            msgwin.Add("Suddenly something disappered from the ground.");
        }

        Invalidate();

        // Erase from contain last, after is_valid is already cleared above
        // - see the matching comment in XItem::Invalidate(). If contain was
        // this corpse's last reference, erasing it here safely runs
        // Own()'s deleter as a plain delete (Invalidate() already ran)
        // instead of risking a reentrant Invalidate() call while this
        // Run() is still executing.
        if (owner_sp) {
            if (auto it = owner_sp->contain.find(this); it != owner_sp->contain.end()) {
                owner_sp->contain.erase(it);
            }
        }

        return false;
    }

    ttm += ttmb;
    return true;
}

CORPSE_CONDITION XCorpse::GetCondition()
{
    double val = ((double)time_of_roating) / pCorpseData->roating_time;

    if (val < 0.1) {
        return CCOND_NICE;
    }

    if (val < 0.2) {
        return CCOND_NORMAL;
    } else if (val < 0.4) {
        return CCOND_SROTED;
    } else if (val < 0.6) {
        return CCOND_ROTED;
    } else {
        return CCOND_VROTED;
    }
}

int XCorpse::GetValue()
{
    return value;
}

void XCorpse::FixupCorpseData()
{
    pCorpseData = &XCreatureStorage::GetCreatureData(cn)->pCorpseData;
}

std::string XCorpse::toString()
{
    if (corpse_flag & CF_COOKED) {
        return fmt::format("cooked %s", name);
    }

    return name;
}
