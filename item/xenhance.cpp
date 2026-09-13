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

#include <iostream>
#include <map>
#include <vector>

#include <cereal/types/map.hpp>
#include <cereal/types/string.hpp>
#include <sol/sol.hpp>

#include <fmt/format.h>

#include "helpers/registry.h"
#include "item/item_cereal.h"
#include "item/xenhance.h"

REGISTER_CLASS(XEnhance);
CEREAL_REGISTER_TYPE(XEnhance);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XItem, XEnhance);

// One sort of ring or amulet. Filled from world/items/enchantments.lua
// as that script loads.
struct ENH_REC {
    std::string id;
    std::string name;
    std::string dv, pv, hit, dice, z, rng, r, s;
    int value{0};

    // What this game calls one nobody has identified, and its colour on
    // screen. Dealt out here rather than declared, and dealt afresh each
    // game - see TakeLook().
    std::string look;
    int color{0};
    bool identified{false};
};

// The looks content offered, and which have been dealt.
struct EnchantmentLook {
    std::string name;
    int color{0};
};

static std::vector<EnchantmentLook> enh_looks;
static Registry<ENH_REC> enh_db{"enchantment"};

const std::string XEnhance::RANDOM;

void SetEnchantmentLooks(const sol::table& looks)
{
    enh_looks.clear();

    for (size_t i = 1; i <= looks.size(); i++) {
        sol::table row = looks[i];
        EnchantmentLook look;
        look.name = row[1];
        look.color = row[2];
        enh_looks.push_back(std::move(look));
    }
}

static ENH_REC* FindEnchantment(const std::string& id)
{
    for (auto& row : enh_db) {
        if (row.id == id) {
            return &row;
        }
    }

    return nullptr;
}

std::string EnchantmentGranting(const RESISTANCE& resist)
{
    std::vector<const ENH_REC*> granting;

    for (const auto& row : enh_db) {
        if (row.r.empty()) {
            continue;
        }

        // Built the same way the item itself builds its resistances, so
        // whatever the spec means there it means here.
        if (XResistance(row.r.c_str()).GetResistance(resist) > 0) {
            granting.push_back(&row);
        }
    }

    if (granting.empty()) {
        return std::string();
    }

    return granting[vRand(static_cast<int>(granting.size()))]->id;
}

// A look no other sort has taken this game. The old table dealt these in a
// static initialiser, before main() and before any seed, and then named
// the item by its *enhancement* index rather than by the look it drew - so
// "of protection" read as "white ring" in every game ever played, which is
// no disguise at all, and the name did not even match the colour drawn.
static const EnchantmentLook* TakeLook()
{
    std::vector<const EnchantmentLook*> free_looks;

    for (const auto& look : enh_looks) {
        bool taken = false;

        for (const auto& row : enh_db) {
            if (row.look == look.name) {
                taken = true;
                break;
            }
        }

        if (!taken) {
            free_looks.push_back(&look);
        }
    }

    if (free_looks.empty()) {
        std::cerr << "world: more sorts of ring than looks to disguise them as"
                  << std::endl;

        return enh_looks.empty() ? nullptr : &enh_looks.front();
    }

    return free_looks[vRand(static_cast<int>(free_looks.size()))];
}

EnchantmentBuilder::EnchantmentBuilder(std::string id) : id(std::move(id)) {}

EnchantmentBuilder& EnchantmentBuilder::Called(const std::string& n)
{
    name = n;
    return *this;
}

EnchantmentBuilder& EnchantmentBuilder::Armour(const std::string& _dv,
    const std::string& _pv)
{
    dv = _dv;
    pv = _pv;
    return *this;
}

EnchantmentBuilder& EnchantmentBuilder::Combat(const std::string& _hit,
    const std::string& _dice, const std::string& extra)
{
    hit = _hit;
    dice = _dice;
    z = extra;
    return *this;
}

EnchantmentBuilder& EnchantmentBuilder::Range(const std::string& _rng)
{
    rng = _rng;
    return *this;
}

EnchantmentBuilder& EnchantmentBuilder::Resist(const std::string& _r)
{
    r = _r;
    return *this;
}

EnchantmentBuilder& EnchantmentBuilder::Stats(const std::string& _s)
{
    s = _s;
    return *this;
}

EnchantmentBuilder& EnchantmentBuilder::Worth(const int v)
{
    value = v;
    return *this;
}

void EnchantmentBuilder::Register()
{
    ENH_REC row;
    row.id = id;
    row.name = name;
    row.dv = dv;
    row.pv = pv;
    row.hit = hit;
    row.dice = dice;
    row.z = z;
    row.rng = rng;
    row.r = r;
    row.s = s;
    row.value = value;

    if (!enh_db.Add(std::move(row))) {
        return;
    }

    // Taken after the row is in the table, so it counts itself out.
    if (const EnchantmentLook* look = TakeLook()) {
        enh_db.back().look = look->name;
        enh_db.back().color = look->color;
    }
}

