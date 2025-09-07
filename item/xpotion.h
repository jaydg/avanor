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

#ifndef __XPOTION_H
#define __XPOTION_H

#include "item/item.h"
#include "magic/effect.h"


enum POTION_COLOR {
    PNC_CLEAR	= 0, PNC_SMOKY	= 1, PNC_GREEN	= 2, PNC_ORANGE	= 3,
    PNC_YELLOW	= 4, PNC_BLACK	= 5, PNC_BLUE	= 6, PNC_WHITE	= 7,
    PNC_CYAN	= 8, PNC_PURPLE	= 9, PNC_HAZE	= 10, PNC_GOLDEN	= 11,
    PNC_SILVER	= 12, PNC_AZURE	= 13, PNC_MURKY	= 14, PNC_RED	= 15,
    PNC_GLOWING	= 16, PNC_MOTTLED = 17, PNC_BLOBBY	= 18, PNC_PINK	= 19,
    PNC_MOULDY	= 20, PNC_GRAY	= 21, PNC_MERCURY = 22, PNC_OILY	= 23,
    PNC_VISCOUS	= 24, PNC_DARK_RED = 25, PNC_LIGHT_RED = 26, PNC_DARK_BLUE = 27,
    PNC_LIGHT_BLUE = 28, PNC_BROWN = 29, PNC_LIGHT_GRAY = 30,
    PNC_DARK_GRAY, PNC_DARK_GREEN, PNC_LIGHT_GREEN, PNC_BEIGE, PNC_AQUAMARINE,
    PNC_CORAL, PNC_IVORY, PNC_MAROON, PNC_TAN, PNC_TURQUOISE, PNC_VIOLET,
    PNC_RANDOM
};

enum POTION_NAME {
    PN_UNKNOWN = -1,
    PN_WATER,
    PN_APPLEJUCE,
    PN_ORANGEJUCE,
    PN_HEALING,
    PN_CURE_LIGHT_WOUNDS,
    PN_CURE_SERIOUS_WOUNDS,
    PN_CURE_CRITICAL_WOUNDS,
    PN_CURE_MORTAL_WOUNDS,
    PN_POWER,
    PN_RESTORATION,
    PN_GAIN_STRENGTH,
    PN_GAIN_WILLPOWER,
    PN_GAIN_MANA,
    PN_GAIN_TOUGHNESS,
    PN_GAIN_DEXTERITY,
    PN_CURE_POISON,
    PN_POISON,
    PN_BLEEDNESS,
    PN_DISEASE,
    PN_CURE_DISEASE,
    PN_HEROISM,
    PN_SEE_INVISIBLE,
    PN_WEAKNESS,
    PN_CLUMSINESS,
    PN_DEATH,
    PN_SATIATION,
    PN_STARVATION,
    PN_BOOST_SPEED,
    PN_SLOWNESS,
    PN_ACID_RESISTANCE,
    PN_FIRE_RESISTANCE,
    PN_COLD_RESISTANCE,
    PN_POISON_RESISTANCE,
    PN_RANDOM
};

struct PN_COLORTABLE {
    const char* name;
    int color;
    int is_used;
};

struct POTION_REC {
    POTION_NAME pn;
    const char* name;
    EFFECT effect;
    int rarity;
    int alchemy_power;
    int value; // how much it cost for one potion_power //value * potion_power * [spell_cost]
    POTION_COLOR force_color;
    int identify;

    static POTION_COLOR SelectColor(POTION_COLOR pnc = PNC_RANDOM);
    void Store(XFile * f);
    void Restore(XFile * f);
    static POTION_NAME GetRandomPotion();
    static void RunOnce();
    static int potion_total_value;
    static POTION_REC* GetRec(POTION_NAME pn);
};

class XItem;

class XPotion : public XItem
{
    public:
        DECLARE_CREATOR(XPotion, XItem);
        XPotion(POTION_NAME _pn = PN_RANDOM);
        XPotion(XPotion * copy);
        virtual XObject* MakeCopy()
        {
            return new XPotion(this);
        }

        virtual int isIdentifed();
        virtual void Identify(int level);
        virtual void toString(char* buf);
        virtual int Compare(XObject * o);
        virtual int GetValue()
        {
            return value;
        }

        POTION_NAME pn;
        int onDrink(XCreature * cr);
        virtual void Store(XFile * f);
        virtual void Restore(XFile * f);
        static void StoreTable(XFile * f);
        static void RestoreTable(XFile * f);
    protected:
        POTION_REC* pdescr;
};

class XAlchemyRec : public XObject
{
    public:
        POTION_NAME pn1;
        POTION_NAME pn2;
        POTION_NAME result;

        DECLARE_CREATOR(XAlchemyRec, XObject);
        XAlchemyRec()
        {
            assert(0);
        }

        XAlchemyRec(POTION_NAME p1, POTION_NAME p2, POTION_NAME res) :
            pn1(p1), pn2(p2), result(res) {}

        virtual void Store(XFile * f)
        {
            XObject::Store(f);
            f->Write(&pn1, sizeof(POTION_NAME));
            f->Write(&pn2, sizeof(POTION_NAME));
            f->Write(&result, sizeof(POTION_NAME));
        }

        virtual void Restore(XFile * f)
        {
            XObject::Restore(f);
            f->Read(&pn1, sizeof(POTION_NAME));
            f->Read(&pn2, sizeof(POTION_NAME));
            f->Read(&result, sizeof(POTION_NAME));
        }
};

class XAlchemy
{
        void BuildReception(int al_lvl);
        int GetPotionCount(int al_lvl, POTION_NAME** pTable);
        XList<XAlchemyRec*> reception;
    public:
        XAlchemy();
        ~XAlchemy();
        static int GetReceptionCount();
        static XAlchemyRec* GetReception(int num);
        static void Init();
        static void Store(XFile * f);
        static void Restore(XFile * f);
        static int isValidReception(POTION_NAME pn1, POTION_NAME pn2, POTION_NAME pn3);
        static void GetReceptionName(char* buf, POTION_NAME pn1, POTION_NAME pn2, POTION_NAME pn3);
        static POTION_NAME GetPotionName(POTION_NAME pn1, POTION_NAME pn2);
};

#endif
