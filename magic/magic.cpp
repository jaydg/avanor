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

#include <algorithm>
#include <iterator>

#include <fmt/format.h>
#include <cereal/archives/json.hpp>
#include <sol/sol.hpp>

#include "creature/creature.h"
#include "helpers/msgwin.h"
#include "magic/magic.h"

// The school table below names one of these on almost every line.
using School = XMagic::School;

void RegisterSpellNameEnum(sol::state_view& lua)
{
    // The names a creature definition uses for the spells it knows.
    // SPELL_EOF is the end marker of the enum, not a spell, so it is
    // deliberately absent - a script asking for Spell.EOF should fail.
    lua.new_enum("Spell",
        "CURE_LIGHT_WOUNDS", SPELL_CURE_LIGHT_WOUNDS,
        "CURE_SERIOUS_WOUNDS", SPELL_CURE_SERIOUS_WOUNDS,
        "CURE_CRITICAL_WOUNDS", SPELL_CURE_CRITICAL_WOUNDS,
        "CURE_MORTAL_WOUNDS", SPELL_CURE_MORTAL_WOUNDS,
        "HEAL", SPELL_HEAL,
        "BURNING_HANDS", SPELL_BURNING_HANDS,
        "ICE_TOUCH", SPELL_ICE_TOUCH,
        "HEROISM", SPELL_HEROISM,
        "DRAIN_LIFE", SPELL_DRAIN_LIFE,
        "IDENTIFY", SPELL_IDENTIFY,
        "MAGIC_ARROW", SPELL_MAGIC_ARROW,
        "FIRE_BOLT", SPELL_FIRE_BOLT,
        "ICE_BOLT", SPELL_ICE_BOLT,
        "LIGHTNING_BOLT", SPELL_LIGHTNING_BOLT,
        "ACID_BOLT", SPELL_ACID_BOLT,
        "SUMMON_MONSTER", SPELL_SUMMON_MONSTER,
        "CREATE_ITEM", SPELL_CREATE_ITEM,
        "CURE_POISON", SPELL_CURE_POISON,
        "CURE_DISEASE", SPELL_CURE_DISEASE,
        "BLINK", SPELL_BLINK,
        "SELF_KNOWLEDGE", SPELL_SELF_KNOWLEDGE,
        "SEE_INVISIBLE", SPELL_SEE_INVISIBLE,
        "ACID_RESISTANCE", SPELL_ACID_RESISTANCE,
        "FIRE_RESISTANCE", SPELL_FIRE_RESISTANCE,
        "COLD_RESISTANCE", SPELL_COLD_RESISTANCE,
        "POISON_RESISTANCE", SPELL_POISON_RESISTANCE
    );
}

struct SPELL_REC {
    XEffect::Id effect;
    School school;
    int cost;
    std::string name;
    SPELL_REC();
};

SPELL_REC spell_db[SPELL_EOF];

