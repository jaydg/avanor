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

#ifndef __DWARFCITY_H
#define __DWARFCITY_H

#include "xanyplace.h"
#include "location.h"
#include "xmapobj.h"
#include "quest.h"

class XDwarfCityLocation : public XLocation
{
public:
	XDwarfCityLocation(LOCATION tl);
	virtual void Init();
};


class XDwarfTreasureLocation : public XLocation
{
public:
	XDwarfTreasureLocation(LOCATION tl);
	virtual void Init();
};

class XDwarvenTresurePlace : public XAnyPlace
{
	int move_count;
public:
	DECLARE_CREATOR(XDwarvenTresurePlace, XAnyPlace);
	XDwarvenTresurePlace(XRect * _area, XLocation * _loc) : XAnyPlace(_area, _loc) {}
	XDwarvenTresurePlace() {assert(0);}
	virtual int onCreatureMove(XCreature * cr);
	virtual int onCreatureEnter(XCreature * cr);
	virtual void Store(XFile * f);
	virtual void Restore(XFile * f);

};


////////////////////////////////////////////////////////////////////////
// Gas Mine
////////////////////////////////////////////////////////////////////////
class XGasPump: public XMapObject
{
public:
	DECLARE_CREATOR(XGasPump, XMapObject);
	XGasPump(int _x, int _y, XLocation * _l);
	int onOuterUse(XCreature * cr);
	const char * GetName(XCreature * viewer) { return "a gas pump"; }
protected:
	XGasPump() {im = IM_MISC;}
};


class XGasMineLocation : public XLocation
{
public:
	XGasMineLocation(LOCATION loc);
};

class XGasPlace : public XAnyPlace
{
public:
	DECLARE_CREATOR(XGasPlace, XAnyPlace);
	XGasPlace() {assert(0);}
	XGasPlace(XRect * _area, XLocation * _loc) : XAnyPlace(_area, _loc) {}
	virtual int onCreatureMove(XCreature * cr);
};


#endif
