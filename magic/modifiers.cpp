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

#include "helpers/msgwin.h"
#include "magic/modifier.h"
#include "magic/modifiers.h"

XBasicModifier::XBasicModifier(MODIFIER_TYPE mt, int _val, XCreature * _cr)
{
    mdt = mt;
    val = _val;
    setter = _cr;
    im = IM_OTHER;
}

void XBasicModifier::Invalidate()
{
    setter = NULL;
    XObject::Invalidate();
}

void XBasicModifier::Store(XFile * f)
{
    XObject::Store(f);
    f->Write(&val, sizeof(int));
    f->Write(&mdt, sizeof(MODIFIER_TYPE));
    setter.Store(f);
}

void XBasicModifier::Restore(XFile * f)
{
    XObject::Restore(f);
    f->Read(&val, sizeof(int));
    f->Read(&mdt, sizeof(MODIFIER_TYPE));
    setter.Restore(f);
}

REGISTER_CLASS(XModWound);

MODIFIER_RESULT XModWound::Run(XCreature * owner)
{
    val -= (owner->GetStats(S_TOU) / 10 + owner->sk->GetLevel(XSkill::Skill::FIRST_AID));
    owner->sk->UseSkill(XSkill::Skill::FIRST_AID);

    if (val > 0) {
        owner->_HP -= val;

        if (owner->isHero()) {
            msgwin.Add(ApplyMsg());
        }
    }

    return XBasicModifier::Run(owner);
}

REGISTER_CLASS(XModPoison);
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

REGISTER_CLASS(XModConfuse);
MODIFIER_RESULT XModConfuse::Run(XCreature * owner)
{
    owner->nx = owner->x + vRand() % 3 - 1;
    owner->ny = owner->y + vRand() % 3 - 1;

    if (owner->isHero()) {
        msgwin.Add(ApplyMsg());
    }

    return XBasicModifier::Run(owner);
}

REGISTER_CLASS(XModStun);
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

REGISTER_CLASS(XModHeroism);
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

REGISTER_CLASS(XModDisease);
int XModDisease::onSet(XCreature * owner)
{
    owner->added_DV -= 5;
    owner->added_HIT -= 5;
    owner->added_stats.Modify(S_STR, -3);
    owner->added_stats.Modify(S_DEX, -4);
    owner->added_stats.Modify(S_TOU, -3);
    return 1;
}

int XModDisease::onRemove(XCreature * owner)
{
    owner->added_DV += 5;
    owner->added_HIT += 5;
    owner->added_stats.Modify(S_STR, +3);
    owner->added_stats.Modify(S_DEX, +4);
    owner->added_stats.Modify(S_TOU, +3);
    return 1;
}

MODIFIER_RESULT XModDisease::Run(XCreature * owner)
{
    switch (vRand(300)) {
        case 0:
            owner->GainAttr(S_STR, -1);
            break;

        case 1:
            owner->GainAttr(S_DEX, -1);
            break;

        case 2:
            owner->GainAttr(S_TOU, -1);
            break;
    }

    return XBasicModifier::Run(owner);
}

REGISTER_CLASS(XModWeak);
int XModWeak::onSet(XCreature * owner)
{
    owner->added_stats.Modify(S_STR, -5);
    return 1;
}

int XModWeak::onRemove(XCreature * owner)
{
    owner->added_stats.Modify(S_STR, +5);
    return 1;
}

MODIFIER_RESULT XModWeak::Run(XCreature * owner)
{
    switch (vRand(100)) {
        case 0:
            owner->GainAttr(S_STR, -1);
            break;
    }

    return XBasicModifier::Run(owner);
}

REGISTER_CLASS(XModParalyse);
MODIFIER_RESULT XModParalyse::Run(XCreature * owner)
{
    owner->nx = owner->x;
    owner->ny = owner->y;
    return XBasicModifier::Run(owner);
}


REGISTER_CLASS(XModDelayed);
MODIFIER_RESULT XModDelayed::Run(XCreature * owner)
{
    MODIFIER_RESULT mr = XBasicModifier::Run(owner);

    if (mr == MR_REMOVE) {
        owner->md->Add(set_mt, set_val, owner);
    }

    return mr;
}

void XModDelayed::Store(XFile * f)
{
    XBasicModifier::Store(f);
    f->Write(&set_mt, sizeof(MODIFIER_TYPE));
    f->Write(&set_val, sizeof(int));
}

void XModDelayed::Restore(XFile * f)
{
    XBasicModifier::Restore(f);
    f->Read(&set_mt, sizeof(MODIFIER_TYPE));
    f->Read(&set_val, sizeof(int));
}


REGISTER_CLASS(XModSeeInvisible);
int XModSeeInvisible::onSet(XCreature * owner)
{
    owner->added_resists.ChangeResistance(R_SEEINVISIBLE, 10);
    return 1;
}

int XModSeeInvisible::onRemove(XCreature * owner)
{
    owner->added_resists.ChangeResistance(R_SEEINVISIBLE, -10);
    return 1;
}

REGISTER_CLASS(XModBoostSpeed);
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

REGISTER_CLASS(XModSlowness);
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

REGISTER_CLASS(XModAcidResistance);
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

REGISTER_CLASS(XModFireResistance);
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

REGISTER_CLASS(XModColdResistance);
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

REGISTER_CLASS(XModPoisonResistance);
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
