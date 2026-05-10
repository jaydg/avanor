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

#include <fmt/format.h>

#include "creature/creature.h"
#include "helpers/msgwin.h"
#include "magic/magic.h"

struct SPELL_REC {
    EFFECT effect;
    MAGIC_SCHOOL school;
    int cost;
    std::string name;
    SPELL_REC();
};

SPELL_REC spell_db[SPELL_EOF];

SPELL_REC::SPELL_REC()
{
    spell_db[SPELL_CURE_LIGHT_WOUNDS].effect	= E_CURE_LIGHT_WOUNDS;
    spell_db[SPELL_CURE_LIGHT_WOUNDS].school	= MS_BODY;
    spell_db[SPELL_CURE_LIGHT_WOUNDS].cost	= 5;
    spell_db[SPELL_CURE_LIGHT_WOUNDS].name	= "cure light wounds";

    spell_db[SPELL_CURE_SERIOUS_WOUNDS].effect	= E_CURE_SERIOUS_WOUNDS;
    spell_db[SPELL_CURE_SERIOUS_WOUNDS].school	= MS_BODY;
    spell_db[SPELL_CURE_SERIOUS_WOUNDS].cost	= 10;
    spell_db[SPELL_CURE_SERIOUS_WOUNDS].name	= "cure serious wounds";

    spell_db[SPELL_CURE_CRITICAL_WOUNDS].effect = E_CURE_CRITICAL_WOUNDS;
    spell_db[SPELL_CURE_CRITICAL_WOUNDS].school = MS_BODY;
    spell_db[SPELL_CURE_CRITICAL_WOUNDS].cost	= 15;
    spell_db[SPELL_CURE_CRITICAL_WOUNDS].name	= "cure critical wounds";

    spell_db[SPELL_CURE_MORTAL_WOUNDS].effect	= E_CURE_MORTAL_WOUNDS;
    spell_db[SPELL_CURE_MORTAL_WOUNDS].school	= MS_BODY;
    spell_db[SPELL_CURE_MORTAL_WOUNDS].cost	= 20;
    spell_db[SPELL_CURE_MORTAL_WOUNDS].name	= "cure mortal wounds";

    spell_db[SPELL_HEAL].effect	= E_HEAL;
    spell_db[SPELL_HEAL].school	= MS_BODY;
    spell_db[SPELL_HEAL].cost	= 30;
    spell_db[SPELL_HEAL].name	= "heal";

    spell_db[SPELL_HEROISM].effect	= E_HEROISM;
    spell_db[SPELL_HEROISM].school	= MS_BODY;
    spell_db[SPELL_HEROISM].cost	= 5;
    spell_db[SPELL_HEROISM].name	= "heroism";

    spell_db[SPELL_CURE_POISON].effect	= E_CURE_POISON;
    spell_db[SPELL_CURE_POISON].school	= MS_BODY;
    spell_db[SPELL_CURE_POISON].cost	= 15;
    spell_db[SPELL_CURE_POISON].name	= "cure poison";

    spell_db[SPELL_CURE_DISEASE].effect	= E_CURE_DISEASE;
    spell_db[SPELL_CURE_DISEASE].school	= MS_BODY;
    spell_db[SPELL_CURE_DISEASE].cost	= 20;
    spell_db[SPELL_CURE_DISEASE].name	= "cure disease";

    spell_db[SPELL_BURNING_HANDS].effect	= E_BURNING_HANDS;
    spell_db[SPELL_BURNING_HANDS].school	= MS_ELEMENTAL;
    spell_db[SPELL_BURNING_HANDS].cost	= 7;
    spell_db[SPELL_BURNING_HANDS].name	= "burning hands";

    spell_db[SPELL_ICE_TOUCH].effect	= E_ICE_TOUCH;
    spell_db[SPELL_ICE_TOUCH].school	= MS_ELEMENTAL;
    spell_db[SPELL_ICE_TOUCH].cost	= 7;
    spell_db[SPELL_ICE_TOUCH].name	= "ice touch";

    spell_db[SPELL_MAGIC_ARROW].effect	= E_MAGIC_ARROW;
    spell_db[SPELL_MAGIC_ARROW].school	= MS_ELEMENTAL;
    spell_db[SPELL_MAGIC_ARROW].cost	= 5;
    spell_db[SPELL_MAGIC_ARROW].name	= "magic arrow";

    spell_db[SPELL_FIRE_BOLT].effect	= E_FIRE_BOLT;
    spell_db[SPELL_FIRE_BOLT].school	= MS_ELEMENTAL;
    spell_db[SPELL_FIRE_BOLT].cost	= 12;
    spell_db[SPELL_FIRE_BOLT].name	= "fire bolt";

    spell_db[SPELL_ICE_BOLT].effect	= E_ICE_BOLT;
    spell_db[SPELL_ICE_BOLT].school	= MS_ELEMENTAL;
    spell_db[SPELL_ICE_BOLT].cost	= 12;
    spell_db[SPELL_ICE_BOLT].name	= "ice bolt";

    spell_db[SPELL_LIGHTNING_BOLT].effect	= E_LIGHTNING_BOLT;
    spell_db[SPELL_LIGHTNING_BOLT].school	= MS_ELEMENTAL;
    spell_db[SPELL_LIGHTNING_BOLT].cost	= 18;
    spell_db[SPELL_LIGHTNING_BOLT].name	= "lightning bolt";

    spell_db[SPELL_DRAIN_LIFE].effect	= E_DRAIN_LIFE;
    spell_db[SPELL_DRAIN_LIFE].school	= MS_DEATH;
    spell_db[SPELL_DRAIN_LIFE].cost	= 7;
    spell_db[SPELL_DRAIN_LIFE].name	= "drain life";

    spell_db[SPELL_ACID_BOLT].effect	= E_ACID_BOLT;
    spell_db[SPELL_ACID_BOLT].school	= MS_DEATH;
    spell_db[SPELL_ACID_BOLT].cost	= 25;
    spell_db[SPELL_ACID_BOLT].name	= "acid bolt";

    spell_db[SPELL_IDENTIFY].effect	= E_IDENTIFY;
    spell_db[SPELL_IDENTIFY].school	= MS_SURVIVING;
    spell_db[SPELL_IDENTIFY].cost	= 25;
    spell_db[SPELL_IDENTIFY].name	= "identify";

    spell_db[SPELL_SUMMON_MONSTER].effect	= E_SUMMON_MONSTER;
    spell_db[SPELL_SUMMON_MONSTER].school	= MS_SURVIVING;
    spell_db[SPELL_SUMMON_MONSTER].cost	= 15;
    spell_db[SPELL_SUMMON_MONSTER].name	= "summon monster";

    spell_db[SPELL_CREATE_ITEM].effect	= E_CREATE_ITEM;
    spell_db[SPELL_CREATE_ITEM].school	= MS_SURVIVING;
    spell_db[SPELL_CREATE_ITEM].cost	= 100;
    spell_db[SPELL_CREATE_ITEM].name	= "create item";

    spell_db[SPELL_BLINK].effect	= E_BLINK;
    spell_db[SPELL_BLINK].school	= MS_SURVIVING;
    spell_db[SPELL_BLINK].cost	= 15;
    spell_db[SPELL_BLINK].name	= "blink";

    spell_db[SPELL_SELF_KNOWLEDGE].effect	= E_SELF_KNOWLEDGE;
    spell_db[SPELL_SELF_KNOWLEDGE].school	= MS_SURVIVING;
    spell_db[SPELL_SELF_KNOWLEDGE].cost	= 25;
    spell_db[SPELL_SELF_KNOWLEDGE].name	= "self knowledge";

    spell_db[SPELL_SEE_INVISIBLE].effect	= E_SEE_INVISIBLE;
    spell_db[SPELL_SEE_INVISIBLE].school	= MS_PROTECTION;
    spell_db[SPELL_SEE_INVISIBLE].cost	= 15;
    spell_db[SPELL_SEE_INVISIBLE].name	= "see invisible";

    spell_db[SPELL_ACID_RESISTANCE].effect	= E_ACID_RESISTANCE;
    spell_db[SPELL_ACID_RESISTANCE].school	= MS_PROTECTION;
    spell_db[SPELL_ACID_RESISTANCE].cost	= 20;
    spell_db[SPELL_ACID_RESISTANCE].name	= "acid resistance";

    spell_db[SPELL_FIRE_RESISTANCE].effect	= E_FIRE_RESISTANCE;
    spell_db[SPELL_FIRE_RESISTANCE].school	= MS_PROTECTION;
    spell_db[SPELL_FIRE_RESISTANCE].cost	= 15;
    spell_db[SPELL_FIRE_RESISTANCE].name	= "fire resistance";

    spell_db[SPELL_COLD_RESISTANCE].effect	= E_COLD_RESISTANCE;
    spell_db[SPELL_COLD_RESISTANCE].school	= MS_PROTECTION;
    spell_db[SPELL_COLD_RESISTANCE].cost	= 15;
    spell_db[SPELL_COLD_RESISTANCE].name	= "cold resistance";

    spell_db[SPELL_POISON_RESISTANCE].effect	= E_POISON_RESISTANCE;
    spell_db[SPELL_POISON_RESISTANCE].school	= MS_PROTECTION;
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

EFFECT XSpell::GetEffect() const
{
    return spell_db[spell_name].effect;
}

int XSpell::GetManaCost() const
{
    return (spell_db[spell_name].cost * 15) / (14 + eff_level);
}

MAGIC_SCHOOL XSpell::GetSchool() const
{
    return spell_db[spell_name].school;
}

std::string XSpell::toString() const
{
    return fmt::format(MSG_YELLOW "{<21s} " MSG_LIGHTGRAY ": " MSG_YELLOW "{}" MSG_LIGHTGRAY "pp  {Eff - {}} (to next level: {})",
        spell_db[spell_name].name,
        GetManaCost(),
        GetEffectivity(),
        eff_level - cast_count);
}

XMagic::XMagic()
{
    for (int & i : magic_level) {
        i = 1;
    }
}

int will_div[10] = {50, 25, 20, 15, 10, 8, 6, 3, 2, 1};

int XMagic::GetSpellRange(const XSpell* spell, XCreature* caster)
{
    int power = caster->GetStats(S_WIL) + spell->GetEffectivity();
    return XEffect::GetRange(spell->GetEffect(), power);
}

RESULT XMagic::Cast(XSpell* spell, XCreature* caster)
{
    const int power = caster->GetStats(S_WIL) + spell->GetEffectivity();

    if (caster->_PP - spell->GetManaCost() >= 0) {
        if (caster->isInVisibleArea() && !caster->isHero()) {
            msgwin.Add(fmt::format("{} {} {}.",
                caster->GetNameEx(CRN_T1),
                caster->GetVerb("cast"),
                spell->GetName()));
        }

        const int res = XEffect::Make(caster, spell->GetEffect(), power);

        if (res != ABORT) {
            caster->_PP -= spell->GetManaCost();
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

int XMagic::Train(MAGIC_SCHOOL school, int count)
{
    /*	magic_count[school] += count;
    	if (magic_count[school] > (magic_level[school] + 1) * 100)
    	{
    		if (GainLevel(school))
    		{
    			magic_count[school] = 0;
    			return 1;
    		}
    	}*/
    return 0;
}

int XMagic::GainLevel(MAGIC_SCHOOL school, int n)
{
    /*	if (magic_level[school] < 9)
    	{
    		magic_level[school] += n;
    		if (magic_level[school] > 9)
    			magic_level[school] = 9;
    		return 1;
    	} */
    return 0;
}

void XMagic::Learn(const SPELL_NAME spell)
{
    for (const auto tsp : spells) {
        if (tsp->GetSpellName() == spell) {
            tsp->GainLevel();

            return;
        }
    }

    spells.push_back(new XSpell(spell));
}

XSpell* XMagic::GetSpell(const SPELL_NAME spell) const
{
    for (const auto tsp : spells) {
        if (tsp->GetSpellName() == spell) {
            return tsp;
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
    MSG_LIGHTGRAY	"Beginner",
    MSG_LIGHTGRAY	"Novice",
    MSG_LIGHTGRAY	"Senior Novice",
    MSG_YELLOW	"Minor Adept",
    MSG_YELLOW	"Adept",
    MSG_YELLOW	"Major Adept",
    MSG_LIGHTRED	"Master",
    MSG_RED	"Senior Master",
    MSG_DARKGRAY	"Grand Master"
};

int XMagic::LevelToString(const MAGIC_SCHOOL school, char* buf) const
{
    if (GetLevel(school) > 0) {
        sprintf(buf, MSG_YELLOW "%-30s %s", mg_name_str[school], mg_level_str[magic_level[school]]);
        return 1;
    }

    return 0;
}

void XMagic::Store(XFile * f)
{
    // FIXME: Implement when porting saving/restoring to Cereal
    /*
    f->Write(magic_level, sizeof(int), MS_EOF);
    f->Write(magic_count, sizeof(int), MS_EOF);
    spells.StoreList(f);
    */
}

void XMagic::Restore(XFile * f)
{
    // FIXME: Implement when porting saving/restoring to Cereal
    /*
    f->Read(magic_level, sizeof(int), MS_EOF);
    f->Read(magic_count, sizeof(int), MS_EOF);
    spells.RestoreList(f);
    */
}
