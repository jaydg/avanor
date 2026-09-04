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

struct ItemMaterial {
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
// (see item/itemkind.h): item/itemdb.h's ItemTemplate::it needs it
// before class XItem is declared.
// What sort of thing an item is - "long_sword", "potion", "rat_tail".
//
// A string, not an enum, for the same reason CREATURE_NAME is one: an item
// is content, and content must be able to add a kind of item without
// touching C++. The names live in world/, and reach Lua as the ItemType
// table (item/itemdef.cpp), which is guarded so a typo is an error rather
// than a silent nil.
using ItemType = std::string;

// Named the way CREATURE_NAME's constants are (CN_*, creature/cr_defs.h),
// and constexpr const char* rather than ItemType for the same documented
// reason: a std::string constant at namespace scope risks the
// static-initialization-order fiasco when it seeds another namespace-scope
// static, where a const char* cannot.
//
// "Unspecified": an item that has no type yet, and the request "make me
// one of these, whichever sort you like". These were two enum values,
// UNKNOWN and RANDOM; as a string they are one, because every path that
// met UNKNOWN went on to ask for a random one anyway.
inline constexpr const char* IT_NONE = "";

// The handful C++ still names itself, because it implements each as a
// class of its own or asks about it directly. Every other sort of item is
// content and is named only in world/.
inline constexpr const char* IT_ALCHEMY_SET = "alchemy_set";
inline constexpr const char* IT_AMULET = "amulet";
inline constexpr const char* IT_BOOK = "book";
inline constexpr const char* IT_CHEST = "chest";
inline constexpr const char* IT_COOKING_SET = "cooking_set";
inline constexpr const char* IT_CORPSE = "corpse";
inline constexpr const char* IT_HERB = "herb";
inline constexpr const char* IT_MONEY = "money";
inline constexpr const char* IT_PICKAXE = "pickaxe";
inline constexpr const char* IT_POTION = "potion";
inline constexpr const char* IT_RING = "ring";
inline constexpr const char* IT_SCROLL = "scroll";

// Registers ItemType as Lua table

#endif
