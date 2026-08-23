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
        // What eating a corpse of this species does to the eater. The
        // members carry no CET_ prefix any more: the class they now live
        // in says which enum they belong to.
        enum class EffectType {
            MODIFY_ST,
            MODIFY_TO,
            MODIFY_MA,
            MODIFY_R_FIRE,
            MODIFY_R_COLD,
            MODIFY_R_ACID,
            MODIFY_R_POISON,
            MODIFY_R_PARALYSE,
            MODIFY_STOMACH,
            POISON,
            DISEASE,
            PARALYSE,
            CONFUSE,
            VOMIT,
            SATIATION,
        };

        // One of those, and how much of it.
        struct Effect {
            EffectType type;
            int value;
        };

        // What the corpses of one species are: how long they keep, what
        // they taste like, and what eating one does. Held once per
        // species in XCreatureStorage's table - a corpse item points at
        // its species' entry rather than carrying a copy (see
        // pCorpseData below).
        struct Data {
            Data() : roating_time(100), ft(FT_NORMALFOOD) {}

            int roating_time;
            FOOD_TYPE ft;
            std::vector<Effect> effect;
        };

    protected:
        int time_of_roating;
        CREATURE_NAME cn; //need for correct restoration of corpse;
        Data* pCorpseData;
    public:
        DECLARE_CREATOR(XCorpse, XAnyFood);
        XCorpse(XCreature * corpse_owner, const Data * pData, CORPSE_FLAG cf = CF_RAW);
        XCorpse(XCorpse * copy);
        XCorpse()
        {
            assert(0);
        }

        XItem* MakeCopy() override
        {
            return new XCorpse(this);
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
        int Compare(XObject * o) override
        {
            return -1;
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
