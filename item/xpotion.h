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

#ifndef XPOTION_H
#define XPOTION_H

#include <memory>
#include <vector>

#include <cereal/archives/json.hpp>
#include <cereal/types/base_class.hpp>
#include <sol/forward.hpp>

#include "helpers/registry.h"
#include "item/item.h"
#include "magic/effect.h"

// What a potion looks like before anyone knows what it is - the id
// world/items/potion_colours.lua registered it under. A string, like
// every other content id: which appearances a world has is content, and
// the engine only needs to know that each potion gets a different one.
using POTION_COLOUR = std::string;

// "Any free appearance": what a potion asks for when content does not
// insist on a particular one.
inline constexpr const char* PC_ANY = "";

struct PotionColourStats {
    POTION_COLOUR id;

    // What it reads as: "dark red" where the id is "dark_red".
    std::string name;

    // The colour it is drawn in.
    int colour = 0;
};

extern Registry<PotionColourStats> potion_colours_db;

const PotionColourStats* FindPotionColour(const POTION_COLOUR& id);

// Fluent builder:
//
//   PotionColour.new("dark_red")
//       :Called("dark red")
//       :Looks(xColor.xRED)
//       :Register()
class PotionColourBuilder
{
    public:
        explicit PotionColourBuilder(std::string id);

        PotionColourBuilder& Called(const std::string& name);
        PotionColourBuilder& Looks(int colour);
        void Register();

    private:
        PotionColourStats t;
};

// Which potion this is - the id world/items/potions.lua registered it
// under. A string like every other content id: which potions exist is
// content, and the engine names none of them.
using PotionName = std::string;

// "No potion": a request that names none, and the answer when two potions
// mix into nothing. Was PN_NONE and ::UNKNOWN, which as strings
// are one value.
inline constexpr const char* PN_NONE = "";

struct PotionDescription {
    PotionName pn;
    std::string name;
    EFFECT effect{EFFECT_NONE};
    int rarity{0};
    int alchemy_power{0};
    int value{0}; // how much it cost for one potion_power //value * potion_power * [spell_cost]

    // The Lua function that runs when this potion is drunk and it names no
    // effect - the drinks, the stat gains, the ones that lay a modifier.
    // Empty for a potion that is its effect and nothing more.
    std::string on_drink;

    POTION_COLOUR force_color;
    bool identified{false};

    static POTION_COLOUR SelectColor(const POTION_COLOUR& pnc = PC_ANY);
    static PotionName GetRandomPotion();

    // A potion drawn with every sort equally likely, ignoring rarity -
    // what the herb table wants when it decides which potion a species
    // distils into, where a common herb yielding a rare potion is the
    // point rather than a mistake.
    static PotionName GetAnyPotion();
    static int potion_total_value;
    static PotionDescription* GetRec(const PotionName& pn);

    // name/effect/rarity/alchemy_power/value are compile-time constants
    // (see potion_descr[]'s static initializer) - only identified/
    // force_color are per-game-session mutable state, same fields the
    // legacy Store/Restore already persisted.
    template<class Archive>
    void serialize(Archive& ar)
    {
        ar(identified, force_color);
    }
};

// Fluent builder for one sort of potion:
//
//   Potion.new("healing")
//       :Called("healing")
//       :Effect(XEffect.HEAL)
//       :Chance(10)
//       :Worth(200)
//       :Alchemy(4)
//       :Looks("white")
//       :Register()
//
// PotionDescription is visible here (XPotion holds one by pointer), but
// the table itself stays private to xpotion.cpp.
class PotionBuilder
{
    public:
        explicit PotionBuilder(std::string id);

        PotionBuilder& Called(const std::string& n);
        PotionBuilder& Effect(const EFFECT& eff);
        PotionBuilder& Chance(int rarity);
        PotionBuilder& Worth(int value);
        PotionBuilder& Alchemy(int power);
        PotionBuilder& Looks(const std::string& colour);
        PotionBuilder& OnDrink(const std::string& handler);

