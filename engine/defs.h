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

#ifndef __DEFS_H
#define __DEFS_H


#include "defs01.h"

#define DUR_INFINITE 65535

enum BODYPART {BP_UNKNOWN = 0,
BP_HEAD, BP_NECK, BP_BODY, BP_CLOAK,
BP_HAND, BP_RING, BP_GLOVES,
BP_BOOTS, BP_LIGHTSORCE, BP_TOOL, BP_MISSILEWEAPON, BP_MISSILE, BP_EOF, BP_OTHER,
BP_ONEHANDED, BP_TWOHANDED};


enum RBP_FLAG {RBP_UNKNOWN = 0, RBP_BLOCK = 1};

enum STAIRWAYTYPE {STW_UNKNOWN = 0, STW_UP, STW_DOWN };


enum ARTIFACT_STATE
{
	ARTIFACT_NO		= 0,
	ARTIFACT_YES	= 1,
	ARTIFACT_CREATED= 2
};

enum TARGET_REASON
{
	TR_NONE				= 0,
	TR_ATTACK_TARGET	= 1,
	TR_ATTACK_POSTION	= 2,
	TR_ATTACK_DIRECTION	= 3,
	TR_IMPROVE			= 4,
	TR_YES_NO			= 5, //by default - no
	TR_NO_YES			= 6, //by default - yes
	TR_HOW_MUCH			= 7, //enter a number betwean to numbers...
	TR_LETTER			= 8, //enrer a letter in range
	TR_STEAL_ITEM		= 9,
	TR_SELECT_ITEM		= 10,
};


enum STDMAP {M_UNKNOWN = 0, M_GREENGRAS, M_GREENTREE, M_SAND, M_WINDOW, M_MAGMA,
M_QUARTZ, M_CAVEFLOOR, M_STONEFLOOR, M_PATH, M_WOODWALL, M_STONEWALL, M_WATER, M_DEEPWATER,
M_HILL, M_LOWMOUNTAIN, M_MOUNTAIN, M_HIGHMOUNTAIN, M_BRIDGE, M_ROAD, M_OBSIDIANFLOOR,
M_FENCE, M_GOLDENFLOOR, M_MARBLEWALL, M_BLACKMARBLEWALL, M_GOLDENFENCE, M_TELEPORTWHITE};


enum RESULT
{
	FAIL		= 0,
	SUCCESS		= 1,
	CONTINUE	= 2,
	ABORT		= 3, 
};


#endif




