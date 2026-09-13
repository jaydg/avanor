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

#include "helpers/registry.h"
#include "item/item.h"
#include <iostream>

#include <sol/sol.hpp>

#include "item/itemdb.h"
#include "magic/brand.h"

Registry<ItemMaterial> item_prop{"material"};

const ItemMaterial* FindMaterial(const std::string& id)
{
    return item_prop.Find(id);
}

MaterialBuilder::MaterialBuilder(std::string id)
{
    t.id = std::move(id);
}

Registry<MaterialSet> material_sets{"material set"};

const MaterialSet* FindMaterialSet(const std::string& id)
{
    return material_sets.Find(id);
}

std::vector<std::string> MaterialsIn(const std::string& set)
{
    if (const MaterialSet* row = FindMaterialSet(set)) {
        return row->members;
    }

    // Not a set: a template may name one material outright.
    if (FindMaterial(set)) {
        return {set};
    }

    return {};
}

MaterialSetBuilder::MaterialSetBuilder(std::string id)
{
    t.id = std::move(id);
}

MaterialSetBuilder& MaterialSetBuilder::Of(const sol::table& members)
{
    for (const auto& entry : members) {
        const std::string name = entry.second.as<std::string>();

        // A set may be built out of other sets: they are declared before
        // they are used, so their members are known by now and spliced in
        // here rather than resolved every time one is drawn from.
        if (const MaterialSet* nested = FindMaterialSet(name)) {
            for (const auto& m : nested->members) {
                t.members.push_back(m);
            }
        } else {
            t.members.push_back(name);
        }
    }

    return *this;
}

void MaterialSetBuilder::Register()
{
    if (t.id.empty()) {
        std::cerr << "world: a material set with no id" << std::endl;
        return;
    }

    if (FindMaterialSet(t.id)) {
        std::cerr << "world: two material sets both called '" << t.id << "'" << std::endl;
        return;
    }

    for (const auto& m : t.members) {
        if (!FindMaterial(m)) {
            std::cerr << "world: the material set '" << t.id << "' contains '" << m
                      << "', which is not a material" << std::endl;
        }
    }

    material_sets.Add(t);
}

MaterialBuilder& MaterialBuilder::Called(const std::string& name)
{
    t.propname = name;
    return *this;
}

MaterialBuilder& MaterialBuilder::Looks(const int color)
{
    t.color = color;
    return *this;
}

MaterialBuilder& MaterialBuilder::Chance(const int probability)
{
    t.probability = probability;
    return *this;
}

MaterialBuilder& MaterialBuilder::Quality(const ITEM_QUALITY iq)
{
    t.iq = iq;
    return *this;
}

MaterialBuilder& MaterialBuilder::Body(const int density, const int value)
{
    t.density = density;
    t.value = value;
    return *this;
}

MaterialBuilder& MaterialBuilder::Armour(const std::string& dv, const std::string& pv)
{
    t.dv = dv;
    t.pv = pv;
    return *this;
}

MaterialBuilder& MaterialBuilder::Combat(const std::string& hit,
    const std::string& dice, const std::string& extra)
{
    t.hit = hit;
    t.dice = dice;
    t.z = extra;
    return *this;
}

MaterialBuilder& MaterialBuilder::Resist(const std::string& r)
{
    t.resistance = r;
    return *this;
}

MaterialBuilder& MaterialBuilder::Property(const SPECIAL_PROPERTY sp)
{
    t.sp = sp;
    return *this;
}

void MaterialBuilder::Register()
{
    if (t.id.empty()) {
        std::cerr << "world: a material with no id" << std::endl;
        return;
    }

    if (FindMaterial(t.id)) {
        std::cerr << "world: two materials both called '" << t.id << "'"
                  << std::endl;
        return;
    }

    if (t.propname.empty()) {
        t.propname = t.id;
    }

    item_prop.Add(t);
}

Registry<ENHANCE_STRUCT> ienh_db{"armour enchantment"};

// The summed weights, so a draw is one roll rather than a walk.
static int ienh_total_weight = 0;

const ENHANCE_STRUCT* FindArmourEnchantment(const std::string& id)
{
    return ienh_db.Find(id);
}

std::string RandomArmourEnchantment()
{
    if (ienh_db.empty()) {
        return {};
    }

    int val = vRand(ienh_total_weight);

    for (const auto& row : ienh_db) {
        val -= row.weight;

        if (val < 0) {
            return row.id;
        }
    }

    return ienh_db.front().id;
}

ArmourEnchantmentBuilder::ArmourEnchantmentBuilder(std::string id)
{
    t.id = std::move(id);
}

ArmourEnchantmentBuilder& ArmourEnchantmentBuilder::Called(const std::string& name)
{
    t.name = name;
    return *this;
}

ArmourEnchantmentBuilder& ArmourEnchantmentBuilder::Looks(const int color)
{
    t.color = color;
    return *this;
}

