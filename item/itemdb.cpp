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

#include "item.h"


#include "itemdb.h"




// name				color			dv  		pv			hit		dice	z			r			iflag
// dv, pv, dice, r - armor
// hit, dice, z, r = armor
//WARNING!!! after adding new 'element' please change

_ITEMPROP item_prop[DB_PROP_SZ] =
{

{"leather",	xBROWN,	"1d2", "1d1-1",	
"",		"0d0",	"",			"", 		
ISET_LEATHER, 	100,		6,		10,	CAN_LEATHERLIKE,
"",
SPP_NONE
},

{"studded leather", xBROWN,			"1d2",	"1d2-1",	
"",		"0d0",	"",			"", 		
ISET_STUDEDLEATHER,		80,		8,		12, CAN_LEATHERLIKE,
"",
SPP_NONE
},

{"wooden", 	xBROWN,			"1d3-3",	"1d3-1",	
"1d2","0d0",	"1d2-4",	"", 		
ISET_WOOD,	100,		8,		7, CAN_WOODLIKE,
"",
SPP_NONE
},

{"stone", xLIGHTGRAY,		"1d3-6",	"1d4",		
"1d3-2","0d0",	"1d2-1",	"1d1-2",	
ISET_STONE,		100,		20,		5, CAN_STONELIKE,
"earth:4d5",
SPP_NONE
},

{"iron", xDARKGRAY,		"1d2-3",	"1d3-1",	
"1d2-2","0d0",	"1d3-2",		"1d2-1", 	
ISET_IRON,		90,		30,		15, CAN_BLACKMETALLIKE,
"",
SPP_NONE
},

{"bronze",	xBROWN,			"1d2-2",	"1d3-1",	
"1d2-2",	"0d0",	"1d3-2",		"1d2-1", 	
ISET_BRONZE,		60,		26,		17, CAN_GOODMETALLIKE,
"stun:1d5",
SPP_NONE
},

{"brass", xBROWN,			"1d2-3",	"1d2-1",	
"1d2-2",	"0d0",	"1d3-2",		"1d2-1", 	
ISET_BRASS,		60,		30,		30, CAN_GOODMETALLIKE,
"stun:1d8",
SPP_SLOWDIGESTION
},

{"silver",	xLIGHTGRAY,		"1d2-3",	"1d3-1",	
"1d2-2",	"0d0",	"1d3-2",	"1d3-1", 	
ISET_SILVER,		30,		20,		30, CAN_GOODMETALLIKE,
"acid:3d5",
SPP_NONE
},

{"golden", 	xYELLOW,		"1d2-3",	"1d3-1",	
"1d2-2",	"0d0",	"1d3-2",	"1d4-1", 	
ISET_GOLD,		15,		22,		50, CAN_BROKE,
"acid:5d5",
(SPECIAL_PROPERTY)(SPP_REGENERATION | SPP_FASTDIGESTION)
},

{"crystal",			xLIGHTMAGENTA,	"1d3-6",	"1d5",		
"1d3-2",	"0d0",	"1d5-1",	"1d2-1", 		
ISET_CRYSTAL,		70,		25,		14, CAN_STONELIKE,
"water:5d5",
SPP_NONE
},

{"steel", 			xLIGHTBLUE,		"1d4-1",	"1d4-1",	
"1d4-2",	"0d0",	"1d4-1",	"1d2-1",	
ISET_STEEL,	70,		15,		20, CAN_BLACKMETALLIKE,
"stun:3d5",
SPP_NONE
},

{"obsidian",		xDARKGRAY,		"1d3-1",	"1d5-1",	
"1d5-2",	"0d0",	"1d5-1",	"1d3-1",	
ISET_OBSIDIAN,	50,		13,		20, CAN_STONELIKE,
"fire:3d5",
SPP_NONE
},

{"mithril",			xLIGHTCYAN,		"2d3",		"2d3",		
"2d4-1",	"0d1",	"1d7-1",	"1d4-1",	
ISET_MITHRIL,	5,			11,		100, CAN_NICEMETALLIKE,
"poison:4d5 stun:3d5 confuse:3d5",
SPP_NONE
},

{"adamantium", 		xLIGHTGREEN,	"2d4",		"2d4",		
"2d6",		"1d0",	"2d4",		"1d4",		
ISET_ADAMANTIUM,	2,			9,		300, CAN_NICEMETALLIKE,
"paralyse:3d5 stun:5d5 confuse:5d5 blind:6d5",
SPP_REGENERATION
}
};

