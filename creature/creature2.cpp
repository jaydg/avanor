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
	
	if (it1 && it1->im & IM_WEAPON)
	{
		ttm_res += MeleeAttack(target, it1);
		flag++;
	} 
	
	if (it2 && it2->im & IM_WEAPON && target->isValid())
	{
		ttm_res += MeleeAttack(target, it2);
		flag++;
	}
	
	if (flag == 0)
	{
		ttm_res += MeleeAttack(target, NULL);
	}
}

/*
int XCreature::AttackCreature(ATTACK_DATA * pData)
{
	XCreature * target = pData->target;
	XCreature * attacker = pData->attacker;

	//check for visibility both of target and attacker
	int vis1 = 0;
	if (attaker)
		vis1 = isVisible();
	int vis2 = target->isVisible();

	//check if was an enemy for backstab
	int wasEnemy = 0;
	if (attaker)
		target->xai->isEnemy(this);

	//check for basic of DV, HIT
	int todv = target->GetDV();
	int tohit = pData->toHIT;

	if (tohit < 0)
	{
		todv -= tohit; // a - (-b) == a + b;
		tohit = 0;
	}

	int p = (tohit * 150 + 1) / (tohit + todv + 1) - todv;
	int v = vRand(100);

	if (v < p || pData->isAbsHit) //if was successful hit (or always hit)
	{
		int sb = target->GetShieldDVBonus();
		if (sb > 0 && vRand(todv) < sb && !pData->isAbsHit) //test for block
		{
			if (vis1 || vis2) //The attack was blocked
			{
				if (pData->attacker_name)
				{
					msgwin.Add(target->GetNameEx(CRN_T1));
					msgwin.Add(target->GetVerb("deflect"));
					msgwin.Add(attacker_name);
					msgwin.Add("with");
					msgwin.Add(target->GetNameEx(CRN_T4));
					msgwin.Add("shield.");
				} else
				{
					msgwin.Add(GetNameEx(CRN_T1));
					msgwin.Add(GetVerb("attack"));
					msgwin.AddLast(target->GetNameEx(CRN_T1));
					msgwin.Add(target->GetNameEx(CRN_T1));
					msgwin.Add(target->GetVerb("block"));
					msgwin.Add(GetNameEx(CRN_T1));
					msgwin.Add("with");
					msgwin.Add(target->GetNameEx(CRN_T4));
					msgwin.Add("shield.");
				}
			}
			target->wsk->UseSkill(WSK_SHIELD);
		} else
		{
			//get random part of body
			XBodyPart * txbp = target->GetRNDBodyPart();
			
			//get initial damage
			int tdam = pData->baseDAM;

			//get initial pv of armour
			int xpv = 0;
			if (txbp && txbp->Item())
				xpv = txbp->Item()->_PV;

			if (txbp && txbp->bp_uin == BP_CLOAK)
			{
				XItem * xtmp = GetItem(BP_BODY);
				if (xtmp)
					xpv += xtmp->_PV;
			}

			//critical hit
			int crtical_flag = 0;
			if (attacker)
			{
				if (vRand(100) < (2 + sk->GetLevel(SKT_FINDWEAKNESS)))
				{
					sk->UseSkill(SKT_FINDWEAKNESS);
					crtical_flag = 1;
					tdam *= 3;
				}
			} else
			{
				if (vRand(100) < 2)
				{
					crtical_flag = 1;
					tdam *= 3;
				}
			}

			//backstab
			int backstab = 0;
			if (pData->weapon && (!target->isCreatureVisible(this) || !wasEnemy))
			{
				if (attacker)
				{
					if (vRand(100) < sk->GetLevel(SKT_BACKSTABBING) * 5 + 5)
					{
						backstab = 1;
						tdam *= 3;
						sk->UseSkill(SKT_BACKSTABBING, 3);
					}
				} else
				{
					if (vRand(100) < 5)
					{
						backstab = 1;
						tdam *= 3;
					}
				}
			}

			int dam = tdam;
			int p_flag = 0;
			
			//ignore armour
			if (attacker)
			{
				if (vRand(100) > (2 + sk->GetLevel(SKT_FINDWEAKNESS)) || xpv == 0)
					dam = tdam - xpv;
				else
				{
					sk->UseSkill(SKT_FINDWEAKNESS);
					p_flag = 1;
				}
			} else
			{
				if (vRand(100) > 2 || xpv == 0)
					dam = tdam - xpv;
				else
					p_flag = 1;
			}

			if (vis1 || vis2)
			{
				if (attacker)
					msgwin.Add(attacker->GetNameEx(CRN_T1));
				else
					msgwin.Add(pData->attacker_name);

				if (weapon)
				{
					if (crtical_flag)
						msgwin.Add("exactly");
					
					if(backstab)
						msgwin.Add(GetVerb("stab"));
					else
						msgwin.Add(GetVerb("hit"));
				} else
				{
					if (crtical_flag)
						msgwin.Add("exactly");
					msgwin.Add(GetVerb(GetMeleeAttackMsg()));
				}

				if (p_flag)
				{
					msgwin.Add(target->GetNameEx(CRN_T1));
					if (target->isHero()) //last message for hero.
						msgwin.Add("penetrating a piece of armour.");
					else
						msgwin.Add("penetrating a piece of armour");
				} else
					if (target->isHero()) //last message for hero
						msgwin.AddLast(target->GetNameEx(CRN_T1));
					else
						msgwin.Add(target->GetNameEx(CRN_T1));
			}

			
			if (!weapon)
			{
				int dmg = dam;
				for (XQList<MELEE_ATTACK>::iterator tit = melee_attack->begin(); tit != melee_attack->end(); tit++)
				{
					if (vRand(100) < (*tit).prob)
					{
						dam += target->CauseEffect(this, dmg, (*tit).r_attack);
					}
				}
				AttackCreature(target, dam);
			} else
			{
				dam = target->CauseEffect(dam, weapon->brt);
				AttackCreature(target, dam);
			}
		}
	} else
	{
		if (vis1 || vis2)
		{
			msgwin.Add(GetNameEx(CRN_T1));
			msgwin.Add(GetVerb("attack"));
			char xxbuf[256];
			sprintf(xxbuf, "%s, but %s.", target->GetNameEx(CRN_T1), GetVerb("miss"));
			msgwin.Add(xxbuf);
		}
	}
}
*/

