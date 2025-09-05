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

#ifndef __XBASEOBJ_H__
#define __XBASEOBJ_H__

#include "xmapobj.h"
#include "dice.h"
#include "resist.h"
#include "stats.h"


class XBaseObject : public XMapObject
{
public:
	DECLARE_CREATOR(XBaseObject, XMapObject);
	XBaseObject();
	virtual void Invalidate();
	XBaseObject(XBaseObject * copy);
	~XBaseObject();

	int _DV, _PV, _HIT, RNG;
	int _HP, _PP;
	int MAX_HP;
	int MAX_PP;

	int weight;
	XDice dice;
	virtual int Compare(XObject * o);

	XResistance * r;
	XStats * s;

	virtual void Store(XFile * f);
	virtual void Restore(XFile * f);

};

#endif
