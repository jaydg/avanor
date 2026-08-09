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

#include <cereal/archives/json.hpp>
#include <cereal/types/polymorphic.hpp>

#include "helpers/msgwin.h"
#include "magic/modifier.h"
#include "magic/modifiers.h"

// None of these are XObject-derived, so there's no REGISTER_CLASS/
// DYNCREATE entry to sit next to - this polymorphic hierarchy (held as
// vector<unique_ptr<XBasicModifier>> in XModifier::ml) never had a
// working save-time type tag at all; the legacy XModifier::Store/
// Restore and XBasicModifier::Store/Restore (since removed) were
// already stubbed out/incomplete before this - Cereal's own
// polymorphic registration is what makes real persistence here possible
// for the first time, not a mechanical port.
//
// The matching CEREAL_LOAD_VIA_PLACEHOLDER_CONSTRUCT for each of these
// lives in modifiers.h, not here: every subclass constructor requires
// args (no usable no-args constructor - the assert(0)-guarded one that
// exists on each is a deliberate guard, not meant to ever run), and
// their construction gets instantiated from inside XModifier::ml's own
// deserialization, which itself gets reinstantiated in every TU that
// reaches XCreature::md - a specialization declared only in this .cpp
// wouldn't be visible to those other TUs (see the comment on
// XStandardAI's own CEREAL_LOAD_VIA_PLACEHOLDER_CONSTRUCT in std_ai.h
// for the same reasoning, first hit there).
// XBasicModifier is itself directly instantiable (not just an abstract
// base), same reasoning as XStandardAI (see std_ai.cpp) - registered
// on its own in addition to each subclass's base/derived relation.
CEREAL_REGISTER_TYPE(XBasicModifier);

CEREAL_REGISTER_TYPE(XModWound);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XBasicModifier, XModWound);

CEREAL_REGISTER_TYPE(XModPoison);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XBasicModifier, XModPoison);

CEREAL_REGISTER_TYPE(XModConfuse);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XBasicModifier, XModConfuse);

CEREAL_REGISTER_TYPE(XModStun);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XBasicModifier, XModStun);

CEREAL_REGISTER_TYPE(XModHeroism);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XBasicModifier, XModHeroism);

CEREAL_REGISTER_TYPE(XModDisease);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XBasicModifier, XModDisease);

CEREAL_REGISTER_TYPE(XModWeak);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XBasicModifier, XModWeak);

CEREAL_REGISTER_TYPE(XModParalyse);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XBasicModifier, XModParalyse);

CEREAL_REGISTER_TYPE(XModDelayed);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XBasicModifier, XModDelayed);

CEREAL_REGISTER_TYPE(XModSeeInvisible);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XBasicModifier, XModSeeInvisible);

CEREAL_REGISTER_TYPE(XModBoostSpeed);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XBasicModifier, XModBoostSpeed);

CEREAL_REGISTER_TYPE(XModSlowness);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XBasicModifier, XModSlowness);

CEREAL_REGISTER_TYPE(XModAcidResistance);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XBasicModifier, XModAcidResistance);

CEREAL_REGISTER_TYPE(XModFireResistance);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XBasicModifier, XModFireResistance);

CEREAL_REGISTER_TYPE(XModColdResistance);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XBasicModifier, XModColdResistance);

CEREAL_REGISTER_TYPE(XModPoisonResistance);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XBasicModifier, XModPoisonResistance);

XBasicModifier::XBasicModifier(MODIFIER_TYPE mt, int _val, XCreature * _cr)
{
    mdt = mt;
    val = _val;
    setter = XCreature::ToWeakPtr(_cr);
}

MODIFIER_RESULT XBasicModifier::Run(XCreature * owner)
{
    if (owner->_HP <= 0) {
        owner->Die(setter.lock().get());
        return MR_DIE;
    } else {
        val--;
        return val > 0 ? MR_OK : MR_REMOVE;
    }
}

MODIFIER_RESULT XModWound::Run(XCreature * owner)
{
    val -= (owner->GetStats(XStats::TOU) / 10 + owner->sk->GetLevel(XSkill::Skill::FIRST_AID));
    owner->sk->UseSkill(XSkill::Skill::FIRST_AID);

    if (val > 0) {
        owner->_HP -= val;

        if (owner->isHero()) {
            msgwin.Add(ApplyMsg());
        }
    }

    return XBasicModifier::Run(owner);
}

MODIFIER_RESULT XModPoison::Run(XCreature * owner)
{
    if (vRand() % 3 == 0) {
        int rnd = vRand() % 4;
        owner->_HP -= rnd;

        if (owner->isHero()) {
            msgwin.Add(ApplyMsg());
        }
    }

    val -= owner->sk->GetLevel(XSkill::Skill::FIRST_AID);
    return XBasicModifier::Run(owner);
}

