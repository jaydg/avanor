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
#include <cmath>
#include <regex>
#include <stdexcept>

#include "engine/global.h"
#include "engine/xfile.h"
#include "helpers/dice.h"

// Parses expressions like "XdY", "XdY+Z" or "XdY - Z".
void XDice::Setup(const std::string& str)
{
    if (str.empty()) {
        count_ = sides_ = bonus_ = 0;
        Throw();
        return;
    }

    static const std::regex pattern(R"(^(\d*)d(\d+)([+-]\d+)?$)");
    std::smatch match;

    if (!std::regex_match(str, match, pattern)) {
        throw std::invalid_argument("Invalid dice format");
    }

    count_ = match[1].matched ? std::stoi(match[1].str()) : 1;
    sides_ = std::stoi(match[2].str());
    bonus_ = match[3].matched ? std::stoi(match[3].str()) : 0;

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
