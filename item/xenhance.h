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

#ifndef	__XENHANCE_H
#define	__XENHANCE_H

#include "item/item.h"

enum ENH_COLOR {
    EC_WHITE	=	0,
    EC_BLUE	=	1,
    EC_BLACK	=	2,
    EC_GOLDEN	=	3,
    EC_COPPER	=	4,
    EC_WOODEN	=	5,
    EC_RED	=	6,
    EC_MARBLE	=	7,
    EC_PEARL	=	8,
    EC_SAPHIRE	=	9,
    EC_DIAMOND	=	10,
    EC_RUBY	=	11,
    EC_STEEL	=	12,
    EC_GLASS	=	13,
    EC_OBSIDIAN	=	14,
    EC_RANDOM,
};

struct _ENH_COLOR_TABLE {
    int color;
    const char* color_name;
    int is_used;
};

enum ENHANCE {
    ENH_PROTECTION, ENH_DAMAGE, ENH_SLAYING, ENH_FREEACTION, ENH_SEEINVISIBLE,
    ENH_INVISIBILITY, ENH_ACIDRESIST, ENH_POISONRESIST, ENH_FIRERESIST,
    ENH_POWER, ENH_STR, ENH_EOF
};

struct ENH_REC {
    ENHANCE enh;
    const char* name;
    const char* dv; //modifers....
    const char* pv;
    const char* hit;
    const char* dice;
    const char* z; //random z to dice;
    const char* rng; //rng
    const char* r; //resists
    const char* s; //stats
    int value;
    ENH_COLOR color;
    int identify;
    static ENH_COLOR SelectColor(ENH_COLOR ec);
};

class XEnhance : public XItem
{
    public:
        DECLARE_CREATOR(XEnhance, XItem);
        XEnhance(ENHANCE enh = ENH_EOF);
        XEnhance(XEnhance * copy) : XItem((XItem*)copy)
        {
            descr = copy->descr;
        }

        std::string toString() override;
        int descr;
        virtual int Compare(XObject * o);
        virtual void Store(XFile * f);
        virtual void Restore(XFile * f);
};

#endif
