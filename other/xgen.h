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

#ifndef __XGEN_H
#define __XGEN_H

#include "map.h"
#include "xmapobj.h"

class XGenerator : public XMapObject
{
protected:
	XGenerator () {}
public:
	DECLARE_CREATOR(XGenerator, XMapObject);
	XGenerator(int run_time)
	{	
		ttmb = run_time; 
		ttm = ttmb; 
		x = -1; 
		y = -1;  
		im = IM_OTHER;
	}
	int Run() {assert(0); return 0;}
	virtual int Compare(XObject * o) {return 1;}
};
/*
class XDebugGen : public XGenerator
{
public:
	XDebugGen() : XGenerator(5000) {}
	virtual void Run();
};
*/

class XUniversalGen : public XGenerator
{
protected:
	XUniversalGen() {}
public:
	DECLARE_CREATOR(XUniversalGen, XGenerator);
	XUniversalGen(XLocation * loc, CREATURE_CLASS _crc, CREATURE_LEVEL _crl, unsigned int _max_creature = 8, int refresh_time = 15000) : XGenerator(refresh_time) 
	{
		l   = loc;
		crl = _crl; 
		crc = _crc;
		max_creature = _max_creature;
	}
	int Run();
	virtual void Store(XFile * f);
	virtual void Restore(XFile * f);
protected:
	CREATURE_LEVEL crl;
	CREATURE_CLASS crc;
	unsigned int max_creature;
};


class XMainLocationGen : public XGenerator
{
public:
	DECLARE_CREATOR(XMainLocationGen, XGenerator);
	XMainLocationGen(XLocation * loc) : XGenerator(1000) 
	{
		l           = loc;
		turns_count = 0;
	}
	int Run();
	virtual void Store(XFile * f);
	virtual void Restore(XFile * f);
protected:
	int turns_count;
};

#endif
