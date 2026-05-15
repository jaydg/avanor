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

#include "item/xanyfood.h"
#include "item/xpotion.h"

enum HERB_TYPE {
    HT_HERB,
    HT_MUSHROOM,
};

struct _HERBS {
    const char* herb_name;
    const char* bush_name;
    const char* post_eat;
    int color;
    HERB_TYPE herb_type;
    POTION_NAME pn;
    int difficulty;
    int identify;
    static void Create();
    static void Store(XFile * f);
    static void Restore(XFile * f);
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

        XObject* MakeCopy() override
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

        POTION_NAME GetTargetPotion();

        void Identify(int level) override;
        int isIdentifed() override;

        std::string toString() override;
        void Store(XFile * f) override
        {
            XAnyFood::Store(f);
            f->Write(&herb_index, sizeof(herb_index));
        }

        void Restore(XFile * f) override
        {
            XAnyFood::Restore(f);
            f->Read(&herb_index, sizeof(herb_index));
        }
};

//////////////////////////////////////////////////////////////////////
//XHerbBush
/////////////////////////////////////////////////////////////////////
class XAnyFood;
class XLocation;

#define BASE_HERB_REFRESH 1000000

class XHerbBush: public XMapObject
{
        int herb_index;
        unsigned char herb_strength;
        int CountNeighbours(int x, int y);

    protected:
        long grownth_rate;

        const std::string GetName(XCreature *viewer) override;
        XHerbBush() { }

    public:
        DECLARE_CREATOR(XHerbBush, XMapObject);
        XHerbBush(int _x, int _y, XLocation * _l);
        void Store(XFile * f) override;
        void Restore(XFile * f) override;

        bool Run() override;

        XObject* Pick(XCreature * picker) override;
};

#define BASE_MUSH_REFRESH 1000000

class XMushSpawn: public XMapObject
{
        int mush_index;
        long grownth_rate;

        const std::string GetName(XCreature *viewer) override;
        XMushSpawn() { }

    public:
        DECLARE_CREATOR(XMushSpawn, XMapObject);
        XMushSpawn(int _x, int _y, XLocation * _l);
        void Store(XFile * f) override;
        void Restore(XFile * f) override;

        bool Run() override;

        XObject* Pick(XCreature * picker) override;
};


#endif
