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

#include "item/itemdef.h"

void RegisterItemDefEnums(sol::state_view& lua)
{
    lua.new_enum("ITEM_TYPE",
        "IT_UNKNOWN", IT_UNKNOWN,
        "IT_HAT", IT_HAT,
        "IT_CAP", IT_CAP,
        "IT_HELMET", IT_HELMET,
        "IT_AMULET", IT_AMULET,
        "IT_NECKLACE", IT_NECKLACE,
        "IT_RING", IT_RING,
        "IT_CLUB", IT_CLUB,
        "IT_WARHAMMER", IT_WARHAMMER,
        "IT_DAGGER", IT_DAGGER,
        "IT_KNIFE", IT_KNIFE,
        "IT_ORCISHDAGGER", IT_ORCISHDAGGER,
        "IT_LONGDAGGER", IT_LONGDAGGER,
        "IT_SHORTSWORD", IT_SHORTSWORD,
        "IT_LONGSWORD", IT_LONGSWORD,
        "IT_BROADSWORD", IT_BROADSWORD,
        "IT_RAPIER", IT_RAPIER,
        "IT_SCIMITAR", IT_SCIMITAR,
        "IT_KATANA", IT_KATANA,
        "IT_WAKIZASHI", IT_WAKIZASHI,
        "IT_SMALLAXE", IT_SMALLAXE,
        "IT_WARAXE", IT_WARAXE,
        "IT_BATTLEAXE", IT_BATTLEAXE,
        "IT_GREATAXE", IT_GREATAXE,
        "IT_ORCISHAXE", IT_ORCISHAXE,
        "IT_MACE", IT_MACE,
        "IT_FLAIL", IT_FLAIL,
        "IT_SHORTSPEAR", IT_SHORTSPEAR,
        "IT_LONGSPEAR", IT_LONGSPEAR,
        "IT_PITCHFORK", IT_PITCHFORK,
        "IT_PIKE", IT_PIKE,
        "IT_HALBERD", IT_HALBERD,
        "IT_STAFF", IT_STAFF,
        "IT_SHORTBOW", IT_SHORTBOW,
        "IT_LONGBOW", IT_LONGBOW,
        "IT_LIGHTCROSSBOW", IT_LIGHTCROSSBOW,
        "IT_CROSSBOW", IT_CROSSBOW,
        "IT_HEAVYCROSSBOW", IT_HEAVYCROSSBOW,
        "IT_SLING", IT_SLING,
        "IT_GLOVES", IT_GLOVES,
        "IT_GAUNTLETS", IT_GAUNTLETS,
        "IT_KNUCKLES", IT_KNUCKLES,
        "IT_SMALLSHIELD", IT_SMALLSHIELD,
        "IT_MEDIUMSHIELD", IT_MEDIUMSHIELD,
        "IT_LARGESHIELD", IT_LARGESHIELD,
        "IT_TOWERSHIELD", IT_TOWERSHIELD,
        "IT_SANDALS", IT_SANDALS,
        "IT_LIGHTBOOTS", IT_LIGHTBOOTS,
        "IT_SOFTBOOTS", IT_SOFTBOOTS,
        "IT_HARDBOOTS", IT_HARDBOOTS,
        "IT_CLOTHES", IT_CLOTHES,
        "IT_DRESS", IT_DRESS,
        "IT_ROBE", IT_ROBE,
        "IT_LIGHTMAIL", IT_LIGHTMAIL,
        "IT_SCALEMAIL", IT_SCALEMAIL,
        "IT_PLATEMAIL", IT_PLATEMAIL,
        "IT_CHAINMAIL", IT_CHAINMAIL,
        "IT_RINGMAIL", IT_RINGMAIL,
        "IT_CLOAK", IT_CLOAK,
        "IT_SHADOWCLOAK", IT_SHADOWCLOAK,
        "IT_CAPE", IT_CAPE,
        "IT_LIGHTCLOAK", IT_LIGHTCLOAK,
        "IT_FORESTBROTHERCLOAK", IT_FORESTBROTHERCLOAK,
        "IT_TORCH", IT_TORCH,
        "IT_SCROLL", IT_SCROLL,
        "IT_BOOK", IT_BOOK,
        "IT_POTION", IT_POTION,
        "IT_HERB", IT_HERB,
        "IT_LARGERATION", IT_LARGERATION,
        "IT_RATION", IT_RATION,
        "IT_SMALLRATION", IT_SMALLRATION,
        "IT_ELVISHWAYBREAD", IT_ELVISHWAYBREAD,
        "IT_CORPSE", IT_CORPSE,
        "IT_BONE", IT_BONE,
        "IT_RATTAIL", IT_RATTAIL,
        "IT_BATWING", IT_BATWING,
        "IT_ARROW", IT_ARROW,
        "IT_QUARREL", IT_QUARREL,
        "IT_SLINGBULLET", IT_SLINGBULLET,
        "IT_ROCK", IT_ROCK,
        "IT_SHURIKEN", IT_SHURIKEN,
        "IT_COOKINGSET", IT_COOKINGSET,
        "IT_PICKAXE", IT_PICKAXE,
        "IT_ANCIENTMACHINEPART", IT_ANCIENTMACHINEPART,
        "IT_EYEOFRAA", IT_EYEOFRAA,
        "IT_ALCHEMYSET", IT_ALCHEMYSET,
        "IT_CHEST", IT_CHEST,
        "IT_MONEY", IT_MONEY,
        "IT_GEM", IT_GEM,
        "IT_RANDOM", IT_RANDOM,
        "IT_EOF", IT_EOF
    );
}
