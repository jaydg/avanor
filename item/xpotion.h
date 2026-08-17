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

#include "item/item.h"
#include "magic/effect.h"

enum class PotionColor {
    CLEAR,
    SMOKY,
    GREEN,
    ORANGE,
    YELLOW,
    BLACK,
    BLUE,
    WHITE,
    CYAN,
    PURPLE,
    HAZE,
    GOLDEN,
    SILVER,
    AZURE,
    MURKY,
    RED,
    GLOWING,
    MOTTLED,
    BLOBBY,
    PINK,
    MOULDY,
    GRAY,
    MERCURY,
    OILY,
    VISCOUS,
    DARK_RED,
    LIGHT_RED,
    DARK_BLUE,
    LIGHT_BLUE,
    BROWN,
    LIGHT_GRAY,
    DARK_GRAY,
    DARK_GREEN,
    LIGHT_GREEN,
    BEIGE,
    AQUAMARINE,
    CORAL,
    IVORY,
    MAROON,
    TAN,
    TURQUOISE,
    VIOLET,
    RANDOM
};

enum class PotionName {
    UNKNOWN = -1,
    WATER,
    APPLEJUCE,
    ORANGEJUCE,
    HEALING,
    CURE_LIGHT_WOUNDS,
    CURE_SERIOUS_WOUNDS,
    CURE_CRITICAL_WOUNDS,
    CURE_MORTAL_WOUNDS,
    POWER,
    RESTORATION,
    GAIN_STRENGTH,
    GAIN_WILLPOWER,
    GAIN_MANA,
    GAIN_TOUGHNESS,
    GAIN_DEXTERITY,
    CURE_POISON,
    POISON,
    BLEEDNESS,
    DISEASE,
    CURE_DISEASE,
    HEROISM,
    SEE_INVISIBLE,
    WEAKNESS,
    CLUMSINESS,
    DEATH,
    SATIATION,
    STARVATION,
    BOOST_SPEED,
    SLOWNESS,
    ACID_RESISTANCE,
    FIRE_RESISTANCE,
    COLD_RESISTANCE,
    POISON_RESISTANCE,
    RANDOM
};

struct PotionDescription {
    PotionName pn;
    const char* name;
    XEffect::Id effect;
    int rarity;
    int alchemy_power;
    int value; // how much it cost for one potion_power //value * potion_power * [spell_cost]
    PotionColor force_color;
    int identify;

    static PotionColor SelectColor(PotionColor pnc = PotionColor::RANDOM);
    static PotionName GetRandomPotion();
    static void RunOnce();
    static int potion_total_value;
    static PotionDescription* GetRec(PotionName pn);

    // name/effect/rarity/alchemy_power/value are compile-time constants
    // (see potion_descr[]'s static initializer) - only identify/
    // force_color are per-game-session mutable state, same fields the
    // legacy Store/Restore already persisted.
    template<class Archive>
    void serialize(Archive& ar)
    {
        ar(identify, force_color);
    }
};

class XPotion : public XItem
{
    public:
        // Registers PotionName as the Lua table PotionName.MEMBER (prefix
        // dropped, same convention as every other Lua-facing enum table) -
        // lets world scripts create a specific potion (e.g. for a unique
        // NPC's starting gear) via CreateObject(PotionName.HEALING), the
        // only axis ItemKind/ItemType-based creation (ICREATEA/ICREATEB,
        // see item/itemf.cpp) can't reach: every ItemKind::POTION item is
        // created as `new XPotion()` (PotionName::RANDOM) regardless of ItemType.
        static void RegisterLua(sol::state_view& lua);

        DECLARE_CREATOR(XPotion, XItem);
        XPotion(PotionName _pn = PotionName::RANDOM);
        XPotion(XPotion * copy);
        XItem* MakeCopy() override
        {
            return new XPotion(this);
        }

        int isIdentifed() override;
        void Identify(int level) override;
        std::string toString() override;
        int Compare(XObject * o) override;
        int GetValue() override
        {
            return value;
        }

        PotionName pn;
        int onDrink(XCreature * cr);

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
CEREAL_LOAD_VIA_PLACEHOLDER_CONSTRUCT(XAlchemyRecipe, serialize, PotionName::WATER, PotionName::WATER, PotionName::WATER);

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
        static int isValidRecipe(PotionName pn1, PotionName pn2, PotionName pn3);
        static std::string GetRecipeName(PotionName pn1, PotionName pn2, PotionName pn3);
        static PotionName GetPotionName(PotionName pn1, PotionName pn2);
};

#endif
