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

#include "engine/xapi.h"
#include "item/item_cereal.h"
#include "item/xweapon.h"
#include "magic/attack_effect_type.h"

REGISTER_CLASS(XWeapon);
CEREAL_REGISTER_TYPE(XWeapon);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XItem, XWeapon);

// Filled from world/items/ as those scripts load.
XItemBasicStructure gi_weapon;

struct WEAPON_BRAND_TYPE_NAME {
    AttackEffectType brt;
    std::string templ;
};

const int weapon_brand_name_db_size = 17;

WEAPON_BRAND_TYPE_NAME weapon_brand_name_db[weapon_brand_name_db_size] = {
    {AttackEffectType::FIRE,	"{} of Fire",	},
    {AttackEffectType::HELLFIRE,	"{} of Hell Fire",	},
    {AttackEffectType::COLD,	"{} of Cold"	},
    {AttackEffectType::ULTIMATECOLD,	"{} of Ultimate Cold"	},
    {AttackEffectType::LIGHTNING,	"{} of Lightning"	},

    {AttackEffectType::ACID,	"{} of Acid"	},
    {AttackEffectType::POISON,	"{} of Poison"	},
    {AttackEffectType::DEATH,	"{} of Death"	},

    {AttackEffectType::UNDEADSLAYER,	"{} of Slay Undead"	},
    {AttackEffectType::HUMANOIDSLAYER,	"{} of Slay Humanoids"	},
    {AttackEffectType::ANIMALSLAYER,	"{} of Slay Animals"	},
    {AttackEffectType::DRAGONSLAYER,	"{} of Dragon Slaying"	},
    {AttackEffectType::GIANTSLAYER,	"{} of Giant Slaying"	},
    {AttackEffectType::ORCSLAYER,	"{} of Slay Orcs"	},
    {AttackEffectType::TROLLSLAYER,	"{} of Slay Trolls"	},
    {AttackEffectType::TROLLSLAYER,	"{} of Slay Trolls"	},
    {AttackEffectType::DEMONSLAYER,	"{} of Slay Demons"	},
};

XWeapon::XWeapon(ItemType _it)
{
    kind = ItemKind::WEAPON;
    BasicFill(_it, &gi_weapon);
    bp = BP_HAND;
    RNG = 0;
}

std::string XWeapon::toString()
{
    ItemMaterial* prop = GetMaterial(material_index);
    std::string w_name = prop->propname;

    for (int i = 0; i < gi_weapon.total_item; i++) {
        if (it == gi_weapon.pFirstItem[i].it) {
            w_name.append(" ");
            w_name.append(gi_weapon.pFirstItem[i].name);
            break;
        }
    }

    std::string brand_templ;

    if (aet != AttackEffectType::NONE) {
        int ec = vBitsCount(static_cast<unsigned int>(aet & AttackEffectType::ELEMENTAL_MASK));
        int bc = vBitsCount(static_cast<unsigned int>(aet & AttackEffectType::BLACK_MASK));
        int sc = vBitsCount(static_cast<unsigned int>(aet & AttackEffectType::SLAYER_MASK));

        if (ec == 1 && bc == 0 && sc == 0) {
            brand_templ = GetTemplate(aet & AttackEffectType::ELEMENTAL_MASK);
        } else if (ec == 0 && bc == 0 && sc == 1) {
            brand_templ = GetTemplate(aet & AttackEffectType::SLAYER_MASK);
        } else if (ec >= 1 && bc == 0 && sc == 1) {
            brand_templ = fmt::format("Elemental {}", GetTemplate(aet & AttackEffectType::SLAYER_MASK));
        } else if (ec >= 1 && bc == 0 && sc > 1) {
            brand_templ = "Elemental {} of Slaying";
        } else if (ec == 0 && bc == 0 && sc > 1) {
            brand_templ = "{} of Slaying";
        } else if (ec > 1 && bc == 0 && sc == 0) {
            brand_templ = "Elemental {}";
        }
    }

    std::string fullname;
    if (quantity == 1) {
        if (!brand_templ.empty()) {
            fullname = fmt::format(brand_templ, w_name);
        } else {
            fullname = w_name;
        }
    } else {
        if (!brand_templ.empty()) {
            fullname = fmt::format("heap of ({})" + brand_templ,
                quantity, w_name);
        } else {
            fullname = fmt::format("heap of ({}) {}s", quantity, w_name);
        }
    }

    if (isIdentified()) {
        if (RNG != 0) {
            fullname.append(fmt::format(" <{:+}>", RNG));
        }

        if (dv != 0 || pv != 0) {
            fullname.append(fmt::format(" [{:+}, {:+}]", dv, pv));
        }

        fullname.append(fmt::format(" ({:+}, {}d{}{:+})",
            to_hit, dice.GetCount(), dice.GetSides(), dice.GetBonus()));

        fullname.append(StatsToString());
    }

    return fullname;
}

std::string XWeapon::GetTemplate(AttackEffectType mask)
{
    std::string brand_templ;

    for (int j = 0; j < weapon_brand_name_db_size; j++) {
        if (weapon_brand_name_db[j].brt == mask) {
            brand_templ = weapon_brand_name_db[j].templ;
            break;
        }
    }

    return brand_templ;
}
