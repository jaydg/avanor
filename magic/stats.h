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

#include "engine/global.h"
#include "helpers/dice.h"

enum STATS {S_UNKNOWN = -1, S_STR, S_DEX, S_TOU, S_LEN, S_WIL, S_MAN, S_PER, S_CHR, S_EOF};

extern const char* stats_str[];

class XStats
{
    public:
        // must be the same! "St:1d2 Dx:1d4 To:2d5"
        explicit XStats(const char* str);

        explicit XStats(const XStats* xs);

        // all stats == 0 by default
        XStats();

        [[nodiscard]] int Get(const STATS s) const
        {
            return stats[s] / 100;
        }

        static const char* GetName(const STATS s)
        {
            return stats_str[s];
        }

        static const char* GetFullName(STATS s);

        void SetStat(const STATS s, const int val)
        {
            stats[s] = val * 100;
        }

        void Modify(const STATS s, const int val)
        {
            stats[s] += val * 100;
        }

        void AddFract(const STATS s, const int val)
        {
            stats[s] += val;
        }

        void Set(const XStats * s);
        void Set(const char* str);
        void Add(const XStats * s);
        void Sub(const XStats * s);
        bool isEqual(const XStats * s) const;

        void Store(XFile * f);
        void Restore(XFile * f);

        static STATS Random()
        {
            return static_cast<STATS>(vRand(S_EOF));
        }

    protected:
        int stats[S_EOF]{};
};

class XStatsGenerator
{
        XDice stats[S_EOF];
    public:
        XStatsGenerator();
        void Init(const char* str);
        std::unique_ptr<XStats> Generate();
};

#endif
