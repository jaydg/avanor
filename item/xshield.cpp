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

#include "xshield.h"

REGISTER_CLASS(XShield);

_MAIN_ITEM_STRUCT SHIELD_STRUCT[] =
{

{IT_SMALLSHIELD,"small shield",	')',	"1d2",	"1d2-1",	"",		"1d2",	"",	"",		ISET_SHIELD, 	3,		3},
{IT_MEDIUMSHIELD,"medium shield",')',	"1d3",	"1d2-1",	"",		"1d3",	"",	"",		ISET_SHIELD, 	5,		5},
{IT_LARGESHIELD,"large shield",	')',	"1d4",	"1d2-1",	"",		"1d4",	"",	"",		ISET_SHIELD, 	10,		8},
{IT_TOWERSHIELD,"tower shield",	')',	"1d6",	"1d2-1",	"",		"1d4",	"",	"",		ISET_SHIELD, 	15,		15}
};
const int r_size = 4;


XShield::XShield(ITEM_TYPE _it)
{
	im = IM_SHIELD;
	BasicFill(_it, SHIELD_STRUCT, r_size);
	_PV = 0;
	bp = BP_HAND;
	RNG = 0;
	wt = WSK_SHIELD;
	dice.Z = 0;
	_HIT = 0;
}

void XShield::toString(char * buf)
{

/*  if (quantity == 1)
		sprintf(buf, "%s ", name);
    else
        sprintf(buf, "heap of (%d) %ss ", quantity, name);
*/
	GetFullName(buf);
    char tbuf[256];

	sprintf(tbuf, "[%+d, %+d]", _DV, _PV);
	strcat(buf, tbuf);

    StatsToString(tbuf);
    strcat(buf, tbuf);
}

