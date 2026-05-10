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

        virtual XObject* MakeCopy()
        {
            return new XHerb(this);
        }

        virtual RESULT onEat(XCreature * eater);

        [[nodiscard]] virtual std::string postEat(XCreature *eater);

        virtual int Compare(XObject * o)
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

        virtual void Identify(int level);
        virtual int isIdentifed();

        std::string toString() override;
        virtual void Store(XFile * f)
        {
            XAnyFood::Store(f);
            f->Write(&herb_index, sizeof(herb_index));
        }

        virtual void Restore(XFile * f)
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

        const std::string GetName(XCreature *viewer);
        XHerbBush() { }

    public:
        DECLARE_CREATOR(XHerbBush, XMapObject);
        XHerbBush(int _x, int _y, XLocation * _l);
        virtual void Store(XFile * f);
        virtual void Restore(XFile * f);
        int Run();

        XObject* Pick(XCreature * picker);
};

#define BASE_MUSH_REFRESH 1000000

class XMushSpawn: public XMapObject
{
        int mush_index;
        long grownth_rate;

        const std::string GetName(XCreature *viewer);
        XMushSpawn() { }

    public:
        DECLARE_CREATOR(XMushSpawn, XMapObject);
        XMushSpawn(int _x, int _y, XLocation * _l);
        virtual void Store(XFile * f);
        virtual void Restore(XFile * f);
        int Run();

        XObject* Pick(XCreature * picker);
};


#endif
