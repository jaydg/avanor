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

#ifndef MODIFIERS_H
#define MODIFIERS_H

#include <algorithm>
#include <string>

#include <utility>
#include <vector>

#include <cereal/types/base_class.hpp>
#include <cereal/types/memory.hpp>
#include <sol/sol.hpp>

#include "creature/creature.h"
#include "magic/resist.h"
#include "engine/xobject.h"

// What can be laid on a creature for a while: a wound that bleeds, a
// quickening, a disease. Named by id, the way resistances and effects
// are, so that a modifier is a thing content can talk about.
using MODIFIER = std::string;

// No modifier at all - a brand that inflicts nothing, a corpse that is
// only food.
inline const MODIFIER MOD_NONE;

// The ones the engine itself acts on by name: paralysis stops the hero's
// input loop, poison forbids running, a prayer for healing lifts a fixed
// list, and a heavy blow inflicts one of the first three. Everything
// else about them - what they say, what they do to the numbers, what
// they do each turn - is content.
inline constexpr const char* MOD_WOUND = "wound";
inline constexpr const char* MOD_POISON = "poison";
inline constexpr const char* MOD_CONFUSE = "confuse";
inline constexpr const char* MOD_STUN = "stun";
inline constexpr const char* MOD_HEROISM = "heroism";
inline constexpr const char* MOD_DISEASE = "disease";
inline constexpr const char* MOD_SEE_INVISIBLE = "see_invisible";
inline constexpr const char* MOD_PARALYSE = "paralyse";
inline constexpr const char* MOD_WEAK = "weak";
inline constexpr const char* MOD_RESISTANCE = "resistance";
inline constexpr const char* MOD_BOOST_SPEED = "boost_speed";
inline constexpr const char* MOD_SLOWNESS = "slowness";


enum MODIFIER_RESULT {
    MR_OK = 0,
    MR_REMOVE = 1,
    MR_DIE = 2,
};

// What a modifier is, as content writes it in world/modifiers.lua: what it
// is called while it is on you, what it says as it comes and goes, and what
// it shifts for as long as it lasts.
struct ModifierStats {
    MODIFIER id;

    // What the status line calls it. The bands are read in order and the
    // first whose ceiling the value has not reached wins, so a wound reads
    // from a graze to a mortal wound as it deepens; a ceiling of 0 means
    // "and anything above". A modifier with no bands at all shows nothing -
    // safety is felt rather than seen.
    struct Band {
        int upto = 0;
        std::string text;
    };

    std::vector<Band> names;

    // What the one carrying it is told: as it takes hold, as it lifts, as
    // it grows or fades, and each turn it actually does something.
    std::string set_msg;
    std::string remove_msg;
    std::string grows_msg;
    std::string fades_msg;
    std::string apply_msg;

    // What shifts while it is on, and shifts back when it goes.
    int dv = 0;
    int hit = 0;
    int slower = 0;   // added to the time each move takes
    std::vector<std::pair<XStats::Id, int>> stats;
    std::vector<std::pair<RESISTANCE, int>> resistances;

    // The amount it is laid on with is multiplied by this first: poison
    // measured in doses rather than turns.
    int scale = 1;

    // What the target gets to shrug part of it off with, if anything.
    RESISTANCE resisted_by;

    // The few whose behaviour turn by turn the engine has to carry -
    // bleeding, staggering, rotting a stat away. Content names one of
    // these rather than describing it, the way an effect names :Engine().
    std::string engine;
};

// The row for an id, or nothing if no world file declares one.
const ModifierStats* FindModifier(const MODIFIER& id);

// Every modifier content has declared, in the order it declared them.
const std::vector<ModifierStats>& AllModifiers();

void RegisterModifierLua(sol::state_view& lua);

class ModifierBuilder
{
    public:
        explicit ModifierBuilder(MODIFIER id);

        ModifierBuilder& Called(const std::string& text);
        ModifierBuilder& Severity(int upto, const std::string& text);
        ModifierBuilder& OnSet(const std::string& text);
        ModifierBuilder& OnRemove(const std::string& text);
        ModifierBuilder& OnChange(const std::string& grows, const std::string& fades);
        ModifierBuilder& Applies(const std::string& text);
        ModifierBuilder& While(const sol::table& shifts);
        ModifierBuilder& Scale(int scale);
        ModifierBuilder& ResistedBy(const std::string& resist);
        ModifierBuilder& Engine(const std::string& which);
        void Register();

    private:
        ModifierStats t;
};

// One modifier on one creature: which row it is, how much of it there is,
// who laid it on, and how long until it takes hold. Everything it says and
// everything it shifts comes from the row.
class XBasicModifier
{
    protected:
        // Cereal constructs one of these directly when reading a save; every
        // field is overwritten by serialize() immediately afterwards.
        friend class cereal::access;

    public:
        XBasicModifier() = default;

        XBasicModifier(const MODIFIER& mt, int _val, XCreature * _cr = nullptr);

        ~XBasicModifier()
        {
            setter.reset();
        }

        // Two are the same modifier when they are the same kind from the
        // same hand - and when both are on the creature, or both are still
        // on their way. A poison already burning and a poison that has not
        // bitten yet are not the same thing, and must not merge.
        int Compare(const XBasicModifier* mod) const
        {
            if (mod->mdt == mdt && mod->setter.lock() == setter.lock()
                && (mod->delay > 0) == (delay > 0)) {
                return 0;
            }

            return -1;
        }

        void Concat(const XBasicModifier* o)
        {
            val += o->val;

            // Two helpings of the same slow poison: the sooner one decides
            // when it bites, and both amounts arrive together.
            if (delay > 0 && o->delay > 0) {
                delay = std::min(delay, o->delay);
            }
        }

        // Defined in modifiers.cpp, not inline here: it needs XCreature
        // complete (owner->HP, owner->Die()), and creature.h needs
        // XModifier (this class's owner, magic/modifier.h) complete for
        // XCreature::load()/save() to dereference `md` - an inline body
        // here would make the two headers require each other's complete
        // type at the same time, which can't work.
        MODIFIER_RESULT Run(XCreature * owner);

        void onSet(XCreature * owner);
        void onRemove(XCreature * owner);

        [[nodiscard]] std::string GetDisplayName(int xval) const;
        [[nodiscard]] const std::string& SetMsg() const;
        [[nodiscard]] const std::string& RemoveMsg() const;
        [[nodiscard]] const std::string& ChangeMsg(int val) const;
        [[nodiscard]] const std::string& ApplyMsg() const;

        // `setter` was never actually persisted even before the
        // shared_ptr migration (the legacy Store/Restore, since
        // removed, only ever wrote val/mdt) - this is new real
        // persistence, not a mechanical port.
        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(mdt, val, setter, delay);
        }

        MODIFIER mdt;
        int val = 0; // value of modifier;

        // Turns still to pass before this takes hold. While it is above
        // zero the modifier is on its way rather than on the creature: it
        // says nothing, does nothing, counts for nothing, and is not what
        // the status line or Get() reports. Eating something that disagrees
        // with you is the usual source.
        int delay = 0;

        std::weak_ptr<XCreature> setter;

    private:
        // The row this one was made from, looked up once. Rows live for as
        // long as the game does, so the pointer stays good; a modifier read
        // back from a save finds its row again in serialize().
        [[nodiscard]] const ModifierStats* Row() const;
};

#endif
