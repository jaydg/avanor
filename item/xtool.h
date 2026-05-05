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

#ifndef XTOOL_H
#define XTOOL_H

#include "item/item.h"

class XCookingSet : public XItem
{
    public:
        DECLARE_CREATOR(XCookingSet, XItem);
        XCookingSet()
        {
            color = xLIGHTGRAY;
            view = '[';
            it = IT_COOKINGSET;
            strcpy(name, "cooking set");
            value = 150;
            weight = 100;
            im = IM_TOOL;
            cooked_item = NULL;
            bp = BP_TOOL;
            stats = new XStats();
            resistances = new XResistance();
        }

        XCookingSet(XCookingSet * copy) : XItem(copy)
        {
            assert(cooked_item == NULL);
        }

        virtual XObject* MakeCopy()
        {
            return new XCookingSet(this);
        }

        virtual RESULT onUse(USE_ITEM_STATE uis, XCreature * cr);
        virtual void toString(char* buf)
        {
            strcpy(buf, name);
        }

        virtual void Store(XFile * f);
        virtual void Restore(XFile * f);
        virtual void Invalidate();
    protected:
        XPtr<XItem> cooked_item;
        int use_time;
};


//using of pick axe and mining skill
//all object on map have some digiting resistance
//lets value of it be 1000
//dig has dice, for example 1d10
//also there is mining skill which gives +5 bonus
//then we remove rock_resist till, untill complete diging the rock
//also str gives +1 for each to points of str

class XPickAxe : public XItem
{
        int tgt_x, tgt_y;
        int rock_resist;
    public:
        DECLARE_CREATOR(XPickAxe, XItem);
        XPickAxe()
        {
            color = xLIGHTGRAY;
            view = '\\';
            it = IT_PICKAXE;
            strcpy(name, "pickaxe");
            value = 150;
            weight = 100;
            im = IM_TOOL;
            bp = BP_TOOL;
            stats = new XStats();
            resistances = new XResistance();
            dice.Setup(1, 10, 0);
        }

        XPickAxe(XPickAxe * copy) : XItem(copy) {}

        virtual XObject* MakeCopy()
        {
            return new XPickAxe(this);
        }

        virtual RESULT onUse(USE_ITEM_STATE uis, XCreature * cr);
        virtual void toString(char* buf)
        {
            strcpy(buf, name);
        }
};

class XEyeOfRaa : public XItem
{
    public:
        DECLARE_CREATOR(XEyeOfRaa, XItem);
        XEyeOfRaa()
        {
            color = xCYAN;
            view = '*';
            it = IT_EYEOFRAA;
            strcpy(name, "Eye of Raa");
            value = 150;
            weight = 100;
            im = IM_TOOL;
            bp = BP_TOOL;
            stats = new XStats("Ma:0d0+10 Wi:0d0+10");
            resistances = new XResistance("air:100");
            dice.Setup(1, 10, 0);
        }

        XEyeOfRaa(XEyeOfRaa * copy) : XItem(copy) {}

        virtual XObject* MakeCopy()
        {
            return new XEyeOfRaa(this);
        }

        virtual RESULT onUse(USE_ITEM_STATE uis, XCreature * cr);
        virtual void toString(char* buf)
        {
            strcpy(buf, name);
        }
};

class XAlchemySet : public XItem
{
    public:
        DECLARE_CREATOR(XAlchemySet, XItem);
        XAlchemySet()
        {
            color = xLIGHTGRAY;
            view = '[';
            it = IT_ALCHEMYSET;
            strcpy(name, "alchemy set");
            value = 150;
            weight = 100;
            im = IM_TOOL;
            bp = BP_TOOL;
            stats = new XStats();
            resistances = new XResistance();
        }

        XAlchemySet(XAlchemySet * copy) : XItem(copy) {}

        virtual XObject* MakeCopy()
        {
            return new XAlchemySet(this);
        }

        virtual RESULT onUse(USE_ITEM_STATE uis, XCreature * cr);
        virtual void toString(char* buf)
        {
            strcpy(buf, name);
        }
};

#endif