MODIFIER_RESULT XModConfuse::Run(XCreature * owner)
{
    owner->nx = owner->x + vRand() % 3 - 1;
    owner->ny = owner->y + vRand() % 3 - 1;

    if (owner->isHero()) {
        msgwin.Add(ApplyMsg());
    }

    return XBasicModifier::Run(owner);
}

int XModStun::onSet(XCreature * owner)
{
    owner->added_DV -= 5;
    owner->added_HIT -= 10;
    return 1;
}

int XModStun::onRemove(XCreature * owner)
{
    owner->added_DV += 5;
    owner->added_HIT += 10;
    return 1;
}

int XModHeroism::onSet(XCreature * owner)
{
    owner->added_DV += 5;
    owner->added_HIT += 10;
    return 1;
}

int XModHeroism::onRemove(XCreature * owner)
{
    owner->added_DV -= 5;
    owner->added_HIT -= 10;
    return 1;
}

int XModDisease::onSet(XCreature * owner)
{
    owner->added_DV -= 5;
    owner->added_HIT -= 5;
    owner->added_stats.Modify(XStats::STR, -3);
    owner->added_stats.Modify(XStats::DEX, -4);
    owner->added_stats.Modify(XStats::TOU, -3);
    return 1;
}

int XModDisease::onRemove(XCreature * owner)
{
    owner->added_DV += 5;
    owner->added_HIT += 5;
    owner->added_stats.Modify(XStats::STR, +3);
    owner->added_stats.Modify(XStats::DEX, +4);
    owner->added_stats.Modify(XStats::TOU, +3);
    return 1;
}

MODIFIER_RESULT XModDisease::Run(XCreature * owner)
{
    switch (vRand(300)) {
        case 0:
            owner->GainAttr(XStats::STR, -1);
            break;

        case 1:
            owner->GainAttr(XStats::DEX, -1);
            break;

        case 2:
            owner->GainAttr(XStats::TOU, -1);
            break;
    }

    return XBasicModifier::Run(owner);
}

int XModWeak::onSet(XCreature * owner)
{
    owner->added_stats.Modify(XStats::STR, -5);
    return 1;
}

int XModWeak::onRemove(XCreature * owner)
{
    owner->added_stats.Modify(XStats::STR, +5);
    return 1;
}

MODIFIER_RESULT XModWeak::Run(XCreature * owner)
{
    switch (vRand(100)) {
        case 0:
            owner->GainAttr(XStats::STR, -1);
            break;
    }

    return XBasicModifier::Run(owner);
}

MODIFIER_RESULT XModParalyse::Run(XCreature * owner)
{
    owner->nx = owner->x;
    owner->ny = owner->y;
    return XBasicModifier::Run(owner);
}

MODIFIER_RESULT XModDelayed::Run(XCreature * owner)
{
    MODIFIER_RESULT mr = XBasicModifier::Run(owner);

    if (mr == MR_REMOVE) {
        owner->md->Add(set_mt, set_val, owner);
    }

    return mr;
}

int XModSeeInvisible::onSet(XCreature * owner)
{
    owner->added_resists.ChangeResistance(R_SEE_INVISIBLE, 10);
    return 1;
}

int XModSeeInvisible::onRemove(XCreature * owner)
{
    owner->added_resists.ChangeResistance(R_SEE_INVISIBLE, -10);
    return 1;
}

int XModBoostSpeed::onSet(XCreature * owner)
{
    owner->ttmb -= 300;
    return 1;
}

int XModBoostSpeed::onRemove(XCreature * owner)
{
    owner->ttmb += 300;
    return 1;
}

int XModSlowness::onSet(XCreature * owner)
{
    owner->ttmb += 300;
    return 1;
}

int XModSlowness::onRemove(XCreature * owner)
{
    owner->ttmb -= 300;
    return 1;
}

int XModAcidResistance::onSet(XCreature * owner)
{
    owner->added_resists.ChangeResistance(R_ACID, 40);
    return 1;
}

int XModAcidResistance::onRemove(XCreature * owner)
{
    owner->added_resists.ChangeResistance(R_ACID, -40);
    return 1;
}

int XModFireResistance::onSet(XCreature * owner)
{
    owner->added_resists.ChangeResistance(R_FIRE, 40);
    return 1;
}

int XModFireResistance::onRemove(XCreature * owner)
{
    owner->added_resists.ChangeResistance(R_FIRE, -40);
    return 1;
}

int XModColdResistance::onSet(XCreature * owner)
{
    owner->added_resists.ChangeResistance(R_COLD, 40);
    return 1;
}

int XModColdResistance::onRemove(XCreature * owner)
{
    owner->added_resists.ChangeResistance(R_COLD, -40);
    return 1;
}

int XModPoisonResistance::onSet(XCreature * owner)
{
    owner->added_resists.ChangeResistance(R_POISON, 40);
    return 1;
}

int XModPoisonResistance::onRemove(XCreature * owner)
{
    owner->added_resists.ChangeResistance(R_POISON, -40);
    return 1;
}
