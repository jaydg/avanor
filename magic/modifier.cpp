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

bool IsKnownModifier(const MODIFIER& mt)
{
    return FindModifier(mt) != nullptr;
}

int XModifier::Add(const MODIFIER& mt, int val, XCreature* owner, XCreature* cr,
    const int delay)
{
    if (val > 0) {
        // Not on the creature yet, only on its way: parked as a plain
        // modifier holding the id and the amount, and handed back to this
        // same function when its time comes. Nothing is resisted, said or
        // applied until then - the meal is over long before the poison is.
        if (delay > 0) {
            XBasicModifier pending(mt, val, cr);
            pending.delay = delay;
            Add(pending, owner);

            return 1;
        }

        const ModifierStats* row = FindModifier(mt);

        // A name nothing declares - a typo in world/, where modifiers are
        // named by hand. Say so and lay nothing on, rather than asserting
        // in the middle of somebody's game.
        if (!row) {
            std::cerr << "world: nothing defines a modifier '" << mt << "'" << std::endl;

            return 0;
        }

        // What the body can shrug off, it does: a resisted modifier lands
        // shortened, or not at all.
        if (!row->resisted_by.empty()) {
            val = owner->onMagicDamage(val, row->resisted_by);

            if (val <= 0) {
                return 0;
            }
        }

        // A modifier the creature did not have yet always goes on.
        Add(XBasicModifier(mt, val * row->scale, cr), owner);

        return 1;
    } else {
        int flag = 0;

        for (auto it = ml.begin(); it != ml.end(); )
        {
            auto& mfr = *it;

            if (mfr.mdt == mt) {
                if (mfr.val + val > 0) {
                    if (owner->isHero())
                        msgwin.Add(mfr.ChangeMsg(val));

                    mfr.val += val;
                    return 1;
                } else {
                    int tmp = mfr.val + val;
                    mfr.val = 0;
                    val = tmp;

                    if (owner->isHero())
                        msgwin.Add(mfr.RemoveMsg());

                    mfr.onRemove(owner);
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

void XModifier::Add(XBasicModifier mod, XCreature* owner)
{
    const bool pending = mod.delay > 0;

    for (auto& existing : ml)
    {
        if (existing.Compare(&mod) == 0)
        {
            if (owner->isHero() && !pending)
                msgwin.Add(mod.ChangeMsg(mod.val));

            existing.Concat(&mod);

            return;
        }
    }

    if (pending) {
        ml.push_back(std::move(mod));

        return;
    }

    if (owner->isHero())
        msgwin.Add(mod.SetMsg());

    mod.onSet(owner);
    ml.push_back(std::move(mod));
}

void XModifier::Remove(const MODIFIER& mdt, XCreature* owner)
{
    bool first = true;

    for (auto it = ml.begin(); it != ml.end();) {
        auto& tmod = *it;

        if (tmod.mdt == mdt) {
            // A modifier still on its way has nothing to say and nothing
            // to undo - but it does go, which is why being cured of poison
            // now also clears the poison that has not bitten yet.
            if (first && tmod.delay <= 0) {
                if (owner->isHero())
                    msgwin.Add(tmod.RemoveMsg());

                tmod.onRemove(owner);
                first = false;
            }

            // gone for real
            it = ml.erase(it);
        } else {
            ++it;
        }
    }
}

int XModifier::Run(XCreature* cr)
{
    // Running a modifier can add another one to this same list: one laid on
    // with a delay applies itself the moment its wait runs out, and Add()
    // pushes onto ml. That reallocates the vector, and any iterator or
    // reference held across the call is left pointing into the freed buffer
    // - which is what an eaten corpse's delayed poison used to crash on,
    // roughly a hundred turns after the meal.
    //
    // So walk by index and read the element back out of the vector each time
    // rather than holding on to it. Only the modifiers that were already here
    // are run: one that arrives partway through starts counting down on the
    // next turn instead of the one that set it.
    for (size_t i = 0, count = ml.size(); i < count && i < ml.size(); ) {
        // Still on its way: count the turn off and let it be. When the
        // wait is over it goes through Add() as if it were being laid on
        // this moment, which is where resistance, merging with one already
        // there, and the announcement all live.
        if (ml[i].delay > 0) {
            if (--ml[i].delay > 0) {
                i++;
                continue;
            }

            const MODIFIER mt = ml[i].mdt;
            const int val = ml[i].val;
            XCreature* setter = ml[i].setter.lock().get();

            ml.erase(ml.begin() + static_cast<std::ptrdiff_t>(i));
            count--;
            Add(mt, val, cr, setter);

            continue;
        }

        const MODIFIER_RESULT mr = ml[i].Run(cr);

        if (mr == MR_REMOVE) {
            ml[i].onRemove(cr);
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

std::string XModifier::toString() const
{
    // What the creature is carrying, in the order world/modifiers.lua
    // declares them - so a modifier that says nothing shows nothing, and
    // a new one content adds needs no line here. The seven this used to
    // name by hand left heroism, weakness, slowness, quickening and every
    // resistance off the status line entirely.
    std::string res;

    for (const auto& row : AllModifiers()) {
        if (row.names.empty()) {
            continue;
        }

        if (const int val = Get(row.id); val > 0) {
            const std::string name = XBasicModifier(row.id, 0, nullptr).GetDisplayName(val);

            if (!name.empty()) {
                res += name + " ";
            }
        }
    }

    return res;
}

int XModifier::Get(const MODIFIER& mt) const
{
    int val = 0;

    for (const auto& mfr : ml)
    {
        // One still on its way is not on the creature: a poison that has
        // not bitten yet neither shows on the status line nor stops the
        // hero from running.
        if (mfr.mdt == mt && mfr.delay <= 0)
            val += mfr.val;
    }

    return val;
}