SPELL_REC::SPELL_REC()
{
    spell_db[SPELL_CURE_LIGHT_WOUNDS].effect	= XEffect::CURE_LIGHT_WOUNDS;
    spell_db[SPELL_CURE_LIGHT_WOUNDS].school	= School::BODY;
    spell_db[SPELL_CURE_LIGHT_WOUNDS].cost	= 5;
    spell_db[SPELL_CURE_LIGHT_WOUNDS].name	= "cure light wounds";

    spell_db[SPELL_CURE_SERIOUS_WOUNDS].effect	= XEffect::CURE_SERIOUS_WOUNDS;
    spell_db[SPELL_CURE_SERIOUS_WOUNDS].school	= School::BODY;
    spell_db[SPELL_CURE_SERIOUS_WOUNDS].cost	= 10;
    spell_db[SPELL_CURE_SERIOUS_WOUNDS].name	= "cure serious wounds";

    spell_db[SPELL_CURE_CRITICAL_WOUNDS].effect = XEffect::CURE_CRITICAL_WOUNDS;
    spell_db[SPELL_CURE_CRITICAL_WOUNDS].school = School::BODY;
    spell_db[SPELL_CURE_CRITICAL_WOUNDS].cost	= 15;
    spell_db[SPELL_CURE_CRITICAL_WOUNDS].name	= "cure critical wounds";

    spell_db[SPELL_CURE_MORTAL_WOUNDS].effect	= XEffect::CURE_MORTAL_WOUNDS;
    spell_db[SPELL_CURE_MORTAL_WOUNDS].school	= School::BODY;
    spell_db[SPELL_CURE_MORTAL_WOUNDS].cost	= 20;
    spell_db[SPELL_CURE_MORTAL_WOUNDS].name	= "cure mortal wounds";

    spell_db[SPELL_HEAL].effect	= XEffect::HEAL;
    spell_db[SPELL_HEAL].school	= School::BODY;
    spell_db[SPELL_HEAL].cost	= 30;
    spell_db[SPELL_HEAL].name	= "heal";

    spell_db[SPELL_HEROISM].effect	= XEffect::HEROISM;
    spell_db[SPELL_HEROISM].school	= School::BODY;
    spell_db[SPELL_HEROISM].cost	= 5;
    spell_db[SPELL_HEROISM].name	= "heroism";

    spell_db[SPELL_CURE_POISON].effect	= XEffect::CURE_POISON;
    spell_db[SPELL_CURE_POISON].school	= School::BODY;
    spell_db[SPELL_CURE_POISON].cost	= 15;
    spell_db[SPELL_CURE_POISON].name	= "cure poison";

    spell_db[SPELL_CURE_DISEASE].effect	= XEffect::CURE_DISEASE;
    spell_db[SPELL_CURE_DISEASE].school	= School::BODY;
    spell_db[SPELL_CURE_DISEASE].cost	= 20;
    spell_db[SPELL_CURE_DISEASE].name	= "cure disease";

    spell_db[SPELL_BURNING_HANDS].effect	= XEffect::BURNING_HANDS;
    spell_db[SPELL_BURNING_HANDS].school	= School::ELEMENTAL;
    spell_db[SPELL_BURNING_HANDS].cost	= 7;
    spell_db[SPELL_BURNING_HANDS].name	= "burning hands";

    spell_db[SPELL_ICE_TOUCH].effect	= XEffect::ICE_TOUCH;
    spell_db[SPELL_ICE_TOUCH].school	= School::ELEMENTAL;
    spell_db[SPELL_ICE_TOUCH].cost	= 7;
    spell_db[SPELL_ICE_TOUCH].name	= "ice touch";

    spell_db[SPELL_MAGIC_ARROW].effect	= XEffect::MAGIC_ARROW;
    spell_db[SPELL_MAGIC_ARROW].school	= School::ELEMENTAL;
    spell_db[SPELL_MAGIC_ARROW].cost	= 5;
    spell_db[SPELL_MAGIC_ARROW].name	= "magic arrow";

    spell_db[SPELL_FIRE_BOLT].effect	= XEffect::FIRE_BOLT;
    spell_db[SPELL_FIRE_BOLT].school	= School::ELEMENTAL;
    spell_db[SPELL_FIRE_BOLT].cost	= 12;
    spell_db[SPELL_FIRE_BOLT].name	= "fire bolt";

    spell_db[SPELL_ICE_BOLT].effect	= XEffect::ICE_BOLT;
    spell_db[SPELL_ICE_BOLT].school	= School::ELEMENTAL;
    spell_db[SPELL_ICE_BOLT].cost	= 12;
    spell_db[SPELL_ICE_BOLT].name	= "ice bolt";

    spell_db[SPELL_LIGHTNING_BOLT].effect	= XEffect::LIGHTNING_BOLT;
    spell_db[SPELL_LIGHTNING_BOLT].school	= School::ELEMENTAL;
    spell_db[SPELL_LIGHTNING_BOLT].cost	= 18;
    spell_db[SPELL_LIGHTNING_BOLT].name	= "lightning bolt";

    spell_db[SPELL_DRAIN_LIFE].effect	= XEffect::DRAIN_LIFE;
    spell_db[SPELL_DRAIN_LIFE].school	= School::DEATH;
    spell_db[SPELL_DRAIN_LIFE].cost	= 7;
    spell_db[SPELL_DRAIN_LIFE].name	= "drain life";

    spell_db[SPELL_ACID_BOLT].effect	= XEffect::ACID_BOLT;
    spell_db[SPELL_ACID_BOLT].school	= School::DEATH;
    spell_db[SPELL_ACID_BOLT].cost	= 25;
    spell_db[SPELL_ACID_BOLT].name	= "acid bolt";

    spell_db[SPELL_IDENTIFY].effect	= XEffect::IDENTIFY;
    spell_db[SPELL_IDENTIFY].school	= School::SURVIVING;
    spell_db[SPELL_IDENTIFY].cost	= 25;
    spell_db[SPELL_IDENTIFY].name	= "identify";

    spell_db[SPELL_SUMMON_MONSTER].effect	= XEffect::SUMMON_MONSTER;
    spell_db[SPELL_SUMMON_MONSTER].school	= School::SURVIVING;
    spell_db[SPELL_SUMMON_MONSTER].cost	= 15;
    spell_db[SPELL_SUMMON_MONSTER].name	= "summon monster";

    spell_db[SPELL_CREATE_ITEM].effect	= XEffect::CREATE_ITEM;
    spell_db[SPELL_CREATE_ITEM].school	= School::SURVIVING;
    spell_db[SPELL_CREATE_ITEM].cost	= 100;
    spell_db[SPELL_CREATE_ITEM].name	= "create item";

    spell_db[SPELL_BLINK].effect	= XEffect::BLINK;
    spell_db[SPELL_BLINK].school	= School::SURVIVING;
    spell_db[SPELL_BLINK].cost	= 15;
    spell_db[SPELL_BLINK].name	= "blink";

    spell_db[SPELL_SELF_KNOWLEDGE].effect	= XEffect::SELF_KNOWLEDGE;
    spell_db[SPELL_SELF_KNOWLEDGE].school	= School::SURVIVING;
    spell_db[SPELL_SELF_KNOWLEDGE].cost	= 25;
    spell_db[SPELL_SELF_KNOWLEDGE].name	= "self knowledge";

    spell_db[SPELL_SEE_INVISIBLE].effect	= XEffect::SEE_INVISIBLE;
    spell_db[SPELL_SEE_INVISIBLE].school	= School::PROTECTION;
    spell_db[SPELL_SEE_INVISIBLE].cost	= 15;
    spell_db[SPELL_SEE_INVISIBLE].name	= "see invisible";

    spell_db[SPELL_ACID_RESISTANCE].effect	= XEffect::ACID_RESISTANCE;
    spell_db[SPELL_ACID_RESISTANCE].school	= School::PROTECTION;
    spell_db[SPELL_ACID_RESISTANCE].cost	= 20;
    spell_db[SPELL_ACID_RESISTANCE].name	= "acid resistance";

    spell_db[SPELL_FIRE_RESISTANCE].effect	= XEffect::FIRE_RESISTANCE;
    spell_db[SPELL_FIRE_RESISTANCE].school	= School::PROTECTION;
    spell_db[SPELL_FIRE_RESISTANCE].cost	= 15;
    spell_db[SPELL_FIRE_RESISTANCE].name	= "fire resistance";

    spell_db[SPELL_COLD_RESISTANCE].effect	= XEffect::COLD_RESISTANCE;
    spell_db[SPELL_COLD_RESISTANCE].school	= School::PROTECTION;
    spell_db[SPELL_COLD_RESISTANCE].cost	= 15;
    spell_db[SPELL_COLD_RESISTANCE].name	= "cold resistance";

    spell_db[SPELL_POISON_RESISTANCE].effect	= XEffect::POISON_RESISTANCE;
    spell_db[SPELL_POISON_RESISTANCE].school	= School::PROTECTION;
    spell_db[SPELL_POISON_RESISTANCE].cost	= 15;
    spell_db[SPELL_POISON_RESISTANCE].name	= "poison resistance";
}

