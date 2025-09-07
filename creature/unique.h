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

#ifndef __UNIQUE_H
#define __UNIQUE_H

#include "creature/anycr.h"

class XBeelzvile : public XAnyCreature
{
        XBeelzvile() {}

    public:
        DECLARE_CREATOR(XBeelzvile, XAnyCreature);
        XBeelzvile(_CREATURE * cr);
        virtual void NewMove();
        virtual void Move();
};

class XGefeon : public XAnyCreature
{
    protected:
        XGefeon() {}

    public:
        DECLARE_CREATOR(XGefeon, XAnyCreature);
        XGefeon(_CREATURE * cr);
        virtual int Chat(XCreature * chatter, const char* msg);
};

class XAhkUlan : public XAnyCreature
{
    protected:
        XAhkUlan() {}

    public:
        DECLARE_CREATOR(XAhkUlan, XAnyCreature);
        XAhkUlan(_CREATURE * cr);
        virtual int Chat(XCreature * chatter, const char* msg);
        virtual void Die(XCreature * killer);
        virtual int onGiveItem(XCreature * giver, XItem * item);
};

class XShop;

class XRoderick : public XAnyCreature
{
    protected:
        XRoderick() {}

    public:
        DECLARE_CREATOR(XRoderick, XAnyCreature);
        XRoderick(_CREATURE * cr);
        virtual int Chat(XCreature * chatter, const char* msg);
        virtual void Die(XCreature * killer);
        virtual int onGiveItem(XCreature * giver, XItem * item);
};

class XRandomMonster : public XAnyCreature
{

};

class XHighPriest : public XAnyCreature
{
    protected:
        XHighPriest() {}

    public:
        DECLARE_CREATOR(XHighPriest, XAnyCreature);
        XHighPriest(_CREATURE * cr);
        virtual int Chat(XCreature * chatter, const char* msg);
        virtual void Die(XCreature * killer);
        virtual int onGiveItem(XCreature * giver, XItem * item);
};

class XRotmoth : public XAnyCreature
{
    protected:
        XRotmoth() {}

    public:
        DECLARE_CREATOR(XRotmoth, XAnyCreature);
        XRotmoth(_CREATURE * cr);
        virtual int Chat(XCreature * chatter, const char* msg);
};

class XRotmothAI : public XStandardAI
{
    protected:
        XRotmothAI() {}

    public:
        DECLARE_CREATOR(XRotmothAI, XStandardAI);
        XRotmothAI(XCreature * cr) : XStandardAI(cr) {}

        void onWasAttacked(XCreature * attacker);
};

class XGiana : public XAnyCreature
{
    protected:
        XGiana() {}

    public:
        DECLARE_CREATOR(XGiana, XAnyCreature);
        XGiana(_CREATURE * cr);
        virtual int Chat(XCreature * chatter, const char* msg);
};

class XBandit : public XAnyCreature
{
    protected:
        XBandit() {}

    public:
        DECLARE_CREATOR(XBandit, XAnyCreature);
        XBandit(_CREATURE * cr);
};

class XBanditAI : public XStandardAI
{
    protected:
        XBanditAI() {}

    public:
        DECLARE_CREATOR(XBanditAI, XStandardAI);
        XBanditAI(XCreature * cr) : XStandardAI(cr) {}

        virtual int isEnemy(XCreature * cr);
};

class XShopkeeper : public XAnyCreature
{
    protected:
        XShopkeeper() {}

    public:
        DECLARE_CREATOR(XShopkeeper, XAnyCreature);
        XShopkeeper(_CREATURE *cr);
        XShopkeeper(char* _name, XShop * shop, CREATURE_LEVEL crl, CREATURE_EXP cre) {}

        virtual const char* StdAnswer();
        virtual void Die(XCreature * killer);
        void SetShop(char* _name, XShop * shop);
};

#endif
