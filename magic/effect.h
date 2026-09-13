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
#ifndef EFFECT_H
#define EFFECT_H

#include <string>
#include <vector>

#include <sol/forward.hpp>

#include "helpers/registry.h"
#include "engine/global.h"
#include "creature/cr_defs.h"
#include "magic/brand.h"
#include "magic/resist.h"

// Which effect this is - the id world/effects.lua registered it under.
// A string, like every other content id: what a world's magic can do is
// content, and the engine only offers the handful of things an effect can
// be built out of.
using EFFECT = std::string;

inline constexpr const char* EFFECT_NONE = "";

// What an effect needs pointed at before it can happen.
enum class EffectTarget {
    NONE,       // it happens to whoever caused it
    DIRECTION,  // a way to face
    TARGET,     // a place within range
    ITEM,       // something the causer is carrying
};

class XCreature;
class XItem;
class XLocation;
struct EFFECT_DATA;

// One thing an effect does. An effect is a list of these, tried in order
// until one of them has something to do - so a potion that heals wounds
// and mends bleeding does whichever the drinker actually needs.
struct EffectPart {
    enum class Kind {
        HEAL,        // hit points back
        CURE,        // bleeding staunched
        MANA,        // power points back
        MODIFIER,    // something laid on the causer for a while
        TOUCH,       // damage where they are facing
        BOLT,        // damage at a place within range
        ENGINE,      // one of the few the engine does itself
    };

    Kind kind = Kind::HEAL;

    // How much: XDice(count, power / divisor, bonus), where power is what
    // the spell, scroll or potion was cast with.
    int count = 1;
    int divisor = 1;
    int bonus = 0;

    // For MODIFIER: which modifier, by id. Empty for the other kinds.
    std::string modifier;

    // For MODIFIER: take the amount away rather than add it - curing
    // poison is poison with the sign turned round.
    bool relieves = false;

    // For MODIFIER and RESISTANCE: it lasts for `power` itself, with no
    // dice rolled.
    bool sustained = false;

    // For TOUCH and BOLT.
    BrandSet brands;
    int colour = 0;
    std::string message;

    // For ENGINE: which of them.
    std::string engine;

    // For the "summon_monster" engine effect: what sort of creature it
    // raises. The engine used to name the undead itself.
    CREATURE_CLASS summons;
};

struct EffectStats {
    EFFECT id;
    EffectTarget targets = EffectTarget::NONE;

    // How far it reaches: power / divisor + bonus. A divisor of zero
    // means the bonus alone, and both zero means it does not reach.
    int range_divisor = 0;
    int range_bonus = 0;

    std::vector<EffectPart> parts;
};

extern Registry<EffectStats> effects_db;

const EffectStats* FindEffect(const EFFECT& id);

class XEffect
{
    public:
        static void RegisterLua(sol::state_view& lua);

    private:
        static int Heal(XCreature * caster, int X, int Y, int Z);
        static int Cure(XCreature * caster, int X, int Y, int Z);
        static int Mana(XCreature * caster, int X, int Y, int Z);
        static int Touch(const EFFECT_DATA* pData, int X, int Y, int Z, int col, const BrandSet& brands, const std::string& msg);
        static int Bolt(const EFFECT_DATA* pData, int X, int Y, int Z, int col, const BrandSet& brands, const std::string& msg);

        // The seven that are not built out of the parts above: they make
        // creatures and items, move people about, and show screens.
        static int Engine(const EffectPart& part, const EFFECT_DATA* pData);

    public:
        static int Make(const EFFECT_DATA* pData);
        static RESULT Make(XCreature * caster, const EFFECT& effect, int power);
        static EffectTarget GetReq(const EFFECT& effect);
        static int GetRange(const EFFECT& effect, int power);
};

// Content-facing builder - see world/effects.lua.
class EffectBuilder
{
    public:
        explicit EffectBuilder(std::string id);

        EffectBuilder& Heals(int count, int divisor, int bonus);
        EffectBuilder& Cures(int count, int divisor, int bonus);
        EffectBuilder& Restores(int count, int divisor, int bonus);
        EffectBuilder& Inflicts(const std::string& modifier, int count, int divisor, int bonus);
        EffectBuilder& Relieves(const std::string& modifier, int count, int divisor, int bonus);
        EffectBuilder& Sustains(const std::string& modifier);
        EffectBuilder& Touches(int count, int divisor, int bonus, int colour,
            const std::string& brand, const std::string& message);
        EffectBuilder& Throws(int count, int divisor, int bonus, int colour,
            const std::string& brand, const std::string& message);
        EffectBuilder& Engine(const std::string& which);
        EffectBuilder& Summons(const std::string& cr_class);
        EffectBuilder& Targets(EffectTarget targets);
        EffectBuilder& Range(int divisor, int bonus);
        void Register();

    private:
        EffectStats t;
};

struct EFFECT_DATA {
    EFFECT effect;

    // In many case effects fill caller, but not when cause by a trap.
    XCreature* caller;

    // For many visual effects this field must be filled.
    XLocation* l;

    // Starting point of effect (for example for trap).
    int call_x, call_y;

    // Target creature, usually not required
    XCreature* target;

    // target position: we can make fire-bolt traps :)
    int target_x, target_y;

    // power == willpower
    int power;
};

#endif
