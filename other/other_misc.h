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

#ifndef __OTHER_MISC_H
#define __OTHER_MISC_H

#include "xmapobj.h"
#include "xherb.h"
#include "deity.h"

enum TRAP_TYPE
{
	TT_UNKNOWN		= -1,
	TT_MAGICARROW	= 0,
	TT_FIREBOLT,
	TT_ACIDBOLT,
	TT_ARROW,
	TT_TELEPORT,
	TT_PIT,
	TT_SPEAR_PIT,
	TT_RANDOM
};

enum TRAP_LEVEL
{
	TL_VERY_LOW		= 0,
	TL_LOW			= 1,
	TL_AVG			= 2,
	TL_HI			= 3,
	TL_VERY_HI		= 4,
	TL_MASTERWORK	= 5,
	TL_RANDOM,
};

class XLocation;

//////////////////////////////////////////////////////////////////////
//XTrap
/////////////////////////////////////////////////////////////////////
class XTrap: public XMapObject
{
	TRAP_TYPE trap_type;
	TRAP_LEVEL trap_level;
	XPtr<XCreature> owner; //owner to get exp...
	int isVisibleForHero;
	XPtr<XItem> trap_item;
	bool isMagic;
	XGUID last_activator; //need for a pits
public:
	int activation_count;
	DECLARE_CREATOR(XTrap, XMapObject);
	XTrap() { assert(0); }
	XTrap(int _x, int _y, XLocation * _l, TRAP_LEVEL tl = TL_RANDOM, TRAP_TYPE tt = TT_RANDOM, XCreature * _owner = NULL, XItem * items = NULL);
//	XTrap(int _x, int _y, XLocation * _l, TRAP_TYPE tt = TT_RANDOM) {}
	virtual int MoveIn(XCreature * cr);
	virtual int MoveOut(XCreature * cr);
	virtual int Activate(XCreature * cr);
	virtual int Check(XCreature * cr);
	virtual int isVisible(XCreature * cr);
	virtual int Disarm(XCreature * cr);
	virtual void Store(XFile * f);
	virtual void Restore(XFile * f);

};


class XLocation;

//////////////////////////////////////////////////////////////////////
//XStairWay
/////////////////////////////////////////////////////////////////////
class XStairWay : public XMapObject
{
public:
	DECLARE_CREATOR(XStairWay, XMapObject);
	XStairWay(int _x, int _y, XLocation * loc, LOCATION _ln, STAIRWAYTYPE type);
	LOCATION ln;
	virtual int Compare(XObject * o) {return -1;}
	void Bind(XStairWay * way);
	virtual void Store(XFile * f);
	virtual void Restore(XFile * f);
	const char * GetName(XCreature * viewer) { return "a stairway"; }
protected:
	XStairWay() {}
};


//////////////////////////////////////////////////////////////////////
//XTeleport
/////////////////////////////////////////////////////////////////////
class XTeleport : public XMapObject
{
public:
	DECLARE_CREATOR(XTeleport, XMapObject);
	XTeleport(int _x, int _y, XLocation * loc, LOCATION _ln, int _nx, int _ny);
	LOCATION ln;
	virtual int Compare(XObject * o) {return -1;}
	virtual int MoveIn(XCreature * cr);	
	virtual void Store(XFile * f);
	virtual void Restore(XFile * f);

	const char * GetName(XCreature * viewer) { return "a magic circle"; }
protected:
	XTeleport() {}

};



//////////////////////////////////////////////////////////////////////
//XDoor
//////////////////////////////////////////////////////////////////////
class XDoor: public XMapObject
{
public:
	DECLARE_CREATOR(XDoor, XMapObject);
	XDoor(int _x, int _y, int flg, XLocation * _l);
//	virtual void Invalidate();
	void Switch();
	int isOpened;

	virtual void Store(XFile * f);
	virtual void Restore(XFile * f);
	const char * GetName(XCreature * viewer) { return "a door"; }
protected:
	XDoor() {im = IM_DOOR;}
};


//////////////////////////////////////////////////////////////////////
//XAltar
//////////////////////////////////////////////////////////////////////
class XAltar: public XMapObject
{
public:
	DECLARE_CREATOR(XAltar, XMapObject);
	XAltar(int _x, int _y, DEITY deity, XLocation * _l);
	virtual void Store(XFile * f);
	virtual void Restore(XFile * f);
	
	const char * GetName(XCreature * viewer) { return color == xWHITE ? "an altar of white granite" : "an altar of black granite"; }
protected:
	XAltar() {im = IM_ALTAR;}
};



//////////////////////////////////////////////////////////////////////
//XGrave
//////////////////////////////////////////////////////////////////////
class XGrave: public XMapObject
{
	int isOpened;
	XItemList hiden_items;
public:
	DECLARE_CREATOR(XGrave, XMapObject);
	XGrave(int _x, int _y, char * subscr, XLocation * _l);
	void HideItem(XItem * item);
	int onOuterUse(XCreature * cr);
	virtual void Store(XFile * f);
	virtual void Restore(XFile * f);
protected:
	XGrave() {im = IM_MISC;}
};

/*
//////////////////////////////////////////////////////////////////////
//XGrave
//////////////////////////////////////////////////////////////////////
class XThrone: public XMapObject
{
public:
	DECLARE_CREATOR(XThrone, XMapObject);
	XThrone(int _x, int _y, char * subscr, XLocation * _l);
protected:
	XThrone() {im = IM_MISC;}
};
*/

//////////////////////////////////////////////////////////////////////
//XFurniture
//////////////////////////////////////////////////////////////////////
class XFurniture: public XMapObject
{
public:
	DECLARE_CREATOR(XFurniture, XMapObject);
	XFurniture(int _x, int _y, int _c, char _v, char * subscr, XLocation * _l);
protected:
	XFurniture() {im = IM_MISC;}
};


//////////////////////////////////////////////////////////////////////
//XOuterObject
//////////////////////////////////////////////////////////////////////
class XOuterObject: public XMapObject
{
	char * onEventLua;
public:
	DECLARE_CREATOR(XOuterObject, XMapObject);
	XOuterObject(int _x, int _y, int _c, char _v, char * subscr, XLocation * _l, const char * event);
	~XOuterObject();
	int onOuterUse(XCreature * cr);
	void Store(XFile * f);
	void Restore(XFile * f);
protected:
	XOuterObject() {im = IM_MISC;}
};



#endif