XSpell::XSpell(const SPELL_NAME spn)
{
    spell_name = spn;
    eff_level = 1;
    cast_count = 0;
}

const char* XSpell::GetName(const SPELL_NAME spn)
{
    return spell_db[spn].name.c_str();
}

void XSpell::Cast()
{
    if (++cast_count >= eff_level) {
        cast_count = 0;
        GainLevel();
    }
}

XEffect::Id XSpell::GetEffect() const
{
    return spell_db[spell_name].effect;
}

int XSpell::GetManaCost() const
{
    return (spell_db[spell_name].cost * 15) / (14 + eff_level);
}

XMagic::School XSpell::GetSchool() const
{
    return spell_db[spell_name].school;
}

std::string XSpell::toString() const
{
    return fmt::format("<VALUE>{:<21s} <TEXT>: <VALUE>{}<TEXT>pp  (Eff - {}) (to next level: {})",
        spell_db[spell_name].name,
        GetManaCost(),
        GetEffectivity(),
        eff_level - cast_count);
}

XMagic::XMagic()
{
    // Every school starts unknown; learning a spell opens its school
    // (see Learn()), so the school list shows only what the caster
    // actually practises.
}

int XMagic::GetSpellPower(const XSpell* spell, XCreature* caster)
{
    return caster->GetStats(XStats::WIL)
        + spell->GetEffectivity()
        + caster->m->GetLevel(spell->GetSchool());
}

