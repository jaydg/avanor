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
#include <algorithm>
#include <iostream>
#include "magic/modifier.h"
#include "magic/modifiers.h"

namespace {

// Which class carries each modifier, and which resistance - if any - lets
// the target shrug part of it off. Everything else about a modifier is
// already content's business, or about to be.
template<class T>
std::unique_ptr<XBasicModifier> MakeModifier(int val, XCreature* cr)
{
    return std::make_unique<T>(val, cr);
}

struct ModifierKind {
    const char* id;
    const char* resisted_by;
    std::unique_ptr<XBasicModifier> (*make)(int val, XCreature* cr);
};

const ModifierKind modifier_kinds[] = {
    {MOD_WOUND,         "",          &MakeModifier<XModWound>},
    {MOD_POISON,        RS_POISON,   &MakeModifier<XModPoison>},
    {MOD_CONFUSE,       RS_CONFUSE,  &MakeModifier<XModConfuse>},
    {MOD_STUN,          RS_STUN,     &MakeModifier<XModStun>},
    {MOD_HEROISM,       "",          &MakeModifier<XModHeroism>},
    {MOD_DISEASE,       "",          &MakeModifier<XModDisease>},
    {MOD_PARALYSE,      "",          &MakeModifier<XModParalyse>},
    {MOD_WEAK,          "",          &MakeModifier<XModWeak>},
    {MOD_SEE_INVISIBLE, "",          &MakeModifier<XModSeeInvisible>},
    {MOD_BOOST_SPEED,   "",          &MakeModifier<XModBoostSpeed>},
    {MOD_SLOWNESS,      "",          &MakeModifier<XModSlowness>},
};

}

bool IsKnownModifier(const MODIFIER& mt)
{
    // The two the engine builds for itself rather than through the table:
    // a resistance needs to be told which resistance, and a delayed
    // modifier is a wrapper around one of the others.
    if (mt == MOD_RESISTANCE || mt == MOD_DELAYED) {
        return true;
    }

    return std::any_of(std::begin(modifier_kinds), std::end(modifier_kinds),
        [&mt](const ModifierKind& k) { return mt == k.id; });
}

int XModifier::Add(const MODIFIER& mt, int val, XCreature* owner, XCreature* cr)
{
    if (val > 0) {
        // Needs to be told which resistance it grants, and this signature
        // has no room to say. Reached through AddResistance() in Lua, or
        // by building one and handing it to the XBasicModifier overload
        // of Add().
        if (mt == MOD_RESISTANCE) {
            std::cerr << "a resistance modifier was asked for without"
                         " saying which resistance - use AddResistance()"
                      << std::endl;

            return 0;
        }

        const auto* kind = std::find_if(std::begin(modifier_kinds), std::end(modifier_kinds),
            [&mt](const ModifierKind& k) { return mt == k.id; });

        // A name nothing knows - a typo in world/, where modifiers are
        // named by hand. Say so and lay nothing on, rather than asserting
        // in the middle of somebody's game.
        if (kind == std::end(modifier_kinds)) {
            std::cerr << "world: nothing defines a modifier '" << mt << "'" << std::endl;

            return 0;
        }

        // What the body can shrug off, it does: a resisted modifier lands
        // shortened, or not at all.
        if (*kind->resisted_by) {
            val = owner->onMagicDamage(val, kind->resisted_by);

            if (val <= 0) {
                return 0;
            }
        }

        std::unique_ptr<XBasicModifier> xbm = kind->make(val, cr);

        // A modifier the creature did not have yet always goes on.
        Add(std::move(xbm), owner);

        return 1;
    } else {
        int flag = 0;

        for (auto it = ml.begin(); it != ml.end(); )
        {
            auto& mfr = *it;

            if (mfr->mdt == mt) {
                if (mfr->val + val > 0) {
                    if (owner->isHero())
                        msgwin.Add(mfr->ChangeMsg(val));

                    mfr->val += val;
                    return 1;
                } else {
                    int tmp = mfr->val + val;
                    mfr->val = 0;
                    val = tmp;

                    if (owner->isHero())
                        msgwin.Add(mfr->RemoveMsg());

                    mfr->onRemove(owner);
                    it = ml.erase(it);
                    flag = 1;

                    continue;
                }
            }

            ++it;
        }

        return flag;
    }
}