int XCreature::MeleeAttack(XCreature * target, XItem * weapon)
{
	assert(isValid());
	
	//need for backstub.
	int wasEnemy = target->xai->isEnemy(this);

	//~~~check this in future
	target->xai->onWasAttacked(this);
	target->stopAction();
	//~~~~~

	bool vis1 = isVisible();
	bool vis2 = target->isVisible();

	int res = 0;
	int tohit;
	if (weapon)
	{
		res += (wsk->GetUseTime(weapon->wt) * GetSpeed()) / 1000;
		tohit = GetHIT() + wsk->GetHIT(weapon->wt) + GetHITFHBonus(weapon);

	} else
	{
		res += (wsk->GetUseTime(WSK_UNARMED) * GetSpeed()) / 1000;
		tohit = GetHIT() + wsk->GetHIT(WSK_UNARMED);
	}

	int todv = target->GetDV();

	if (tohit < 0)
	{
		todv -= tohit; // a - (-b) == a + b;
		tohit = 0;
	}

	int p =  (int)(1000 * ((float)(tohit + 1) / (tohit + todv + 1)));
	int v = vRand() % 1000;

	if (v < p)
	{
		//test for block
		int sb = target->GetShieldDVBonus();
		if (sb > 0 && vRand(todv) < sb)
		{
			if (vis1 || vis2)
			{
				msgwin.Add(GetNameEx(CRN_T1));
				msgwin.Add(GetVerb("attack"));
				msgwin.AddLast(target->GetNameEx(CRN_T1));
				msgwin.Add(target->GetNameEx(CRN_T1));
				msgwin.Add(target->GetVerb("block"));
				msgwin.Add(GetNameEx(CRN_T1));
				msgwin.Add("with");
				msgwin.Add(target->GetNameEx(CRN_T4));
				msgwin.Add("shield.");
			}
			target->wsk->UseSkill(WSK_SHIELD);
		} else
		{
			//get random part of body
			XBodyPart * txbp = target->GetRNDBodyPart();
			
			//get initial damage
			int tdam;
			if (weapon)
			{
				tdam = weapon->dice.Throw() + wsk->GetDMG(weapon->wt) + GetDMGFHBonus(weapon) + GetDMG();
				wsk->UseSkill(weapon->wt);
			}
			else
			{
				tdam = dice.Throw() + GetDMG() + wsk->GetDMG(WSK_UNARMED);
				wsk->UseSkill(WSK_UNARMED);
			}

			//get initial pv of armour
			int xpv = 0;
			if (txbp && txbp->Item())
				xpv = txbp->Item()->_PV;

			if (txbp && txbp->bp_uin == BP_CLOAK)
			{
				XItem * xtmp = GetItem(BP_BODY);
				if (xtmp)
					xpv += xtmp->_PV;
			}

			//write hiter name
			if (vis1 || vis2)
				msgwin.Add(GetNameEx(CRN_T1));
			
			//critical hit
			int crtical_flag = 0;
			if ((vRand() % 100) < (2 + sk->GetLevel(SKT_FINDWEAKNESS)))
			{
				sk->UseSkill(SKT_FINDWEAKNESS);
				crtical_flag = 1;
				tdam *= 3;
			}

			//backstab
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

			int dam = tdam;
			int p_flag = 0;
			if ((vRand() % 100) > (2 + sk->GetLevel(SKT_FINDWEAKNESS)) || xpv == 0)
				dam = tdam - xpv;
			else
			{
				sk->UseSkill(SKT_FINDWEAKNESS);
				p_flag = 1;
			}

			if (vis1 || vis2)
			{
				if (weapon)
				{
					if (crtical_flag)
						msgwin.Add("exactly");
					
					if(backstab)
						msgwin.Add(GetVerb("stab"));
					else
						msgwin.Add(GetVerb("hit"));
				} else
				{
					if (crtical_flag)
						msgwin.Add("exactly");
					msgwin.Add(GetVerb(GetMeleeAttackMsg()));
				}

				if (p_flag)
				{
					msgwin.Add(target->GetNameEx(CRN_T1));
					if (target->isHero())
						msgwin.Add("penetrating a piece of armour.");
					else
						msgwin.Add("penetrating a piece of armour");
				} else
					if (target->isHero())
						msgwin.AddLast(target->GetNameEx(CRN_T1));
					else
						msgwin.Add(target->GetNameEx(CRN_T1));
			}

			
			if (!weapon)
			{
				int dmg = dam;
				for (XQList<MELEE_ATTACK>::iterator tit = melee_attack->begin(); tit != melee_attack->end(); tit++)
				{
					if (vRand(100) < (*tit).prob)
					{
						if ((*tit).r_attack > 0)
							dam += target->CauseEffect(this, dmg, (*tit).r_attack);
						else
							;
					}
				}
				AttackCreature(target, dam);
			} else
			{
				dam = target->CauseEffect(dam, weapon->brt);
				AttackCreature(target, dam);
			}
		}
	} else
	{
		if (vis1 || vis2)
		{
			msgwin.Add(GetNameEx(CRN_T1));
			msgwin.Add(GetVerb("attack"));
			char xxbuf[256];
			sprintf(xxbuf, "%s, but %s.", target->GetNameEx(CRN_T1), GetVerb("miss"));
			msgwin.Add(xxbuf);
		}
	}

	return res;
}


