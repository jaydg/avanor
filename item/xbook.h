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

#ifndef XBOOK_H
#define XBOOK_H

#include "item/item.h"
#include "magic/magic.h"

enum BOOK_NAME {
    BOOK_BURNING_HANDS,
    BOOK_ICE_TOUCH,
    BOOK_CURE_LIGHT_WOUNDS,
    BOOK_DRAIN_LIFE,
    BOOK_IDENTIFY,
    BOOK_MAGIC_ARROW,
    BOOK_FIRE_BOLT,
    BOOK_ICE_BOLT,
    BOOK_LIGHTNING_BOLT,
    BOOK_ACID_BOLT,
    BOOK_CURE_DISEASE,
    BOOK_CURE_POISON,
    BOOK_BLINK,
    BOOK_SELF_KNOWLEDGE,
    BOOK_RANDOM
};

struct BOOK_REC {
    BOOK_REC(int rarity, BOOK_NAME bn, SPELL_NAME sn);
    int name_index;
    BOOK_NAME book_name;
    SPELL_NAME spell_name;
    int identify;
    int rarity;

    void Store(XFile * f);
    void Restore(XFile * f);

    static int GetBook(BOOK_NAME); //number in array of books

    static int current_descr;
    static int total_value;
};

class XBook: public XItem
{
    public:
        DECLARE_CREATOR(XBook, XItem)
        XBook(BOOK_NAME bn = BOOK_RANDOM);
        XBook(XBook * copy) : XItem((XItem*)copy)
        {
            descr = copy->descr;
            reader_guid = copy->reader_guid;
            left_to_read = copy->left_to_read;
        }

        virtual XObject* MakeCopy()
        {
            return new XBook(this);
        }

        virtual int isIdentifed();
        virtual void Identify(int level);
        std::string toString() override;
        virtual int Compare(XObject * o);
        virtual int onRead(XCreature * reader);
        static void StoreTable(XFile * f);
        static void RestoreTable(XFile * f);
        virtual void Store(XFile * f);
        virtual void Restore(XFile * f);
        int left_to_read;
    protected:
        int descr;
        XGUID reader_guid;
};

#endif
