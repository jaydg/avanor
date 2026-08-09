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

#include "item/item.h"
#include "magic/effect.h"

enum POTION_COLOR {
    PNC_CLEAR,
    PNC_SMOKY,
    PNC_GREEN,
    PNC_ORANGE,
    PNC_YELLOW,
    PNC_BLACK,
    PNC_BLUE,
    PNC_WHITE,
    PNC_CYAN,
    PNC_PURPLE,
    PNC_HAZE,
    PNC_GOLDEN,
    PNC_SILVER,
    PNC_AZURE,
    PNC_MURKY,
    PNC_RED,
    PNC_GLOWING,
    PNC_MOTTLED,
    PNC_BLOBBY,
    PNC_PINK,
    PNC_MOULDY,
    PNC_GRAY,
    PNC_MERCURY,
    PNC_OILY,
    PNC_VISCOUS,
    PNC_DARK_RED,
    PNC_LIGHT_RED,
    PNC_DARK_BLUE,
    PNC_LIGHT_BLUE,
    PNC_BROWN,
    PNC_LIGHT_GRAY,
    PNC_DARK_GRAY,
    PNC_DARK_GREEN,
    PNC_LIGHT_GREEN,
    PNC_BEIGE,
    PNC_AQUAMARINE,
    PNC_CORAL,
    PNC_IVORY,
    PNC_MAROON,
    PNC_TAN,
    PNC_TURQUOISE,
    PNC_VIOLET,
    PNC_RANDOM
};

enum POTION_NAME {
    PN_UNKNOWN = -1,
    PN_WATER,
    PN_APPLEJUCE,
    PN_ORANGEJUCE,
    PN_HEALING,
    PN_CURE_LIGHT_WOUNDS,
    PN_CURE_SERIOUS_WOUNDS,
    PN_CURE_CRITICAL_WOUNDS,
    PN_CURE_MORTAL_WOUNDS,
    PN_POWER,
    PN_RESTORATION,
    PN_GAIN_STRENGTH,
    PN_GAIN_WILLPOWER,
    PN_GAIN_MANA,
    PN_GAIN_TOUGHNESS,
    PN_GAIN_DEXTERITY,
    PN_CURE_POISON,
    PN_POISON,
    PN_BLEEDNESS,
    PN_DISEASE,
    PN_CURE_DISEASE,
    PN_HEROISM,
    PN_SEE_INVISIBLE,
    PN_WEAKNESS,
    PN_CLUMSINESS,
    PN_DEATH,
    PN_SATIATION,
    PN_STARVATION,
    PN_BOOST_SPEED,
    PN_SLOWNESS,
    PN_ACID_RESISTANCE,
    PN_FIRE_RESISTANCE,
    PN_COLD_RESISTANCE,
    PN_POISON_RESISTANCE,
    PN_RANDOM
};

struct POTION_REC {
    POTION_NAME pn;
    const char* name;
    XEffect::Id effect;
    int rarity;
    int alchemy_power;
    int value; // how much it cost for one potion_power //value * potion_power * [spell_cost]
    POTION_COLOR force_color;
    int identify;

    static POTION_COLOR SelectColor(POTION_COLOR pnc = PNC_RANDOM);
    static POTION_NAME GetRandomPotion();
    static void RunOnce();
    static int potion_total_value;
    static POTION_REC* GetRec(POTION_NAME pn);

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
        DECLARE_CREATOR(XPotion, XItem);
        XPotion(POTION_NAME _pn = PN_RANDOM);
        XPotion(XPotion * copy);
        XObject* MakeCopy() override
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

        POTION_NAME pn;
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
        POTION_REC* pdescr;
};

class XAlchemyRec
{
    public:
        POTION_NAME pn1;
        POTION_NAME pn2;
        POTION_NAME result;

        XAlchemyRec(POTION_NAME p1, POTION_NAME p2, POTION_NAME res) :
            pn1(p1), pn2(p2), result(res) {}

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(pn1, pn2, result);
        }
};

// XAlchemyRec has no default constructor at all (not even a deleted
// one) - route Cereal's load-time construction through the real
// constructor with placeholders. Lives here, not in xpotion.cpp:
// reception_list (a vector<unique_ptr<XAlchemyRec>>) is a field of
// XHero, and its own deserialization is inline in XHero::serialize()
// (a header template) - a specialization declared only in xpotion.cpp
// wouldn't be visible wherever that gets instantiated (see the same
// reasoning, first hit for XStandardAI, in std_ai.h).
CEREAL_LOAD_VIA_PLACEHOLDER_CONSTRUCT(XAlchemyRec, serialize, PN_WATER, PN_WATER, PN_WATER);

class XAlchemy
{
        void BuildReception(int al_lvl);

        static int GetPotionCount(int al_lvl, POTION_NAME** pTable);
        std::vector<std::unique_ptr<XAlchemyRec>> reception;
    public:
        XAlchemy();
        ~XAlchemy();
        static int GetReceptionCount();
        static XAlchemyRec* GetReception(int num);
        static void Init();
        static int isValidReception(POTION_NAME pn1, POTION_NAME pn2, POTION_NAME pn3);
        static std::string GetReceptionName(POTION_NAME pn1, POTION_NAME pn2, POTION_NAME pn3);
        static POTION_NAME GetPotionName(POTION_NAME pn1, POTION_NAME pn2);
};

#endif