ArmourEnchantmentBuilder& ArmourEnchantmentBuilder::Rarity(const int rarity)
{
    t.rarity = rarity;
    return *this;
}

ArmourEnchantmentBuilder& ArmourEnchantmentBuilder::Weight(const int weight)
{
    t.weight = weight;
    return *this;
}

ArmourEnchantmentBuilder& ArmourEnchantmentBuilder::Fits(const ItemKind kind)
{
    t.kind = kind;
    return *this;
}

// One brand per call, as on an item definition.
ArmourEnchantmentBuilder& ArmourEnchantmentBuilder::Brand(const std::string& brt)
{
    if (CheckBrandExists(brt, "an armour enchantment")) {
        t.brt.Add(brt);
    }

    return *this;
}

ArmourEnchantmentBuilder& ArmourEnchantmentBuilder::Property(const SPECIAL_PROPERTY spp)
{
    t.spp = spp;
    return *this;
}

ArmourEnchantmentBuilder& ArmourEnchantmentBuilder::Stats(const std::string& s)
{
    t.s = s;
    return *this;
}

ArmourEnchantmentBuilder& ArmourEnchantmentBuilder::Resist(const std::string& r)
{
    t.r = r;
    return *this;
}

void ArmourEnchantmentBuilder::Register()
{
    if (t.id.empty()) {
        std::cerr << "world: an armour enchantment with no id" << std::endl;
        return;
    }

    if (FindArmourEnchantment(t.id)) {
        std::cerr << "world: two armour enchantments both called '" << t.id
                  << "'" << std::endl;
        return;
    }

    ienh_total_weight += t.weight;
    ienh_db.Add(t);
}


TemplateBuilder::TemplateBuilder(const ItemKind kind, const ItemType it) : kind(kind)
{
    t.it = it;
    t.view = '?';
    t.iset = "iron";
    t.iq = IQ_AVG;
    t.wt = CS_NONE;
    t.launcher = CS_NONE;
}

TemplateBuilder& TemplateBuilder::View(const std::string& name, const std::string& view)
{
    t.name = name;
    t.view = view.empty() ? '?' : view[0];
    return *this;
}

TemplateBuilder& TemplateBuilder::Made(const ITEM_SET& iset, const ITEM_QUALITY iq)
{
    t.iset = iset;
    t.iq = iq;
    return *this;
}

TemplateBuilder& TemplateBuilder::Skill(const COMBAT_SKILL wt)
{
    t.wt = wt;
    return *this;
}

TemplateBuilder& TemplateBuilder::Verb(const std::string& verb)
{
    t.verb = verb;
    return *this;
}

TemplateBuilder& TemplateBuilder::Launcher(const COMBAT_SKILL wt)
{
    t.launcher = wt;
    return *this;
}

TemplateBuilder& TemplateBuilder::Worth(const int value, const int weight)
{
    t.value = value;
    t.valume = weight;
    return *this;
}

TemplateBuilder& TemplateBuilder::Armour(const std::string& dv, const std::string& pv)
{
    t.dv = dv;
    t.pv = pv;
    return *this;
}

TemplateBuilder& TemplateBuilder::Combat(const std::string& hit, const std::string& dice,
    const std::string& extra)
{
    t.hit = hit;
    t.dice = dice;
    t.z = extra;
    return *this;
}

TemplateBuilder& TemplateBuilder::Range(const std::string& range)
{
    t.r = range;
    return *this;
}

TemplateBuilder& TemplateBuilder::Chance(const int probability)
{
    t.probability = probability;
    return *this;
}

TemplateBuilder& TemplateBuilder::OnOutfit(const std::string& handler)
{
    t.on_outfit = handler;
    return *this;
}

TemplateBuilder& TemplateBuilder::OnCreate(const std::string& handler)
{
    t.on_create = handler;
    return *this;
}

XItemBasicStructure* PoolFor(const ItemKind kind)
{
    switch (kind) {
        case ItemKind::WEAPON:   return &gi_weapon;
        case ItemKind::MISSILEW: return &gi_missilew;
        case ItemKind::MISSILE:  return &gi_missile;
        case ItemKind::BODY:     return &gi_armour;
        case ItemKind::SHIELD:   return &gi_shield;
        case ItemKind::HAT:      return &gi_cap;
        case ItemKind::CLOAK:    return &gi_cloaks;
        case ItemKind::BOOTS:    return &gi_boots;
        case ItemKind::GLOVES:   return &gi_gloves;
        default: return nullptr;
    }
}

void TemplateBuilder::Register()
{
    XItemBasicStructure* pool = PoolFor(kind);

    if (!pool) {
        std::cerr << "world: '" << t.name
                  << "' is of a kind that has no table of its own" << std::endl;

        return;
    }

    if (t.name.empty()) {
        std::cerr << "world: a template of kind " << static_cast<int>(kind)
                  << " has no name" << std::endl;
    }

    pool->Add(t);
}
