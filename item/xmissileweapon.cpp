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

#include <fmt/format.h>

#include "item/item_cereal.h"
#include "item/xmissileweapon.h"

REGISTER_CLASS(XMissileWeapon);
CEREAL_REGISTER_TYPE(XMissileWeapon);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XItem, XMissileWeapon);

// Filled from world/items/ as those scripts load.
XItemBasicStructure gi_missilew;

XMissileWeapon::XMissileWeapon(ItemType _it)
{
    kind = ItemKind::MISSILEW;
    BasicFill(_it, &gi_missilew);
    bp = BP_MISSILE_WEAPON;
    dv = 0;
    pv = 0;
}

std::string XMissileWeapon::toString()
{
    auto fullname = GetFullName();

    fullname.append(fmt::format(" <{:+}>", RNG));
    fullname.append(fmt::format(" ({:+}, {}d{}{:+})",
        to_hit, dice.GetCount(), dice.GetSides(), dice.GetBonus()));
    fullname.append(StatsToString());

    return fullname;
}
