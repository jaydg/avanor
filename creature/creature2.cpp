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

#include "creature.h"
#include "modifer.h"
#include "xapi.h"

void XCreature::Attack()
{
    assert(isValid());
    XCreature * target = l->map->GetMonster(nx, ny);
    assert(target);

    XItem * it1 = GetItem(BP_HAND, 0);
    XItem * it2 = GetItem(BP_HAND, 1);
    int flag = 0;
    int ttm_res = 0;

    if (it1 && it1->im & IM_WEAPON) {
        ttm_res += MeleeAttack(target, it1);
        flag++;
    }

    if (it2 && it2->im & IM_WEAPON && target->isValid()) {
        ttm_res += MeleeAttack(target, it2);
        flag++;
    }

    if (flag == 0) {
        ttm_res += MeleeAttack(target, NULL);
    }
}

int XCreature::MeleeAttack(XCreature * target, XItem * weapon)
{
    assert(isValid());

    //need for backstub.
    int wasEnemy = target->xai->isEnemy(this);

    bool vis1 = isVisible();
    bool vis2 = target->isVisible();

    int res = 0;
    int tohit;
    int tdam;
    unsigned int brt;

    if (weapon) {
        res += (wsk->GetUseTime(weapon->wt) * GetSpeed()) / 1000;
        tohit = GetHIT() + wsk->GetHIT(weapon->wt) + GetHITFHBonus(weapon);
        tdam = weapon->dice.Throw() + wsk->GetDMG(weapon->wt) + GetDMGFHBonus(weapon) + GetDMG();
        brt = weapon->brt;
    } else {
        res += (wsk->GetUseTime(WSK_UNARMED) * GetSpeed()) / 1000;
        tohit = GetHIT() + wsk->GetHIT(WSK_UNARMED);
        tdam = dice.Throw() + GetDMG() + wsk->GetDMG(WSK_UNARMED);
        brt = BR_NONE;

        for (XQList<MELEE_ATTACK>::iterator tit = melee_attack->begin(); tit != melee_attack->end(); tit++) {
            if (vRand(100) < (*tit).prob) {
                brt = brt | (*tit).br_attack;
            }
        }
    }

    DAMAGE_DATA_EX dd;
    dd.damage	= tdam;
    dd.attacker	= this;
    dd.attack_name	= NULL;
    dd.attack_HIT	= tohit;
    dd.attack_brand	= (BRAND_TYPE)brt;
    dd.flags	= DF_MAGIC_BOLT;
    dd.weapon	= weapon;

    if (target->InflictDamage(&dd)) {
        if (weapon) {
            wsk->UseSkill(weapon->wt);
        } else {
            wsk->UseSkill(WSK_UNARMED);
        }
    }

    /*			//backstab
    			int backstab = 0;
    			if (weapon && (!target->isCreatureVisible(this) || !wasEnemy))
    			{
    				if (vRand(100) < sk->GetLevel(SKT_BACKSTABBING) * 5 + 5)
    				{
    					backstab = 1;
    					tdam *= 3;
    					sk->UseSkill(SKT_BACKSTABBING, 3);
    				}
    			}
    */
    return res;
}

int XCreature::onMagicDamage(int dmg, RESISTANCE tr)
{
    assert(isValid());
    int damage = dmg - (dmg * GetResistance(tr)) / 100;
    return damage < 0 ? 0 : damage;
}

int XCreature::CauseEffect(int dmg, BRAND_TYPE brt, XCreature * attacker)
{
    int damage = 0;

    if (brt > BR_NONE) {
        if (brt & BR_FIRE) {
            damage += onMagicDamage(dmg, R_FIRE);
        }

        if (brt & BR_COLD) {
            damage += onMagicDamage(dmg, R_COLD);
        }

        if (brt & BR_ACID) {
            damage += onMagicDamage(dmg, R_ACID);
        }

        if (brt & BR_EARTH) {
            damage += onMagicDamage(dmg, R_EARTH);
        }

        if (brt & BR_LIGHTNING) {
            damage += onMagicDamage(dmg, R_AIR);
        }

        if (brt & BR_DEMONSLAYER && creature_class & CR_DEMON) {
            damage += dmg * 3;
        }

        if (brt & BR_ORCSLAYER && creature_class & CR_ORC) {
            damage += dmg * 3;
        }

        if (damage == 0) {
            return dmg;
        } else {
            return damage;
        }
    } else {
        return dmg;
    }
}

