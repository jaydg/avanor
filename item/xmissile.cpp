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

#include "xmissile.h"
#include "xweapon.h"

REGISTER_CLASS(XMissile);

_MAIN_ITEM_STRUCT MISSILE_STRUCT[] =
{
{IT_ARROW,		"arrow",		'\\',	"",	"",		"1d7-4","1d4",  "1d3-2","1d2-1", 	ISET_MISSILE,	1,	1,	0,	""},
{IT_QUARREL,	"quarrel",		'\\',	"",	"",		"1d7-4","1d6",  "1d3-2","1d2-1", 	ISET_MISSILE,	1,	1,	0,	""},
{IT_SLINGBULLET,"sling bullet",	'\\',	"",	"",		"1d7-4","1d5",  "1d3-2","1d2-1", 	ISET_MISSILE,	1,	1,	0,	""},
{IT_ROCK,		"rock",			'*',	"",	"",		"1d3-2","1d3",  "1d3-2","1d2-1", 	ISET_STONE,		1,	1,	0,	""}
};
const int msl_size = 4;

/*
// TODO: Bind launchers to missiles.  
// Rock is special b/c it can be used with or without a launcher...

 _WEAPON_BIND mbind[] = {
{IT_LONGBOW,		WSK_BOW},
{IT_LIGHTCROSSBOW,	WSK_CROSSBOW},
{IT_SLING,			WSK_SLING},
{IT_SLING,			WSK_THROW}
};*/

XMissile::XMissile(ITEM_TYPE _it)
{
	im = IM_MISSILE;
	bp = BP_MISSILE;
	BasicFill(_it, MISSILE_STRUCT, msl_size);
	if (it == IT_ROCK)
	{
		strcpy(name, "rock");
		view = '*';
		color = xDARKGRAY;
		XDice d(1, 3);
		_HIT = d.S;
		d.Setup(1, 2);
		dice.Setup(1, 5, d.S);
		value = 1;
		weight = 5;
		RNG = 0;
	}
	r->Sub(r);
	s->Sub(s);
	quantity = vRand() % 20 + 3;
	weight = weight / 5 + 1;
}

void XMissile::toString(char * buf)
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
