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

#include <fmt/format.h>

#include "item/item_cereal.h"
#include "item/xshield.h"

REGISTER_CLASS(XShield);
CEREAL_REGISTER_TYPE(XShield);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XItem, XShield);

// Filled from world/items/ as those scripts load.
XItemBasicStructure gi_shield;

XShield::XShield(ItemType _it)
{
    kind = ItemKind::SHIELD;
    BasicFill(_it, &gi_shield);
    pv = 0;
    bp = BP_HAND;
    RNG = 0;
    dice.SetBonus(0);
    to_hit = 0;
}

std::string XShield::toString()
{
    auto fullname = GetFullName();

    fullname.append(fmt::format(" [{:+}, {:+}]", dv, pv));
    fullname.append(StatsToString());

    return fullname;
}
