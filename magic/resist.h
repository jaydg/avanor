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

#ifndef RESIST_H
#define RESIST_H

#include <memory>

#include <cereal/cereal.hpp>
#include <sol/forward.hpp>

#include "helpers/dice.h"

class XResistance
{
    public:
        enum Id {
            NONE = -1,
            WHITE,
            BLACK,
            FIRE,
            WATER,
            AIR,
            EARTH,
            ACID,
            COLD,
            POISON,
            DISEASE,
            PARALYSE,
            STUN,
            CONFUSE,
            BLIND,
            LIGHT,
            DARKNESS,
            INVISIBLE,
            SEE_INVISIBLE,
            COUNT
        };

        // Registers this enum as the Lua table XResistance.MEMBER.
        static void RegisterLua(sol::state_view& lua);

        explicit XResistance(const XResistance* xr);

        XResistance();

        // format fire:3d6+N water:2d2+3
        explicit XResistance(const char* str1);

        [[nodiscard]] int GetResistance(const Id r) const
        {
            return resistances[r];
        }

        void SetResistance(const Id r, const int val)
        {
            resistances[r] = val;
        }

        void ChangeResistance(const Id r, const int val)
        {
            resistances[r] += val;
        }

        void Add(const XResistance* r);

        void Sub(const XResistance* r);

        void Set(const XResistance* r);

        static const char* GetResistanceName(Id r);

        [[nodiscard]] const char* GetResistanceLevel(Id r) const;

        bool isEqual(const XResistance* xr) const;

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(resistances);
        }

    protected:
        int resistances[COUNT]{};
};

class XResistGenerator
{
        XDice resist[XResistance::COUNT];
    public:
        XResistGenerator();
        void Init(const char* str);
        std::unique_ptr<XResistance> Generate();
};

#endif
