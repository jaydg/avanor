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

#ifndef	XENHANCE_H
#define	XENHANCE_H

#include <cereal/types/base_class.hpp>

#include <string>

#include <sol/forward.hpp>

#include <cereal/archives/json.hpp>

#include "item/item.h"
#include "magic/resist.h"

// Fluent builder for one sort of ring or amulet:
//
//   Enchantment.new("protection")
//       :Called("of protection")
//       :Armour("", "1d6-2")
//       :Worth(150)
//       :Register()
//
// ENH_REC stays private to xenhance.cpp, so the builder holds the fields.
class EnchantmentBuilder
{
    public:
        explicit EnchantmentBuilder(std::string id);

        EnchantmentBuilder& Called(const std::string& n);
        EnchantmentBuilder& Armour(const std::string& dv, const std::string& pv);
        EnchantmentBuilder& Combat(const std::string& hit, const std::string& dice,
            const std::string& extra);
        EnchantmentBuilder& Range(const std::string& rng);
        EnchantmentBuilder& Resist(const std::string& r);
        EnchantmentBuilder& Stats(const std::string& s);
        EnchantmentBuilder& Worth(int value);

        void Register();

    private:
        std::string id;
        std::string name;
        std::string dv, pv, hit, dice, z, rng, r, s;
        int value{0};
};

// The id of an enchantment that grants this resistance, drawn at random from
// those that do, or empty when content declares none. What "a ring that lets
// you see the invisible" is made of is declared by the enchantment table.
std::string EnchantmentGranting(const RESISTANCE& resist);

// The looks an unknown ring or amulet can have - "a ruby ring", "an
// obsidian amulet" - given to the engine by world/items/enchantments.lua
// and dealt out at random, one per sort, each game.
void SetEnchantmentLooks(const sol::table& looks);

class XEnhance : public XItem
{
    protected:
        // Which sort this is - the id world/items/enchantments.lua
        // registered it under. Was an index into a fixed table.
        std::string descr;
    public:
        // "Any": the request to pick a sort at random.
        static const std::string RANDOM;

        DECLARE_CREATOR(XEnhance, XItem);

        explicit XEnhance(const std::string& enh = "");

        explicit XEnhance(XEnhance* copy) : XItem(static_cast<XItem *>(copy))
        {
            descr = copy->descr;
        }

        // What this game calls it before anyone knows it, and whether they
        // do. Kept here rather than in two parallel arrays in XRing and
        // XAmulet, which is what they used to be.
        [[nodiscard]] const std::string& AppearanceName() const;

        bool isIdentified() override;
        void Identify() override;
        std::string toString() override;

        // Non-template, concrete-archive-typed: the table is private to
        // xenhance.cpp. Was two arrays, one per XRing and XAmulet - one
        // table serves both, because a ring and an amulet of the same sort
        // are the same discovery.
        static void SaveTable(cereal::JSONOutputArchive& ar);
        static void LoadTable(cereal::JSONInputArchive& ar);
        bool Compare(XObject* o) override;

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XItem>(this));
            ar(descr);
        }
};

#endif
