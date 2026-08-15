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

#ifndef XBASEOBJ_H
#define XBASEOBJ_H

#include <cereal/types/base_class.hpp>
#include <cereal/types/memory.hpp>

#include "xobject.h"
#include "engine/xmapobj.h"
#include "helpers/dice.h"
#include "magic/resist.h"
#include "magic/stats.h"

class XBaseObject : public XMapObject
{
    public:
        DECLARE_CREATOR(XBaseObject, XMapObject);
        XBaseObject();
    protected:
        // teardown hook, called by XObject::Invalidate()
        void OnInvalidate() override;
    public:
        explicit XBaseObject(XBaseObject * copy);

        int _DV{}, _PV{}, _HIT{}, RNG;
        int _HP{}, _PP{};
        int MAX_HP{};
        int MAX_PP{};

        int weight;
        XDice dice;
        // Compare() is an item-stacking predicate rooted at XItem, and this
        // is only ever reached as a qualified call from XItem::Compare().
        int Compare(XObject * o);

        std::unique_ptr<XResistance> resistances;
        std::unique_ptr<XStats> stats;

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XMapObject>(this));
            ar(_DV, _PV, _HIT, RNG, _HP, _PP, MAX_HP, MAX_PP, weight, dice, resistances, stats);
        }
};

#endif