XEnhance::XEnhance(const std::string& enh)
{
    descr = enh;

    if (descr.empty() && !enh_db.empty()) {
        descr = enh_db[vRand(static_cast<int>(enh_db.size()))].id;
    }

    const ENH_REC* is = FindEnchantment(descr);

    if (!is) {
        std::cerr << "world: nothing defines an enchantment '" << descr << "'"
                  << std::endl;

        weight = 3;
        return;
    }

    color = is->color;
    value = is->value;
    weight = 1;
    auto d = new XDice(is->dv);
    dv = d->Throw();

    d->Setup(is->pv);
    pv = d->Throw();

    d->Setup(is->hit);
    to_hit = d->Throw();

    d->Setup(is->dice);
    int tx = d->GetCount();
    int ty = d->GetSides();

    d->Setup(is->z);
    dice.Setup(tx, ty, d->Throw());

    d->Setup(is->rng);
    RNG = d->Throw();
    delete d;
    stats = std::make_unique<XStats>(is->s.c_str());
    resistances = std::make_unique<XResistance>(is->r.c_str());

    weight = 3;
}

const std::string& XEnhance::AppearanceName() const
{
    static const std::string unknown = "plain";
    const ENH_REC* row = FindEnchantment(descr);
    return row ? row->look : unknown;
}

bool XEnhance::isIdentified()
{
    const ENH_REC* row = FindEnchantment(descr);
    return row && row->identified;
}

void XEnhance::Identify()
{
    if (ENH_REC* row = FindEnchantment(descr)) {
        row->identified = true;
    }
}

// What this game came to know about each sort, keyed by id: the look it
// drew and whether anyone has worked it out. Was two parallel bool arrays,
// one in XRing and one in XAmulet, indexed by position.
struct EnchantmentMemory {
    bool identified{false};
    std::string look;
    int color{0};

    template<class Archive>
    void serialize(Archive& ar)
    {
        ar(identified, look, color);
    }
};

void XEnhance::SaveTable(cereal::JSONOutputArchive& ar)
{
    std::map<std::string, EnchantmentMemory> learned;

    for (const auto& row : enh_db) {
        learned[row.id] = EnchantmentMemory{row.identified, row.look, row.color};
    }

    ar(learned);
}

void XEnhance::LoadTable(cereal::JSONInputArchive& ar)
{
    std::map<std::string, EnchantmentMemory> learned;
    ar(learned);

    for (auto& row : enh_db) {
        if (const auto it = learned.find(row.id); it != learned.end()) {
            row.identified = it->second.identified;
            row.look = it->second.look;
            row.color = it->second.color;
        }
    }
}

bool XEnhance::Compare(XObject* o)
{
    const auto tit = dynamic_cast<XEnhance *>(o);

    // Not a ring or an amulet of the ordinary sort at all. Items are
    // offered to each other for merging on matching kind alone (see
    // XItemList::TryMerge), and an artifact ring is a plain item of kind
    // RING rather than an XEnhance - so this cast can fail, and used to
    // be dereferenced without asking.
    if (!tit) {
        return false;
    }

    return descr == tit->descr && XItem::Compare(o);
}

std::string XEnhance::toString()
{
    if (!isIdentified()) {
        if (quantity == 1) {
            return fmt::format("{} {}", AppearanceName(), name);
        }

        return fmt::format("heap of ({}) {} {}s",
            quantity, AppearanceName(), name);
    }

    std::string fullname;

    if (quantity == 1) {
        const ENH_REC* row = FindEnchantment(descr);
        fullname = fmt::format("{} {}", name, row ? row->name : std::string());
    } else {
        const ENH_REC* row = FindEnchantment(descr);
        fullname = fmt::format("heap of ({}) {}s {}",
            quantity, name, row ? row->name : std::string());
    }

    if (RNG != 0) {
        fullname.append(fmt::format(" <{:+}>", RNG));
    }

    if (dice.GetBonus() != 0 && to_hit != 0) {
        fullname.append(fmt::format(" ({:+}, {:+})", to_hit, dice.GetBonus()));
    } else {
        if (to_hit != 0) {
            fullname.append(fmt::format(" ({:+})", to_hit));
        }

        if (dice.GetBonus() != 0) {
            fullname.append(fmt::format(" ({:+})", dice.GetBonus()));
        }
    }

    if (dv != 0 && pv != 0) {
        fullname.append(fmt::format(" [{:+}, {:+}]", dv, pv));
    } else {
        if (dv != 0) {
            fullname.append(fmt::format(" [{:+}]", dv));
        }

        if (pv != 0) {
            fullname.append(fmt::format(" [{:+}]", pv));
        }
    }

    fullname.append(StatsToString());

    return fullname;
}

