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

#include "xmissileweapon.h"
#include "xweapon.h"

REGISTER_CLASS(XMissileWeapon);

_MAIN_ITEM_STRUCT MWEAPON_STRUCT[] =
{
{IT_LONGBOW,		"long bow",			'{',	"",	"",		"1d4-1",	"1d6",  "1d3",	"1d3+2", 	ISET_BOW,	20,	10,		0,	""},
{IT_SHORTBOW,		"short bow",		'{',	"",	"",		"1d3-1",	"1d4",  "1d3",	"1d2+1", 	ISET_BOW,	10,	5,		0,	""},
{IT_LIGHTCROSSBOW,	"light crossbow",	'{',	"",	"",		"1d2-1",	"1d5",  "1d3",	"1d2+1", 	ISET_BOW,	10,	5,		0,	""},
{IT_CROSSBOW,		"crossbow",			'{',	"",	"",		"1d4-1",	"1d6",  "1d3",	"1d3+1", 	ISET_BOW,	15,	5,		0,	""},
{IT_HEAVYCROSSBOW,	"heavy crossbow",	'{',	"",	"",		"1d6-1",	"2d3",  "1d4",	"1d3+2", 	ISET_BOW,	35,	30,		0,	""},
{IT_SLING,			"sling",			'{',	"",	"",		"1d2-1",	"1d3",  "1d3",	"1d2+1", 	ISET_SOFT,	5,	2,		0,	""}
};
const int mw_size = 6;

_WEAPON_BIND mwbind[] = {
{IT_LONGBOW,		WSK_BOW},
{IT_SHORTBOW,		WSK_BOW},
{IT_LIGHTCROSSBOW,	WSK_CROSSBOW},
{IT_CROSSBOW,		WSK_CROSSBOW},
{IT_HEAVYCROSSBOW,	WSK_CROSSBOW},
{IT_SLING,			WSK_SLING}
};

XMissileWeapon::XMissileWeapon(ITEM_TYPE _it)
{
	im = IM_MISSILEW;
	BasicFill(_it, MWEAPON_STRUCT, mw_size);
	bp = BP_MISSILEWEAPON;
	_DV = 0;
	_PV = 0;
	for (int i = 0; i < mw_size; i++)
		if (mwbind[i].it == it)
		{
			wt = mwbind[i].ws;
			break;
		}
}

void XMissileWeapon::toString(char * buf)
{

	GetFullName(buf);

    char tbuf[256];

	sprintf(tbuf, "<%+d>", RNG);
	strcat(buf, tbuf);

	sprintf(tbuf, "(%+d, %dd%d%+d)", _HIT, dice.X, dice.Y, dice.Z);
	strcat(buf, tbuf);

    StatsToString(tbuf);
    strcat(buf, tbuf);
}
