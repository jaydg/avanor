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

#include "deity.h"
#include "creature.h"
#include "item.h"

XCreature * XDeity::death = NULL;
XCreature * XDeity::life = NULL;

DEITY_HELP life_help[] = {
	{"cure light wounds",			3,	PRAY_CURE_LIGHT_WOUNDS},
	{"minor divine intervention",	5,	PRAY_MINOR_INTERVENTION},

	{"cure poison",					10, PRAY_CURE_POISON},
	{"heroism",						10, PRAY_HEROISM},

	{"cure critical wounds",		20, PRAY_CURE_CRITICAL_WOUNDS},

	{"great knowledge",				30, PRAY_IDENTIFY},

	{"divine restoration",			50, PRAY_RESTORATION}
};

DEITY_HELP death_help[] =
{
	{"cure light wounds",			5,	PRAY_CURE_LIGHT_WOUNDS},
	{"minor divine intervention",	5,	PRAY_MINOR_INTERVENTION},

	{"divine intervention",			5,	PRAY_INTERVENTION},
	{"divine escape",				50, PRAY_TELEPORT},

	{"cure critical wounds",		30, PRAY_CURE_CRITICAL_WOUNDS},

	{"knowledge of insight",		50, PRAY_SELF_KNOWLEDGE},

	{"major divine intervention",	5,	PRAY_MAJOR_INTERVENTION},
};


struct DEITY_ACT
{
	int good;
	int bad;
};

DEITY_ACT life_deity_act[] = {
	{5, -15}, {5, -14}, {5, -12}, {5, -10}, {10, -10},
	{10, -9}, {10, -8}, {10, -7}, {10, -6}, {10, -5},
	{10, -4}, {10, -3}, {10, -2}, {10, -1}, {10, 0}, {10, 0}
};

void XReligion::KillCreature(XCreature * killer, XCreature * victim)
{
	int val = killer->sk->GetLevel(SKT_RELIGION);
	int bad = life_deity_act[val].bad;
	int good = life_deity_act[val].good;
	if (victim->creature_class == CR_UNDEAD)
	{
		death_act += bad;
		life_act += good;
	} else
	{
		death_act += good;
		life_act += bad;
	}
}

int XReligion::SacrificeItem(XCreature * cr, XItem * item, DEITY deity)
{
	int val = cr->sk->GetLevel(SKT_RELIGION);
	if (deity == D_UNKNOWN)
	{
		if (life_act > death_act)
			deity = D_LIFE;
		else
			deity = D_DEATH;
	}

	XMapObject * tmo = cr->l->map->GetSpecial(cr->x, cr->y);
	if (tmo && tmo->im & IM_ALTAR)
	{
		if (tmo->color == xWHITE)
			deity = D_LIFE;
		else
			deity = D_DEATH;
	}

	if (cr->isVisible())
	{
		char buf[256];
		sprintf(buf, "%s prays to %s.", cr->name, GetDeityName(deity));
		msgwin.Add(buf);
		item->toString(buf);
		msgwin.Add(buf);
		msgwin.Add("disappears in a bright light.");
	}

	int sacrifice_value;
	if (item->im & IM_MONEY)
		sacrifice_value = (int)(sqrt((float)item->quantity) + 1) * (val / 4 + 1);
	else
		sacrifice_value = (int)((sqrt((float)item->GetValue()) * item->quantity) + 1) * (val / 4 + 1);

	cr->sk->UseSkill(SKT_RELIGION);

	if (tmo && tmo->im & IM_ALTAR)
	{
		sacrifice_value *= 3;
		cr->sk->UseSkill(SKT_RELIGION, 5);
	}

	item->UnCarry();
	item->Invalidate();

	if (deity == D_LIFE)
	{
		life_act += sacrifice_value;
	} else
	{
		death_act += sacrifice_value;
	}

	if (!cr->isHero() && vRand(5) == 0)
	{
		cr->s->Modify(XStats::Random(), 1);
		if (cr->isVisible())
		{
			msgwin.Add(cr->name);
			msgwin.Add("looks more powerful!");
		}
	}

	return 1;
}

