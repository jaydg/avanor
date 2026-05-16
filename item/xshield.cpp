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

#include <fmt/format.h>

#include "item/xshield.h"

REGISTER_CLASS(XShield);

_MAIN_ITEM_STRUCT SHIELD_STRUCT[] = {

    {IT_SMALLSHIELD, "small shield",	')',	"0d0+2",	"1d2",	"",	"1d2",	"",	"",	ISET_SHIELD,	3,	3,	200,	IQ_POOR,	""},
    {IT_MEDIUMSHIELD, "medium shield", ')',	"0d0+3",	"1d2",	"",	"1d3",	"",	"",	ISET_SHIELD,	5,	5,	100,	IQ_AVG,	""},
    {IT_LARGESHIELD, "large shield",	')',	"0d0+4",	"1d2",	"",	"1d4",	"",	"",	ISET_SHIELD,	10,	8,	50,	IQ_AVG,	""},
    {IT_TOWERSHIELD, "tower shield",	')',	"0d0+6",	"1d2",	"",	"1d4",	"",	"",	ISET_SHIELD,	15,	15,	10,	IQ_FAIR,	""}
};

XItemBasicStructure gi_shield(SHIELD_STRUCT, 4);

XShield::XShield(ITEM_TYPE _it)
{
    im = IM_SHIELD;
    BasicFill(_it, &gi_shield);
    _PV = 0;
    bp = BP_HAND;
    RNG = 0;
    wt = XWarSkills::SHIELD;
    dice.SetBonus(0);
    _HIT = 0;
}

std::string XShield::toString()
{
    auto fullname = GetFullName();

    fullname.append(fmt::format(" [{:+}, {:+}]", _DV, _PV));
    fullname.append(StatsToString());

    return fullname;
}
