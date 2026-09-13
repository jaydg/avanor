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

#ifndef XCORPSE_H
#define XCORPSE_H

#include <vector>

#include <cereal/types/string.hpp>
#include <sol/forward.hpp>

#include "creature/cr_defs.h"
#include "item/xanyfood.h"

enum CORPSE_CONDITION {
    CCOND_NICE,
    CCOND_NORMAL,
    CCOND_SROTED,
    CCOND_ROTED,
    CCOND_VROTED,
};

enum CORPSE_FLAG {
    CF_RAW,
    CF_DRY,
    CF_COOKED,
    CF_ROASTED,
    CF_FRIZED,
};

class XCorpse : public XAnyFood
{
    public:
        // What eating a corpse of this species does to the eater.
        //
        // The first three name what they act on rather than being one
        // member per stat and per resistance: which resistances exist is
        // world/resistances.lua's business, so an enum here could only
        // ever privilege a handful of them. STOMACH and VOMIT stay
        // members because they are mechanisms with nothing to name.
        enum class EffectType {
            STAT,       // target names a stat, "St"
            RESIST,     // target names a resistance row, "fire"
            MODIFIER,   // modifier is laid on the eater after a delay
            STOMACH,
            VOMIT,
            SATIATION,
        };

        // One of those, and how much of it.
        struct Effect {
            EffectType type = EffectType::STAT;

            // Which stat or which resistance, for the two types that act
            // on one. Empty otherwise.
            std::string target;

            // For MODIFIER: which modifier, by id. Empty otherwise.
            std::string modifier;

            int value = 0;
        };

        // What the corpses of one species are: how long they keep, what
        // they taste like, and what eating one does. Held once per
        // species in XCreatureStorage's table - a corpse item points at
        // its species' entry rather than carrying a copy (see
        // pCorpseData below).
        struct Data {
            Data() : roating_time(100) {}

            int roating_time;

            // How a corpse of this species sits, by id. Empty means the
            // ordinary sort, the same as for any other food.
            TASTE taste;
            std::vector<Effect> effect;
        };

    protected:
        int time_of_roating;
        CREATURE_NAME cn; //need for correct restoration of corpse;
        Data* pCorpseData;
    public:
        DECLARE_CREATOR(XCorpse, XAnyFood);
        XCorpse(XCreature* corpse_owner, CORPSE_FLAG cf = CF_RAW);
        // Not copyable: a corpse points at its species'
        // shared data and is scheduled in its own right.
        XCorpse(XCorpse*) = delete;
        XCorpse()
        {
            assert(0);
        }

        RESULT onEat(XCreature * eater) override;

        bool Run() override;

        // pCorpseData is a non-owning pointer into a static per-species
        // table (XCreatureStorage), not owned/serialized data - only `cn`
        // (which species) is persisted, and pCorpseData is re-derived from
        // it on load via FixupCorpseData() (defined in the .cpp, where
        // creature/anycr.h - which itself needs XCorpse::Data from this
        // header - can be included without a cycle).
        //
        // One symmetric serialize() rather than a split save()/load()
        // pair: as found and fixed for XCreature/XPotion earlier this
        // session, a split pair - even a correctly-disambiguated one -
        // silently breaks Cereal's *polymorphic type registration* for
        // the type at runtime whenever an ancestor (XBaseObject here)
        // has its own plain serialize().
        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XAnyFood>(this));
            ar(corpse_flag, time_of_roating, cn);

            if constexpr (Archive::is_loading::value) {
                FixupCorpseData();
            }
        }

    protected:
        void FixupCorpseData();

    public:

        [[nodiscard]] std::string postEat(XCreature *eater) override;
        // A corpse never stacks with anything, not even another of the
        // same species: each rots on its own clock.
        bool Compare(XObject * /*o*/) override
        {
            return false;
        }

        int GetValue() override;
        std::string toString() override;
        CORPSE_CONDITION GetCondition();
        void Cook()
        {
            corpse_flag = CF_COOKED;
        }

        int corpse_flag;
        int roating_stopped; //flag for stoping of roating during Cooking
};

// XCorpse's default constructor asserts(0) - route Cereal's load-time
// construction through DECLARE_CREATOR's DUMMY_STRUCT constructor
// instead. Lives here rather than xcorpse.cpp: XCorpse can be directly
// scheduled (Game.Scheduler.Add(this) in xcorpse.cpp), so this gets
// triggered from inside XScheduler::serialize() (a template in
// xscheduler.h, reinstantiated per translation unit) - same reasoning
// as XStandardAI/XSpell/XUniversalGen/XLocation/etc. earlier this
// session.
CEREAL_LOAD_VIA_DUMMY_CONSTRUCT(XCorpse, serialize);

// Registers CorpseEffectType.MEMBER in Lua - what a creature definition
// names in its CorpseEffect() call.
void RegisterCorpseEffectEnum(sol::state_view& lua);

#endif
