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
#include <iostream>
#include "magic/effect.h"
#include "magic/modifiers.h"
#include "magic/modifier.h"

void XEffect::RegisterLua(sol::state_view& lua)
{
    lua.new_enum("EffectTarget",
        "NONE", EffectTarget::NONE,
        "DIRECTION", EffectTarget::DIRECTION,
        "TARGET", EffectTarget::TARGET,
        "ITEM", EffectTarget::ITEM
    );

    lua.new_usertype<EffectBuilder>("Effect",
        sol::constructors<EffectBuilder(std::string)>(),
        "Heals", &EffectBuilder::Heals,
        "Cures", &EffectBuilder::Cures,
        "Restores", &EffectBuilder::Restores,
        "Inflicts", &EffectBuilder::Inflicts,
        "Relieves", &EffectBuilder::Relieves,
        "Sustains", &EffectBuilder::Sustains,
        "Touches", &EffectBuilder::Touches,
        "Throws", &EffectBuilder::Throws,
        "Engine", &EffectBuilder::Engine,
        "Summons", &EffectBuilder::Summons,
        "Targets", &EffectBuilder::Targets,
        "Range", &EffectBuilder::Range,
        "Register", &EffectBuilder::Register
    );
}

std::vector<EffectStats> effects_db;

const EffectStats* FindEffect(const EFFECT& id)
{
    for (const auto& row : effects_db) {
        if (row.id == id) {
            return &row;
        }
    }

    return nullptr;
}

EffectBuilder::EffectBuilder(std::string id)
{
    t.id = std::move(id);
}

static EffectPart Dice(const EffectPart::Kind kind, const int count,
    const int divisor, const int bonus)
{
    EffectPart part;
    part.kind = kind;
    part.count = count;
    part.divisor = divisor < 1 ? 1 : divisor;
    part.bonus = bonus;

    return part;
}

EffectBuilder& EffectBuilder::Heals(const int count, const int divisor, const int bonus)
{
    t.parts.push_back(Dice(EffectPart::Kind::HEAL, count, divisor, bonus));
    return *this;
}

EffectBuilder& EffectBuilder::Cures(const int count, const int divisor, const int bonus)
{
    t.parts.push_back(Dice(EffectPart::Kind::CURE, count, divisor, bonus));
    return *this;
}

EffectBuilder& EffectBuilder::Restores(const int count, const int divisor, const int bonus)
{
    t.parts.push_back(Dice(EffectPart::Kind::MANA, count, divisor, bonus));
    return *this;
}

EffectBuilder& EffectBuilder::Inflicts(const std::string& modifier, const int count,
    const int divisor, const int bonus)
{
    // A name nothing knows: say so while the file that wrote it is
    // loading, and lay nothing on.
    if (!IsKnownModifier(modifier)) {
        std::cerr << "world: :Inflicts() names a modifier '" << modifier
                  << "', which nothing defines" << std::endl;

        return *this;
    }

    EffectPart part = Dice(EffectPart::Kind::MODIFIER, count, divisor, bonus);
    part.modifier = modifier;
    t.parts.push_back(part);
    return *this;
}

EffectBuilder& EffectBuilder::Relieves(const std::string& modifier, const int count,
    const int divisor, const int bonus)
{
    // A name nothing knows: say so while the file that wrote it is
    // loading, and lay nothing on.
    if (!IsKnownModifier(modifier)) {
        std::cerr << "world: :Relieves() names a modifier '" << modifier
                  << "', which nothing defines" << std::endl;

        return *this;
    }

    EffectPart part = Dice(EffectPart::Kind::MODIFIER, count, divisor, bonus);
    part.modifier = modifier;
    part.relieves = true;
    t.parts.push_back(part);
    return *this;
}

EffectBuilder& EffectBuilder::Sustains(const std::string& modifier)
{
    // A name nothing knows: say so while the file that wrote it is
    // loading, and lay nothing on.
    if (!IsKnownModifier(modifier)) {
        std::cerr << "world: :Sustains() names a modifier '" << modifier
                  << "', which nothing defines" << std::endl;

        return *this;
    }

    EffectPart part;
    part.kind = EffectPart::Kind::MODIFIER;
    part.modifier = modifier;
    part.sustained = true;
    t.parts.push_back(part);
    return *this;
}

