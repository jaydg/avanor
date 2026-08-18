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

struct PlantDefinition {
    const char* herb_name;
    const char* bush_name;
    const char* post_eat;
    int color;
    HERB_TYPE herb_type;
    PotionName pn;
    int difficulty;
    int identify;
    static void Create();

    // herbs[] is private to xherb.cpp.
    static void SaveTable(cereal::JSONOutputArchive& ar);
    static void LoadTable(cereal::JSONInputArchive& ar);
};

class XHerb : public XAnyFood
{
        int herb_index;
    public:
        DECLARE_CREATOR(XHerb, XAnyFood);
        XHerb(int _herb_index);
        XHerb(XHerb * copy) : herb_index(copy->herb_index), XAnyFood((XAnyFood*)copy) { }

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
            if (herb_index == ((XHerb*)o)->herb_index) {
                return 0;
            } else if (herb_index < ((XHerb*)o)->herb_index) {
                return -1;
            } else {
                return 1;
            }
        }

        PotionName GetTargetPotion();

        void Identify(int level) override;
        int isIdentified() override;

        std::string toString() override;
                        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XAnyFood>(this));
            ar(herb_index);
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
        // Index into herbs[]; the species is drawn at placement time.
        int herb_index{};

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
            ar(herb_index);
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
