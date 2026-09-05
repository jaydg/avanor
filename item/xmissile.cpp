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
#include "item/xmissile.h"
#include "item/xweapon.h"
#include "magic/brand.h"

REGISTER_CLASS(XMissile);
CEREAL_REGISTER_TYPE(XMissile);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XItem, XMissile);

// Filled from world/items/ as those scripts load.
XItemBasicStructure gi_missile;

XMissile::XMissile(ItemType _it)
{
    kind = ItemKind::MISSILE;
    bp = BP_MISSILE;
    BasicFill(_it, &gi_missile);

    resistances->Sub(resistances.get());
    stats->Sub(stats.get());

    // How many come in a heap. A row that finishes its own missiles off
    // may well cut this down - see world/items/missiles.lua.
    quantity = vRand() % 20 + 3;
    weight = (weight / 5 + 1);

    OnCreated(gi_missile);
}

std::string XMissile::toString()
{
    if (quantity == 1)
        return fmt::format("{}{}{} <{:+}>({:+}, {}d{}{:+}){}",
            aet.Has("poison") ? "poisoned " : "",
            aet.Has("undead_slayer") ? "holy " : "",
            name, RNG, to_hit,
            dice.GetCount(), dice.GetSides(), dice.GetBonus(),
            aet.Has("fire") ? " of fire" : ""
        );

    return fmt::format("heap of ({}) {}{}{}s <{:+}>({:+}, {}d{}{:+}){}",
        quantity,
        aet.Has("poison") ? "poisoned " : "",
        aet.Has("undead_slayer") ? "holy " : "",
        name, RNG, to_hit,
        dice.GetCount(), dice.GetSides(), dice.GetBonus(),
        aet.Has("fire") ? " of fire" : ""
    );
}

bool XMissile::isProperWeapon(XItem * missile, XItem * weapon)
{
    // Nothing in hand: anything at all can be thrown.
    if (!weapon) {
        return true;
    }

    // Otherwise the missile has to be the kind this launcher fires - no
    // rocks down a crossbow. Which launcher that is belongs to the missile
    // and is stated with the rest of its row (world/items/missiles.lua);
    // a missile that names none, like a shuriken, can only ever be thrown.
    const ItemTemplate* row = gi_missile.Find(missile->it);

    if (!row) {
        return false;
    }

    return row->launcher != CS_NONE && row->launcher == weapon->wt;
}