EffectBuilder& EffectBuilder::Touches(const int count, const int divisor, const int bonus,
    const int colour, const std::string& brand, const std::string& message)
{
    EffectPart part = Dice(EffectPart::Kind::TOUCH, count, divisor, bonus);
    part.colour = colour;
    part.brands = BrandSet(brand);
    part.message = message;
    t.parts.push_back(part);
    return *this;
}

EffectBuilder& EffectBuilder::Throws(const int count, const int divisor, const int bonus,
    const int colour, const std::string& brand, const std::string& message)
{
    EffectPart part = Dice(EffectPart::Kind::BOLT, count, divisor, bonus);
    part.colour = colour;
    part.brands = BrandSet(brand);
    part.message = message;
    t.parts.push_back(part);
    return *this;
}

EffectBuilder& EffectBuilder::Summons(const std::string& cr_class)
{
    if (t.parts.empty()) {
        std::cerr << "world: the effect '" << t.id << "' summons '" << cr_class
                  << "' before saying what it does" << std::endl;

        return *this;
    }

    if (CheckCreatureClassExists(cr_class, ("effect '" + t.id + "'").c_str())) {
        t.parts.back().summons = cr_class;
    }

    return *this;
}

EffectBuilder& EffectBuilder::Engine(const std::string& which)
{
    EffectPart part;
    part.kind = EffectPart::Kind::ENGINE;
    part.engine = which;
    t.parts.push_back(part);
    return *this;
}

EffectBuilder& EffectBuilder::Targets(const EffectTarget targets)
{
    t.targets = targets;
    return *this;
}

EffectBuilder& EffectBuilder::Range(const int divisor, const int bonus)
{
    t.range_divisor = divisor;
    t.range_bonus = bonus;
    return *this;
}

void EffectBuilder::Register()
{
    // The one engine effect that needs more than its name: without a
    // class to raise it would call up nothing at all, once per cast,
    // which is a thing to hear about while the file is loading rather
    // than the first time somebody reads the scroll.
    for (const auto& part : t.parts) {
        if (part.kind == EffectPart::Kind::ENGINE
            && part.engine == "summon_monster" && part.summons.empty()) {
            std::cerr << "world: the effect '" << t.id
                      << "' summons, but says nothing of what" << std::endl;
        }
    }

    if (t.id.empty()) {
        std::cerr << "world: an effect with no id" << std::endl;
        return;
    }

    if (FindEffect(t.id)) {
        std::cerr << "world: two effects both called '" << t.id << "'" << std::endl;
        return;
    }


    effects_db.push_back(t);
}

EffectTarget XEffect::GetReq(const EFFECT& effect)
{
    const EffectStats* row = FindEffect(effect);

    return row ? row->targets : EffectTarget::NONE;
}

