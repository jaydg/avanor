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

ItemTemplate MISSILE_STRUCT[] = {
    {ItemType::ARROW,	"arrow",	'\\',	"",	"",	"1d2", "1d4", "1d3", "1d2+3",	ISET_MISSILE,	1,	1,	100,	IQ_AVG,	""},
    {ItemType::QUARREL,	"quarrel",	'\\',	"",	"",	"1d2", "1d6", "1d3", "1d2+3",	ISET_MISSILE,	1,	1,	100,	IQ_AVG,	""},
    {ItemType::SLINGBULLET, "sling bullet",	'\\',	"",	"",	"1d2", "1d5", "1d2", "1d2+3",	ISET_MISSILE,	1,	1,	30,	IQ_FAIR, ""},
    {ItemType::ROCK,	"rock",	'*',	"",	"",	"1d1", "1d3", "1d1", "1d2+2",	ISET_STONE,	1,	1,	300,	IQ_POOR, ""},
    {ItemType::SHURIKEN,	"shuriken",	'*',	"",	"",	"1d2", "1d6", "1d4", "1d2+2",	ISET_METAL,	1,	1,	50,	IQ_FAIR, ""}
};

XItemBasicStructure gi_missile(MISSILE_STRUCT, 5);

/*
// TODO: Bind launchers to missiles.
// Rock is special b/c it can be used with or without a launcher...

 WeaponSkillBinding mbind[] = {
{ItemType::LONGBOW,        XWarSkills::BOW},
{ItemType::LIGHTCROSSBOW,  XWarSkills::CROSSBOW},
{ItemType::SLING,          XWarSkills::SLING},
{ItemType::SLING,          XWarSkills::THROW}
};*/

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
        _HIT = d.GetResult();
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
                _HIT += 10;
                RNG += 2;
                name = "hunter arrow";
            }

            if (it == ItemType::QUARREL) {
                _HIT += 10;
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
            name, RNG, _HIT,
            dice.GetCount(), dice.GetSides(), dice.GetBonus(),
            (aet & AttackEffectType::FIRE) != AttackEffectType::NONE ? " of fire" : ""
        );

    return fmt::format("heap of ({}) {}{}{}s <{:+}>({:+}, {}d{}{:+}){}",
        quantity,
        (aet & AttackEffectType::POISON) != AttackEffectType::NONE ? "poisoned " : "",
        (aet & AttackEffectType::UNDEADSLAYER) != AttackEffectType::NONE ? "holy " : "",
        name, RNG, _HIT,
        dice.GetCount(), dice.GetSides(), dice.GetBonus(),
        (aet & AttackEffectType::FIRE) != AttackEffectType::NONE ? " of fire" : ""
    );
}

bool XMissile::isProperWeapon(XItem * missile, XItem * weapon)
{
    if (weapon) {
        switch (weapon->wt) {
            case XWarSkills::BOW:
                if (missile->it == ItemType::ARROW) {
                    return true;
                } else {
                    return false;
                }

                break;

            case XWarSkills::CROSSBOW:
                if (missile->it == ItemType::QUARREL) {
                    return true;
                } else {
                    return false;
                }

                break;

            case XWarSkills::SLING:
                if (missile->it == ItemType::ROCK || missile->it == ItemType::SLINGBULLET) {
                    return true;
                } else {
                    return false;
                }

                break;

            default:
                return false;
        }

    } else {
        return true; //all can be throwed without weapon
    }
}
