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

#include "anycr.h"


class XBeelzvile : public XAnyCreature
{
	XBeelzvile() {}
public:
	DECLARE_CREATOR(XBeelzvile, XAnyCreature);
	XBeelzvile(_CREATURE * cr);
	virtual void Die(XCreature * killer);
	virtual void NewMove();
	virtual void Move();
};


class XVillageElder : public XAnyCreature
{
protected:
	XVillageElder() {}
public:
	DECLARE_CREATOR(XVillageElder, XAnyCreature);
	XVillageElder(_CREATURE * cr);
	virtual int Chat(XCreature * chater, char * msg);
};

class XOzorik : public XAnyCreature
{
protected:
	XOzorik() {}
public:
	DECLARE_CREATOR(XOzorik, XAnyCreature);
	virtual void Store(XFile * f);
	virtual void Restore(XFile * f);
	XOzorik(_CREATURE * cr);
	virtual int Chat(XCreature * chater, char * msg);
	virtual int onGiveItem(XCreature * giver, XItem * item);
protected:
	XGUID deathhack_guid;
};


class XYohjishiro : public XAnyCreature
{
protected:
	XYohjishiro() {}
public:
	DECLARE_CREATOR(XYohjishiro, XAnyCreature);
	XYohjishiro(_CREATURE * cr);
	virtual int Chat(XCreature * chater, char * msg);
	virtual int onGiveItem(XCreature * giver, XItem * item);
};

class XGefeon : public XAnyCreature
{
protected:
	XGefeon() {}
public:
	DECLARE_CREATOR(XGefeon, XAnyCreature);
	XGefeon(_CREATURE * cr);
	virtual int Chat(XCreature * chater, char * msg);
};


class XAhkUlan : public XAnyCreature
{
protected:
	XAhkUlan() {}
public:
	DECLARE_CREATOR(XAhkUlan, XAnyCreature);
	XAhkUlan(_CREATURE * cr);
	virtual int Chat(XCreature * chater, char * msg);
	virtual void Die(XCreature * killer);
	virtual int onGiveItem(XCreature * giver, XItem * item);
};



class XGekta : public XAnyCreature
{
protected:
	XGekta() {}
public:
	DECLARE_CREATOR(XGekta, XAnyCreature);
	XGekta(_CREATURE * cr);
	virtual char * StdAnswer() {return "'Woof!'";}
	virtual int onGiveItem(XCreature * giver, XItem * item);
};


class XTodin : public XAnyCreature
{
protected:
	XTodin() {}
public:
	DECLARE_CREATOR(XTodin, XAnyCreature);
	XTodin(_CREATURE * cr);
	virtual int Chat(XCreature * chater, char * msg);
	virtual int onGiveItem(XCreature * giver, XItem * item);
};

class XShop;


class XJorgus : public XAnyCreature
{
protected:
	XJorgus() {}
public:
	DECLARE_CREATOR(XJorgus, XAnyCreature);
	XJorgus(_CREATURE * cr);
	virtual int Chat(XCreature * chater, char * msg);
};


class XRoderick : public XAnyCreature
{
protected:
	XRoderick() {}
public:
	DECLARE_CREATOR(XRoderick, XAnyCreature);
	XRoderick(_CREATURE * cr);
	virtual int Chat(XCreature * chater, char * msg);
	virtual void Die(XCreature * killer);
	virtual int onGiveItem(XCreature * giver, XItem * item);
};


class XTorin : public XAnyCreature
{
	XGUID showel_guid;
protected:
	XTorin() {}
public:
	DECLARE_CREATOR(XTorin, XAnyCreature);
	XTorin(_CREATURE * cr);
	virtual int Chat(XCreature * chater, char * msg);
	virtual int onGiveItem(XCreature * giver, XItem * item);
	virtual void Store(XFile * f);
	virtual void Restore(XFile * f);
};

class XXSheeVoo : public XAnyCreature
{
protected:
	XXSheeVoo() {}
public:
	DECLARE_CREATOR(XXSheeVoo, XAnyCreature);
	XXSheeVoo(_CREATURE * cr);
	virtual int Chat(XCreature * chater, char * msg);
	virtual void Die(XCreature * killer);
};


class XMagnush : public XAnyCreature
{
protected:
	XMagnush() {}
public:
	DECLARE_CREATOR(XMagnush, XAnyCreature);
	XMagnush(_CREATURE * cr);
	virtual void Die(XCreature * killer);
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
	virtual int Chat(XCreature * chater, char * msg);
	virtual void Die(XCreature * killer);
	virtual int onGiveItem(XCreature * giver, XItem * item);
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
	XShopkeeper(char * _name, XShop * shop, CREATURE_LEVEL crl, CREATURE_EXP cre) {}
	virtual char * StdAnswer();
	virtual void Die(XCreature * killer);
	void SetShop(char * _name, XShop * shop);
};


#endif
