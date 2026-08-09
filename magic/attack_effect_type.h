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

#ifndef ATTACK_EFFECT_TYPE_H
#define ATTACK_EFFECT_TYPE_H

#include <sol/forward.hpp>

enum class AttackEffectType : unsigned int {
    NONE = 0x00000000,

    FIRE = 0x00000001,
    HELLFIRE = 0x00000002,
    COLD = 0x00000004,
    ULTIMATECOLD = 0x00000008,
    LIGHTNING = 0x00000010,
    EARTH = 0x00000020,
    ELEMENTAL_MASK = 0x000000FF, //mask for 'elemental'

    ACID = 0x00000100,
    POISON = 0x00000200, //cause poison
    DEATH = 0x00000400, //triple chance to score critical hit
    DISEASE = 0x00000800, //cause disease
    PARALYSE = 0x00001000,
    STUN = 0x00002000,
    CONFUSE = 0x00004000,
    DRAIN_LIFE = 0x00008000,
    BLACK_MASK = 0x0000FF00, //mask for black

    UNDEADSLAYER = 0x00010000, //3x vs undead
    HUMANOIDSLAYER = 0x00020000,
    ANIMALSLAYER = 0x00040000,
    DRAGONSLAYER = 0x00080000,
    GIANTSLAYER = 0x00100000,
    ORCSLAYER = 0x00200000,
    TROLLSLAYER = 0x00400000,
    DEMONSLAYER = 0x00800000,
    SLAYER_MASK = 0x00FF0000,

    HOLYSLAYER = UNDEADSLAYER | DEMONSLAYER,
    EVILSLAYER = DRAGONSLAYER | GIANTSLAYER | ORCSLAYER | TROLLSLAYER,
    ANY = 0xFFFF
};

// Combines flags - e.g. AttackEffectType::FIRE | AttackEffectType::COLD.
constexpr AttackEffectType operator|(AttackEffectType a, AttackEffectType b)
{
    return static_cast<AttackEffectType>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
}

// Unlike ItemKind::operator&, this returns AttackEffectType (the
// conventional same-type result) rather than bool: some call sites need
// the actual masked value, not just a truthy test (XWeapon::toString()'s
// vBitsCount()/GetTemplate() calls on `aet & ELEMENTAL_MASK` etc.) -
// truthy sites append `!= AttackEffectType::NONE` instead.
constexpr AttackEffectType operator&(AttackEffectType a, AttackEffectType b)
{
    return static_cast<AttackEffectType>(static_cast<unsigned int>(a) & static_cast<unsigned int>(b));
}

// Registers AttackEffectType.MEMBER in Lua.
void RegisterAttackEffectTypeLua(sol::state_view& lua);

#endif
