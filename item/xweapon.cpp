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
#include "magic/brand.h"

REGISTER_CLASS(XWeapon);
CEREAL_REGISTER_TYPE(XWeapon);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XItem, XWeapon);

// Filled from world/items/ as those scripts load.
XItemBasicStructure gi_weapon;


XWeapon::XWeapon(ItemType _it)
{
    kind = ItemKind::WEAPON;
    BasicFill(_it, &gi_weapon);
    bp = BP_HAND;
    RNG = 0;
}

std::string XWeapon::toString()
{
    const ItemMaterial* prop = FindMaterial(material_index);
    std::string w_name = prop ? prop->propname : std::string();

    for (int i = 0; i < gi_weapon.total_item; i++) {
        if (it == gi_weapon.pFirstItem[i].it) {
            w_name.append(" ");
            w_name.append(gi_weapon.pFirstItem[i].name);
            break;
        }
    }

    // How many brands of each kind this weapon carries. Content says
    // which kind a brand belongs to; the way the kinds combine into a
    // name is the engine's.
    int ec = 0;
    int bc = 0;
    int sc = 0;

    for (const BRAND& id : aet) {
        const BrandStats* row = FindBrand(id);

        if (!row) {
            continue;
        }

        switch (row->group) {
            case BrandGroup::ELEMENTAL: ec++; break;
            case BrandGroup::BLACK:     bc++; break;
            case BrandGroup::SLAYER:    sc++; break;
        }
    }

    // A black brand suppresses the name altogether - every branch below
    // asks for bc == 0 - so a poisoned sword is named as a plain sword.
    std::string brand_templ;

    if (ec == 1 && bc == 0 && sc == 0) {
        brand_templ = GetTemplate(BrandGroup::ELEMENTAL);
    } else if (ec == 0 && bc == 0 && sc == 1) {
        brand_templ = GetTemplate(BrandGroup::SLAYER);
    } else if (ec >= 1 && bc == 0 && sc == 1) {
        brand_templ = fmt::format("Elemental {}", GetTemplate(BrandGroup::SLAYER));
    } else if (ec >= 1 && bc == 0 && sc > 1) {
        brand_templ = "Elemental {} of Slaying";
    } else if (ec == 0 && bc == 0 && sc > 1) {
        brand_templ = "{} of Slaying";
    } else if (ec > 1 && bc == 0 && sc == 0) {
        brand_templ = "Elemental {}";
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

// The name template of the one brand of this kind that the weapon
// carries. Only called where the count is exactly one.
std::string XWeapon::GetTemplate(const BrandGroup group)
{
    for (const BRAND& id : aet) {
        const BrandStats* row = FindBrand(id);

        if (row && row->group == group) {
            return row->templ;
        }
    }

    return std::string();
}
