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
#include <string>
#include <string_view>
#include <stdexcept>

#include "engine/global.h"
#include "engine/xfile.h"
#include "helpers/dice.h"

// Parses expressions like "XdY", "XdY+Z" or "XdY - Z".
void XDice::Setup(std::string_view str)
{
    count_ = sides_ = bonus_ = 0;

    if (str.size() < 2) {
        Throw();
        return;
    }

    // working copy: normalize 'd' → ' ' and "+ N" / "- N"
    std::string buf(str);

    for (std::size_t i = 0; i < buf.size(); ++i) {
        if (buf[i] == 'd') {
            buf[i] = ' ';
        }
        // "+ N" / "- N": swap sign and whitespace so that sscanf can
        // "%d %d %d" read three values.
        if ((buf[i] == '+' || buf[i] == '-') && i + 1 < buf.size() && buf[i + 1] == ' ') {
            buf[i + 1] = buf[i];
            buf[i]     = ' ';
        }
    }

    // Read three integers: die count, sides, bonus
    std::size_t pos  = 0;
    int*        dest[] = {&count_, &sides_, &bonus_};
    int         read   = 0;

    while (read < 3 && pos < buf.size()) {
        // skip spaces
        while (pos < buf.size() && buf[pos] == ' ')
            ++pos;

        if (pos >= buf.size())
            break;

        // stoi reads an integer and sets pos to the first unparsable character
        std::size_t chars = 0;
        *dest[read] = std::stoi(buf.substr(pos), &chars);
        pos += chars;
        ++read;
    }

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

void XDice::Store(const XFile* f) const
{
    std::size_t wc;
    wc = f->Write(&result_); assert(wc == sizeof(result_));
    wc = f->Write(&count_); assert(wc == sizeof(count_));
    wc = f->Write(&sides_); assert(wc == sizeof(sides_));
    wc = f->Write(&bonus_); assert(wc == sizeof(bonus_));
}

void XDice::Restore(const XFile* f)
{
    f->Read(&result_);
    f->Read(&count_);
    f->Read(&sides_);
    f->Read(&bonus_);
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