void XModifier::Add(std::unique_ptr<XBasicModifier> mod, XCreature* owner)
{
    for (const auto& existing : ml)
    {
        if (existing->Compare(mod.get()) == 0)
        {
            if (owner->isHero())
                msgwin.Add(mod->ChangeMsg(mod->val));

            existing->Concat(mod.get());

            return;
        }
    }

    if (owner->isHero())
        msgwin.Add(mod->SetMsg());

    mod->onSet(owner);
    ml.push_back(std::move(mod));
}

void XModifier::Remove(const MODIFIER& mdt, XCreature* owner)
{
    bool first = true;

    for (auto it = ml.begin(); it != ml.end();) {
        auto& tmod = *it;

        if (tmod->mdt == mdt) {
            if (first) {
                if (owner->isHero())
                    msgwin.Add(tmod->RemoveMsg());

                tmod->onRemove(owner);
                first = false;
            }

            // gone for real
            it = ml.erase(it);
        } else {
            ++it;
        }
    }
}

std::string XModifier::toString() const
{
    // Every active status contributes to the string, in a fixed order.
    std::string res;

    // The wording lives in each modifier subclass, so a throwaway
    // instance is what gives access to it.
    const auto append = [&res](const XBasicModifier& mod, int val) {
        res += mod.GetDisplayName(val) + " ";
    };

    if (const int val = Get(MOD_WOUND); val > 0) {
        append(XModWound(0, nullptr), val);
    }

    if (const int val = Get(MOD_POISON); val > 0) {
        append(XModPoison(0, nullptr), val);
    }

    if (const int val = Get(MOD_STUN); val > 0) {
        append(XModStun(0, nullptr), val);
    }

    if (const int val = Get(MOD_CONFUSE); val > 0) {
        append(XModConfuse(0, nullptr), val);
    }

    if (const int val = Get(MOD_DISEASE); val > 0) {
        append(XModDisease(0, nullptr), val);
    }

    if (const int val = Get(MOD_PARALYSE); val > 0) {
        append(XModParalyse(0, nullptr), val);
    }

    if (const int val = Get(MOD_SEE_INVISIBLE); val > 0) {
        append(XModSeeInvisible(0, nullptr), val);
    }

    return res;
}

int XModifier::Run(XCreature* cr)
{
    // Running a modifier can add another one to this same list: a delayed
    // effect applies itself the moment its timer runs out, and Add() pushes
    // onto ml. That reallocates the vector, and any iterator or reference
    // held across the call is left pointing into the freed buffer - which is
    // what an eaten corpse's delayed poison used to crash on, roughly a
    // hundred turns after the meal.
    //
    // So walk by index and read the element back out of the vector each time
    // rather than holding on to it. Only the modifiers that were already here
    // are run: one that arrives partway through starts counting down on the
    // next turn instead of the one that set it.
    for (size_t i = 0, count = ml.size(); i < count && i < ml.size(); ) {
        const MODIFIER_RESULT mr = ml[i]->Run(cr);

        if (mr == MR_REMOVE) {
            ml[i]->onRemove(cr);
            ml.erase(ml.begin() + static_cast<std::ptrdiff_t>(i));
            count--;
        } else if (mr == MR_OK) {
            i++;
        } else {
            return 0;
        }
    }

    return 1;
}

// warning! this function return val
int XModifier::Get(const MODIFIER& mt) const
{
    int val = 0;

    for (const auto& mfr : ml)
    {
        if (mfr->mdt == mt)
            val += mfr->val;
    }

    return val;
}

