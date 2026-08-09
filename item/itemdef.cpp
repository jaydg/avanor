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
    lua.new_enum("ItemType",
        "UNKNOWN", ItemType::UNKNOWN,
        "HAT", ItemType::HAT,
        "CAP", ItemType::CAP,
        "HELMET", ItemType::HELMET,
        "AMULET", ItemType::AMULET,
        "NECKLACE", ItemType::NECKLACE,
        "RING", ItemType::RING,
        "CLUB", ItemType::CLUB,
        "WARHAMMER", ItemType::WARHAMMER,
        "DAGGER", ItemType::DAGGER,
        "KNIFE", ItemType::KNIFE,
        "ORCISHDAGGER", ItemType::ORCISHDAGGER,
        "LONGDAGGER", ItemType::LONGDAGGER,
        "SHORTSWORD", ItemType::SHORTSWORD,
        "LONGSWORD", ItemType::LONGSWORD,
        "BROADSWORD", ItemType::BROADSWORD,
        "RAPIER", ItemType::RAPIER,
        "SCIMITAR", ItemType::SCIMITAR,
        "KATANA", ItemType::KATANA,
        "WAKIZASHI", ItemType::WAKIZASHI,
        "SMALLAXE", ItemType::SMALLAXE,
        "WARAXE", ItemType::WARAXE,
        "BATTLEAXE", ItemType::BATTLEAXE,
        "GREATAXE", ItemType::GREATAXE,
        "ORCISHAXE", ItemType::ORCISHAXE,
        "MACE", ItemType::MACE,
        "FLAIL", ItemType::FLAIL,
        "SHORTSPEAR", ItemType::SHORTSPEAR,
        "LONGSPEAR", ItemType::LONGSPEAR,
        "PITCHFORK", ItemType::PITCHFORK,
        "PIKE", ItemType::PIKE,
        "HALBERD", ItemType::HALBERD,
        "STAFF", ItemType::STAFF,
        "SHORTBOW", ItemType::SHORTBOW,
        "LONGBOW", ItemType::LONGBOW,
        "LIGHTCROSSBOW", ItemType::LIGHTCROSSBOW,
        "CROSSBOW", ItemType::CROSSBOW,
        "HEAVYCROSSBOW", ItemType::HEAVYCROSSBOW,
        "SLING", ItemType::SLING,
        "GLOVES", ItemType::GLOVES,
        "GAUNTLETS", ItemType::GAUNTLETS,
        "KNUCKLES", ItemType::KNUCKLES,
        "SMALLSHIELD", ItemType::SMALLSHIELD,
        "MEDIUMSHIELD", ItemType::MEDIUMSHIELD,
        "LARGESHIELD", ItemType::LARGESHIELD,
        "TOWERSHIELD", ItemType::TOWERSHIELD,
        "SANDALS", ItemType::SANDALS,
        "LIGHTBOOTS", ItemType::LIGHTBOOTS,
        "SOFTBOOTS", ItemType::SOFTBOOTS,
        "HARDBOOTS", ItemType::HARDBOOTS,
        "CLOTHES", ItemType::CLOTHES,
        "DRESS", ItemType::DRESS,
        "ROBE", ItemType::ROBE,
        "LIGHTMAIL", ItemType::LIGHTMAIL,
        "SCALEMAIL", ItemType::SCALEMAIL,
        "PLATEMAIL", ItemType::PLATEMAIL,
        "CHAINMAIL", ItemType::CHAINMAIL,
        "RINGMAIL", ItemType::RINGMAIL,
        "CLOAK", ItemType::CLOAK,
        "SHADOWCLOAK", ItemType::SHADOWCLOAK,
        "CAPE", ItemType::CAPE,
        "LIGHTCLOAK", ItemType::LIGHTCLOAK,
        "FORESTBROTHERCLOAK", ItemType::FORESTBROTHERCLOAK,
        "TORCH", ItemType::TORCH,
        "SCROLL", ItemType::SCROLL,
        "BOOK", ItemType::BOOK,
        "POTION", ItemType::POTION,
        "HERB", ItemType::HERB,
        "LARGERATION", ItemType::LARGERATION,
        "RATION", ItemType::RATION,
        "SMALLRATION", ItemType::SMALLRATION,
        "ELVISHWAYBREAD", ItemType::ELVISHWAYBREAD,
        "CORPSE", ItemType::CORPSE,
        "BONE", ItemType::BONE,
        "RATTAIL", ItemType::RATTAIL,
        "BATWING", ItemType::BATWING,
        "ARROW", ItemType::ARROW,
        "QUARREL", ItemType::QUARREL,
        "SLINGBULLET", ItemType::SLINGBULLET,
        "ROCK", ItemType::ROCK,
        "SHURIKEN", ItemType::SHURIKEN,
        "COOKINGSET", ItemType::COOKINGSET,
        "PICKAXE", ItemType::PICKAXE,
        "ANCIENTMACHINEPART", ItemType::ANCIENTMACHINEPART,
        "EYEOFRAA", ItemType::EYEOFRAA,
        "ALCHEMYSET", ItemType::ALCHEMYSET,
        "CHEST", ItemType::CHEST,
        "MONEY", ItemType::MONEY,
        "GEM", ItemType::GEM,
        "RANDOM", ItemType::RANDOM,
        "ALL", ItemType::ALL
    );
}
