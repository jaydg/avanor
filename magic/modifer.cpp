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

#include "modifer.h"
#include "modifers.h"

XModifer::XModifer()
{
}

XModifer::~XModifer()
{
    ml.KillAll();
}

XModifer::XModifer(XModifer * m)
{
    assert(m);
    assert(0);
}

int XModifer::Add(MODIFER_TYPE mt, int _val, XCreature * owner, XCreature * _cr)
{
    if (_val > 0) {
        XBasicModifer * xbm;

        switch (mt) {
            case MOD_WOUND :
                xbm = new XModWound(_val, _cr);
                break;

            case MOD_POISON :
                _val = owner->onMagicDamage(_val, R_POISON);

                if (_val > 0) {
                    xbm = new XModPoison(_val, _cr);
                } else {
                    return 0;
                }

                break;

            case MOD_CONFUSE :
                _val = owner->onMagicDamage(_val, R_CONFUSE);

                if (_val > 0) {
                    xbm = new XModConfuse(_val, _cr);
                } else {
                    return 0;
                }

                break;

            case MOD_STUN :
                _val = owner->onMagicDamage(_val, R_STUN);

                if (_val > 0) {
                    xbm = new XModStun(_val, _cr);
                } else {
                    return 0;
                }

                break;

            case MOD_HEROISM :
                xbm = new XModHeroism(_val, _cr);
                break;

            case MOD_DISEASE :
                xbm = new XModDisease(_val, _cr);
                break;

            case MOD_PARALYSE :
                xbm = new XModParalyse(_val, _cr);
                break;

            case MOD_WEAK :
                xbm = new XModWeak(_val, _cr);
                break;

            case MOD_SEE_INVISIBLE :
                xbm = new XModSeeInvisible(_val, _cr);
                break;

            case MOD_BOOST_SPEED :
                xbm = new XModBoostSpeed(_val, _cr);
                break;

            case MOD_SLOWNESS :
                xbm = new XModSlowness(_val, _cr);
                break;

            case MOD_ACID_RESISTANCE :
                xbm = new XModAcidResistance(_val, _cr);
                break;

            case MOD_FIRE_RESISTANCE :
                xbm = new XModFireResistance(_val, _cr);
                break;

            case MOD_COLD_RESISTANCE :
                xbm = new XModColdResistance(_val, _cr);
                break;

            case MOD_POISON_RESISTANCE :
                xbm = new XModPoisonResistance(_val, _cr);
                break;

            default :
                assert(0);
        };

        Add(xbm, owner);

        return 1;
    } else {
        XList<XBasicModifer*>::iterator mfr = ml.begin();
        int flag = 0;

        while (mfr != ml.end()) {
            if (mfr->mdt == mt)
                if (mfr->val + _val > 0) {
                    if (owner->isHero()) {
                        msgwin.Add(mfr->ChangeMsg(_val));
                    }

                    mfr->val += _val; //_val always negative
                    return 1;
                } else {
                    int tmp = mfr->val + _val;
                    mfr->val = 0;
                    _val = tmp;

                    if (owner->isHero()) {
                        if (_val > 0) {
                            msgwin.Add(mfr->ChangeMsg(_val));
                        } else {
                            msgwin.Add(mfr->RemoveMsg());
                        }
                    }

                    flag = 1;
                    mfr->onRemove(owner);
                    XBasicModifer * xtmp = mfr;
                    mfr = ml.erase(mfr);
                    xtmp->Invalidate();
                    continue;
                }

            mfr++;
        }

        return flag;
    }
}

int XModifer::Add(XBasicModifer * mod, XCreature * owner)
{
    int flag = 1;

    for (XList<XBasicModifer*>::iterator it = ml.begin(); it != ml.end(); it++) {
        XBasicModifer * tmod = static_cast<XBasicModifer*>(static_cast<XObject*>(it));

        if (tmod->Compare(mod) == 0) {
            if (owner->isHero()) {
                msgwin.Add(mod->ChangeMsg(mod->val));
            }

            tmod->Concat(mod);
            flag = 0;
            break;
        }
    }

    if (flag) {
        if (owner->isHero()) {
            msgwin.Add(mod->SetMsg());
        }

        mod->onSet(owner);
        ml.Add(mod);
    }

    return 1;
}