//ENH_DB_SZ - very important var, it used with ring, amulets too
ENHANCE_STRUCT ienh_db[ENH_DB_SZ] = {
{
	"%s of fire resistance", 	xRED, 	
	"", 	"", 	"", 	"", 	"", 	"", 	1,
	IM_ARMOUR, 
	BR_NONE, 
	CAN_FIRERESIST,
	SPP_NONE,
	"",
	"fire:5d5+25"
},

{
	"%s of cold resistance", 	xWHITE,	
	"", 	"", 	"", 	"", 	"", 	"", 	1,
	IM_ARMOUR, 
	BR_NONE, 
	CAN_COLDRESIST,
	SPP_NONE,
	"",
	"cold:5d5+25"
},

{
	"%s of air resistance", 	xLIGHTBLUE,	
	"", 	"", 	"", 	"", 	"", 	"", 	1,
	IM_ARMOUR, 
	BR_NONE, 
	CAN_COLDRESIST,
	SPP_NONE,
	"",
	"air:5d5+25"
},

{
	"%s of acid resistance", 	xDARKGRAY,
	"", 	"", 	"", 	"", 	"", 	"", 	5,
	IM_ARMOUR, 
	BR_NONE, 
	CAN_ACIDRESIST,
	SPP_NONE,
	"",
	"acid:5d5+25"
},

{
	"%s of poison resistance", 	0,
	"", 	"", 	"", 	"", 	"", 	"", 	2,
	IM_ARMOUR, 
	BR_NONE, 
	CAN_RESISTNONE,
	SPP_NONE,
	"",
	"poison:5d5+25"
},

{
	"%s of Resistance", 	xDARKGRAY,
	"", 	"", 	"", 	"", 	"", 	"", 	80,
	IM_ARMOUR, 
	BR_NONE, 
	CAN_NICEMETALLIKE,
	SPP_FASTDIGESTION,
	"",
	"poison:5d5+25 fire:5d5+25 cold:5d5+25 water:5d5+25 acid:5d5+25 earth:5d5+25"
},

{
	"%s of the Magi", 	0,
	"", 	"", 	"", 	"", 	"", 	"", 	50,
	IM_ARMOUR, 
	BR_NONE, 
	(CAN_FLAG)(CAN_FIRERESIST | CAN_COLDRESIST),
	SPP_NONE,
	"Le:1d5 Wi:1d5 Ma:1d5",
	"fire:5d5+25 cold:5d5+25"
},

{
	"%s of free action", 	0,
	"", 	"", 	"", 	"", 	"", 	"", 	50,
	IM_ARMOUR, 
	BR_NONE, 
	(CAN_FLAG)(CAN_FIRERESIST | CAN_COLDRESIST),
	SPP_NONE,
	"",
	"confuse:5d5+25 stun:5d5+25"
},

{
	"%s of slow digestion", 	0,
	"", 	"", 	"", 	"", 	"", 	"", 	1,
	IM_ARMOUR, 
	BR_NONE, 
	CAN_RESISTNONE,
	SPP_SLOWDIGESTION,
	"",
	""
},

{
	"%s of Strength",	0,
	"", 	"", 	"", 	"", 	"", 	"", 	1,
	IM_ARMOUR, 
	BR_NONE, 
	CAN_RESISTNONE,
	SPP_NONE,
	"St:1d4",
	""
},

{
	"%s of Swiftness",	0,
	"", 	"", 	"", 	"", 	"", 	"", 	1,
	IM_ARMOUR, 
	BR_NONE, 
	CAN_RESISTNONE,
	SPP_NONE,
	"Dx:1d4",
	""
},

{
	"%s of Toughness",	0,
	"", 	"", 	"", 	"", 	"", 	"", 	1,
	IM_ARMOUR, 
	BR_NONE, 
	CAN_RESISTNONE,
	SPP_NONE,
	"To:1d4",
	""
},

{
	"%s of the Giants",	0,
	"", 	"", 	"", 	"", 	"", 	"", 	30,
	IM_ARMOUR, 
	BR_NONE, 
	CAN_RESISTNONE,
	SPP_NONE,
	"St:1d5+5",
	""
},

{
	"%s of the Titans",	xCYAN,
	"", 	"", 	"", 	"", 	"", 	"", 	60,
	IM_ARMOUR, 
	BR_NONE, 
	CAN_RESISTALL,
	SPP_NONE,
	"St:1d10+10",
	""
},

{
	"%s of the High Kings",	xWHITE,
	"", 	"", 	"", 	"", 	"", 	"", 	90,
	IM_ARMOUR, 
	BR_NONE, 
	CAN_RESISTALL,
	SPP_NONE,
	"St:1d1+24",
	""
},

{
	"%s of the High Kings",	xWHITE,
	"", 	"", 	"", 	"", 	"", 	"", 	90,
	IM_ARMOUR, 
	BR_NONE, 
	CAN_RESISTALL,
	SPP_NONE,
	"St:1d1+24",
	""
},

{
	"%s of the High Kings",	xWHITE,
	"", 	"", 	"", 	"", 	"", 	"", 	90,
	IM_ARMOUR, 
	BR_NONE, 
	CAN_RESISTALL,
	SPP_NONE,
	"To:1d1+24",
	""
},

{
	"%s of the High Kings",	xWHITE,
	"", 	"", 	"", 	"", 	"", 	"", 	90,
	IM_ARMOUR, 
	BR_NONE, 
	CAN_RESISTALL,
	SPP_NONE,
	"Dx:1d1+24",
	""
},

{
	"%s of the Elven Kings",	xWHITE,
	"", 	"", 	"", 	"", 	"", 	"", 	90,
	IM_ARMOUR, 
	BR_NONE, 
	CAN_RESISTALL,
	SPP_NONE,
	"Le:1d1+24",
	""
},

{
	"clean %s",	0,
	"", 	"", 	"", 	"", 	"", 	"", 	1,
	IM_ARMOUR, 
	BR_NONE, 
	CAN_RESISTNONE,
	SPP_NONE,
	"ch:1d6+0",
	""
},
/*
{
	"%s of Fire",	xRED,
	"", 	"", 	"", 	"", 	"", 	"", 	10,
	IM_WEAPON, 
	BR_FIRE, 
	CAN_COLDRESIST,
	SPP_NONE,
	"",
	""
},

{
	"%s of Cold",	xWHITE,
	"", 	"", 	"", 	"", 	"", 	"", 	10,
	IM_WEAPON, 
	BR_COLD, 
	CAN_COLDRESIST,
	SPP_NONE,
	"",
	""
},

{
	"%s of Orc slaying",	0,
	"", 	"", 	"", 	"", 	"", 	"", 	15,
	IM_WEAPON, 
	BR_ORCSLAYER, 
	CAN_RESISTNONE,
	SPP_NONE,
	"",
	""
}
*/
};


