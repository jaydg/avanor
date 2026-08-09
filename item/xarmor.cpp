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
#include "item/xarmor.h"

REGISTER_CLASS(XArmor);
CEREAL_REGISTER_TYPE(XArmor);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XItem, XArmor);

_MAIN_ITEM_STRUCT ARMOR_STRUCT[] = {
    {ItemType::CLOTHES,	"clothes",	'[',	"1d2",	"1d1",	"",	"1d1",	"",	"",	ISET_SOFT,	3,	5,	100,	IQ_POOR,	""},
    {ItemType::DRESS,	"dress",	'[',	"1d2",	"1d1",	"",	"1d1",	"",	"",	ISET_SOFT,	3,	5,	100,	IQ_POOR,	""},
    {ItemType::ROBE,	"robe",	'[',	"1d2",	"1d1",	"",	"1d1",	"",	"",	ISET_SOFT,	5,	9,	150,	IQ_POOR,	""},
    {ItemType::LIGHTMAIL,	"scale mail",	'[',	"1d3+1", "1d2",	"",	"1d1",	"",	"",	ISET_ALLLEATHER,	15,	15,	70,	IQ_AVG,	""},
    {ItemType::SCALEMAIL,	"scale mail",	'[',	"1d3",	"1d2+2", "1d2-2", "1d4",	"",	"",	ISET_ALLMETAL,	30,	30,	20,	IQ_FAIR,	""},
    {ItemType::PLATEMAIL,	"plate mail",	'[',	"1d3-1", "1d2+3", "1d2-3", "1d4",	"",	"",	ISET_ALLMETAL,	45,	30,	15,	IQ_FAIR,	""},
    {ItemType::CHAINMAIL,	"chain mail",	'[',	"1d3-1", "1d2+4", "1d2-4", "1d4",	"",	"",	ISET_ALLMETAL,	45,	30,	10,	IQ_GOOD,	""},
    {ItemType::RINGMAIL,	"ring mail",	'[',	"1d3-1", "1d2+4", "1d2-4", "1d4",	"",	"",	ISET_ALLMETAL,	50,	30,	10,	IQ_GOOD,	""}
};

XItemBasicStructure gi_armour(ARMOR_STRUCT, 8);

XArmor::XArmor(const ItemType it)
{
    BasicFill(it, &gi_armour);
    bp = BP_BODY;
    kind = ItemKind::BODY;
    dice.SetBonus(0);
    RNG = 0;
    _HIT = -(weight / 500);
}
