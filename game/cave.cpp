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

#include "cave.h"


XCave::XCave(XRect * _r)
{
	r.Setup(_r);
}


XCave::XCave(int len, int hgt)
{
	assert(len > 4);
	assert(hgt > 4);

	int x, y, l, h;
	if (1) //random cave
	{
		while (1)
		{
			x = vRand() % (len - 7) + 1;
			y = vRand() % (hgt - 4) + 1;
			l = vRand() % 6 + 7;
			h = vRand() % 3 + 3;
			if (x + l < len && y + h < hgt) break;
		}
		for (int i = x; i < x + l; i++)
		{
			exits.push_back(XPoint(i, y));
			exits.push_back(XPoint(i, y + h));
		}
		for (int j = y; j < y + h; j++)
		{
			exits.push_back(XPoint(x, j));
			exits.push_back(XPoint(x + l, j));
		}
	} else //special cave
	{

	}
	r.Setup(&XRect(x, y, x + l, y + h));
}

int XCave::Compare(XObject * o)
{
//	return 0;

	XCave * xc = (XCave *)o;
	if (xc->r.left < r.left && xc->r.top <= r.top) return -1;
	else return 1;

}

int XCave::Intersect(XCave * xc, int dist)
{
	XRect tr(xc->r);
	tr.Grow(dist);
	return tr.Intersect(&r);
}

void XCave::Draw(XMap * m)
{
	for (int i = r.top; i < r.bottom; i++)
		for (int j = r.left; j < r.right; j++)
			m->SetXY(j, i, M_CAVEFLOOR);
}

bool XCave::GetFreeExit(XPoint * pt)
{
	int n = vRand(exits.size());
	XQList<XPoint>::iterator it = exits.begin();
	while (n > 0)
	{
		it++;
		n--;
	}
	*pt = *it;
	exits.erase(it);
/*	pt->x = r.left + vRand(r.Width());
	pt->y = r.top + vRand(r.Hight());*/
	return true;
}
