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

#include "skill.h"
#include "creature.h"
#include "other_misc.h"

REGISTER_CLASS(XSkill);

SKILL_DB skill_db[] = {
{"Archery",         1},
{"Find Weakness",   1},
{"Healing",         2},
{"Concentration",   3},
{"Dodge",           1},
{"Trading",         1},
{"Stealing",        1},
{"Literacy",        1},
{"Detect trap",     1},
{"Disarm trap",     1},
{"Cooking",         1},
{"Mining",          5},
{"Herbalism",       1},
{"Religion",        1},
{"Backstabbing",    1},
{"First aid",		2},
{"Tactics",			1},
{"Alchemy",			1},
{"Woodcraft",		1},
{"Create trap",		1},
{"Necromancy",		1},
{"Athletics",		1},
};

char * skill_level_name[16] = {
MSG_LIGHTGRAY       "NONE",
MSG_LIGHTGREEN      "Basic", 
MSG_LIGHTGREEN      "Basic", 
MSG_LIGHTGREEN      "Basic",
MSG_GREEN           "Skilled",
MSG_GREEN           "Skilled",
MSG_GREEN           "Skilled",
MSG_GREEN           "Skilled",
MSG_YELLOW          "Expert", 
MSG_YELLOW          "Expert", 
MSG_YELLOW          "Expert",
MSG_YELLOW          "Expert",
MSG_LIGHTRED        "Master",
MSG_LIGHTRED        "Master",
MSG_LIGHTRED        "Master",
MSG_DARKGRAY        "Grand Master"
};


XSkill::XSkill(SKILL_TYPE _skt, int _level)
{
	skt = _skt; 
	level = _level; 
	im = IM_OTHER;
	if (_level < 4)
	{
		used_time = skill_db[skt].use_per_level * (4 * 4 - 1) * 2;
	} else
	{
		used_time = skill_db[skt].use_per_level * (level * level - 1) * 2;
	}
}

char * XSkill::GetName()
{
	return skill_db[skt].name;
}

char * XSkill::GetSkillLevel()
{
	assert(level <= SKILL_MAX_LEVEL);
	return skill_level_name[level];
}

int XSkill::GetMaxLevel()
{
	int xlevel = (int)(sqrt(used_time / (2 * skill_db[skt].use_per_level) + 1));
	return xlevel < 15 ? xlevel : 15;
}

int XSkill::IncLevel()
{
	if (level < GetMaxLevel())
	{
		level++;
		return 1;
	} else
	{
		return 0;
	}
}

SKILL_MASTERY XSkill::GetMastery()
{
	switch (level)
	{
		case 1 :
		case 2 :
		case 3 : return SM_BASIC; break;
		case 4 :
		case 5 :
		case 6 :
		case 7 : return SM_SKILLED; break;
		case 8 :
		case 9 :
		case 10:
		case 11: return SM_EXPERT; break;
		case 12:
		case 13:
		case 14: return SM_MASTER; break;
		case 15: return SM_GRANDMASTER; break;
	}
	return SM_BASIC;
}

int XSkill::Use(XCreature * user)
{
	switch (skt)
	{
		case SKT_STEALING :			return UseSteal(user);
		case SKT_DISARMTRAP :		return UseDisarm(user);
		case SKT_CREATETRAP:		return UseCreate(user);

	};
	return 1;
}

int XSkill::isUseable()
{
	if (skt == SKT_STEALING || skt == SKT_DISARMTRAP || skt == SKT_CREATETRAP)
		return 1;
	else
		return 0;
}


void XSkill::UseSkill(int n)
{
	used_time += n; 
}

void XSkill::Store(XFile * f)
{
	XObject::Store(f);
	
	f->Write(&level, sizeof(int));
	f->Write(&skt, sizeof(SKILL_TYPE));
	f->Write(&used_time, sizeof(int));
}

void XSkill::Restore(XFile * f)
{
	XObject::Restore(f);
	
	f->Read(&level, sizeof(int));
	f->Read(&skt, sizeof(SKILL_TYPE));
	f->Read(&used_time, sizeof(int));
}


int XSkill::Compare(XObject * o)
{
	return strcmp(GetName(), ((XSkill *)o)->GetName());
}


int XSkill::UseSteal(XCreature * user)
{
	XPoint pt;
	XItem * object;
	if (user->GetTarget(TR_STEAL_ITEM, &pt, 0, (XObject **)&object))
	{
		if (pt.x == 0 && pt.y == 0)
		{
			msgwin.Add("Stealing from yourself? You are successful!");
			user->contain.Add(object);
		} else
		{
			XCreature * cr = user->l->map->GetMonster(user->x + pt.x, user->y + pt.y);
			int flag = 0;
			if (cr == NULL)
			{
				XAnyPlace * pl = user->l->map->GetPlace(user->x + pt.x, user->y + pt.y);
				if (pl)
				{
					cr = pl->GetOwner().get();
					flag = 1;
				}
			}
			assert(cr);
			double perception = 1 + cr->s->Get(S_PER);
			double stealing = 1 + user->sk->GetLevel(SKT_STEALING);
			int p = (int)((stealing * 300) / perception);
			if (vRand() % 100 < p || !user->isVisible())
			{
				if (user->isVisible())
				{
					char buf[256];
					XItem * it = (XItem *)object;
					it->toString(buf);
					char buf2[256];
					sprintf(buf2, "You steal %s.", buf);
					msgwin.Add(buf2);
				}
				cr->UnCarryItem((XItem *)object);
				user->CarryItem((XItem *)object);
				user->contain.Add(object);
				UseSkill(6);
			} else
			{
				if (user->isVisible())
				{
					msgwin.Add(cr->name);
					msgwin.Add("notices your efforts and becomes angry.");
				}
				cr->xai->onSteal(user);
				if (flag)
				{
					(user->l->map->GetItemList(user->x + pt.x, user->y + pt.y))->Add(object);
				} else
				{
					cr->contain.Add(object);
				}
			}
		}
	}
	return 1;
}

int XSkill::UseDisarm(XCreature * user)
{
	XMapObject * obj = user->l->map->GetSpecial(user->x, user->y);
	if (obj && obj->im & IM_TRAP)
	{
		if (((XTrap *)obj)->Disarm(user))
			UseSkill(1);
	} else
		msgwin.Add("There is no trap here.");
	return 1;
}

int XSkill::UseCreate(XCreature * user)
{
	return 1;
}


