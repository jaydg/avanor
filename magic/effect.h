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

#ifndef __EFFECT_H
#define __EFFECT_H

#include "global.h"
#include "resist.h"
#include "mod_defs.h"
#include "defs.h"

enum EFFECT_REQ
{
	ER_NONE		= 0,
	ER_TARGET	= 1,
	ER_DIRECTION= 3,
	ER_ITEM		= 4,
};

enum EFFECT
{
	E_NONE						= -1,
	E_CURE_LIGHT_WOUNDS			= 0, //0
	E_CURE_SERIOUS_WOUNDS,
	E_CURE_CRITICAL_WOUNDS,
	E_CURE_MORTAL_WOUNDS,
	E_HEAL,
	E_ULTRAHEAL,
	E_POWER,
	E_ULTRAPOWER,
	E_RESTORATION,
	E_CURE_POISON,
	E_CURE_DISEASE,

	E_BURNING_HANDS,
	E_ICE_TOUCH,
	E_DRAIN_LIFE,

	E_MAGIC_ARROW,
	E_FIRE_BOLT,
	E_ICE_BOLT,
	E_LIGHTNING_BOLT,
	E_ACID_BOLT,

	E_HEROISM,
	E_IDENTIFY,
	E_SUMMON_MONSTER,
	E_CREATE_ITEM,
	E_BLINK,
	E_TELEPORT,
	E_SELF_KNOWLEDGE,
	E_SEE_INVISIBLE,
	E_ACID_RESISTANCE,
	E_FIRE_RESISTANCE,
	E_COLD_RESISTANCE,
	E_POISON_RESISTANCE,
};

class XCreature;
class XItem;
class XLocation;

struct EFFECT_DATA
{
	EFFECT effect;
	XCreature * caller; //in much case effect makes caller, but some time it can be trap.
	XLocation * l; //for much visual effect this field must be filed
	int call_x, call_y; //starting pt of effect (for example for trap)
	XCreature * target; //target creature, usualy not required
	int target_x, target_y; //target position: we can make fire-bolt trap :)
	int power; //power == willpower)
	XItem * item; //some effect need an item...
};

class XEffect
{
	static int Heal(XCreature * caster, int X, int Y, int Z);
	static int Cure(XCreature * caster, int X, int Y, int Z);
	static int Mana(XCreature * caster, int X, int Y, int Z);
	static int Touch(EFFECT_DATA * pData, int X, int Y, int Z, xColor col, RESISTANCE r, char * msg);
	static int Bolt(EFFECT_DATA * pData, int X, int Y, int Z, xColor col, RESISTANCE r, char * msg);
public:
	static int Make(EFFECT_DATA * pData);
	static RESULT Make(XCreature * caster, EFFECT effect, int power);
	static EFFECT_REQ GetReq(EFFECT effect);
	static int GetRange(EFFECT effect, int power);
};

#endif
