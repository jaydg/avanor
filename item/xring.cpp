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

#include "xring.h"

REGISTER_CLASS(XRing);

int ridentify_db[20] = {
0, 0, 0, 0, 0,
0, 0, 0, 0, 0,
0, 0, 0, 0, 0,
0, 0, 0, 0, 0
};

void XRing::StoreTable(XFile * f)
{
	f->Write(ridentify_db, sizeof(int), 20);
}


void XRing::RestoreTable(XFile * f)
{
	f->Read(ridentify_db, sizeof(int), 20);
}



XRing::XRing(ENHANCE enh) : XEnhance(enh)
{
	im = IM_RING;
	bp = BP_RING;
	it = IT_RING;
	strcpy(name, "ring");
	view = '=';
}

int XRing::isIdentifed()
{
	return ridentify_db[descr];
}

void XRing::Identify(int level)
{
	ridentify_db[descr] = level;
}


