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

#define DUR_INFINITE 65535

enum BODY_PART {
    BP_UNKNOWN,
    BP_HEAD,
    BP_NECK,
    BP_BODY,
    BP_CLOAK,
    BP_HAND,
    BP_RING,
    BP_GLOVES,
    BP_BOOTS,
    BP_LIGHT_SOURCE,
    BP_TOOL,
    BP_MISSILE_WEAPON,
    BP_MISSILE,
    BP_EOF,
    BP_OTHER,
    BP_ONE_HANDED,
    BP_TWO_HANDED
};

enum RBP_FLAG {
    RBP_UNKNOWN,
    RBP_BLOCK
};

enum STAIRWAY_TYPE {
    STW_UNKNOWN,
    STW_UP,
    STW_DOWN
};

enum ARTIFACT_STATE {
    ARTIFACT_NO,
    ARTIFACT_YES,
    ARTIFACT_CREATED
};

enum TARGET_REASON {
    TR_NONE,
    TR_ATTACK_TARGET,
    TR_ATTACK_POSITION,
    TR_ATTACK_DIRECTION,
    TR_IMPROVE,
    TR_YES_NO,      // by default - no
    TR_NO_YES,      // by default - yes
    TR_HOW_MUCH,    // enter a number between two numbers
    TR_LETTER,      // enter a letter in range
    TR_STEAL_ITEM,
    TR_SELECT_ITEM,
};

enum STDMAP {
    M_UNKNOWN,
    M_GREENGRAS,
    M_GREENTREE,
    M_SAND,
    M_WINDOW,
    M_MAGMA,
    M_QUARTZ,
    M_CAVEFLOOR,
    M_STONEFLOOR,
    M_PATH,
    M_WOODWALL,
    M_STONEWALL,
    M_WATER,
    M_DEEPWATER,
    M_LAVA,
    M_HILL,
    M_LOWMOUNTAIN,
    M_MOUNTAIN,
    M_HIGHMOUNTAIN,
    M_BRIDGE,
    M_ROAD,
    M_OBSIDIANFLOOR,
    M_FENCE,
    M_GOLDENFLOOR,
    M_MARBLEWALL,
    M_BLACKMARBLEWALL,
    M_GOLDENFENCE,
    M_TELEPORTWHITE
};

enum RESULT {
    FAIL,
    SUCCESS,
    CONTINUE,
    ABORT,
};

#endif
