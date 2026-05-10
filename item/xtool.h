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
            name = "cooking set";
            value = 150;
            weight = 100;
            im = IM_TOOL;
            cooked_item = nullptr;
            bp = BP_TOOL;
            stats = std::make_unique<XStats>();
            resistances = std::make_unique<XResistance>();
        }

        XCookingSet(XCookingSet * copy) : XItem(copy)
        {
            assert(cooked_item == nullptr);
        }

        XObject* MakeCopy() override
        {
            return new XCookingSet(this);
        }

        RESULT onUse(USE_ITEM_STATE uis, XCreature * cr) override;
        std::string toString() override
        {
            return name;
        }

        void Store(XFile * f) override;
        void Restore(XFile * f) override;
        void Invalidate() override;
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
            name = "pickaxe";
            value = 150;
            weight = 100;
            im = IM_TOOL;
            bp = BP_TOOL;
            stats = std::make_unique<XStats>();
            resistances = std::make_unique<XResistance>();
            dice.Setup(1, 10, 0);
        }

        XPickAxe(XPickAxe * copy) : XItem(copy) {}

        XObject* MakeCopy() override
        {
            return new XPickAxe(this);
        }

        RESULT onUse(USE_ITEM_STATE uis, XCreature * cr) override;
        std::string toString() override
        {
            return name;
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
            name = "Eye of Raa";
            value = 150;
            weight = 100;
            im = IM_TOOL;
            bp = BP_TOOL;
            stats = std::make_unique<XStats>("Ma:0d0+10 Wi:0d0+10");
            resistances = std::make_unique<XResistance>("air:100");
            dice.Setup(1, 10, 0);
        }

        XEyeOfRaa(XEyeOfRaa * copy) : XItem(copy) {}

        XObject* MakeCopy() override
        {
            return new XEyeOfRaa(this);
        }

        RESULT onUse(USE_ITEM_STATE uis, XCreature * cr) override;
        std::string toString() override
        {
            return name;
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
            name = "alchemy set";
            value = 150;
            weight = 100;
            im = IM_TOOL;
            bp = BP_TOOL;
            stats = std::make_unique<XStats>();
            resistances = std::make_unique<XResistance>();
        }

        XAlchemySet(XAlchemySet * copy) : XItem(copy) {}

        XObject* MakeCopy() override
        {
            return new XAlchemySet(this);
        }

        RESULT onUse(USE_ITEM_STATE uis, XCreature * cr) override;
        std::string toString() override
        {
            return name;
        }
};

#endif