void XCreature::CausePostEffect(int dmg, BRAND_TYPE brt, XCreature * attacker)
{
    if (brt > BR_NONE) {
        if (brt & BR_POISON) {
            md->Add(MOD_POISON, dmg, this, attacker);
        }

        if (brt & BR_DISEASE) {
            md->Add(MOD_DISEASE, dmg, this, attacker);
        }

        if (brt & BR_PARALYSE) {
            md->Add(MOD_PARALYSE, dmg, this, attacker);
        }

        if (brt & BR_STUN) {
            md->Add(MOD_STUN, dmg, this, attacker);
        }

        if (brt & BR_CONFUSE) {
            md->Add(MOD_CONFUSE, dmg, this, attacker);
        }
    }

    //this should be rewrited once.
    if (vRand(1000) < 200) {
        md->Add(MOD_WOUND, dmg / 4, this, attacker);
    }

    if (vRand(1000) < 150) {
        md->Add(MOD_STUN, dmg / 4, this, attacker);
    }

    if (vRand(1000) < 50) {
        md->Add(MOD_CONFUSE, dmg / 4, this, attacker);
    }
}

MF_RESULT XCreature::MissileFlight(MF_DATA * mfd)
{
    XMap * tmap = mfd->l->map;
    int range = mfd->max_range;
    float fdx = float(mfd->ex - mfd->sx);
    float fdy = float(mfd->ey - mfd->sy);
    float xrng = (float)sqrt(fdx * fdx + fdy * fdy);
    float cos_alpha = 0, sin_alpha = 0;

    if (xrng > 0) {
        cos_alpha = fdx / xrng;
        sin_alpha = fdy / xrng;
    }

    float mx = (float)mfd->sx;
    float my = (float)mfd->sy;
    int epx;
    int epy;
    int fl_range = 0;

    bool self_flag = false;

    if (fdx == fdy && fdy == 0.0) {
        self_flag = true;
    }

    while ((range > 0 /*&& (fabs(mfd->ex - mx) >= 0.5f || fabs(mfd->ey - my) >= 0.5f)*/) || self_flag) {
        self_flag = false;
        range--;
        fl_range++;
        float nmx = mx + cos_alpha;
        float nmy = my + sin_alpha;
        epx = vRound(nmx);
        epy = vRound(nmy);

        if (tmap->GetMovability(epx, epy) >= MO_WALL) {
            epx = vRound(mx);
            epy = vRound(my);
            break;
        }

        if (tmap->GetVisible(epx, epy) && __animation_flag) {
            tmap->Put(main_creature);

            if (mfd->arrow_type == MFT_BALL) {
                tmap->PutChar(epx, epy, '*', mfd->arrow_color);
            } else {
                int ttx = vRound(nmx - mx);
                int tty = vRound(nmy - my);

                if (ttx * tty == 1) {
                    tmap->PutChar(epx, epy, '\\', mfd->arrow_color);
                } else if (ttx * tty == -1) {
                    tmap->PutChar(epx, epy, '/', mfd->arrow_color);
                } else if (ttx == 0) {
                    tmap->PutChar(epx, epy, '|', mfd->arrow_color);
                } else if (tty == 0) {
                    tmap->PutChar(epx, epy, '-', mfd->arrow_color);
                }
            }

            vRefresh();
            vDelay(__animation_flag);
        }

        mx = nmx;
        my = nmy;


        //if (fabs(mfd->ex - mx) < 0.5f && fabs(mfd->ey - my) < 0.5f)
        if (epx == mfd->ex && epy == mfd->ey) {
            XCreature * tgt = tmap->GetMonster(epx, epy);

            if (tgt) {
                int tdv = tgt->GetDV() + fl_range * tgt->GetDV() / 6;
                int tht = mfd->to_hit > 0 ? mfd->to_hit : 1;

                if (tdv < 0) {
                    tht -= tdv;
                    tdv = 0;
                }

                int p = (100 * tht + 1) / (tht + tdv + 1);

                if (vRand(100) <= p) {
                    mfd->pt.x = epx;
                    mfd->pt.y = epy;
                    return MF_HIT;
                }
            }
        }
    }

    mfd->pt.x = epx;
    mfd->pt.y = epy;
    return MF_AVOID;
}

int XCreature::continueUseItem()
{
    assert(isValid());
    XItem * tool = (XItem*)action_data.item.get();
    assert(tool->im & IM_TOOL);
    RESULT res = tool->onUse(UIS_CONTINUE, this);

    if (res != CONTINUE) {
        action_data.action = A_MOVE;
        action_data.item = NULL;
    }

    return 1;
}

