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

#ifndef XHERB_H
#define XHERB_H

#include <cereal/archives/json.hpp>

#include "item/xanyfood.h"
#include "item/xpotion.h"

enum HERB_TYPE {
    HT_HERB,
    HT_MUSHROOM,
};

// One species of plant: what the picked part is called, what the plant it
// grows on is called, how it tastes, how it looks, and whether it is a herb
// or a mushroom. Filled from world/items/herbs.lua as that script loads.
//
// Which potion a species distils into, and how hard that is, are NOT
// content: they are dealt out afresh each game by Create(), so learning
// that valeriana made healing last game tells you nothing this game.
struct PlantDefinition {
    std::string id;
    std::string herb_name;
    std::string bush_name;
    std::string post_eat;
    int color{0};
    HERB_TYPE herb_type{HT_HERB};

    // Per-game, not content, and saved with the game.
    PotionName pn;
    int difficulty{0};
    bool identified{false};

    static void Create();

    // The species with this id, or nullptr for one nothing defines.
    static PlantDefinition* Find(const std::string& id);

    // A species of that kind, drawn at random - what a bush or a mushroom
    // patch asks for when it decides what it is growing.
    static std::string RandomOfType(HERB_TYPE type);

    // herbs[] is private to xherb.cpp.
    static void SaveTable(cereal::JSONOutputArchive& ar);
    static void LoadTable(cereal::JSONInputArchive& ar);
};

// Fluent builder for one species of plant:
//
//   Herb.new("valeriana_root")
//       :Called("valeriana root")
//       :Growing("valeriana")
//       :Taste("sedative")
//       :Looks(xColor.xGREEN)
//       :Register()
//
// A mushroom says :Mushroom() and no :Growing() - it is its own plant and
// stands under its own name.
class HerbBuilder
{
    public:
        explicit HerbBuilder(std::string id);

        HerbBuilder& Called(const std::string& n);
        HerbBuilder& Growing(const std::string& n);
        HerbBuilder& Mushroom();
        HerbBuilder& Taste(const std::string& t);
        HerbBuilder& Looks(int colour);

        void Register();

    private:
        std::string id;
        std::string herb_name;
        std::string bush_name;
        std::string post_eat;
        int color{0};
        HERB_TYPE herb_type{HT_HERB};
};

class XHerb : public XAnyFood
{
        // The species this was picked from - the id world/items/herbs.lua
        // registered it under.
        std::string species;
    public:
        DECLARE_CREATOR(XHerb, XAnyFood);
        explicit XHerb(std::string _species);
        XHerb(XHerb * copy) : XAnyFood((XAnyFood*)copy), species(copy->species) { }

        XHerb()
        {
            assert(0);
        }

        XItem* MakeCopy() override
        {
            return new XHerb(this);
        }

        RESULT onEat(XCreature * eater) override;

        [[nodiscard]] std::string postEat(XCreature *eater) override;

        bool Compare(XObject* o) override
        {
            return species == ((XHerb*)o)->species;
        }

        PotionName GetTargetPotion();

        void Identify() override;
        bool isIdentified() override;

        std::string toString() override;
                        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XAnyFood>(this));
            ar(species);
        }
};

// The plants themselves - a herb bush and a patch of mushrooms - are not
// here any more. They are map objects declared in world/plants.lua and
// they do their growing, spreading and dying in Lua; what stays in C++ is
// the species table above, which says what can grow.
#endif
