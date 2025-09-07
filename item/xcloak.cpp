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

#include "item/xcloak.h"


_MAIN_ITEM_STRUCT CLOAK_STRUCT[] = {

    {IT_LIGHTCLOAK,	"light cloak",	'(',	"1d2+1",	"1d2",	"",	"1d1",	"",	"",	ISET_ALLLEATHER,	1,	2,	100,	IQ_AVG,	""},
    {IT_CLOAK,	"cloak",	'(',	"1d2",	"1d2+1",	"",	"1d1",	"",	"",	ISET_ALLLEATHER,	2,	3,	60,	IQ_AVG,	""},
    {IT_SHADOWCLOAK, "shadow cloak",	'(',	"1d2+2",	"1d5+3",	"",	"1d1",	"",	"",	ISET_ALLLEATHER,	15,	8,	10,	IQ_FAIR,	""},
    {IT_CAPE,	"cape",	'(',	"1d2",	"1d1",	"",	"1d1",	"",	"",	ISET_SOFT,	2,	2,	120,	IQ_AVG,	""}
};

XItemBasicStructure gi_cloaks(CLOAK_STRUCT, 4);

REGISTER_CLASS(XCloak);

XCloak::XCloak(ITEM_TYPE _it)
{
    BasicFill(_it, &gi_cloaks);
    bp = BP_CLOAK;
    im = IM_CLOAK;
    dice.Z = 0;
    RNG = 0;
    _HIT = 0;
}

XCloak::XCloak(XCloak * copy) : XClothe((XClothe*)copy)
{
}
