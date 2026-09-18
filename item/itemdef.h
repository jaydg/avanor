/*
This file is part of "Avanor, the Land of Mystery" roguelike game

Copyright (C) 2000-2006 Vadim Gaidukevich
Copyright (C) 2025,2026 Joachim de Groot

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

// What a thing is made of - "iron", "mithril", "cloth". Filled from
// world/items/materials.lua as that script loads.
struct ItemMaterial {
    std::string id;
    std::string propname; //wooden, metall,
    int color{0};
    std::string dv; //modifers....
    std::string pv;
    std::string hit;
    std::string dice;
    std::string z; //random z to dice;

    // Which sets this material belongs to, so a template asking for
    // "hard_metal" can be given steel, mithril or adamantium.

    int probability{0};
    ITEM_QUALITY iq{IQ_AVG};

    // Multiplies the item's weight; and its value, in tenths.
    int density{1};
    int value{10};

    std::string resistance; //to monster

    // Declared, but nothing carries it onto the item - PropFill() has
    // never copied it, in any version. Kept so the intent survives; see
    // the note in world/items/materials.lua.
    SPECIAL_PROPERTY sp{SPP_NONE};
};

// Which materials a template may be made of - the id of a set declared
// in world/items/materials.lua, or the id of a single material. What
// sets a world has is content: they are nothing but names for groups of
// materials, and the engine only ever asks "which materials does this
// name allow".
using ITEM_SET = std::string;


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
