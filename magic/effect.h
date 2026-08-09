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

#ifndef EFFECT_H
#define EFFECT_H

#include <sol/forward.hpp>

#include "engine/global.h"
#include "item/itemdef.h"

enum EFFECT_REQ {
    ER_NONE	= 0,
    ER_TARGET	= 1,
    ER_DIRECTION = 3,
    ER_ITEM	= 4,
};

class XCreature;
class XItem;
class XLocation;
struct EFFECT_DATA;

class XEffect
{
    public:
        enum Id {
            NONE	= -1,
            CURE_LIGHT_WOUNDS	= 0, //0
            CURE_SERIOUS_WOUNDS,
            CURE_CRITICAL_WOUNDS,
            CURE_MORTAL_WOUNDS,
            HEAL,
            ULTRAHEAL,
            POWER,
            ULTRAPOWER,
            RESTORATION,
            CURE_POISON,
            CURE_DISEASE,

            BURNING_HANDS,
            ICE_TOUCH,
            DRAIN_LIFE,

            MAGIC_ARROW,
            FIRE_BOLT,
            ICE_BOLT,
            LIGHTNING_BOLT,
            ACID_BOLT,

            HEROISM,
            IDENTIFY,
            GREAT_IDENTIFY,
            SUMMON_MONSTER,
            CREATE_ITEM,
            BLINK,
            TELEPORT,
            SELF_KNOWLEDGE,
            SEE_INVISIBLE,
            ACID_RESISTANCE,
            FIRE_RESISTANCE,
            COLD_RESISTANCE,
            POISON_RESISTANCE,
        };

        // Registers this enum as the Lua table XEffect.MEMBER.
        static void RegisterLua(sol::state_view& lua);

    private:
        static int Heal(XCreature * caster, int X, int Y, int Z);
        static int Cure(XCreature * caster, int X, int Y, int Z);
        static int Mana(XCreature * caster, int X, int Y, int Z);
        static int Touch(const EFFECT_DATA* pData, int X, int Y, int Z, xColor col, BRAND_TYPE brt, const char* msg);
        static int Bolt(const EFFECT_DATA* pData, int X, int Y, int Z, xColor col, BRAND_TYPE brt, const char* msg);
    public:
        static int Make(const EFFECT_DATA* pData);
        static RESULT Make(XCreature * caster, Id effect, int power);
        static EFFECT_REQ GetReq(Id effect);
        static int GetRange(Id effect, int power);
};

struct EFFECT_DATA {
    XEffect::Id effect;

    // In many case effects fill caller, but not when cause by a trap.
    XCreature* caller;

    // For many visual effects this field must be filled.
    XLocation* l;

    // Starting point of effect (for example for trap).
    int call_x, call_y;

    // Target creature, usually not required
    XCreature* target;

    // target position: we can make fire-bolt traps :)
    int target_x, target_y;

    // power == willpower
    int power;
};

#endif