int XModifer::Remove(MODIFER_TYPE mdt, XCreature * owner)
{
    int flag = 1;
    XList<XBasicModifer*>::iterator it = ml.begin();

    while (it != ml.end()) {
        XBasicModifer * tmod = static_cast<XBasicModifer*>(static_cast<XObject*>(it));
        it++;

        if (tmod->mdt == mdt) {
            if (flag) {
                if (owner->isHero()) {
                    msgwin.Add(tmod->RemoveMsg());
                }

                tmod->onRemove(owner);
                flag = 0;
            }

            tmod->Invalidate();
        }
    }

    return 1;
}

void XModifer::toString(char* buf)
{
    strcpy(buf, "");

    int xval = Get(MOD_WOUND);

    if (xval > 0) {
        XModWound * tmp = new XModWound(0, NULL);
        strcat(buf, tmp->GetDisplayName(xval));
        strcat(buf, " ");
        tmp->Invalidate();
    }

    if (Get(MOD_POISON) > 0) {
        XModPoison * tmp = new XModPoison(0, NULL);
        strcat(buf, tmp->GetDisplayName(1));
        strcat(buf, " ");
        tmp->Invalidate();
    }

    xval = Get(MOD_STUN);

    if (xval > 0) {
        XModStun * tmp = new XModStun(0, NULL);
        strcat(buf, tmp->GetDisplayName(xval));
        strcat(buf, " ");
        tmp->Invalidate();
    }

    if (Get(MOD_CONFUSE) > 0) {
        XModConfuse * tmp = new XModConfuse(0, NULL);
        strcat(buf, tmp->GetDisplayName(1));
        strcat(buf, " ");
        tmp->Invalidate();
    }

    if (Get(MOD_DISEASE) > 0) {
        XModDisease * tmp = new XModDisease(0, NULL);
        strcat(buf, tmp->GetDisplayName(1));
        strcat(buf, " ");
        tmp->Invalidate();
    }

    if (Get(MOD_PARALYSE) > 0) {
        XModParalyse * tmp = new XModParalyse(0, NULL);
        strcat(buf, tmp->GetDisplayName(1));
        strcat(buf, " ");
        tmp->Invalidate();
    }

    if (Get(MOD_SEE_INVISIBLE) > 0) {
        XModSeeInvisible * tmp = new XModSeeInvisible(0, NULL);
        strcat(buf, tmp->GetDisplayName(1));
        strcat(buf, " ");
        tmp->Invalidate();
    }
}

int XModifer::Run(XCreature * cr)
{
    XList<XBasicModifer*>::iterator mfr = ml.begin();

    while (mfr != ml.end()) {
        MODIFER_RESULT mr = mfr->Run(cr);

        if (mr == MR_REMOVE) {
            mfr->onRemove(cr);
            XBasicModifer * tmp = mfr;
            mfr = ml.erase(mfr);
            tmp->Invalidate();
            continue;
        } else if (mr == MR_OK) {
            mfr++;
        } else {
            return 0;
        }
    }

    return 1;
}

// warning! this function return val
int XModifer::Get(MODIFER_TYPE mt)
{
    XList<XBasicModifer*>::iterator mfr = ml.begin();
    int val = 0;

    while (mfr != ml.end()) {
        if (mfr->mdt == mt) {
            val += mfr->val;
        }

        mfr++;
    }

    return val;
}

void XModifer::Store(XFile * f)
{
    ml.StoreList(f);
}

void XModifer::Restore(XFile * f, XCreature * owner)
{
    assert(ml.empty());
    ml.RestoreList(f);
}