int XMagic::GetSpellRange(const XSpell* spell, XCreature* caster)
{
    return XEffect::GetRange(spell->GetEffect(), GetSpellPower(spell, caster));
}

RESULT XMagic::Cast(XSpell* spell, XCreature* caster)
{
    const int power = GetSpellPower(spell, caster);

    if (caster->PP - spell->GetManaCost() >= 0) {
        if (caster->isInVisibleArea() && !caster->isHero()) {
            msgwin.Add(fmt::format("{} {} {}.",
                caster->GetNameEx(CRN_T1),
                caster->GetVerb("cast"),
                spell->GetName()));
        }

        const int res = XEffect::Make(caster, spell->GetEffect(), power);

        if (res != ABORT) {
            caster->PP -= spell->GetManaCost();
        }

        if (res == SUCCESS) {
            spell->Cast(); //increase effectivity of spell
            Train(spell->GetSchool(), 1);

            return SUCCESS;
        }

        if (res == FAIL) {
            if (caster->isVisible()) {
                msgwin.Add(caster->GetNameEx(CRN_T1));
                msgwin.Add(caster->GetVerb("waste"));
                msgwin.Add(caster->GetNameEx(CRN_T4));
                msgwin.Add("power.");
            }

            return FAIL;
        }
    } else {
        if (caster->isHero()) {
            msgwin.Add("You have not enough mana to cast the spell.");
        }
    }

    return CONTINUE;
}

int XMagic::Train(const School school, const int count)
{
    if (school == School::UNKNOWN || GetLevel(school) >= MAX_LEVEL) {
        return 0;
    }

    int& counter = magic_count[Index(school)];
    counter += count;

    if (counter > (GetLevel(school) + 1) * 100) {
        counter = 0;

        return GainLevel(school);
    }

    return 0;
}

int XMagic::GainLevel(const School school, const int n)
{
    if (school == School::UNKNOWN || GetLevel(school) >= MAX_LEVEL) {
        return 0;
    }

    int& level = magic_level[Index(school)];
    level = std::min(level + n, MAX_LEVEL);

    return 1;
}

void XMagic::Learn(const SPELL_NAME spell)
{
    // Knowing any spell of a school makes the caster a Beginner in it.
    if (const School school = spell_db[spell].school;
        school != School::UNKNOWN && GetLevel(school) == 0) {
        magic_level[Index(school)] = 1;
    }

    for (const auto& tsp : spells) {
        if (tsp->GetSpellName() == spell) {
            tsp->GainLevel();

            return;
        }
    }

    spells.push_back(std::make_unique<XSpell>(spell));
}

XSpell* XMagic::GetSpell(const SPELL_NAME spell) const
{
    for (const auto& tsp : spells) {
        if (tsp->GetSpellName() == spell) {
            return tsp.get();
        }
    }

    return nullptr;
}

const char* mg_name_str[] = {
    "Elemental",
    "Body and Spirit",
    "Protection and Resistance",
    "Death and Devastation",
    "Surviving and Enlightenment",
    "Power"
};

const char* mg_level_str[] = {
    "",
    "<PROGRESS_NONE>Beginner",
    "<PROGRESS_NONE>Novice",
    "<PROGRESS_NONE>Senior Novice",
    "<PROGRESS_EXPERT>Minor Adept",
    "<PROGRESS_EXPERT>Adept",
    "<PROGRESS_EXPERT>Major Adept",
    "<PROGRESS_MASTER>Master",
    "<PROGRESS_SENIOR_MASTER>Senior Master",
    "<PROGRESS_GRANDMASTER>Grand Master"
};

static_assert(std::size(mg_name_str) == XMagic::SCHOOL_COUNT,
    "mg_name_str[] must name every school");
static_assert(std::size(mg_level_str) == XMagic::MAX_LEVEL + 1,
    "mg_level_str[] must name every rank from 0 to MAX_LEVEL");

std::string XMagic::LevelToString(const School school) const
{
    if (GetLevel(school) > 0) {
        return fmt::format("<VALUE>{:<30} {}", mg_name_str[Index(school)], mg_level_str[GetLevel(school)]);
    }

    return "";
}

