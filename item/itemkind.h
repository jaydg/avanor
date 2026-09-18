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

#ifndef ITEMKIND_H
#define ITEMKIND_H

#include <sol/forward.hpp>

// Free-standing rather than nested in XItem: XItem's own header pulls in
// both this enum (via ENHANCE_STRUCT in itemdb.h) and BODY_PART (via
// bodypart.h, whose XBodyPart::GetProperKind() also returns ItemKind)
// before class XItem is declared, so nesting it there would make it
// unusable at both of those points. See XItem::RegisterLua() for the
// Lua-facing ItemKind.MEMBER registration - kept next to XItem despite
// the enum itself living here.
//
// `enum class` (not the codebase's usual bare nested enum) per the
// standing "nest into the owning class, or make it an enum class when
// nesting isn't possible" convention - every use site outside this file
// spells enumerators as ItemKind::X. No IM_ prefix on the members: the
// old free enum needed it to avoid polluting the global namespace, but
// enum class already namespaces every member under ItemKind:: in C++
// too now, same reasoning that already dropped the prefix on the
// Lua-facing table members.
enum class ItemKind : unsigned int {
    UNKNOWN = 0x00000000,

    HAT = 0x00000100,
    NECK = 0x00000200,
    BODY = 0x00000400,
    CLOAK = 0x00000800,
    WEAPON = 0x00001000,
    SHIELD = 0x00002000,
    HAND = WEAPON | SHIELD, // for a bodyparts
    GLOVES = 0x00004000,
    RING = 0x00008000,
    BOOTS = 0x00010000,
    MISSILEW = 0x00020000,
    MISSILE = 0x00040000,
    POTION = 0x00100000,
    SCROLL = 0x00200000,
    BOOK = 0x00400000,
    WAND = 0x00800000,
    FOOD = 0x01000000,
    OTHER = 0x02000000, // reuses the bit HERB used to occupy; still used as XBodyPart::GetProperKind()'s BP_UNKNOWN placeholder
    LIGHTSOURCE = 0x04000000,
    TOOL = 0x08000000,
    GEM = 0x10000000,
    MONEY = 0x20000000,
    STACKABLE = 0x40000000, // for spells
    CHEST = 0x80000000,
    ITEM = 0x2FFFFF00, // all items!

    // A group content names as one, the only such member left: the four
    // others (TOHIT, VALUEDICE, VALUEDVPV, VALUEHITDMG) said what a kind
    // of item DOES rather than what it is, and are declared in
    // world/items/kinds.lua now - see the Kind* rules below.
    ARMOUR = HAT | BODY | CLOAK | GLOVES | SHIELD | BOOTS,
    ALL = 0xFFFFFFFF
};

// Combines flags - e.g. ItemKind::BOOTS | ItemKind::GLOVES.
constexpr ItemKind operator|(ItemKind a, ItemKind b)
{
    return static_cast<ItemKind>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
}

// Every `kind & mask` site in this codebase is a truthy intersection
// test (`if (kind & ItemKind::WEAPON)`), never a value kept for further
// bit manipulation - returning bool directly here, instead of the
// conventional same-type ItemKind, means every one of those call sites
// keeps working unchanged, with no separate `!= ItemKind::UNKNOWN`
// needed at each one.
constexpr bool operator&(ItemKind a, ItemKind b)
{
    return (static_cast<unsigned int>(a) & static_cast<unsigned int>(b)) != 0;
}

// What a kind of item does beyond being that kind. Which kinds each of
// these covers used to be a composite member of the enum above -
// ItemKind::TOHIT, ::VALUEDICE and the rest - so a world could not give a
// new sort of item a to-hit bonus, or have it priced by its damage, from
// content. The rules themselves are the engine's (the pricing sums and
// the to-hit stacking are here); which kinds they apply to is content's,
// declared in world/items/kinds.lua.
[[nodiscard]] bool KindTakesToHit(ItemKind kind);
[[nodiscard]] bool KindPricedByDice(ItemKind kind);
[[nodiscard]] bool KindPricedByArmour(ItemKind kind);
[[nodiscard]] bool KindPricedByDamage(ItemKind kind);

// Whether it is armour, which nothing but the item dump asks.
[[nodiscard]] bool KindIsArmour(ItemKind kind);

// One line of world/items/kinds.lua. The kind it names may be a single
// one or several at once, since ItemKind.ARMOUR and its like are still
// what content writes.
class ItemKindRulesBuilder
{
    public:
        explicit ItemKindRulesBuilder(int kind);

        ItemKindRulesBuilder& TakesToHit();
        ItemKindRulesBuilder& PricedByDice();
        ItemKindRulesBuilder& PricedByArmour();
        ItemKindRulesBuilder& PricedByDamage();
        ItemKindRulesBuilder& IsArmour();
        void Register();

    private:
        ItemKind kind;
        ItemKind takes_to_hit{ItemKind::UNKNOWN};
        ItemKind priced_by_dice{ItemKind::UNKNOWN};
        ItemKind priced_by_armour{ItemKind::UNKNOWN};
        ItemKind priced_by_damage{ItemKind::UNKNOWN};
        ItemKind is_armour{ItemKind::UNKNOWN};
};

void RegisterItemKindRulesLua(sol::state_view& lua);

#endif
