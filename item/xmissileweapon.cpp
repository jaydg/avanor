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

#include "item/xmissileweapon.h"
#include "item/xweapon.h"

REGISTER_CLASS(XMissileWeapon);

_MAIN_ITEM_STRUCT MWEAPON_STRUCT[] = {
    {IT_LONGBOW,	"long bow",	'{',	"",	"",	"1d3+2",	"1d6", "1d3+2",	"1d1+2",	ISET_BOW,	20,	10,	30,	IQ_FAIR,	""},
    {IT_SHORTBOW,	"short bow",	'{',	"",	"",	"1d2",	"1d4", "1d2",	"1d2",	ISET_BOW,	10,	5,	150,	IQ_AVG,	""},
    {IT_LIGHTCROSSBOW,	"light crossbow",	'{',	"",	"",	"1d2",	"1d5", "1d3",	"1d2",	ISET_BOW,	10,	5,	120,	IQ_AVG,	""},
    {IT_CROSSBOW,	"crossbow",	'{',	"",	"",	"1d4",	"1d6", "1d3+1",	"1d2",	ISET_BOW,	15,	5,	70,	IQ_AVG,	""},
    {IT_HEAVYCROSSBOW,	"heavy crossbow",	'{',	"",	"",	"1d3-1",	"1d8", "1d4+2",	"1d2+1",	ISET_BOW,	35,	30,	10,	IQ_GOOD,	""},
    {IT_SLING,	"sling",	'{',	"",	"",	"1d2",	"1d3+1", "1d3",	"1d1+1",	ISET_ALLLEATHER,	5,	2,	200,	IQ_POOR,	""}
};

XItemBasicStructure gi_missilew(MWEAPON_STRUCT, 6);

_WEAPON_BIND mwbind[] = {
    {IT_LONGBOW,	WSK_BOW},
    {IT_SHORTBOW,	WSK_BOW},
    {IT_LIGHTCROSSBOW,	WSK_CROSSBOW},
    {IT_CROSSBOW,	WSK_CROSSBOW},
    {IT_HEAVYCROSSBOW,	WSK_CROSSBOW},
    {IT_SLING,	WSK_SLING}
};

XMissileWeapon::XMissileWeapon(ITEM_TYPE _it)
{
    im = IM_MISSILEW;
    BasicFill(_it, &gi_missilew);
    bp = BP_MISSILE_WEAPON;
    _DV = 0;
    _PV = 0;

    for (int i = 0; i < 6; i++)
        if (mwbind[i].it == it) {
            wt = mwbind[i].ws;
            break;
        }
}

std::string XMissileWeapon::toString()
{
    auto fullname = GetFullName();

    fullname.append(fmt::format(" <{:+}>", RNG));
    fullname.append(fmt::format(" ({:+}, {}d{}{:+})",
        _HIT, dice.X, dice.Y, dice.Z));
    fullname.append(StatsToString());

    return fullname;
}
