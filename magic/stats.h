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

#ifndef STATS_H
#define STATS_H

#include <memory>

#include <cereal/cereal.hpp>
#include <sol/forward.hpp>

#include "engine/global.h"
#include "helpers/dice.h"

extern const char* stats_str[];

class XStats
{
    public:
        enum Id {UNKNOWN = -1, STR, DEX, TOU, LEN, WIL, MAN, PER, CHR, COUNT};

        // Registers this enum as the Lua table XStats.MEMBER.
        static void RegisterLua(sol::state_view& lua);

        // must be the same! "St:1d2 Dx:1d4 To:2d5"
        explicit XStats(const char* str);

        explicit XStats(const XStats* xs);

        // all stats == 0 by default
        XStats();

        [[nodiscard]] int Get(const Id s) const
        {
            return stats[s] / 100;
        }

        static std::string GetName(const Id s)
        {
            return stats_str[s];
        }

        static const char* GetFullName(Id s);

        void SetStat(const Id s, const int val)
        {
            stats[s] = val * 100;
        }

        void Modify(const Id s, const int val)
        {
            stats[s] += val * 100;
        }

        void AddFract(const Id s, const int val)
        {
            stats[s] += val;
        }

        void Set(const XStats * s);
        void Set(const char* str);
        void Add(const XStats * s);
        void Sub(const XStats * s);
        bool isEqual(const XStats * s) const;

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(stats);
        }

        static Id Random()
        {
            return static_cast<Id>(vRand(COUNT));
        }

    protected:
        int stats[COUNT]{};
};

class XStatsGenerator
{
        XDice stats[XStats::COUNT];
    public:
        XStatsGenerator();
        void Init(const char* str);
        std::unique_ptr<XStats> Generate();
};

#endif
