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

#ifndef __ITEM_MISC_H
#define __ITEM_MISC_H

#include "xanyfood.h"

class XBatWing : public XAnyFood
{
    public:
        DECLARE_CREATOR(XBatWing, XAnyFood);
        XBatWing()
        {
            color = xBROWN;
            it = IT_BATWING;
            food_nutrio = 10;
            consume_nutrio = 10;
            strcpy(name, "bat wing");
            value = 100;
            weight = 2;

        }

        XBatWing(XBatWing * copy) : XAnyFood(copy) {}

        virtual XObject* MakeCopy()
        {
            return new XBatWing(this);
        }
};

class XRatTail : public XAnyFood
{
    public:
        DECLARE_CREATOR(XRatTail, XAnyFood);
        XRatTail()
        {
            color = xBROWN;
            it = IT_RATTAIL;
            food_nutrio = 10;
            consume_nutrio = 10;
            strcpy(name, "rat tail");
            value = 100;
            weight = 3;

        }

        XRatTail(XRatTail * copy) : XAnyFood(copy) {}

        virtual XObject* MakeCopy()
        {
            return new XRatTail(this);
        }
};

class XBone : public XAnyFood
{
    public:
        DECLARE_CREATOR(XBone, XAnyFood);
        XBone()
        {
            color = xWHITE;
            it = IT_BONE;
            food_nutrio = 10;
            consume_nutrio = 10;
            strcpy(name, "bone");
            value = 1;
            weight = 5;
        }

        XBone(XBone * copy) : XAnyFood(copy) {}

        virtual XObject* MakeCopy()
        {
            return new XBone(this);
        }
};

class XChest : public XItem
{
    public:
        XItemList contain;
        DECLARE_CREATOR(XChest, XItem);
        XChest()
        {
            assert(0);
        }

        XChest(int item_count, ITEM_MASK imask, int low_v, int high_v);
        XChest(XChest * copy)
        {
            assert(0);
        }

        int Compare(XObject * o)
        {
            return -1;
        }

        virtual XObject* MakeCopy()
        {
            return new XChest(this);
        }

        int Add(XItem * item);
        virtual void toString(char* buf);
        virtual void Store(XFile * f);
        virtual void Restore(XFile * f);
};

class XAncientMachinePart : public XItem
{
    public:
        DECLARE_CREATOR(XAncientMachinePart, XItem);
        XAncientMachinePart()
        {
            color = xDARKGRAY;
            view = ']';
            strcpy(name, "ancient machine part");
            value = 1000;
            weight = 15;
            im = IM_TOOL;
            bp = BP_OTHER;
            it = IT_ANCIENTMACHINEPART;
        }

        XAncientMachinePart(XAncientMachinePart * copy) : XItem(copy) {}

        virtual XObject* MakeCopy()
        {
            return new XAncientMachinePart(this);
        }

        virtual void toString(char* buf)
        {
            GetFullName(buf);
        }

};

#endif
