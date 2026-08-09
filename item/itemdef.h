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

#ifndef ITEMDEF_H
#define ITEMDEF_H

#include <string>

#include <sol/forward.hpp>

enum CAN_FLAG {
    CAN_UNKNOWN	= 0x0000,

    CAN_BROKE	= 0x0001,
    CAN_RUST	= 0x0002,
    CAN_CORD	= 0x0004,
    CAN_DUST	= 0x0008,
    CAN_BURN	= 0x0010,
    CAN_MELT	= 0x0020,
    CAN_FROST	= 0x0040,
    CAN_LEATHERLIKE	= CAN_BURN,
    CAN_GOODMETALLIKE	= CAN_BROKE | CAN_MELT,
    CAN_BLACKMETALLIKE	= CAN_GOODMETALLIKE | CAN_CORD | CAN_RUST,
    CAN_WOODLIKE	= CAN_BROKE | CAN_BURN,
    CAN_STONELIKE	= CAN_DUST,
    CAN_POTIONLIKE	= CAN_FROST | CAN_DUST,
    CAN_PAPERLIKE	= CAN_BURN,
    CAN_FIRERESIST	= 0xffff ^ (CAN_BURN | CAN_MELT),
    CAN_COLDRESIST	= 0xffff ^ (CAN_FROST),
    CAN_ACIDRESIST	= 0xffff ^ (CAN_CORD),
    CAN_RESISTNONE	= 0xffff,
    CAN_NICEMETALLIKE	= 0,
    CAN_RESISTALL	= 0
};

enum SPECIAL_PROPERTY {
    SPP_NONE	= 0x0000,
    SPP_FASTDIGESTION	= 0x0001,
    SPP_SLOWDIGESTION	= 0x0002,
    SPP_INVISIBLE	= 0x0004,
    SPP_REGENERATION	= 0x0008
};

enum ITEM_QUALITY {
    IQ_TERRIBLE	= -4,
    IQ_VERY_BAD	= -3,
    IQ_BAD	= -2,
    IQ_POOR	= -1,
    IQ_AVG	= 0,
    IQ_FAIR	= 1,
    IQ_GOOD	= 2,
    IQ_EXCELLENT	= 3,
    IQ_SUPERB	= 4,
};

struct _ITEMPROP {
    std::string propname; //wooden, metall,
    int color;
    const char* dv; //modifers....
    const char* pv;
    const char* hit;
    const char* dice;
    const char* z; //random z to dice;
    const char* r; //range
    unsigned int iflag; //wich item can consist from this material
    int probability;
    ITEM_QUALITY iq;
    int density;
    int value;
    CAN_FLAG mflag;
    const char* resistance; //to monster
    SPECIAL_PROPERTY sp;
};

enum ITEM_SET {
    ISET_CLOTH	= 0x00000001,
    ISET_LEATHER	= 0x00000002,
    ISET_STUDEDLEATHER	= 0x00000004,
    ISET_STONE	= 0x00000008,
    ISET_WOOD	= 0x00000010,
    ISET_IRON	= 0x00000020,
    ISET_BRONZE	= 0x00000040,
    ISET_BRASS	= 0x00000080,
    ISET_SILVER	= 0x00000100,
    ISET_GOLD	= 0x00000200,
    ISET_CRYSTAL	= 0x00000400,
    ISET_STEEL	= 0x00000800,
    ISET_OBSIDIAN	= 0x00001000,
    ISET_MITHRIL	= 0x00002000,
    ISET_ADAMANTIUM	= 0x00004000,

    ISET_SOFT	= ISET_LEATHER | ISET_STUDEDLEATHER | ISET_CLOTH,
    ISET_ALLLEATHER	= ISET_LEATHER | ISET_STUDEDLEATHER,
    ISET_METAL	= ISET_IRON | ISET_BRONZE | ISET_BRASS | ISET_SILVER | ISET_GOLD,
    ISET_METALSOFT	= ISET_METAL | ISET_SOFT,
    ISET_HARDMETAL	= ISET_STEEL | ISET_MITHRIL | ISET_ADAMANTIUM,
    ISET_ALLMETAL	= ISET_METAL | ISET_HARDMETAL | ISET_OBSIDIAN,
    ISET_OBSIMETAL	= ISET_OBSIDIAN | ISET_HARDMETAL,
    ISET_STONEFROM	= ISET_STONE | ISET_CRYSTAL | ISET_OBSIDIAN,
    ISET_WOODEN	= ISET_WOOD, //wooden
    ISET_SHIELD	= ISET_ALLLEATHER | ISET_ALLMETAL | ISET_WOODEN, //for shields
    ISET_BOW	= ISET_WOODEN,
    ISET_MISSILE	= ISET_ALLMETAL | ISET_WOODEN | ISET_STONE,
    ISET_WOODSTONE	= ISET_STONEFROM | ISET_WOODEN,
    ISET_WEAPON	= ISET_STONEFROM | ISET_HARDMETAL,
    ISET_SIMPLEWEAPON = ISET_IRON | ISET_STEEL,
    ISET_CROWNMETAL	= ISET_STEEL | ISET_SILVER | ISET_GOLD | ISET_MITHRIL,
    ISET_BLACKMETAL	= ISET_STEEL | ISET_IRON,
    ISET_OTHER	= 0x0,
    ISET_ALL	= 0x00000fff | ISET_MITHRIL | ISET_ADAMANTIUM,
};

// Free-standing rather than nested in XItem, same reason as ItemKind
// (see item/itemkind.h): item/itemdb.h's _MAIN_ITEM_STRUCT::it needs it
// before class XItem is declared.
enum class ItemType {
    UNKNOWN = 0,
    HAT, CAP, HELMET,
    AMULET, NECKLACE,
    RING,
    CLUB, WARHAMMER,
    DAGGER, KNIFE, ORCISHDAGGER, LONGDAGGER,
    SHORTSWORD, LONGSWORD, BROADSWORD, RAPIER, SCIMITAR, KATANA, WAKIZASHI,
    SMALLAXE, WARAXE, BATTLEAXE, GREATAXE, ORCISHAXE,
    MACE, FLAIL,
    SHORTSPEAR, LONGSPEAR, PITCHFORK, PIKE, HALBERD,
    STAFF,
    SHORTBOW, LONGBOW, LIGHTCROSSBOW, CROSSBOW, HEAVYCROSSBOW, SLING,
    GLOVES, GAUNTLETS, KNUCKLES,
    SMALLSHIELD, MEDIUMSHIELD, LARGESHIELD, TOWERSHIELD,
    SANDALS, LIGHTBOOTS, SOFTBOOTS, HARDBOOTS,
    CLOTHES, DRESS, ROBE, LIGHTMAIL, SCALEMAIL, PLATEMAIL, CHAINMAIL, RINGMAIL,
    CLOAK, SHADOWCLOAK, CAPE, LIGHTCLOAK, FORESTBROTHERCLOAK,
    TORCH,
    SCROLL,
    BOOK,
    POTION,
    HERB,
    LARGERATION, RATION, SMALLRATION, ELVISHWAYBREAD, CORPSE, BONE, RATTAIL, BATWING,
    ARROW, QUARREL, SLINGBULLET, ROCK, SHURIKEN,
    COOKINGSET, PICKAXE, ANCIENTMACHINEPART, EYEOFRAA, ALCHEMYSET,
    CHEST,
    MONEY,
    GEM,
    RANDOM,
    ALL // was IT_EOF - "EOF" collides with the <cstdio> macro of the same name
};

// Registers ItemType as Lua table
void RegisterItemDefEnums(sol::state_view& lua);

#endif