int XCreature::InflictDamage(DAMAGE_DATA * pData)
{
	assert(pData->target->isValid());
	if (pData->attacker)
		pData->target->xai->onWasAttacked(pData->attacker);
	pData->target->stopAction();

	int damage = pData->damage;
	damage -= pData->target->_PV; // always - monster PV

	if (damage <= 0)
	{
		if (pData->target->isHero())
		{
			if (pData->attacker_name)
				msgwin.Add(pData->attacker_name);
			else
				msgwin.Add(pData->attacker->GetNameEx(CRN_T1));
			msgwin.Add("does not manage to harm you.");
		} else if (pData->target->isVisible())
		{
			msgwin.Add("but does not manage to harm");
			msgwin.AddLast(pData->target->GetNameEx(CRN_T3));
		}
	} else
	{
		pData->target->_HP -= damage;

		if (pData->target->_HP > 0)
		{
			if (pData->target->isVisible() && !pData->target->isHero())
			{
				char buf[256];
				sprintf(buf, "and %s %s.",  pData->target->GetWoundMsg(1), pData->target->GetNameEx(CRN_T3));
				msgwin.Add(buf);
			}
		} else
		{ // and kill HIM!!!
			if (pData->target->isVisible() && !pData->target->isHero())
			{
				msgwin.Add("and");
				if (pData->attacker_name)
					msgwin.Add("kills");
				else
					msgwin.Add(pData->attacker->GetVerb("kills"));
				msgwin.AddLast(pData->target->GetNameEx(CRN_T3));
			}
			pData->target->Die(pData->attacker);
			cr_kiled++; //temporare counter special for statistic
			return 1;
		}
	}
	return 0;
}

int XCreature::AttackCreature(XCreature * target, int dmg)
{
	assert(isValid());
	target->xai->onWasAttacked(this);
	target->stopAction();
	if (target->onAttacked(this, dmg))
	{
		target->Die(this);
		cr_kiled++; //temporare counter special for statistic
		return 1;
	}
	return 0;
}

