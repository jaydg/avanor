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

#include "spell.h"
/*
SPELL spell_db[] = {

{"burning hand", 		SP_BURNINGHAND,		MG_FIRE, 	MG2_TOUCH, 	MG3_ALL,	2, 	2,	2,	R_FIRE},
{"fire bolt", 			SP_FIREBOLT,		MG_FIRE, 	MG2_BOLT,	MG3_SPELL, 	4, 	4,	3,	R_FIRE},
//{"fire ball", 			SP_FIREBALL,		MG_FIRE, 	MG2_BALL, 	2, 	2, 	5,	R_FIRE},
//{"hell fire", 			SP_HELLFIRE,		MG_FIRE, 	MG2_STORM, 	3, 	3, 	8,	R_FIRE},

{"magic touch", 		SP_MAGICTOUCH,		MG_EARTH, 	MG2_TOUCH,	MG3_ALL, 	1, 	1, 	1,	R_EARTH},
{"magic missile", 		SP_MAGICMISSILE,	MG_EARTH, 	MG2_BOLT, 	MG3_SPELL,	2, 	2, 	2,	R_EARTH},
*/
/*{"acid touch", 			SP_ACIDTOUCH,		MG_BLACK, 	MG2_TOUCH, 	2, 	3, 	3,	R_ACID},
{"acid bolt", 			SP_ACIDBOLT,		MG_BLACK, 	MG2_BOLT, 	4, 	5, 	4,	R_ACID},
{"acid ball", 			SP_ACIDBALL,		MG_BLACK, 	MG2_BALL, 	3, 	4, 	6,	R_ACID},


{"drain life", 			SP_DRAINLIFE,		MG_BLACK, 	MG2_DRAIN, 	2, 	2, 4,	R_BLACK},

{"ligning bolt",		SP_LIGHTNINGBOLT,	MG_AIR, 	MG2_BOLT, 	3, 	4, 3,	R_AIR},

*/
//{"cure light wound",	SP_CURELIGHTWOUND,	MG_WHITE, 	MG2_CURE, 	MG3_SP,	1, 	1, 	1,	R_WHITE},
/*{"cure medium wound",	SP_MEDIUMWOUND,		MG_WHITE, 	MG2_CURE, 	1, 	2, 	2,	R_WHITE},
{"cure serve wound",	SP_CURESERIOUSWOUND,MG_WHITE, 	MG2_CURE, 	1, 	3, 	2,	R_WHITE},
{"cure critical wound",	SP_CURECRITICALWOUND,MG_WHITE,	MG2_CURE, 	1, 	5, 	3,	R_WHITE},
*/
//{"heal",	 			SP_HEAL,			MG_WHITE, 	MG2_HEAL, 	MG3_SP,	1, 	8, 	5,	R_WHITE},
//{"cure poison",			SP_CUREPOISON,		MG_WHITE, 	MG2_HEAL, 	MG3_SP,	1, 	8, 	5,	R_WHITE},
/*
{"fire resistance",		SP_FIRERESISTANCE,	MG_FIRE,	MG2_RESIST,	MG3_ALL,1, 	2, 	2,	R_FIRE},
{"fire immunity",		SP_FIREIMMUNITY,	MG_FIRE,	MG2_RESIST,	1, 	4, 	4,	R_FIRE},
*/
/*{"xxxxxx",	 			SP_POISON,			MG_UNKNOWN,	MG2_UNKNOWN, MG3_POTION,	1, 	8, 	5,	R_WHITE},
{"xxxxxx",	 			SP_WATER,			MG_UNKNOWN,	MG2_UNKNOWN, MG3_POTION,	0, 	0, 	0,	R_EOF},
{"eof",					SP_EOF,				MG_UNKNOWN,	MG2_UNKNOWN, MG3_UNKNOWN,	0, 	0, 	0,	R_EOF}
};
*/
//					  F   W    E  A   Wt  Bl  R
/*
int spell_cost1[] = { 20, 15, 10, 15, 25, 30, 50 };

int spell_cost2[] = {
10, 25, 100, 250,
150, 75, 50,
50,
15,
25, 75,
40
};

int sp_colors[] = {xRED, xBLUE, xBROWN, xCYAN, xWHITE, xDARKGRAY, xMAGENTA};

SPELL * XSpell::GetSpell(SPELL_NAME sname)
{
	for (int i = 0; i < SP_EOF; i++)
		if (spell_db[i].spell == sname) return &spell_db[i];

	assert(0);
	return NULL;
};

int XSpell::GetColor()
{
	return sp_colors[(GetSpell(spell))->mgt2];
}

void XSpell::Cast()
{
	if (++eff_count > eff_level)
	{
		eff_level++;
		eff_count = 0;
	}
}

int XSpell::GetDMG(int W, int L, int RL)
{
	SPELL * sp = GetSpell(spell);

	XDice * d = new XDice();
	d->Setup(sp->X + (W / 4 + eff_level + L) / 4 , sp->Y + RL);
	int val = d->Throw();
	delete d;
	return val;
}

int XSpell::GetRNG(int W, int L)
{
	return 3 + (W / 4 + L + eff_level) / 4;
}

int XSpell::GetRadius(int W, int L)
{
	return 2 + (W / 4 + L + eff_level) / 8;
}

int XSpell::GetCost(int L)
{
	SPELL * sp = GetSpell(spell);

	float val = (float)(spell_cost1[sp->mgt] * spell_cost2[sp->mgt2]);
	float val2 = (float)((eff_level + 4 * L) * 2 + 30);
	return vRound(val / val2);
}

int XSpell::Probability(int W, int L) //probability to fail
{
	SPELL * sp = GetSpell(spell);
	int p = (15 * sp->half_level) / (L + 1) - W / 5 - eff_level;
	if (p > 100) p = 100;
	if (p < 0) p = 0;
	return p;
}

MAGIC_TYPE XSpell::GetMGT()
{
	return (GetSpell(spell))->mgt;
}

MAGIC_TYPE2 XSpell::GetMGT2()
{
	return (GetSpell(spell))->mgt2;
}

char * XSpell::GetName()
{
	return (GetSpell(spell))->name;
}

//calling compare fo XSpell mean that we read new book...
int XSpell::Compare(XObject * o)
{
	assert(dynamic_cast<XSpell *>(o));
	XSpell * tsp = (XSpell *)o;
	if (tsp->spell == spell)
		return 0;
	else
		return 1;
}
*/
