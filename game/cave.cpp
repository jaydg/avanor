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

bool CAVE_DATA::isExit(int x, int y)
{
	char ch = cave[x + y * width];
	if (ch == '+' || ch == '.')
		return true;
	else
		return false;
}

char CAVE_DATA::GetCode(int x, int y)
{
	return cave[x + y * width];
}

CAVE_DATA random_caves[] = {
	//RCT_SIMPLE1
	{9, 7,
		"####+####"
		"##.....##"
		"#..###..#"
		"+.#####.+"
		"#..###..#"
		"##.....##"
		"####+####"		
	},

	//RCT_SIMPLE2
	{9, 7,
		"####+####"
		"#.......#"
		"#..#+#..#"
		"+..# #..+"
		"#..###..#"
		"#.......#"
		"####+####"		
	},

};


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
		rct = RCT_USUAL;
		while (1)
		{
			x = vRand() % (len - 7) + 1;
			y = vRand() % (hgt - 4) + 1;
			l = vRand() % 6 + 7;
			h = vRand() % 3 + 3;
			if (x + l < len && y + h < hgt) break;
		}
		//all borders of such cave is exit
		for (int i = x; i < x + l; i++)
		{
			exits.push_back(XPoint(i, y));
			exits.push_back(XPoint(i, y + h - 1));
		}
		for (int j = y; j < y + h; j++)
		{
			exits.push_back(XPoint(x, j));
			exits.push_back(XPoint(x + l - 1, j));
		}
	} else //special cave
	{
		rct = (RANDOM_CAVE_TYPE)vRand(RCT_USUAL);
		l = random_caves[rct].width;
		h = random_caves[rct].height;
		x = vRand() % (len - l - 1) + 1;
		y = vRand() % (hgt - h - 1) + 1;

		//searching for exits (doors, empty spaces etc.)
		for (int i = 0; i < l; i++)
		{
			if (random_caves[rct].isExit(i, 0))
				exits.push_back(XPoint(i + x, y));
			if (random_caves[rct].isExit(i, h - 1))
				exits.push_back(XPoint(i + x, y + h - 1));

		}
		for (int j = 0; j < h; j++)
		{
			if (random_caves[rct].isExit(0, j))
				exits.push_back(XPoint(x, y + j));
			if (random_caves[rct].isExit(l - 1, j))
				exits.push_back(XPoint(l - 1 + x, y + j));
		}
	
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
	if (rct == RCT_USUAL)
	{
		for (int i = r.top; i < r.bottom; i++)
			for (int j = r.left; j < r.right; j++)
				m->SetXY(j, i, M_CAVEFLOOR);
	} else
	{
		STDMAP sm = M_CAVEFLOOR;
		for (int i = r.top; i < r.bottom; i++)
			for (int j = r.left; j < r.right; j++)
			{
				char ch = random_caves[rct].GetCode(j - r.left, i - r.top);
				switch (ch)
				{
					case '#': sm = M_STONEWALL; break;
					case '.': sm = M_STONEFLOOR; break;	
				}
				m->SetXY(j, i, sm);
			}
	}
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
	return true;
}
