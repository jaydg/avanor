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
#include "magic/attack_effect_type.h"

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

    if (it == ItemType::ROCK) {
        name = "rock";
        view = '*';
        color = xDARKGRAY;
        XDice d(1, 3);
        to_hit = d.GetResult();
        d.Setup(1, 2);
        dice.Setup(1, 5, d.GetResult());
        value = 1;
        weight = 5;
        RNG = 0;
    }

    resistances->Sub(resistances.get());
    stats->Sub(stats.get());

    int rcount = 20;

    if (vRand(20) == 0 && (it == ItemType::ARROW || it == ItemType::QUARREL || it == ItemType::SLINGBULLET)) { //something special...
        rcount = 10;
        int tr = vRand(4);

        switch (tr) {
            case 0:
                aet = AttackEffectType::POISON;
                break; //poisoned

            case 1:
                aet = AttackEffectType::FIRE;
                break; //hell arrows

            case 2:
                aet = AttackEffectType::UNDEADSLAYER;
                break; //

            case 3:
                aet = AttackEffectType::ORCSLAYER;
                break; //
        }

    }

    if (vRand(20) == 0) {
        rcount = rcount / 2;
        int xr = vRand(3);

        if (xr == 0) {
            if (it == ItemType::ARROW) {
                dice.Add(2, 2, 0);
                RNG += 1;
                name = "seeker arrow";
            }

            if (it == ItemType::QUARREL) {
                dice.Add(2, 2, 0);
                RNG += 1;
                name = "seeker quarrel";
            }
        } else if (xr == 1) {
            if (it == ItemType::ARROW) {
                to_hit += 10;
                RNG += 2;
                name = "hunter arrow";
            }

            if (it == ItemType::QUARREL) {
                to_hit += 10;
                RNG += 2;
                name = "hunter quarrel";
            }
        } else if (xr == 2) {
            if (it == ItemType::ARROW) {
                dice.Add(1, 1, 10);
                RNG += 2;
                name = "sharp arrow";
            }

            if (it == ItemType::QUARREL) {
                dice.Add(1, 1, 10);
                RNG += 2;
                name = "sharp quarrel";
            }
        }
    }

    quantity = vRand() % rcount + 3;
    weight = (weight / 5 + 1);
}

std::string XMissile::toString()
{
    if (quantity == 1)
        return fmt::format("{}{}{} <{:+}>({:+}, {}d{}{:+}){}",
            (aet & AttackEffectType::POISON) != AttackEffectType::NONE ? "poisoned " : "",
            (aet & AttackEffectType::UNDEADSLAYER) != AttackEffectType::NONE ? "holy " : "",
            name, RNG, to_hit,
            dice.GetCount(), dice.GetSides(), dice.GetBonus(),
            (aet & AttackEffectType::FIRE) != AttackEffectType::NONE ? " of fire" : ""
        );

    return fmt::format("heap of ({}) {}{}{}s <{:+}>({:+}, {}d{}{:+}){}",
        quantity,
        (aet & AttackEffectType::POISON) != AttackEffectType::NONE ? "poisoned " : "",
        (aet & AttackEffectType::UNDEADSLAYER) != AttackEffectType::NONE ? "holy " : "",
        name, RNG, to_hit,
        dice.GetCount(), dice.GetSides(), dice.GetBonus(),
        (aet & AttackEffectType::FIRE) != AttackEffectType::NONE ? " of fire" : ""
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
    for (int i = 0; i < gi_missile.total_item; i++) {
        if (gi_missile.pFirstItem[i].it == missile->it) {
            const XWarSkills::Type launcher = gi_missile.pFirstItem[i].launcher;

            return launcher != XWarSkills::OTHER && launcher == weapon->wt;
        }
    }

    return false;
}
