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

#include "xarmor.h"

REGISTER_CLASS(XArmor);

_MAIN_ITEM_STRUCT ARMOR_STRUCT[] =
{

{IT_CLOTHES,	"clothes",		'[',		"1d2-1",	"1d2-1",	"",		"1d1",	"",			"",		ISET_SOFT,  	3,		5,	0,	""},
{IT_ROBE,		"robe",			'[',		"1d2",		"1d2-1",	"",		"1d1",	"",			"",		ISET_SOFT,  	5,		9,	0,	""},
{IT_LIGHTMAIL,	"scale mail",	'[',		"1d3",		"1d2-1",	"",		"1d1",	"",			"",		ISET_SOFT,  	15,		15,	0,	""},
{IT_SCALEMAIL,	"scale mail",	'[',		"1d3-1",	"1d4-1",	"1d2-2","1d4",	"",			"",		ISET_ALLMETAL,  30,		30,	0,	""},
{IT_PLATEMAIL,	"plate mail",	'[',		"1d3-2",	"1d5-1",	"1d2-3","1d4",	"",			"",		ISET_ALLMETAL,  45,		30,	0,	""},
{IT_CHAINMAIL,	"chain mail",	'[',		"1d3-3",	"1d6-1",	"1d2-4","1d4",	"",			"",		ISET_ALLMETAL,  45,		30,	0,	""},
{IT_RINGMAIL,	"ring mail", 	'[',		"1d3-3",	"1d6-1",	"1d2-4","1d4",	"",			"",		ISET_ALLMETAL,  50,		30,	0,	""}
};
const int r_size = 7;

XArmor::XArmor(ITEM_TYPE _it)
{
	BasicFill(_it, ARMOR_STRUCT, r_size);
	bp = BP_BODY;
	im = IM_BODY;
	dice.Z = 0;
	RNG = 0;
	_HIT = -(weight / 500);
}

XArmor::XArmor(XArmor * armor) : XClothe((XClothe *)armor)
{
}

