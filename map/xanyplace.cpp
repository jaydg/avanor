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

#include "xanyplace.h"
#include "map.h"
#include "location.h"
#include "creature.h"

XAnyPlace::XAnyPlace(XRect * _arial, XLocation * _loc) : arial(_arial), XObject()
{
	Setup(_loc);
	im = IM_OTHER;
}

void XAnyPlace::Invalidate()
{
	location = NULL;
	owner    = NULL;
	XObject::Invalidate();
}

void XAnyPlace::onShowItem(XItem * item, char * buf)
{
	item->toString(buf);
}

void XAnyPlace::Setup(XLocation * _loc)
{
	location = _loc;
	for (int i = arial.left; i < arial.right; i++)
		for (int j = arial.top; j < arial.bottom; j++)
			location->map->SetPlace(i, j, this);
}

void XAnyPlace::Store(XFile * f)
{
	XObject::Store(f);
	
	location.Store(f);
	owner.Store(f);
	arial.Store(f);
}

void XAnyPlace::Restore(XFile * f)
{
	XObject::Restore(f);

	location.Restore(f);
	owner.Restore(f);

	arial.Restore(f);
}
