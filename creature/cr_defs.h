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

#ifndef CR_DEFS_H
#define CR_DEFS_H

#include <string>

#include <sol/forward.hpp>

enum class CreatureClass : unsigned int {
    NONE     = 0x00000000,
    RAT      = 0x00000001,
    FELINE   = 0x00000002,
    CANINE   = 0x00000004,
    REPTILE  = 0x00000008,
    INSECT   = 0x00000010,
    HUMAN    = 0x00000020,
    ORC      = 0x00000040,
    GIANT    = 0x00000080,
    KOBOLD   = 0x00000100,
    UNDEAD   = 0x00000200,
    GOBLIN   = 0x00000400,
    DEMON    = 0x00000800,
    HUMANOID = 0x00001000,
    BLOB     = 0x00002000, // warm mass, ooze
    OTHER    = 0x00004000,
    ALL      = HUMAN | INSECT | KOBOLD | UNDEAD | GOBLIN | REPTILE | FELINE | RAT | CANINE | HUMANOID,
    ALL_IMPL = RAT | FELINE | CANINE | REPTILE | KOBOLD | INSECT | GOBLIN | UNDEAD,
};

// Combines flags - e.g. CreatureClass::RAT | CreatureClass::FELINE.
constexpr CreatureClass operator|(CreatureClass a, CreatureClass b)
{
    return static_cast<CreatureClass>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
}

// Excludes flags - e.g. CreatureClass::ALL ^ CreatureClass::HUMAN, the
// only real use case found for this operator (SetEnemyClass() callers
// building "everything except X").
constexpr CreatureClass operator^(CreatureClass a, CreatureClass b)
{
    return static_cast<CreatureClass>(static_cast<unsigned int>(a) ^ static_cast<unsigned int>(b));
}

// Every `creature_class & mask` site in this codebase is a truthy
// intersection test, never a value kept for further bit manipulation -
// returning bool directly here, instead of the conventional same-type
// CreatureClass, means every one of those call sites keeps working
// unchanged, with no separate `!= CreatureClass::NONE` needed anywhere.
constexpr bool operator&(CreatureClass a, CreatureClass b)
{
    return (static_cast<unsigned int>(a) & static_cast<unsigned int>(b)) != 0;
}

// A monster's identity, everywhere: XCreatureStorage::creature_storage's
// key, XCreature::creature_name/XCorpse::cn (both persisted via Cereal),
// and the id a Lua script uses to define (Monster.new(id)...) or spawn
// (NewCreature(id, ...)) it. Was a fixed CN_EOF=350-slot enum, hand-
// mirrored (108 lines, flagged in a comment as fragile) in world/ids.lua
// to keep the two in sync; a plain string removes that mirror entirely -
// scripts just use the same literal id C++ does, or one of their own for
// the ~340 monsters C++ never needs to name directly.
//
// Only monsters C++ itself references by name - special-cased combat/
// drop logic, or one of the hand-written unique-NPC classes dispatched
// in XCreatureStorage::Create() - get a constant here. constexpr
// const char*, not a CREATURE_NAME/std::string: these are used to
// initialize other namespace-scope statics (XCreatureStorage's unique-
// NPC registry), and a std::string constant would risk the classic
// static-initialization-order-fiasco across translation units: a
// dynamically-initialized const char* has no such ordering, since its
// value doesn't depend on any runtime construction.
using CREATURE_NAME = std::string;

inline constexpr const char* CN_NONE = "";

inline constexpr const char* CN_RAT = "rat";
inline constexpr const char* CN_LARGE_RAT = "large_rat";
inline constexpr const char* CN_BAT = "bat";
inline constexpr const char* CN_HUGE_BAT = "huge_bat";
inline constexpr const char* CN_DOG = "dog";
inline constexpr const char* CN_SKELETON = "skeleton";

inline constexpr const char* CN_SHOPKEEPER = "shopkeeper";

enum CREATURE_EXP {
    CRE_NONE = 0x0001,
    CRE_LOW  = 0x0002, // 1-8
    CRE_AVG  = 0x0004, // 8-16
    CRE_HI   = 0x0008, // 16 - 24
    CRE_ANY  = CRE_LOW | CRE_AVG | CRE_HI
};

enum CARRY_STATE {
    CSTATE_NORMAL     = 0x001, // the normal carrying
    CSTATE_BURDENED   = 0x002,
    CSTATE_STRAINED   = 0x003,
    CSTATE_OVERBURDEN = 0x004,
    CSTATE_DIE        = 0x005,
};

// A free-form tag grouping creatures spawned together (see
// XCreature::groupID/setGroupID, XCreatureGroupMap).
using GROUP_ID = std::string;

// GID_NONE is the only group id that earns a constant: it's a sentinel
// ("this creature isn't in a group") compared at several C++ call sites
// (XCreature::setGroupID/getGroupMembers, XStandardAI::onWasAttacked/
// onDie), so a named symbol documents that repeated meaning.
inline constexpr const char* GID_NONE = "";

enum CREATURE_SIZE {
    CS_VERY_SMALL = 1, // insects like, rats, bats
    CS_SMALL      = 2, // kobold, hobbit like
    CS_NORMAL     = 3, // human like
    CS_LARGE      = 4, // TROLL
    CS_VERY_LARGE = 5, // Titan, dragon
};

enum FOOD_FEELING {
    FF_TOLERANT,
    FF_NORMAL,
    FF_SENSITIVE,
};

enum CR_ATTACK_TYPE {
    CRAT_MELEE_ONLY,
    CRAT_BOTH
};

// Registers CreatureClass as a Lua table. GROUP_ID no longer gets one -
// it's a plain string now, see the comment above it.
void RegisterCrDefsEnums(sol::state_view& lua);

#endif
