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

#ifndef RESIST_H
#define RESIST_H

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <cereal/types/map.hpp>
#include <cereal/types/string.hpp>

#include <cereal/cereal.hpp>
#include <sol/forward.hpp>

#include "helpers/registry.h"
#include "helpers/dice.h"

// One of the ways a creature or an item can be hardened against harm -
// fire, poison, being blinded. Which ones exist is content: a world with
// radiation in it declares a resistance to radiation, and nothing in the
// engine has to hear about it.
using RESISTANCE = std::string;

// "No resistance": the answer when something is asked about one nothing
// defines. Was XResistance::NONE.
inline const RESISTANCE RS_NONE;

// The ones the engine still names, because it implements a rule that
// mentions them: what a modifier's damage is resisted by, and the pair
// that decides who can see whom. Everything else is named only in world/.
inline constexpr const char* RS_POISON = "poison";
inline constexpr const char* RS_CONFUSE = "confuse";
inline constexpr const char* RS_STUN = "stun";
inline constexpr const char* RS_INVISIBLE = "invisible";
inline constexpr const char* RS_SEE_INVISIBLE = "see_invisible";

// One sort of resistance, as world/resistances.lua declares it.
struct ResistanceStats {
    RESISTANCE id;
    std::string name;

    // What the body notices when it gains or loses one. Either may be
    // empty, and then nothing is said - which is what most of them did
    // before, there being no text written for thirteen of the eighteen.
    std::string gained;
    std::string lost;
};

// Every resistance content declared, in declaration order - which is the
// order they are listed in wherever they are shown.
extern Registry<ResistanceStats> resistances_db;

const ResistanceStats* FindResistance(const RESISTANCE& r);

// Fluent builder:
//
//   Resistance.new("fire")
//       :Called("fire")
//       :Gained("Your blood cools down!")
//       :Lost("Your blood warms up!")
//       :Register()
class ResistanceBuilder
{
    public:
        explicit ResistanceBuilder(std::string id);

        ResistanceBuilder& Called(const std::string& name);
        ResistanceBuilder& Gained(const std::string& text);
        ResistanceBuilder& Lost(const std::string& text);

        void Register();

    private:
        ResistanceStats t;
};

class XResistance
{
    public:
        // Registers the Resistance builder. There is no table of names any
        // more: content spells them as strings, the way it does item types.
        static void RegisterLua(sol::state_view& lua);

        explicit XResistance(const XResistance* xr);

        XResistance() = default;

        // format fire:3d6+N water:2d2+3
        explicit XResistance(const char* str1);

        [[nodiscard]] int GetResistance(const RESISTANCE& r) const
        {
            const auto it = resistances.find(r);
            return it == resistances.end() ? 0 : it->second;
        }

        void SetResistance(const RESISTANCE& r, const int val)
        {
            if (!r.empty()) {
                resistances[r] = val;
            }
        }

        void ChangeResistance(const RESISTANCE& r, const int val)
        {
            if (!r.empty()) {
                resistances[r] += val;
            }
        }

        void Add(const XResistance* r);

        void Sub(const XResistance* r);

        void Set(const XResistance* r);

        static const std::string& GetResistanceName(const RESISTANCE& r);

        [[nodiscard]] const char* GetResistanceLevel(const RESISTANCE& r) const;

        bool isEqual(const XResistance* xr) const;

        // Everything this carries, so callers can walk what is actually
        // set rather than every resistance that could exist.
        [[nodiscard]] const std::map<RESISTANCE, int>& All() const
        {
            return resistances;
        }

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(resistances);
        }

    protected:
        // Keyed by id, not indexed: content may add or reorder without
        // spoiling a save, and a creature carries only what it has.
        std::map<RESISTANCE, int> resistances;
};

class XResistGenerator
{
        std::map<RESISTANCE, XDice> resist;
    public:
        XResistGenerator();
        void Init(const char* str);
        std::unique_ptr<XResistance> Generate();
};

#endif
