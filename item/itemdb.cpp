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

{"cloth",	xWHITE,		"1d1", "1d1",	
"",		"0d0",	"",			"", 		
ISET_CLOTH, 	200,		IQ_POOR,	4,		8,	CAN_PAPERLIKE,
"",
SPP_NONE
},

{"leather",	xBROWN,		"1d2", "1d1",	
"",		"0d0",	"",			"", 		
ISET_LEATHER, 	200,		IQ_POOR,	6,		10,	CAN_LEATHERLIKE,
"",
SPP_NONE
},

{"studded leather", xBROWN,			"1d3",	"1d2",	
"",		"0d0",	"",			"", 		
ISET_STUDEDLEATHER,	100,		IQ_AVG,	8,		12, CAN_LEATHERLIKE,
"",
SPP_NONE
},

{"wooden", 	xBROWN,		"1d2-1",	"1d3",
"1d2-1",	"0d0",		"1d2-1",	"", 		
ISET_WOOD,	200,		IQ_POOR,	8,		7, CAN_WOODLIKE,
"",
SPP_NONE
},

{"stone", xLIGHTGRAY,	"1d3-2",	"1d4+1",
"1d3-1",	"0d0",		"1d2-1",	"1d2-1",	
ISET_STONE,		200,	IQ_POOR,	20,		5, CAN_STONELIKE,
"earth:0d0+10",
SPP_NONE
},

{"iron", xDARKGRAY,		"1d3-2",	"1d5+1",
"1d2-1",	"0d0",		"1d3+0",	"1d2-1", 	
ISET_IRON,		120,	IQ_POOR,	30,		15, CAN_BLACKMETALLIKE,
"",
SPP_NONE
},

{"bronze",	xBROWN,		"1d3-1",	"1d5+1",
"1d4-1",	"0d0",		"1d3+0",	"1d2-1", 	
ISET_BRONZE,	60,		IQ_AVG,		26,		17, CAN_GOODMETALLIKE,
"stun:0d0+5",
SPP_NONE
},

{"brass", xBROWN,		"1d3-1",	"1d5+1",	
"1d3-2",	"0d0",		"1d3+0",	"1d2-1", 	
ISET_BRASS,		60,		IQ_AVG,		30,		30, CAN_GOODMETALLIKE,
"stun:0d0+10",
SPP_SLOWDIGESTION
},

{"silver",	xLIGHTGRAY,	"1d3",	"1d5+2",	
"1d4+0",	"0d0",		"1d4+0",	"1d3-1", 	
ISET_SILVER,	30,		IQ_FAIR,	20,		30, CAN_GOODMETALLIKE,
"acid:0d0+10",
SPP_NONE
},

{"golden", 	xYELLOW,	"1d3+1",		"1d5+2",
"1d5-1",	"0d0",		"1d5+0",	"1d4-1", 	
ISET_GOLD,		15,		IQ_GOOD,	22,		50, CAN_BROKE,
"acid:0d0+20",
(SPECIAL_PROPERTY)(SPP_REGENERATION | SPP_FASTDIGESTION)
},

{"crystal",	xLIGHTMAGENTA,"1d3+3",	"1d5+2",
"1d5+1",	"0d0",		"1d5+1",	"1d2-1", 		
ISET_CRYSTAL,	50,		IQ_AVG,		25,		14, CAN_STONELIKE,
"water:0d0+10",
SPP_NONE
},

{"steel",	xLIGHTBLUE,	"1d3+3",	"1d5+2",
"1d6+1",	"0d0",		"1d5+1",	"1d2-1",	
ISET_STEEL,		50,		IQ_FAIR,	15,		20, CAN_BLACKMETALLIKE,
"stun:0d0+15",
SPP_NONE
},

{"obsidian",xDARKGRAY,	"1d3+3",	"1d6+2",	
"1d6+1",	"0d0",		"1d6+1",	"1d3-1",	
ISET_OBSIDIAN,	50,		IQ_FAIR,	13,		20, CAN_STONELIKE,
"fire:0d0+15",
SPP_NONE
},

{"mithril",	xLIGHTCYAN,	"1d3+6",	"2d3+3",
"2d4+4",	"0d1",		"2d4+3",	"1d4-1",	
ISET_MITHRIL,	5,		IQ_GOOD,	11,		100, CAN_NICEMETALLIKE,
"poison:0d0+10 stun:0d0+10 confuse:0d0+20",
SPP_NONE
},

{"adamantium",xLIGHTGREEN,"1d3+12",	"2d4+5",
"2d6+6",	"1d0",		"2d6+3",	"1d4",		
ISET_ADAMANTIUM,	2,	IQ_EXCELLENT,		9,		300, CAN_NICEMETALLIKE,
"paralyse:0d0+20 stun:0d0+15 confuse:0d0+30 blind:0d0+30",
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


