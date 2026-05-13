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

#ifndef XSCROLL_H
#define XSCROLL_H

#include "item/item.h"
#include "magic/effect.h"

enum SCROLL_NAME {
    SCROLL_BURNING_HANDS,
    SCROLL_ICE_TOUCH,
    SCROLL_HEROISM,
    SCROLL_HEALING,
    SCROLL_POWER,
    SCROLL_IDENTIFY,
    SCROLL_MAGIC_ARROW,
    SCROLL_FIRE_BOLT,
    SCROLL_ICE_BOLT,
    SCROLL_LIGHTNING_BOLT,
    SCROLL_ACID_BOLT,
    SCROLL_SUMMON_MONSTER,
    SCROLL_CREATE_ITEM,
    SCROLL_CURE_DISEASE,
    SCROLL_CURE_POISON,
    SCROLL_BLINK,
    SCROLL_SELF_KNOWLEDGE,
    SCROLL_SEE_INVISIBLE,
    SCROLL_RECIPIE,
    SCROLL_RANDOM
};

class XScroll : public XItem
{
    public:
        DECLARE_CREATOR(XScroll, XItem);
        XScroll(SCROLL_NAME _scrn = SCROLL_RANDOM);
        XScroll(XScroll * copy): XItem((XItem*)copy)
        {
            descr = copy->descr;
            sc_name = copy->sc_name;
        }

        XObject* MakeCopy() override
        {
            return new XScroll(this);
        }

        int isIdentifed() override;
        void Identify(int level) override;
        std::string toString() override;
        int Compare(XObject * o) override;
        virtual int onRead(XCreature * cr);
        void Store(XFile * f) override;
        void Restore(XFile * f) override;
        static void StoreTable(XFile * f);
        static void RestoreTable(XFile * f);
        SCROLL_NAME sc_name;
    protected:
        int descr;
};

#endif