int XEffect::GetRange(const EFFECT& effect, const int power)
{
    const EffectStats* row = FindEffect(effect);

    if (!row) {
        return 0;
    }

    return row->range_divisor > 0
        ? power / row->range_divisor + row->range_bonus
        : row->range_bonus;
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

int XEffect::Touch(const EFFECT_DATA* pData, int X, int Y, int Z, int col, const BrandSet& brt, const std::string& msg)
{
    XCreature * target = pData->l->map->GetMonster(pData->target_x, pData->target_y);

    if (pData->l->map->GetVisible(pData->target_x, pData->target_y) && __animation_flag) {
        pData->l->map->Put(pData->caller);
        pData->l->map->PutChar(pData->target_x, pData->target_y, '*', static_cast<xColor>(col));
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


int XEffect::Bolt(const EFFECT_DATA* pData, int X, int Y, int Z, int col, const BrandSet& brt, const std::string& msg)
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

RESULT XEffect::Make(XCreature * caster, const EFFECT& effect, int power)
{
    EFFECT_DATA ed{};
    ed.caller	= caster;
    ed.l	= caster->l;
    ed.effect	= effect;
    ed.power	= power;
    ed.call_x	= caster->x;
    ed.call_y	= caster->y;

    // Whoever the effect acts on. Somebody casting one acts on themselves
    // unless another caller says otherwise - a trap names the creature
    // that stepped on it, and MakeEffect() lets script name anyone.
    ed.target	= caster;

    if (GetReq(effect) == EffectTarget::DIRECTION) {
        XPoint pt;

        if (caster->GetTarget(TR_ATTACK_DIRECTION, &pt) == SUCCESS) {
            ed.target_x = pt.x + caster->x;
            ed.target_y = pt.y + caster->y;
        } else {
            return ABORT;
        }
    } else if (GetReq(effect) == EffectTarget::TARGET) {
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

// The few an effect cannot be built out of: they make creatures and items,
// move people about the map, and show screens. Content names one of these
// with :Engine("teleport") rather than describing it, because there is
// nothing here to describe in dice and colours.
int XEffect::Engine(const EffectPart& part, const EFFECT_DATA* pData)
{
    const std::string& which = part.engine;

    if (which == "self_knowledge") {
        if (pData->caller->isHero()) {
            dynamic_cast<XHero *>(pData->caller)->ShowResistance();
        }

        return pData->caller->isHero();
    }

    if (which == "identify") {
        if (auto it = pData->caller->onIdentifyItem()) {
            if (it->isIdentified()) {
                if (pData->caller->isVisible()) {
                    msgwin.Add(fmt::format(
                        "{} learns nothing new about their items.",
                        pData->caller->name));
                }
            } else {
                it->Identify();

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

    if (which == "great_identify") {
        for (auto i : pData->target->contain) {
            i->Identify();
        }

        for (auto& bp: pData->target->components) {
            if (bp->Item()) {
                bp->Item()->Identify();
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

    if (which == "summon_monster") {
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
            XCreature* cr = pData->l->NewCreatureOfClass({part.summons});

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

    if (which == "create_item") {
        XItem * item = ICREATEA(ItemKind::ITEM);
        pData->caller->DropItem(item);

        if (pData->caller->isVisible()) {
            msgwin.Add(pData->caller->name);
            msgwin.Add("creates an item.");
        }

        return 1;
    }

    if (which == "blink") {
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
    }

    if (which == "teleport") {
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
    }

    std::cerr << "world: an effect asks the engine for '" << which
          << "', which it does not know how to do" << std::endl;

    return 0;
}

int XEffect::Make(const EFFECT_DATA* pData)
{
    const EffectStats* row = FindEffect(pData->effect);

    if (!row) {
        std::cerr << "world: nothing defines an effect '" << pData->effect << "'"
                  << std::endl;

        return 0;
    }

    // The parts are tried in turn and the first that has something to do
    // wins: a potion that heals wounds and mends bleeding does whichever
    // the drinker actually needs, never both. That is what the chain of
    // Heal() || Cure() || Mana() meant when this was a switch.
    for (const auto& part : row->parts) {
        const int sides = part.divisor > 0 ? pData->power / part.divisor : pData->power;
        int done = 0;

        switch (part.kind) {
            case EffectPart::Kind::HEAL:
                done = Heal(pData->caller, part.count, sides, part.bonus);
                break;

            case EffectPart::Kind::CURE:
                done = Cure(pData->caller, part.count, sides, part.bonus);
                break;

            case EffectPart::Kind::MANA:
                done = Mana(pData->caller, part.count, sides, part.bonus);
                break;

            case EffectPart::Kind::MODIFIER: {
                const int val = part.sustained
                    ? pData->power
                    : XDice(part.count, sides, part.bonus).GetResult();

                done = pData->caller->md->Add(part.modifier,
                    part.relieves ? -val : val, pData->caller);
                break;
            }

            case EffectPart::Kind::TOUCH:
                done = Touch(pData, part.count, sides, part.bonus,
                    part.colour, part.brands, part.message);
                break;

            case EffectPart::Kind::BOLT:
                done = Bolt(pData, part.count, sides, part.bonus,
                    part.colour, part.brands, part.message);
                break;

            case EffectPart::Kind::ENGINE:
                done = Engine(part, pData);
                break;
        }

        if (done) {
            return 1;
        }
    }

    return 0;
}