int XCreature::AttackCreature(XCreature * target, int dmg, RESISTANCE tr, const char * magic_name)
{
	assert(isValid());
	target->xai->onWasAttacked(this);
	target->stopAction();
	if (target->onAttackedByMagic(this, dmg, tr, magic_name))
	{
		target->Die(this);
		cr_kiled++; //temporare counter special for statistic
		return 1;
	}
	return 0;
}


int XCreature::onMagicDamage(int dmg, RESISTANCE tr)
{
	assert(isValid());
	int damage = dmg - (dmg * GetResistance(tr)) / 100;
	return damage < 0 ? 0 : damage;
}

int XCreature::CauseEffect(int dmg, BRAND_TYPE brt)
{
	int damage = dmg;
	
	if (brt > BR_NONE)
	{
		if (brt & BR_FIRE)
		{
			damage += onMagicDamage(dmg, R_FIRE);
		}
		if (brt & BR_COLD)
		{
			damage += onMagicDamage(dmg, R_COLD);
		}
		if (brt & BR_DEMONSLAYER && creature_class & CR_DEMON)
		{
			damage += dmg * 3;
		}
		if (brt & BR_ORCSLAYER && creature_class & CR_ORC)
		{
			damage += dmg * 3;
		}
	}
	return damage;
}

int XCreature::CauseEffect(XCreature * attacker, int dmg, RESISTANCE tr)
{
	int damage = dmg - (dmg * GetResistance(tr)) / 100;
	damage = damage < 0 ? 0 : damage;
	
	if (damage > 0)
	{
		switch (tr)
		{
			case R_WHITE:
			case R_BLACK:
			case R_FIRE:
			case R_WATER:
			case R_AIR:
			case R_EARTH:
			case R_ACID:
			case R_COLD:
				break;
			case R_POISON: 
				md->Add(MOD_POISON, damage, this, attacker);
				break;
			case R_DISEASE: 
				md->Add(MOD_DISEASE, damage, this, attacker);
				break;
			case R_PARALYSE: 
				md->Add(MOD_PARALYSE, damage, this, attacker);
				break;
			case R_STUN: 
				md->Add(MOD_STUN, damage, this, attacker);
				break;
			case R_CONFUSE: 
				md->Add(MOD_CONFUSE, damage, this, attacker);
				break;
			case R_BLIND: 
//				md->Add(MOD_BLIND, damage, this, attacker);
				break;
			default:
				return 0;
		}
	}
	return damage;
}

int XCreature::onAttacked(XCreature * attacker, int dmg)
{
	assert(isValid());
	int damage = dmg;
	damage -= _PV; // always - monster PV

	if (damage > 0)
		_HP -= damage;

	if (_HP > 0)
	{
		if (l->map->GetVisible(x, y))
		{
			char buf[256];
			if (damage <= 0)
			{
				if (isHero())
				{
					msgwin.Add(attacker->GetNameEx(CRN_T1));
					msgwin.Add("does not manage to harm");
					msgwin.AddLast(GetNameEx(CRN_T3));
				} else
				{
					msgwin.Add("but does not manage to harm");
					msgwin.AddLast(GetNameEx(CRN_T3));
				}
			} else if (!isHero())
			{
				sprintf(buf, "and %s %s.",  GetWoundMsg(1), GetNameEx(CRN_T3));
				msgwin.Add(buf);
			}
		}

		if (vRand() % 1000 < 300)
		{
			md->Add(MOD_WOUND, damage / 4, this, attacker);
		}
		
		if (vRand() % 1000 < 250)
		{
			md->Add(MOD_STUN, damage / 4, this, attacker);
		}

		if (vRand() % 1000 < 150)
		{
			md->Add(MOD_CONFUSE, damage / 4, this, attacker);
		}


		return 0;
	} else
	{ // and kill HIM!!!
		if (l->map->GetVisible(x, y) && !isHero())
		{
			msgwin.Add("and");
			msgwin.Add(attacker->GetVerb("kill"));
			msgwin.AddLast(GetNameEx(CRN_T3));
		}
		return 1;
	}
}


