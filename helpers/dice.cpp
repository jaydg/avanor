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
#include <cassert>
#include <charconv>
#include <cmath>
#include <stdexcept>

#include "engine/global.h"
#include "helpers/dice.h"

namespace {
    bool isDigit(char c)
    {
        return c >= '0' && c <= '9';
    }
}

// Parses expressions like "XdY" or "XdY+Z"/"XdY-Z" (e.g. "2d6", "4d12+30",
// "1d8-2") - hand-rolled rather than std::regex, since MainFill() runs this
// on every item creation (up to 6 fields each) and the grammar is trivial
// enough that a regex engine's generality buys nothing here. Matches
// ^(\d*)d(\d+)([+-]\d+)?$ exactly, including its quirks: no whitespace
// tolerance anywhere (despite what the old regex's doc comment implied -
// "2d6 - 5" never actually matched it either), and no leading sign on the
// count/sides groups even though std::from_chars would otherwise accept one.
void XDice::Setup(const std::string& str)
{
    if (str.empty()) {
        count_ = sides_ = bonus_ = 0;
        Throw();
        return;
    }

    const char* cur = str.data();
    const char* const end = cur + str.size();

    int count = 1; // \d* - optional, defaults to 1 when absent

    if (cur != end && isDigit(*cur)) {
        const auto res = std::from_chars(cur, end, count);

        if (res.ec != std::errc()) {
            throw std::invalid_argument("Invalid dice format");
        }

        cur = res.ptr;
    }

    if (cur == end || *cur != 'd') {
        throw std::invalid_argument("Invalid dice format");
    }

    ++cur;

    int sides = 0; // \d+ - required, no leading sign allowed

    if (cur == end || !isDigit(*cur)) {
        throw std::invalid_argument("Invalid dice format");
    }

    {
        const auto res = std::from_chars(cur, end, sides);

        if (res.ec != std::errc()) {
            throw std::invalid_argument("Invalid dice format");
        }

        cur = res.ptr;
    }

    int bonus = 0; // ([+-]\d+)? - optional

    if (cur != end) {
        // from_chars parses a leading '-' itself, but not '+' - skip it
        // manually so both signs land on the same parse call.
        const char* bonus_start = (*cur == '+') ? cur + 1 : cur;

        if (*cur != '+' && *cur != '-') {
            throw std::invalid_argument("Invalid dice format");
        }

        const auto res = std::from_chars(bonus_start, end, bonus);

        if (res.ec != std::errc() || res.ptr != end) {
            throw std::invalid_argument("Invalid dice format");
        }
    }

    count_ = count;
    sides_ = sides;
    bonus_ = bonus;

    Throw();
}

int XDice::Throw()
{
    result_ = bonus_;

    if (sides_ > 0)
        for (int i = 0; i < count_; ++i)
            result_ += vRand() % sides_ + 1;

    return result_;
}

int XDice::NThrow() const
{
    return NDFunc(count_ * sides_) + bonus_;
}

static constexpr int dfunc_data[] = {
    750, 800, 840, 870, 890, 902, 914, 926, 937, 947,
    956, 964, 971, 977, 982, 986, 990, 993, 996, 998, 999, 100000
};

int XDice::DFunc()
{
    const int r = vRand(1000);
    const auto it = std::lower_bound(std::begin(dfunc_data), std::end(dfunc_data), r);

    return static_cast<int>(it - std::begin(dfunc_data));
}

int XDice::NDFunc(const int maximum)
{
    return static_cast<int>(std::lround((DFunc() * maximum) / 20.0));
}
