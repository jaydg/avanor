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

#include "xmapobj.h"
#include "xarchive.h"

XMapObject::XMapObject() : l(NULL), x(-1), y(-1)
{
}

XMapObject::XMapObject(XMapObject * copy) :
	XObject((XObject *)copy),
	x       (copy->x),
	y       (copy->y),
	nx      (copy->nx),
	ny      (copy->ny),
	view    (copy->view),
	color   (copy->color)
{
	l = copy->l.get();
	strcpy(name, copy->name);
}

void XMapObject::Invalidate()
{
	SetLocation(NULL);
	XObject::Invalidate();
}

void XMapObject::Store(XFile * f)
{
	XObject::Store(f);

	f->Write(&x, sizeof(int));
	f->Write(&y, sizeof(int));
	f->Write(&nx, sizeof(int));
	f->Write(&ny, sizeof(int));
	f->Write(&view, sizeof(char));
	f->Write(&color, sizeof(int));
	f->Write(name, sizeof(char), 50);
	l.Store(f);
}


void XMapObject::Restore(XFile * f)
{
	XObject::Restore(f);

	f->Read(&x, sizeof(int));
	f->Read(&y, sizeof(int));
	f->Read(&nx, sizeof(int));
	f->Read(&ny, sizeof(int));
	f->Read(&view, sizeof(char));
	f->Read(&color, sizeof(int));
	f->Read(name, sizeof(char), 50);

	l.Restore(f);;
}

int XMapObject::Compare(XObject * o)
{
	assert(dynamic_cast<XMapObject *>(o));

	if(!isValid() || !o->isValid()) return 1;

	XMapObject * tit = (XMapObject *)o;
	if (tit->x == x && tit->y == y && tit->view == view)
		return 0;
	else
		return 1;
}

bool XMapObject::SetLocation(XLocation * new_l)
{
	l = new_l;
	return true;
}