int XCreature::onAttackedByMagic(XCreature * attacker, int dmg, RESISTANCE tr, const char * magic_name)
{
	char buf[256];
	int  damage = onMagicDamage(dmg, tr) - _PV;

	if (damage > 0)
		_HP -= damage;

	if (_HP > 0)
	{
		if (l->map->GetVisible(x, y))
		{
			if (damage <= 0)
			{
				if (isHero())
				{
					msgwin.Add(magic_name);
					msgwin.Add("does not manage to harm");
					msgwin.AddLast(GetNameEx(CRN_T3));
				} else
				{
					msgwin.Add("but does not manage to harm");
					msgwin.AddLast(GetNameEx(CRN_T3));
				}
			} else if (!isHero())
			{
				sprintf(buf, "and %s %s.",  attacker->GetWoundMsg(1), GetNameEx(CRN_T3));
				msgwin.Add(buf);
			}
		}
		return 0;
	} else 
	{ // and kill HIM!!!
		if (l->map->GetVisible(x, y))
		{
			msgwin.Add("and");
			msgwin.Add(attacker->GetVerb("kill"));
			msgwin.AddLast(GetNameEx(CRN_T3));
		}
		return 1;
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
	if (xrng > 0)
	{
		cos_alpha = fdx / xrng;
		sin_alpha = fdy / xrng;
	}

	float mx = (float)mfd->sx;
	float my = (float)mfd->sy;
	int epx;
	int epy;
	int fl_range = 0;

	bool self_flag = false;
	if (fdx == fdy && fdy == 0.0)
		self_flag = true;

	while ((range > 0 && (fabs(mfd->ex - mx) >= 0.5f || fabs(mfd->ey - my) >= 0.5f)) || self_flag)
	{
		self_flag = false;
		range--;
		fl_range++;
		float nmx = mx + cos_alpha;
		float nmy = my + sin_alpha;
		epx = vRound(nmx);
		epy = vRound(nmy);

		if (tmap->GetMovability(epx, epy) >= MO_WALL)
		{
			epx = vRound(mx);
			epy = vRound(my);
			break;
		}

		if (tmap->GetVisible(epx, epy) && __animation_flag)
		{
			tmap->Put(main_creature);
			if (mfd->arrow_type == MFT_BALL)
				tmap->PutChar(epx, epy, '*', mfd->arrow_color);
			else
			{
				int ttx = vRound(nmx - mx);
				int tty = vRound(nmy - my);
				if (ttx * tty == 1)
					tmap->PutChar(epx, epy, '\\', mfd->arrow_color);
				else if (ttx * tty == -1)
					tmap->PutChar(epx, epy, '/', mfd->arrow_color);
				else if (ttx == 0)
					tmap->PutChar(epx, epy, '|', mfd->arrow_color);
				else if (tty == 0)
					tmap->PutChar(epx, epy, '-', mfd->arrow_color);
			}

			vRefresh();
			vDelay(__animation_flag);
		}

		mx = nmx;
		my = nmy;

		XCreature * tgt = tmap->GetMonster(epx, epy);
		if (tgt)
		{
			int tdv = tgt->GetDV() + fl_range * tgt->GetDV() / 4;
			int tht = mfd->to_hit > 0 ? mfd->to_hit : 1;
			if (tdv < 0)
			{
				tht -= tdv;
				tdv = 0;
			}
			int p =  (100 * tht + 1) / (tht + tdv + 1) - tdv;
			if (vRand(100) <= p)
			{
				mfd->pt.x = epx;
				mfd->pt.y = epy;
				//check for shield
				int sb = tgt->GetShieldDVBonus();
				sb += fl_range * sb / 4;
				if (sb > 0 && vRand(tdv) < sb)
				{
					tgt->wsk->UseSkill(WSK_SHIELD);
					return MF_BLOCK;
				}
				return MF_HIT;
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
	XItem * tool = (XItem *)action_data.item.get();
	assert(tool->im & IM_TOOL);
	RESULT res = tool->onUse(UIS_CONTINUE, this);
	if (res != CONTINUE)
	{
		action_data.action = A_MOVE;
		action_data.item = NULL;
	}
	return 1;
}

int XCreature::UseItem(XItem * item)
{
	assert(isValid());
	RESULT res = item->onUse(UIS_BEGIN, this);
	if (res)
	{
		if (res == CONTINUE)
		{
			action_data.action = A_USETOOL;
			action_data.item = item;
			return 1;
		} else
			return 1;
	} else
		return 0;

	return 1;
}

int XCreature::TargetOp(TARGET_REASON tr, XTREQUEST * rq, XTRESPONSE * rp)
{
	return 0;
}

void XCreature::Sacrifice(XItem * item)
{
	assert(isValid());
	religion.SacrificeItem(this, item, D_UNKNOWN);
}


int XCreature::isCreatureVisible(XCreature * cr)
{
	assert(isValid());
	if ((cr->GetResistance(R_INVISIBLE) <= GetResistance(R_SEEINVISIBLE)) || cr == this)
		return 1;
	else
		return 0;

}

bool XCreature::isVisible()
{
	assert(isValid());
	if (l->map->GetVisible(x, y))
	{
		if (XCreature::main_creature->isCreatureVisible(this))
			return true;
		else
			return false;
	} else
		return false;
}

