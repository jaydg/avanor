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

#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>

#include "magic/effect.h"

/* Forward declaration */
class XCreature;

enum SPELL_NAME {
    SPELL_CURE_LIGHT_WOUNDS,
    SPELL_CURE_SERIOUS_WOUNDS,
    SPELL_CURE_CRITICAL_WOUNDS,
    SPELL_CURE_MORTAL_WOUNDS,
    SPELL_HEAL,
    SPELL_BURNING_HANDS,
    SPELL_ICE_TOUCH,
    SPELL_HEROISM,
    SPELL_DRAIN_LIFE,
    SPELL_IDENTIFY,
    SPELL_MAGIC_ARROW,
    SPELL_FIRE_BOLT,
    SPELL_ICE_BOLT,
    SPELL_LIGHTNING_BOLT,
    SPELL_ACID_BOLT,
    SPELL_SUMMON_MONSTER,
    SPELL_CREATE_ITEM,
    SPELL_CURE_POISON,
    SPELL_CURE_DISEASE,
    SPELL_BLINK,
    SPELL_SELF_KNOWLEDGE,
    SPELL_SEE_INVISIBLE,
    SPELL_ACID_RESISTANCE,
    SPELL_FIRE_RESISTANCE,
    SPELL_COLD_RESISTANCE,
    SPELL_POISON_RESISTANCE,
    SPELL_EOF
};

enum MAGIC_SCHOOL {
    MS_UNKNOWN = -1,
    MS_ELEMENTAL,
    MS_BODY,
    MS_PROTECTION,
    MS_DEATH,
    MS_SURVIVING,
    MS_POWER,
    MS_EOF
};

class XSpell
{
        int cast_count;
        int eff_level;
        SPELL_NAME spell_name;
   public:
        XSpell() = delete;

        explicit XSpell(SPELL_NAME spn);

        [[nodiscard]] SPELL_NAME GetSpellName() const
        {
            return spell_name;
        }

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

        [[nodiscard]] MAGIC_SCHOOL GetSchool() const;

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
CEREAL_LOAD_VIA_PLACEHOLDER_CONSTRUCT(XSpell, serialize, SPELL_CURE_LIGHT_WOUNDS);

class XMagic
{
    protected:
        int magic_level[MS_EOF]{};
        int magic_count[MS_EOF]{};

    public:
        XMagic();
        explicit XMagic(XMagic*) = delete;

        RESULT Cast(XSpell* spell, XCreature* caster);
        static int GetSpellRange(const XSpell* spell, XCreature* caster);
        int Train(MAGIC_SCHOOL school, int count);
        int GainLevel(MAGIC_SCHOOL school, int n = 1);

        std::string LevelToString(MAGIC_SCHOOL school) const;
        void Learn(SPELL_NAME spell);
        [[nodiscard]] XSpell* GetSpell(SPELL_NAME spell) const;

        [[nodiscard]] int GetLevel(const MAGIC_SCHOOL ms) const {
            return magic_level[ms];
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

#endif
