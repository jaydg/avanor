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

#include "item/item_cereal.h"
#include "item/xcap.h"

REGISTER_CLASS(XCap);
CEREAL_REGISTER_TYPE(XCap);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XItem, XCap);

_MAIN_ITEM_STRUCT CAP_STRUCT[] = {
    {ItemType::HAT,	"hat",	'[',	"1d1",	"0d0",	"",	"1d1",	"",	"",	ISET_SOFT,	1,	1,	100,	IQ_POOR,	""},
    {ItemType::CAP,	"cap",	'[',	"1d2+1",	"1d2",	"",	"1d1", "", "",	ISET_ALLLEATHER,	2,	2,	50,	IQ_AVG,	""},
    {ItemType::HELMET,	"helmet",	'[',	"1d3+2",	"1d3+1",	"",	"1d3", "",	"",	ISET_HARDMETAL,	3,	3,	10,	IQ_FAIR,	""},
};

XItemBasicStructure gi_cap(CAP_STRUCT, 3);

XCap::XCap(ItemType _it)
{
    BasicFill(_it, &gi_cap);
    bp = BP_HEAD;
    kind = ItemKind::HAT;
    _HIT = 0;
    dice.SetBonus(0);
    RNG = 0;
}
