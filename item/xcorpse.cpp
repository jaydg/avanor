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
#include "modifers.h"
#include "xcorpse.h"
#include "game.h"
#include "anycr.h"

REGISTER_CLASS(XCorpse);

XCorpse::XCorpse(XCreature * corpse_owner, const CORPSE_DATA * pData, CORPSE_FLAG cf)
{
	im = IM_FOOD;
	view = '%';
	color = corpse_owner->color;
	
	weight = corpse_owner->weight / 2;
	
	food_nutrio = (int)(weight / log((weight + 7.0) / 5.0));
	food_nutrio = food_nutrio == 0 ? 1 : food_nutrio;
	
	value = food_nutrio / 10;
	value = value == 0 ? 1 : value;

	consume_nutrio = food_nutrio  / (vRand(5) + 1);
	consume_nutrio = consume_nutrio == 0 ? 1 : consume_nutrio;

	ttmb = 1000;
	ttm = 1000;
	it = IT_CORPSE;
	sprintf(name, "%s corpse", corpse_owner->name);

	corpse_flag = cf;
	time_of_roating = 0;
	roating_stopped = 0;
	pCorpseData = &XCreatureStorage::GetCreatureData(corpse_owner->creature_name)->pCorpseData;
	cn = corpse_owner->creature_name;
	Game.Scheduler.Add(this);
}


XCorpse::XCorpse(XCorpse * copy) : XAnyFood((XAnyFood *)copy)
{
	assert(0);
	corpse_flag = copy->corpse_flag;
	time_of_roating = copy->time_of_roating;
	roating_stopped = copy->roating_stopped;
}

RESULT XCorpse::onEat(XCreature * eater)
{
	//prevent corpse from distruction
	AddRef();

	RESULT flag = XAnyFood::onEat(eater);
	if (flag == SUCCESS)
	{
		for (XQList<CORPSE_EFFECT>::iterator it = pCorpseData->effect.begin(); it != pCorpseData->effect.end(); it++)
		{
			switch ((*it).type)
			{
				case CET_MODIFY_ST:
					eater->GainAttr(S_STR, (*it).value);
					break;

				case CET_MODIFY_TO:
					eater->GainAttr(S_TOU, (*it).value);
					break;

				case CET_MODIFY_MA:
					eater->GainAttr(S_MAN, (*it).value);
					break;

				case CET_MODIFY_R_FIRE:
					eater->GainResist(R_FIRE, (*it).value);
					break;

				case CET_MODIFY_R_COLD:
					eater->GainResist(R_COLD, (*it).value);
					break;

				case CET_MODIFY_R_ACID:
					eater->GainResist(R_ACID, (*it).value);
					break;

				case CET_MODIFY_R_POISON:
					eater->GainResist(R_POISON, (*it).value);
					break;

				case CET_MODIFY_R_PARALYSE:
					eater->GainResist(R_PARALYSE, (*it).value);
					break;

				case CET_POISON:
					{
						XModDelayed * mod = new XModDelayed(MOD_POISON, (*it).value, vRand(100), eater);
						eater->md->Add(mod, eater);
					}
					break;

				case CET_DISEASE:
					{
						XModDelayed * mod = new XModDelayed(MOD_DISEASE, (*it).value, vRand(100), eater);
						eater->md->Add(mod, eater);
					}
					break;

				case CET_PARALYSE:
					{
						XModDelayed * mod = new XModDelayed(MOD_PARALYSE, (*it).value, vRand(100), eater);
						eater->md->Add(mod, eater);
					}
					break;

				case CET_CONFUSE:
					{
						XModDelayed * mod = new XModDelayed(MOD_CONFUSE, (*it).value, vRand(100), eater);
						eater->md->Add(mod, eater);
					}
					break;

				case CET_VOMIT:
					if (eater->isHero())
					{
						msgwin.Add("You are vomit!");
						if (eater->nutrio > 1000)
							eater->nutrio = 1000;
					}
					break;

				case CET_MODIFY_STOMACH:
					if (eater->isHero())
					{
						if ((*it).value < 0)
						{
							msgwin.Add("You stomach shrinks from pain!");
							eater->nutrio_speed++;
						} else
						{
							msgwin.Add("You stomach rumble peacefull!");
							eater->nutrio_speed++;
						}
					}
					break;
			}
		}
	} 
	
	Release();
	return flag;
}

char * XCorpse::postEat(XCreature * eater)
{
	return "tasty";
}

int XCorpse::Run()
{
	CORPSE_CONDITION cc = GetCondition();
	if (corpse_flag == CF_RAW)
	{
		time_of_roating++;
		if (!owner) time_of_roating += 3;
	}

	if (cc != GetCondition())
	{
		if (owner && owner->im & IM_HERO)
			msgwin.Add("Something in your backpack seems to rotting.");
		else if (l && isInVisibleArea())
			msgwin.Add("Something seems to rotting.");
	} else if (time_of_roating > pCorpseData->roating_time)
	{
		if (owner)
		{
			if (owner->im & IM_HERO) 
				msgwin.Add("Suddenly your equipment weight less.");
			owner->UnCarryItem(this);
		} else
			if (l && isInVisibleArea())
				msgwin.Add("Suddenly something disappered from the ground.");
		Invalidate();
		return 0;
	}
	ttm += ttmb;
	return 1;
}

CORPSE_CONDITION XCorpse::GetCondition()
{
	double val = ((double)time_of_roating) / pCorpseData->roating_time;
	if (val < 0.1)
		return CCOND_NICE;
	if (val < 0.2)
		return CCOND_NORMAL;
	else if (val < 0.4)
		return CCOND_SROTED;
	else if (val < 0.6)
		return CCOND_ROTED;
	else
		return CCOND_VROTED;
}

int XCorpse::GetValue()
{
	return value;
}

void XCorpse::Store(XFile * f)
{
	XAnyFood::Store(f);
	f->Write(&corpse_flag, sizeof(unsigned int));
	f->Write(&time_of_roating, sizeof(int));
	f->Write(&cn, sizeof(CREATURE_NAME));
}

void XCorpse::Restore(XFile * f)
{
	XAnyFood::Restore(f);
	f->Read(&corpse_flag, sizeof(unsigned int));
	f->Read(&time_of_roating, sizeof(int));
	f->Read(&cn, sizeof(CREATURE_NAME));
	pCorpseData = &XCreatureStorage::GetCreatureData(cn)->pCorpseData;
}

void XCorpse::toString(char * buf)
{
	if (corpse_flag & CF_COOKED)
		sprintf(buf, "cooked %s", name);
	else
		strcpy(buf, name);
}
