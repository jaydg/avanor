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

#include <sol/sol.hpp>

#include "creature/cr_defs.h"

void RegisterCrDefsEnums(sol::state_view& lua)
{
    lua.new_enum("CREATURE_CLASS",
        "CR_RAT", CR_RAT,
        "CR_FELINE", CR_FELINE,
        "CR_CANINE", CR_CANINE,
        "CR_REPTILE", CR_REPTILE,
        "CR_INSECT", CR_INSECT,
        "CR_HUMAN", CR_HUMAN,
        "CR_ORC", CR_ORC,
        "CR_GIANT", CR_GIANT,
        "CR_KOBOLD", CR_KOBOLD,
        "CR_UNDEAD", CR_UNDEAD,
        "CR_GOBLIN", CR_GOBLIN,
        "CR_DEMON", CR_DEMON,
        "CR_HUMANOID", CR_HUMANOID,
        "CR_BLOB", CR_BLOB
    );

    lua.new_enum("CREATURE_LEVEL",
        "CRL_VERY_LOW", CRL_VERY_LOW,
        "CRL_LOW", CRL_LOW,
        "CRL_ABOVE_LOW", CRL_ABOVE_LOW,
        "CRL_AVG", CRL_AVG,
        "CRL_ABOVE_AVG", CRL_ABOVE_AVG,
        "CRL_HI", CRL_HI,
        "CRL_ABOVE_HI", CRL_ABOVE_HI,
        "CRL_VERY_HI", CRL_VERY_HI,
        "CRL_EXTREM_HI", CRL_EXTREM_HI,
        "CRL_UNIQUE", CRL_UNIQUE,
        "CRL_ANY", CRL_ANY,
        "CRL_VL", CRL_VL,
        "CRL_LA", CRL_LA,
        "CRL_AH", CRL_AH,
        "CRL_HVH", CRL_HVH
    );

    lua.new_enum("GROUP_ID",
        "GID_ORCS_WAR_PARTY", GID_ORCS_WAR_PARTY,
        "GID_FOREST_BROTHER", GID_FOREST_BROTHER,
        "GID_GUARDIAN", GID_GUARDIAN,
        "GID_SMALL_VILLAGE_FARMER", GID_SMALL_VILLAGE_FARMER,
        "GID_TOWNEE_1", GID_TOWNEE_1,
        "GID_DWARVEN_GUARDIAN", GID_DWARVEN_GUARDIAN,
        "GID_AHKULAN_GUARDIAN", GID_AHKULAN_GUARDIAN,
        "GID_RODERICK_GUARDIAN", GID_RODERICK_GUARDIAN
    );
}
