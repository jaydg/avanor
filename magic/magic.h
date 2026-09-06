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

#ifndef MAGIC_H
#define MAGIC_H

#include <memory>
#include <vector>

#include <sol/forward.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>

#include "magic/effect.h"

/* Forward declaration */
class XCreature;

// Which spell this is - the id world/spells.lua registered it under. A
// string like every other content id: which spells exist is content.
using SPELL_NAME = std::string;

// "No spell": a trap built from things rather than cast, a request that
// names none. Was SPELL_EOF, which also doubled as the table's size.
inline constexpr const char* SP_NONE = "";

class XSpell;

// A creature's grasp of magic: the spells it knows, and how deeply it
// has studied the schools those spells belong to.
class XMagic
{
    public:
        // The schools the scholars of old divided the Power into. Every
        // spell belongs to exactly one, and a caster is ranked in each
        // one separately.
        enum class School {
            UNKNOWN = -1,
            ELEMENTAL,
            BODY,
            PROTECTION,
            DEATH,
            SURVIVING,
            COUNT       // bounds the per-school tables, never a school
        };

        static constexpr int SCHOOL_COUNT = static_cast<int>(School::COUNT);

        // The highest rank a school can be trained to; mg_level_str[]
        // names every rank from 0 (school unknown) up to this one.
        static constexpr int MAX_LEVEL = 9;

    private:
        // A scoped enum never indexes a table on its own.
        static constexpr int Index(const School school)
        {
            return static_cast<int>(school);
        }

    protected:
        int magic_level[SCHOOL_COUNT]{};
        int magic_count[SCHOOL_COUNT]{};

    public:
        XMagic();
        explicit XMagic(XMagic*) = delete;

        RESULT Cast(XSpell* spell, XCreature* caster);

        // How hard a spell hits and how far it reaches: the caster's
        // willpower, what they know of this particular spell, and what
        // they know of its school as a whole.
        static int GetSpellPower(const XSpell* spell, XCreature* caster);
        static int GetSpellRange(const XSpell* spell, XCreature* caster);

        // Credits `count` towards the school's next rank, the way
        // XCombatSkills::UseSkill() credits a weapon class. Returns 1 if
        // that was enough to gain a rank.
        int Train(School school, int count);
        int GainLevel(School school, int n = 1);

        std::string LevelToString(School school) const;

        // Visible length of a rank's name alone, skipping the color tag
        // LevelToString() bakes in front of it - lets a caller right-pad
        // a line ending in LevelToString() output without having to
        // parse that tag itself.
        static int GetLevelNameLength(int level);

        void Learn(SPELL_NAME spell);
        [[nodiscard]] XSpell* GetSpell(SPELL_NAME spell) const;

        [[nodiscard]] int GetLevel(const School school) const
        {
            return magic_level[Index(school)];
        }

        // Successful casts of this school's spells credited so far
        // towards its next rank - the numerator Train() compares
        // against GetNextLevelAt() below, exposed so the UI can show
        // real progress instead of guessing it.
        [[nodiscard]] int GetCount(const School school) const
        {
            return magic_count[Index(school)];
        }

        // The count GetCount() needs to pass for Train() to grant the
        // next rank - single source of truth for both Train()'s own
        // check and the UI's progress readout.
        [[nodiscard]] int GetNextLevelAt(const School school) const
        {
            return (GetLevel(school) + 1) * 10;
        }

        std::vector<std::unique_ptr<XSpell>> spells;

        // The legacy Store/Restore (since removed) were already
        // entirely stubbed out - this is new real persistence, not a
        // mechanical port.
        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(magic_level, magic_count, spells);
        }
};

// Binds the Spell table the world scripts name their spells through.
void RegisterSpellNameEnum(sol::state_view& lua);

class XSpell
{
        int cast_count;
        int eff_level;
        SPELL_NAME spell_name;
   public:
        XSpell() = delete;

        explicit XSpell(SPELL_NAME spn);

        // What a caster reaches for a spell FOR. The AI picks by this
        // rather than by naming spells one at a time, so a spell added
        // to the table is one every caster can already use - and a spell
        // left out of the table's attack column is one no AI will waste
        // a turn on. HEALING means "restores hit points"; a remedy like
        // cure poison is OTHER, since it does nothing for a creature
        // that is merely hurt.
        enum class Use {
            OTHER,
            ATTACK,
            HEALING,
        };

        [[nodiscard]] SPELL_NAME GetSpellName() const
        {
            return spell_name;
        }

        [[nodiscard]] Use GetUse() const;

        // Whether this caster could actually land the spell on a target
        // that far away right now. A bolt carries as far as its range
        // allows; a touch spell reaches the next square and no further.
        // Asking first is what keeps a creature from announcing a spell
        // every turn and standing still while it fizzles.
        [[nodiscard]] bool CanReach(int distance, int power) const;

        [[nodiscard]] EFFECT GetEffect() const;

        // called after successful casting of spell
        void Cast();

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(spell_name, eff_level, cast_count);
        }

        [[nodiscard]] int GetManaCost() const;

        [[nodiscard]] int GetEffectivity() const
        {
            return eff_level;
        }

        [[nodiscard]] XMagic::School GetSchool() const;

        void GainLevel(const int n = 1)
        {
            eff_level += n;
        }

        std::string toString() const;
        static const char* GetName(SPELL_NAME spn);
        [[nodiscard]] const char* GetName() const
        {
            return GetName(spell_name);
        }
};

// Fluent builder for one spell:
//
//   Spell.new("fire_bolt")
//       :Called("fire bolt")
//       :Effect(XEffect.FIRE_BOLT)
//       :School(MagicSchool.ELEMENTAL)
//       :Cost(10)
//       :Use(SpellUse.ATTACK)
//       :Register()
//
// SPELL_REC stays private to magic.cpp, so the builder holds the fields.
class SpellBuilder
{
    public:
        explicit SpellBuilder(std::string id);

        SpellBuilder& Called(const std::string& n);
        SpellBuilder& Effect(const EFFECT& eff);
        SpellBuilder& School(XMagic::School sch);
        SpellBuilder& Cost(int c);
        SpellBuilder& Use(XSpell::Use u);

        void Register();

    private:
        std::string id;
        std::string name;
        EFFECT effect{EFFECT_NONE};
        XMagic::School school{XMagic::School::UNKNOWN};
        int cost{0};
        XSpell::Use use{XSpell::Use::OTHER};
};

// XSpell() is deleted (real construction always takes a SPELL_NAME) -
// route Cereal's load-time construction through the real constructor
// with a placeholder, same idea as CEREAL_LOAD_VIA_DUMMY_CONSTRUCT but
// for a non-XObject class with no DUMMY_STRUCT support. Not
// polymorphic, so no CEREAL_REGISTER_TYPE needed. Must live here, not
// in magic.cpp: XSpell's construction gets instantiated from inside
// XMagic::serialize(), which itself gets reinstantiated in every TU
// that reaches XCreature::m (e.g. any item's owner weak_ptr<XCreature>
// chain) - a specialization declared only in magic.cpp wouldn't be
// visible to those other TUs.
CEREAL_LOAD_VIA_PLACEHOLDER_CONSTRUCT(XSpell, serialize, SP_NONE);

#endif
