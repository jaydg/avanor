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

#include <vector>
#include "cbuilder.h"
#include "xapi.h"

void XCaveBuilder::Build()
{
	int i;
	for (i = 0; i < m->hgt; i++)
		for (int j = 0; j < m->len; j++)
			m->SetXY(j, i, M_MAGMA);

	std::vector<XCave *> quae;

	int nCave = m->hgt * m->len / 200;

	for (i = 0; i < nCave; i++)
	{
		int iflag = 10000;
		while (iflag--)
		{
			XCave * xc = new XCave(m->len, m->hgt);

			int tflag = 1;
			for (unsigned int q = 0; q < quae.size() && tflag; q++)
			{
				XCave * txc = quae[q];
				if (txc->Intersect(xc, 3))
					tflag = 0;
			}

			if (tflag)
			{
				quae.push_back(xc);
				xc->Draw(m);
				iflag = 0;
			} else 
				delete xc;
		}
	}

	for (int k = 0; k < quae.size() - 1; k++)
	{
		XCave * tc1 = quae[k];
		XCave * tc2 = quae[k + 1];
		XPoint pt1;
		XPoint pt2;
		tc1->GetFreeExit(&pt1);
		tc2->GetFreeExit(&pt2);
		int attempt = 2;
		while (attempt > 0)
		{
			if (Link(&pt1, &pt2))
				break;
			attempt--;
		}
		assert(attempt != 0);
	}

	for (unsigned int w = 0; w < quae.size(); w++)
	{
		XCave * tmpxc = quae[w];
		tmpxc->Draw(m);
		delete quae[w];
	}
}

bool XCaveBuilder::Link(XPoint * p1, XPoint * p2)
{
	int dx = sgn(p2->x - p1->x);
	int dy = sgn(p2->y - p1->y);

	int i;
	for (i = p1->x; i != p2->x; i += dx)
		m->SetXY(i, p1->y, M_CAVEFLOOR);
	for (i = p1->y; i != p2->y; i += dy)
		m->SetXY(p2->x, i, M_CAVEFLOOR);
	return true;
}
