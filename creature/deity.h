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

#ifndef DEITY_H
#define DEITY_H

#include <map>
#include <string>
#include <vector>

#include "helpers/registry.h"

#include <cereal/types/map.hpp>
#include <cereal/types/string.hpp>

#include <cereal/cereal.hpp>
#include <sol/forward.hpp>

class XCreature;
class XItem;

// Which god this is - the id world/deities.lua registered it under. The
// engine names none of them: it knows only that a creature stands in some
// relation to each, and asks content what that is worth.
using DEITY = std::string;

// "No particular god": a sacrifice that names none goes to whichever the
// sacrificer already stands best with.
inline constexpr const char* DEITY_NONE = "";

// One thing a god will do for a follower who asks. The cost comes off
// their favour, so a god's regard is spent as well as earned.
struct DeityHelp {
    std::string name;

    // The rank a follower must have reached to ask for this. Named, not
    // counted: what a rank opens is a property of the grant, so content
    // may reorder or insert grants without disturbing anything.
    std::string needs;

    int cost = 0;

    // Which effect the prayer brings about - an id from world/effects.lua.
    std::string effect;

    // A second effect, one of the two picked at random per prayer, for
    // content that wants "divine intervention" to be either fire or ice.
    // Empty for a grant that always does the one above.
    std::string alternative;
};

// A god.
struct DeityStats {
    DEITY id;
    std::string name;

    // Called whenever the killer of anything has a religion, with the
    // killer and the victim. Which kills please a god is the god's own
    // business - the engine has no opinion about undead - so this is where
    // a world says so.
    std::string on_kill;

    std::vector<DeityHelp> grants;
};

// How much regard a given amount of favour amounts to. Shared by every
// god: the ranks are the scale a world measures devotion on, not
// something each god has its own version of.
struct DeityRank {
    // What grants name when they say what they need.
    std::string id;

    std::string name;

    // The lowest favour that still counts as this rank. The rank of a
    // given favour is the most favourable row whose threshold it meets.
    int from = 0;

    // What standing this far with one god is worth on the final score.
    int score = 0;
};

extern Registry<DeityStats> deities_db;
extern Registry<DeityRank> deity_ranks_db;

const DeityStats* FindDeity(const DEITY& id);

// What a god is called, or the bare id if content named none.
const std::string& DeityName(const DEITY& id);

// The rank a favour amounts to, or nullptr if content declared no ranks.
const DeityRank* RankFor(int favour);

// The rank registered under an id, or nullptr for one no world declares.
const DeityRank* FindRank(const std::string& id);

class XDeity
{
    public:
        static void RegisterLua(sol::state_view& lua);
};

// Content-facing builders.
class DeityBuilder
{
    public:
        explicit DeityBuilder(std::string id);

        DeityBuilder& Called(const std::string& name);
        DeityBuilder& OnKill(const std::string& handler);
        DeityBuilder& Grants(const std::string& name, const std::string& needs,
            int cost, const std::string& effect,
            sol::optional<std::string> alternative);
        void Register();

    private:
        DeityStats t;
};

class DeityRankBuilder
{
    public:
        explicit DeityRankBuilder(std::string name);

        DeityRankBuilder& Called(const std::string& name);
        DeityRankBuilder& From(int favour);
        DeityRankBuilder& Score(int points);
        void Register();

    private:
        DeityRank t;
};

class XReligion
{
    public:
        XReligion() = default;

        static void RegisterLua(sol::state_view& lua);

        // How each god regards this creature, keyed by id rather than held
        // in a field per god - so a world may have one god or ten, and a
        // saved game keeps meaning what it meant when it was written.
        std::map<DEITY, int> favour;

        int GetFavour(const DEITY& deity) const;
        void SetFavour(const DEITY& deity, int value);
        void ChangeFavour(const DEITY& deity, int delta);

        // The god this creature stands best with - where a sacrifice goes
        // when it names none.
        [[nodiscard]] DEITY BestRegarded() const;

        void KillCreature(XCreature* killer, XCreature* victim);
        int SacrificeItem(XCreature* cr, XItem* item, const DEITY& deity = DEITY_NONE);

        [[nodiscard]] const DeityRank* GetRank(const DEITY& deity) const;

        // Everything this creature's standing with that god lets them ask
        // for, in the order content declared it.
        [[nodiscard]] std::vector<const DeityHelp*> AvailableHelp(const DEITY& deity) const;

        void Pray(const DEITY& deity, const DeityHelp& help, XCreature* prayer);

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(favour);
        }
};

#endif
