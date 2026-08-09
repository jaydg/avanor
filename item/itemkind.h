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

#ifndef ITEMKIND_H
#define ITEMKIND_H

// Free-standing rather than nested in XItem: XItem's own header pulls in
// both this enum (via ENHANCE_STRUCT in itemdb.h) and BODY_PART (via
// bodypart.h, whose XBodyPart::GetProperKind() also returns ItemKind)
// before class XItem is declared, so nesting it there would make it
// unusable at both of those points. See XItem::RegisterLua() for the
// Lua-facing ItemKind.MEMBER registration - kept next to XItem despite
// the enum itself living here.
enum ItemKind {
    IM_UNKNOWN = 0x00000000,

    IM_HAT = 0x00000100,
    IM_NECK = 0x00000200,
    IM_BODY = 0x00000400,
    IM_CLOAK = 0x00000800,
    IM_WEAPON = 0x00001000,
    IM_SHIELD = 0x00002000,
    IM_HAND = IM_WEAPON | IM_SHIELD, // for a bodyparts
    IM_GLOVES = 0x00004000,
    IM_RING = 0x00008000,
    IM_BOOTS = 0x00010000,
    IM_MISSILEW = 0x00020000,
    IM_MISSILE = 0x00040000,
    IM_POTION = 0x00100000,
    IM_SCROLL = 0x00200000,
    IM_BOOK = 0x00400000,
    IM_WAND = 0x00800000,
    IM_FOOD = 0x01000000,
    IM_OTHER = 0x02000000, // reuses the bit IM_HERB used to occupy; still used as XBodyPart::GetProperKind()'s BP_UNKNOWN placeholder
    IM_LIGHTSOURCE = 0x04000000,
    IM_TOOL = 0x08000000,
    IM_GEM = 0x10000000,
    IM_MONEY = 0x20000000,
    IM_STACKABLE = 0x40000000, // for spells
    IM_CHEST = 0x80000000,
    IM_ITEM = 0x2FFFFF00, // all items!

    IM_TOHIT = IM_HAT | IM_NECK | IM_BODY | IM_CLOAK | IM_GLOVES | IM_SHIELD | IM_BOOTS | IM_RING | IM_WEAPON,
    IM_ARMOUR = IM_HAT | IM_BODY | IM_CLOAK | IM_GLOVES | IM_SHIELD | IM_BOOTS,
    IM_VALUEDICE = IM_WEAPON | IM_MISSILEW | IM_MISSILE,
    IM_VALUEDVPV = IM_HAT | IM_BODY | IM_CLOAK | IM_GLOVES | IM_SHIELD | IM_BOOTS | IM_WEAPON,
    IM_VALUEHITDMG = IM_HAT | IM_BODY | IM_CLOAK | IM_GLOVES | IM_BOOTS | IM_WEAPON,
    IM_ALL = 0xFFFFFFFF
};

#endif