        void Register();

    private:
        PotionDescription t;
};

class XPotion : public XItem
{
    public:
        // Only PotionColor now: which potions exist is content, so there
        // is no table of potion names to register any more.
        static void RegisterLua(sol::state_view& lua);

        DECLARE_CREATOR(XPotion, XItem);
        explicit XPotion(const PotionName& _pn = PN_NONE);
        XPotion(XPotion * copy);
        XItem* MakeCopy() override
        {
            return new XPotion(this);
        }

        bool isIdentified() override;
        void Identify() override;
        std::string toString() override;
        bool Compare(XObject* o) override;
        int GetValue() override
        {
            return value;
        }

        PotionName pn;
        void onDrink(XCreature * cr);

        // Non-template, concrete-archive-typed (like XCreature::Save/
        // LoadModifier): potion_descr[] is private to xpotion.cpp, so
        // these are declared here but defined there, only ever called
        // from the top-level save/restore code (never nested inside
        // another type's own template serialize(), so no per-TU
        // visibility concern).
        static void SaveTable(cereal::JSONOutputArchive& ar);
        static void LoadTable(cereal::JSONInputArchive& ar);

        // pdescr is a non-owning pointer into a static table
        // (potion_descr[], private to xpotion.cpp), not owned/serialized
        // data - only `pn` is persisted, and pdescr is re-derived from it
        // on load via FixupDescr() (defined in the .cpp, where
        // potion_descr[] is visible).
        //
        // One symmetric serialize() rather than a split save()/load()
        // pair: XBaseObject (an ancestor) has its own member serialize(),
        // which makes a derived save()/load() pair ambiguous to Cereal -
        // and, as found and fixed for XCreature earlier this session,
        // even a correctly-disambiguated split pair silently breaks
        // Cereal's *polymorphic type registration* for the type at
        // runtime ("Trying to save an unregistered polymorphic type"),
        // not just compile-time resolution.
        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XItem>(this));
            ar(pn);

            if constexpr (Archive::is_loading::value) {
                FixupDescr();
            }
        }

    protected:
        void FixupDescr();
        PotionDescription* pdescr;
};

class XAlchemyRecipe
{
    public:
        PotionName pn1;
        PotionName pn2;
        PotionName result;

        XAlchemyRecipe(PotionName p1, PotionName p2, PotionName res) :
            pn1(p1), pn2(p2), result(res) {}

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(pn1, pn2, result);
        }
};

// XAlchemyRecipe has no default constructor at all (not even a deleted
// one) - route Cereal's load-time construction through the real
// constructor with placeholders. Lives here, not in xpotion.cpp:
// recipe_list (a vector<unique_ptr<XAlchemyRecipe>>) is a field of
// XHero, and its own deserialization is inline in XHero::serialize()
// (a header template) - a specialization declared only in xpotion.cpp
// wouldn't be visible wherever that gets instantiated (see the same
// reasoning, first hit for XStandardAI, in std_ai.h).
CEREAL_LOAD_VIA_PLACEHOLDER_CONSTRUCT(XAlchemyRecipe, serialize, "water", "water", "water");

class XAlchemy
{
        void BuildRecipes(int al_lvl);

        static int GetPotionCount(int al_lvl, PotionName** pTable);
        std::vector<std::unique_ptr<XAlchemyRecipe>> recipes;
    public:
        XAlchemy();
        ~XAlchemy();
        static int GetRecipeCount();
        static XAlchemyRecipe* GetRecipe(int num);
        static void Init();
        static int isValidRecipe(const PotionName& pn1, const PotionName& pn2, const PotionName& pn3);
        static std::string GetRecipeName(const PotionName& pn1, const PotionName& pn2, const PotionName& pn3);
        static PotionName GetPotionName(const PotionName& pn1, const PotionName& pn2);
};

#endif