DEITY_RELATION XReligion::GetRelation(DEITY deity)
{
	int val = 0;
	switch (deity)
	{
		case D_LIFE: val = life_act; break;
		case D_DEATH: val = death_act; break;
	}
	if (val < -10000000)
		return DR_FALLEN_CHAMPION;
	else if (val < -100)
		return DR_VERY_BAD;
	else if (val < 0)
		return DR_BAD;
	else if (val < 100)
		return DR_NORMAL;
	else if (val < 1000)
		return DR_ADEPT;
	else if (val < 3000)
		return DR_FOLLOWER;
	else if (val < 10000)
		return DR_MESSIAH;
	else
		return DR_CHAMPION;
}

const char* relation_name[] = {
MSG_RED "fallen champion", MSG_RED "very bad", MSG_RED "bad",
MSG_LIGHTGRAY "normal", MSG_LIGHTGRAY "adept", MSG_LIGHTGREEN "follower",
MSG_YELLOW "messiah", MSG_WHITE "champion"
};

const char* XReligion::GetRelationName(DEITY_RELATION dr)
{
	return relation_name[dr];
}

const char* XReligion::GetDeityName(DEITY deity)
{
	if (deity == D_LIFE)
		return "Tiamat";
	else
		return "Marduk";
}

int XReligion::GetAvailHelp(DEITY deity, DEITY_HELP ** help)
{
	if (deity == D_LIFE)
	{
		*help = &life_help[0];

	} else
	{
		*help = &death_help[0];
	}

	DEITY_RELATION rel = GetRelation(deity);
	if (rel < DR_ADEPT)
		return 0;
	if (rel < DR_FOLLOWER)
		return 2;
	if (rel < DR_MESSIAH)
		return 4;
	if (rel < DR_CHAMPION)
		return 6;
	return 7;
}

int XReligion::Pray(DEITY deity, DEITY_HELP * pray, XCreature * prayer)
{
	EFFECT effect = E_CURE_LIGHT_WOUNDS;
	switch (pray->pray)
	{
		case PRAY_CURE_LIGHT_WOUNDS:	effect = E_CURE_LIGHT_WOUNDS; break;
		case PRAY_CURE_CRITICAL_WOUNDS:	effect = E_CURE_CRITICAL_WOUNDS; break;
		case PRAY_RESTORATION:			effect = E_RESTORATION; break;
		case PRAY_IDENTIFY:				effect = E_IDENTIFY; break;
		case PRAY_SELF_KNOWLEDGE:		effect = E_SELF_KNOWLEDGE; break;
		case PRAY_CURE_POISON:			effect = E_CURE_POISON; break;
		case PRAY_HEROISM:				effect = E_HEROISM; break;
		case PRAY_TELEPORT:				effect = E_TELEPORT; break;

		case PRAY_MINOR_PUNISHMENT:		effect = E_MAGIC_ARROW; break;
		case PRAY_MINOR_INTERVENTION:	effect = E_MAGIC_ARROW; break;
		case PRAY_INTERVENTION:			effect = vRand(2) == 0 ? E_FIRE_BOLT : E_ICE_BOLT; break;
		case PRAY_MAJOR_INTERVENTION:	effect = vRand(2) == 0 ? E_LIGHTNING_BOLT : E_ACID_BOLT; break;
	}

	RESULT res = XEffect::Make(prayer, effect, 50);

	if (res == SUCCESS)
	{
		if (deity == D_LIFE)
		{
			life_act -= pray->help_cost;
		} else
		{
			death_act -= pray->help_cost;
		}
		prayer->sk->UseSkill(SKT_RELIGION, 3);
	} if (res != ABORT)
	{
		msgwin.Add("Your prayer was unheard.");
	}
	return 1;
}

void XReligion::Store(XFile * f)
{
	f->Write(&life_act);
	f->Write(&death_act);
}

void XReligion::Restore(XFile * f)
{
	f->Read(&life_act);
	f->Read(&death_act);
}
