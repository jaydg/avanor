/*
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

#ifndef DEITY_H
#define DEITY_H

#include <cereal/cereal.hpp>
#include <sol/forward.hpp>

class XCreature;
class XItem;

enum DEITY_RELATION {
    DR_FALLEN_CHAMPION, // no way up
    DR_VERY_BAD,        // praying can kill
    DR_BAD,             // you won't be heard
    DR_NORMAL,          // from zero till little more
    DR_ADEPT,           // you can ask some help
    DR_FOLLOWER,        // you can ask for a lot of help
    DR_MESSIAH,         // you can ask for much of help
    DR_CHAMPION,        // you can ask for all help
};

class XDeity
{
    public:
        enum Id {
            LIFE = 0, DEATH = 1, UNKNOWN
        };

        // Registers this enum as the Lua table XDeity.MEMBER.
        static void RegisterLua(sol::state_view& lua);

        static XCreature* life;  // Tiamat
        static XCreature* death; // Murdok
        /*	static XCreature * fire;
        	static XCreature * water;
        	static XCreature * earth;
        	static XCreature * air;
        */
};

enum PRAY {
    PRAY_CURE_LIGHT_WOUNDS,
    PRAY_CURE_CRITICAL_WOUNDS,
    PRAY_RESTORATION,
    PRAY_IDENTIFY,
    PRAY_SELF_KNOWLEDGE,
    PRAY_CURE_POISON,
    PRAY_HEROISM,
    PRAY_TELEPORT,

    PRAY_MINOR_PUNISHMENT,
    PRAY_MINOR_INTERVENTION,
    PRAY_INTERVENTION,
    PRAY_MAJOR_INTERVENTION,
};

struct DEITY_HELP {
    const char* help_name;
    int help_cost;
    PRAY pray;
};

class XReligion
{
    public:
        XReligion() : life_act(0), death_act(0) {}

        // Registers the XReligion usertype (life_act, death_act,
        // SacrificeItem, GetDeityName) - reachable from Lua as
        // AsCreature(cr).religion, since religion is a plain member of the
        // (also Lua-registered) XCreature.
        static void RegisterLua(sol::state_view& lua);

        int life_act;   // killing undead
        int death_act;  // killing anyone, especially with life.
        void KillCreature(XCreature* killer, XCreature* victim);
        int SacrificeItem(XCreature* cr, XItem* item, XDeity::Id deity = XDeity::UNKNOWN);
        [[nodiscard]] DEITY_RELATION GetRelation(XDeity::Id deity) const;
        static const char* GetRelationName(DEITY_RELATION dr);
        static const char* GetDeityName(XDeity::Id deity);
        int GetAvailHelp(XDeity::Id deity, DEITY_HELP** help) const;
        int Pray(XDeity::Id deity, DEITY_HELP* pray, XCreature* prayer);

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(life_act, death_act);
        }
        /*	int fire_act;
        	int water_act;
        	int earth_act;
        	int air_act;
        */
};

#endif
