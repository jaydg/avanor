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

int XModifier::Add(MODIFIER_TYPE mt, int val, XCreature* owner, XCreature* cr)
{
    if (val > 0) {
        std::unique_ptr<XBasicModifier> xbm;

        switch (mt) {
            case MOD_WOUND :
                xbm = std::make_unique<XModWound>(val, cr);
                break;

            case MOD_POISON :
                val = owner->onMagicDamage(val, XResistance::POISON);

                if (val > 0) {
                    xbm = std::make_unique<XModPoison>(val, cr);
                } else {
                    return 0;
                }

                break;

            case MOD_CONFUSE :
                val = owner->onMagicDamage(val, XResistance::CONFUSE);

                if (val > 0) {
                    xbm = std::make_unique<XModConfuse>(val, cr);
                } else {
                    return 0;
                }

                break;

            case MOD_STUN :
                val = owner->onMagicDamage(val, XResistance::STUN);

                if (val > 0) {
                    xbm = std::make_unique<XModStun>(val, cr);
                } else {
                    return 0;
                }

                break;

            case MOD_HEROISM :
                xbm = std::make_unique<XModHeroism>(val, cr);
                break;

            case MOD_DISEASE :
                xbm = std::make_unique<XModDisease>(val, cr);
                break;

            case MOD_PARALYSE :
                xbm = std::make_unique<XModParalyse>(val, cr);
                break;

            case MOD_WEAK :
                xbm = std::make_unique<XModWeak>(val, cr);
                break;

            case MOD_SEE_INVISIBLE :
                xbm = std::make_unique<XModSeeInvisible>(val, cr);
                break;

            case MOD_BOOST_SPEED :
                xbm = std::make_unique<XModBoostSpeed>(val, cr);
                break;

            case MOD_SLOWNESS :
                xbm = std::make_unique<XModSlowness>(val, cr);
                break;

            case MOD_ACID_RESISTANCE :
                xbm = std::make_unique<XModAcidResistance>(val, cr);
                break;

            case MOD_FIRE_RESISTANCE :
                xbm = std::make_unique<XModFireResistance>(val, cr);
                break;

            case MOD_COLD_RESISTANCE :
                xbm = std::make_unique<XModColdResistance>(val, cr);
                break;

            case MOD_POISON_RESISTANCE :
                xbm = std::make_unique<XModPoisonResistance>(val, cr);
                break;

            default :
                assert(0);
        };

        return Add(std::move(xbm), owner);
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

int XModifier::Add(std::unique_ptr<XBasicModifier> mod, XCreature* owner)
{
    for (const auto& existing : ml)
    {
        if (existing->Compare(mod.get()) == 0)
        {
            if (owner->isHero())
                msgwin.Add(mod->ChangeMsg(mod->val));

            existing->Concat(mod.get());

            return 1;
        }
    }

    if (owner->isHero())
        msgwin.Add(mod->SetMsg());

    mod->onSet(owner);
    ml.push_back(std::move(mod));

    return 1;
}

int XModifier::Remove(const MODIFIER_TYPE mdt, XCreature* owner)
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

    return 1;
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
    for (auto it = ml.begin(); it != ml.end(); )
    {
        auto& mfr = *it;
        MODIFIER_RESULT mr = mfr->Run(cr);

        if (mr == MR_REMOVE) {
            mfr->onRemove(cr);
            it = ml.erase(it);
        } else if (mr == MR_OK) {
            ++it;
        } else {
            return 0;
        }
    }

    return 1;
}

// warning! this function return val
int XModifier::Get(const MODIFIER_TYPE mt) const
{
    int val = 0;

    for (const auto& mfr : ml)
    {
        if (mfr->mdt == mt)
            val += mfr->val;
    }

    return val;
}

