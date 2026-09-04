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

        int Compare(XObject* o) override
        {
            if (species == ((XHerb*)o)->species) {
                return 0;
            } else if (species < ((XHerb*)o)->species) {
                return -1;
            } else {
                return 1;
            }
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

//////////////////////////////////////////////////////////////////////
//XHerbBush
/////////////////////////////////////////////////////////////////////
class XAnyFood;
class XLocation;

#define BASE_HERB_REFRESH 1000000

// Shared base for the map objects that grow a plant: a herb bush and a
// mushroom patch were near-identical classes, each with its own index into
// the shared species table and its own copy of the same placement code.
//
// Everything that differed between them is now three questions a subclass
// answers - which species type to draw from, which glyph to draw, and how
// long before its first turn - so placement exists once.
class XPlant : public XMapObject
{
    protected:
        // Which species this plant is growing; drawn at placement time.
        std::string species;

        virtual HERB_TYPE SpeciesType() const = 0;
        virtual char SpeciesView() const = 0;
        virtual int FirstRunDelay() const = 0;

        // herbs[] is private to xherb.cpp, so these are defined there.
        // The non-const overload exists because naming a plant can mark
        // its species identified (see XHerbBush::GetName()).
        const PlantDefinition& Species() const;
        PlantDefinition& Species();

        XPlant() {}
        explicit XPlant(DUMMY_STRUCT* ds) : XMapObject(ds) {}
        friend class cereal::access;

    public:
        // Draws the species, places the object and schedules it.
        bool PlaceAt(XLocation* location, int _x, int _y) override;

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XMapObject>(this));
            ar(species);
        }
};

class XHerbBush: public XPlant
{
        unsigned char herb_strength;
        int CountNeighbours(int x, int y);

    protected:
        HERB_TYPE SpeciesType() const override { return HT_HERB; }
        char SpeciesView() const override { return '"'; }
        int FirstRunDelay() const override { return 1; }

        const std::string GetName(XCreature *viewer) override;
        XHerbBush() { }
        // Grants Cereal access to the otherwise-inaccessible constructor
        // above - harmless to call directly (unlike XHerb/XCorpse's own
        // no-args guards), every field gets overwritten by serialize()
        // immediately afterward anyway.
        friend class cereal::access;

    public:
        DECLARE_CREATOR(XHerbBush, XPlant);
        XHerbBush(int _x, int _y, XLocation * _l);

        // Adds the bush's own starting strength on top of XPlant's.
        bool PlaceAt(XLocation* location, int _x, int _y) override;

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XPlant>(this));
            ar(herb_strength);
        }

        bool Run() override;

        XObject* Pick(XCreature * picker) override;
};

#define BASE_MUSH_REFRESH 1000000

class XMushSpawn: public XPlant
{
    protected:
        HERB_TYPE SpeciesType() const override { return HT_MUSHROOM; }
        char SpeciesView() const override { return '`'; }
        int FirstRunDelay() const override { return vRand(BASE_MUSH_REFRESH); }

        const std::string GetName(XCreature *viewer) override;
        XMushSpawn() { }
        friend class cereal::access;

    public:
        DECLARE_CREATOR(XMushSpawn, XPlant);
        XMushSpawn(int _x, int _y, XLocation * _l);

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XPlant>(this));
        }

        bool Run() override;

        XObject* Pick(XCreature * picker) override;
};

#endif
