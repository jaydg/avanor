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

#include "xboots.h"

REGISTER_CLASS(XBoots);

_MAIN_ITEM_STRUCT BOOTS_STRUCT[] =
{

{IT_SANDALS,	"sandals", 		']',		"1d2-1",	"1d1-1",	"",		"1d1",	"",			"",		ISET_SOFT, 		1,		1,	0,	""},
{IT_LIGHTBOOTS,	"light boots", 	']',		"1d3-1",	"1d2-1",	"",		"1d1",	"",			"",		ISET_SOFT, 		2,		2,	0,	""},
{IT_SOFTBOOTS,	"soft boots", 	']',		"1d3-2",	"1d2-1",	"",		"1d1",	"",			"",		ISET_SOFT, 		3,		3,	0,	""},
{IT_HARDBOOTS,	"hard boots", 	']',		"1d3-3",	"1d2-1",	"",		"1d1",	"",			"",		ISET_HARDMETAL, 5,		5,	0,	""}
};
const int r_size = 4;

XBoots::XBoots(ITEM_TYPE _it)
{
	im = IM_BOOTS;
	BasicFill(_it, BOOTS_STRUCT, r_size);
	bp = BP_BOOTS;
	dice.Z = 0;
	RNG = 0;
	_HIT = -(weight / 500);
}

XBoots::XBoots(XBoots * copy) : XClothe((XClothe *)copy)
{
}