int XCreature::UseItem(XItem * item)
{
    assert(isValid());
    RESULT res = item->onUse(UIS_BEGIN, this);

    if (res) {
        if (res == CONTINUE) {
            action_data.action = A_USETOOL;
            action_data.item = item;
            return 1;
        } else {
            return 1;
        }
    } else {
        return 0;
    }

    return 1;
}


void XCreature::Sacrifice(XItem * item)
{
    assert(isValid());
    religion.SacrificeItem(this, item, D_UNKNOWN);
}

int XCreature::isCreatureVisible(XCreature * cr)
{
    assert(isValid());

    if ((cr->GetResistance(R_INVISIBLE) <= GetResistance(R_SEEINVISIBLE)) || cr == this) {
        return 1;
    } else {
        return 0;
    }

}

bool XCreature::isVisible()
{
    assert(isValid());

    if (l->map->GetVisible(x, y)) {
        if (XCreature::main_creature->isCreatureVisible(this)) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

const char* XCreature::GetMeleeAttackMsg(XItem * weapon)
{
    if (weapon) {
        //very temporary solution
        //if we accept this feature we should add verb to the weapon table
        if (weapon->it == IT_SMALLAXE ||
            weapon->it == IT_WARAXE	||
            weapon->it == IT_BATTLEAXE ||
            weapon->it == IT_GREATAXE ||
            weapon->it == IT_ORCISHAXE) {
            return "hack";
        } else {
            return "hit";
        }
    } else {
        return "attack";
    }
}

int XCreature::InflictDamage(DAMAGE_DATA_EX * pData)
{
    //make creature hostle
    if (pData->attacker) {
        xai->onWasAttacked(pData->attacker);
    }

    //stop current action
    stopAction();

    bool vis1 = isVisible();
    bool vis2 = pData->attacker ? pData->attacker->isVisible() : vis1;

    //this attack can be avoided

    int v = 0; //v by default less then p (there will 100% hit)
    int p = 1;
    int todv = 1;

    if (pData->flags & DF_AFFECT_HIT) {
        //get creature overall DV
        todv = GetDV();
        int tohit = pData->attack_HIT;

        if (tohit < 0) {
            todv -= tohit; // a - (-b) == a + b;
            tohit = 0;
        }

        p = ((tohit + 1) * 20) / (tohit + todv + 1);
        v = vRand(20); /*0..19*/
    }

    if (v <= p) {
        //oh yeah. hited!

        //get random part of body
        XBodyPart * txbp = GetRNDBodyPart();

        //requested base damage...
        int dmg = pData->damage;

        //critical hit (hit into vital spot)
        bool critical_hit = false;
        bool ignore_armour = false;

        if (pData->attacker && vRand(100) < (2 + pData->attacker->sk->GetLevel(SKT_FINDWEAKNESS))) {
            pData->attacker->sk->UseSkill(SKT_FINDWEAKNESS);
            critical_hit = true;
            dmg *= 3;
        }

        //calculates suposed damage counting creature type and resistance
        //also Adds modifers such 'Poison'
        if (pData->attack_name) { //this is poor magic
            dmg = CauseEffect(dmg, (BRAND_TYPE)pData->attack_brand, pData->attacker);
        } else { //this is hit with weapon or unarmed hit(snakes beat for example)
            dmg += CauseEffect(dmg, (BRAND_TYPE)pData->attack_brand, pData->attacker);
        }

        //always count intrinsic PV
        dmg -= _PV;

        if (pData->flags & DF_AFFECT_PV) {
            //get initial pv of armour
            int xpv = 0;

            if (txbp && txbp->Item()) {
                xpv = txbp->Item()->_PV;
            }

            if (!pData->attacker || vRand(100) >= (2 + pData->attacker->sk->GetLevel(SKT_FINDWEAKNESS)) || xpv == 0) {

                //if it is a body, then we should count PV of a cloak.
                if (txbp && txbp->bp_uin == BP_BODY) {
                    XItem * xtmp = GetItem(BP_CLOAK);

                    if (xtmp) {
                        xpv += xtmp->_PV;
                    }
                }

                //some dmg absorbed by armour
                dmg -= xpv;
            } else {
                pData->attacker->sk->UseSkill(SKT_FINDWEAKNESS);
                ignore_armour = true;
            }
        }

        if (vis1 || vis2) {
            if (pData->attack_name) {
                msgwin.Add(pData->attack_name);

                if (critical_hit) {
                    msgwin.Add("exactly");
                }

                msgwin.Add("hits");
            } else {
                msgwin.Add(pData->attacker->GetNameEx(CRN_T1));

                if (critical_hit) {
                    msgwin.Add("exactly");
                }

                msgwin.Add(pData->attacker->GetVerb(GetMeleeAttackMsg(pData->weapon)));
            }

            if (ignore_armour) {
                msgwin.Add(GetNameEx(CRN_T1));

                if (isHero()) {
                    msgwin.Add("penetrating a piece of armour.");
                } else {
                    msgwin.Add("penetrating a piece of armour");
                }
            } else {
                if (isHero()) {
                    msgwin.AddLast(GetNameEx(CRN_T1));
                } else {
                    msgwin.Add(GetNameEx(CRN_T1));
                }
            }
        }

        if (dmg <= 0) {
            if (isHero()) {
                if (pData->attack_name) {
                    msgwin.Add(pData->attack_name);
                } else {
                    msgwin.Add(pData->attacker->GetNameEx(CRN_T1));
                }

                msgwin.Add("does not manage to harm you.");
            } else if (isVisible()) {
                msgwin.Add("but does not manage to harm");
                msgwin.AddLast(GetNameEx(CRN_T3));
            }
        } else {
            _HP -= dmg;

            if (_HP > 0) {
                if ((vis1 || vis2) && !isHero()) {
                    msgwin.Add("and");
                    msgwin.Add(GetWoundMsg(1));
                    msgwin.AddLast(GetNameEx(CRN_T3));
                }

                CausePostEffect(dmg, (BRAND_TYPE)pData->attack_brand, pData->attacker);
            } else {
                // and kill IT!!!
                if ((vis1 || vis2) && !isHero()) {
                    msgwin.Add("and");

                    if (creature_class & CR_UNDEAD) {
                        if (pData->attack_name) {
                            msgwin.Add("destroys");
                        } else {
                            msgwin.Add(pData->attacker->GetVerb("destroy"));
                        }
                    } else {
                        if (pData->attack_name) {
                            msgwin.Add("kills");
                        } else {
                            msgwin.Add(pData->attacker->GetVerb("kill"));
                        }
                    }

                    msgwin.AddLast(GetNameEx(CRN_T3));
                }

                Die(pData->attacker);
                cr_kiled++; //temporary counter special for statistic
            }
        }

        return 1;
    } else { // avoided!!!
        //first of all check if save was caused by shield
        int sb = GetShieldDVBonus();

        if (sb > 0 && vRand(todv) < sb) {
            if (vis1 || vis2) {
                if (pData->attack_name) {
                    //Kobold shaman cast firbolt.
                    //You deflect small ball of fire with your shield
                    msgwin.Add(GetNameEx(CRN_T1));
                    msgwin.Add(GetVerb("deflect"));
                    msgwin.Add(pData->attack_name);
                    msgwin.Add("with");
                    msgwin.Add(GetNameEx(CRN_T4));
                    msgwin.Add("shield.");
                } else {
                    //Kobold attacks you.
                    //You blok kobold with your shiled
                    msgwin.Add(pData->attacker->GetNameEx(CRN_T1));
                    msgwin.Add(pData->attacker->GetVerb("attack"));
                    msgwin.AddLast(GetNameEx(CRN_T1));
                    msgwin.Add(GetNameEx(CRN_T1));
                    msgwin.Add(GetVerb("block"));
                    msgwin.Add(pData->attacker->GetNameEx(CRN_T1));
                    msgwin.Add("with");
                    msgwin.Add(GetNameEx(CRN_T4));
                    msgwin.Add("shield.");
                }
            }

            wsk->UseSkill(WSK_SHIELD);
        } else { //it was not shield (miss or avoid)
            if (vis1 || vis2) {
                if (pData->attack_name) {
                    msgwin.Add(GetNameEx(CRN_T1));
                    msgwin.Add(GetVerb("avoid"));
                    msgwin.AddLast(pData->attack_name);
                } else {
                    msgwin.Add(pData->attacker->GetNameEx(CRN_T1));
                    msgwin.Add(pData->attacker->GetVerb("attack"));
                    char xxbuf[256];
                    sprintf(xxbuf, "%s, but %s.", GetNameEx(CRN_T1), pData->attacker->GetVerb("miss"));
                    msgwin.Add(xxbuf);
                }
            }
        }
    }

    return 0;
}

/***** uncompleted ********/
// 1) Backstab
// 3) Correct using proper ammo type
// 4) Drain life
