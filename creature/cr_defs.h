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

#ifndef CR_DEFS_H
#define CR_DEFS_H

#include <initializer_list>
#include <string>
#include <vector>

#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>

#include <sol/forward.hpp>

// What sort of creature this is - "rat", "undead", "orc". Which sorts
// exist is content (world/creature_classes.lua).
using CREATURE_CLASS = std::string;

// No class at all.
inline const CREATURE_CLASS CRC_NONE;

// A set of them: whom a brand slays, whom an AI counts as an enemy, what
// a generator may spawn. Holds ids rather than a bit apiece, so content
// may declare as many classes as it likes and a saved game keeps meaning
// what it meant when it was written.
class CreatureClassSet
{
    public:
        CreatureClassSet() = default;

        // The ids themselves, one per entry - CreatureClassSet{"rat", "feline"}.
        CreatureClassSet(std::initializer_list<CREATURE_CLASS> ids);
        explicit CreatureClassSet(std::vector<CREATURE_CLASS> ids);

        [[nodiscard]] bool Has(const CREATURE_CLASS& id) const;
        void Add(const CREATURE_CLASS& id);
        void Add(const CreatureClassSet& other);
        void Remove(const CREATURE_CLASS& id);
        [[nodiscard]] bool Empty() const { return classes.empty(); }

        // The ids in the order they were added, for saying in a message
        // what a set was. A save holds the list itself, not this.
        [[nodiscard]] std::string toString() const;

        auto begin() const { return classes.begin(); }
        auto end() const { return classes.end(); }

        bool operator==(const CreatureClassSet& o) const { return classes == o.classes; }

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(classes);
        }

    private:
        std::vector<CREATURE_CLASS> classes;
};

// What content says about one sort of creature. Nothing here is a rule
// the engine invented: each field replaced a place where the engine
// named a class outright.
struct CreatureClassStats {
    CREATURE_CLASS id;

    // Whether one of these leaves a body behind when it dies. The undead
    // do not - there is nothing left to leave.
    bool leaves_corpse = true;

    // The verb for finishing one off. An undead is destroyed rather than
    // killed.
    std::string slain_verb = "kill";

    // Whether a creature that has been told nothing about whom to fight
    // counts this sort an enemy. Orcs, giants, demons and blobs are not
    // on that list and never have been: they are somebody else's quarrel.
    bool enemy_by_default = false;

    // Whether these are the people a guard is set to protect, rather than
    // something for it to fight.
    bool folk = false;
};

// The row for an id, or nothing if no world file declares one.
const CreatureClassStats* FindCreatureClass(const CREATURE_CLASS& id);

// Every class content has declared, in the order it declared them.
const std::vector<CreatureClassStats>& AllCreatureClasses();

// Whom a creature fights when it has been told nothing, and whom a guard
// fights - everything in the first set that is not one of the folk it
// was posted to protect. Both are gathered from the rows, so a world that
// adds a class decides for itself which side of these it falls on.
CreatureClassSet DefaultEnemies();
CreatureClassSet GuardsEnemies();

class CreatureClassBuilder
{
    public:
        explicit CreatureClassBuilder(CREATURE_CLASS id);

        CreatureClassBuilder& NoCorpse();
        CreatureClassBuilder& Slain(const std::string& verb);
        CreatureClassBuilder& Enemy();
        CreatureClassBuilder& Folk();
        void Register();

    private:
        CreatureClassStats t;
};

void RegisterCreatureClassLua(sol::state_view& lua);

// Complains if no row is registered under this id and returns false.
bool CheckCreatureClassExists(const CREATURE_CLASS& id, const char* where);

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

enum FOOD_FEELING {
    FF_TOLERANT,
    FF_NORMAL,
    FF_SENSITIVE,
};

enum CR_ATTACK_TYPE {
    CRAT_MELEE_ONLY,
    CRAT_BOTH
};

void RegisterCrDefsEnums(sol::state_view& lua);

#endif
