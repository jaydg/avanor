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
#include "other_misc.h"


XCaveBuilder::XCaveBuilder(XLocation * _l, int create_door_trap_chest)
{
	m = _l->map;
	location = _l;
	isCreateDoorTrapChest = create_door_trap_chest > 0;
}


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
			XCave * xc = new XCave(m->len, m->hgt, isCreateDoorTrapChest);

			int tflag = 1;
			for (unsigned int q = 0; q < quae.size() && tflag; q++)
			{
				XCave * txc = quae[q];
				if (txc->Intersect(xc, 0))
					tflag = 0;
			}

			if (tflag)
			{
				quae.push_back(xc);
				xc->Draw(location);
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

		if (tc1->GetFreeExit(&pt1) && tc2->GetFreeExit(&pt2))
		{
			Link(&pt1, &pt2);
		} else
			assert(0);
	}

	if (isCreateDoorTrapChest)
	{
		CreateDoors();
	}
}


struct LINK_STACK
{
	int pt_count;
	XPOINT pa[2000]; //it is seems that will never be more than 2000 points.
	void PushPoint(int x, int y)
	{
		pa[pt_count].x = x;
		pa[pt_count].y = y;
		pt_count++;
		assert(pt_count < 2000);
	}
};


//linking of room is using standard flood-fill algorithm
bool XCaveBuilder::Link(XPoint * p1, XPoint * p2)
{
	//create and reset table equal with map
	int * tbl = new int[m->hgt * m->len];
	memset(tbl, 0, sizeof(int) * m->hgt * m->len);

	//create stack of points (we will exchange them to increase perfomance)
	LINK_STACK st1;
	st1.pt_count = 0;
	LINK_STACK st2;
	st2.pt_count = 0;
	LINK_STACK * current = &st1;
	LINK_STACK * other = &st2;

	current->pa[current->pt_count].x = p1->x;
	current->pa[current->pt_count].y = p1->y;
	current->pt_count = 1;
	int step = 1;
	tbl[p1->x + p1->y * m->len] = step;

	bool flag = true;
	while (flag)
	{
		for (int i = 0; i < current->pt_count; i++)
		{
			int tx = current->pa[i].x;
			int ty = current->pa[i].y;
			if (tx > 0 && tx < m->len - 1 && ty > 0 && ty < m->hgt - 1)
			{
				if (tbl[tx + 1 + ty * m->len] == 0)
				{
					if (tx + 1 == p2->x && ty == p2->y)
					{
						tbl[tx + 1 + ty * m->len] = step;
						flag = false;
						break;
					}
					if (m->GetRoom(tx + 1, ty) == 0)
					{
						other->PushPoint(tx + 1, ty);
						tbl[tx + 1 + ty * m->len] = step;
					}
				}

				if (tbl[tx - 1 + ty * m->len] == 0)
				{
					if (tx - 1 == p2->x && ty == p2->y)
					{
						tbl[tx - 1 + ty * m->len] = step;
						flag = false;
						break;
					}
					if (m->GetRoom(tx - 1, ty) == 0)
					{
						other->PushPoint(tx - 1, ty);
						tbl[tx - 1 + ty * m->len] = step;
					}
				}

				if (tbl[tx + (ty + 1) * m->len] == 0)
				{

					if (tx == p2->x && ty + 1== p2->y)
					{
						tbl[tx + (ty + 1) * m->len] = step;
						flag = false;
						break;
					}
					if (m->GetRoom(tx, ty + 1) == 0)
					{
						other->PushPoint(tx, ty + 1);
						tbl[tx + (ty + 1) * m->len] = step;
					}
				}

				if (tbl[tx + (ty - 1) * m->len] == 0)
				{
					if (tx == p2->x && ty - 1== p2->y)
					{
						tbl[tx + (ty - 1) * m->len] = step;
						flag = false;
						break;
					}
					if (m->GetRoom(tx, ty - 1) == 0)
					{
						other->PushPoint(tx, ty - 1);
						tbl[tx + (ty - 1) * m->len] = step;
					}
				}
			}
		}
		LINK_STACK * tmp = other;
		other = current;
		current = tmp;
		other->pt_count = 0;
		step++;
		if (current->pt_count == 0)
			return false;
	}

//we have found the way! yahoo!

	int px = p2->x;
	int py = p2->y;
	int stp = tbl[px + py * m->len];
	while (stp > 1)
	{
		stp = tbl[px + py * m->len];
		m->SetXY(px, py, M_CAVEFLOOR);
		if (tbl[px + 1 + py * m->len] == stp - 1)
			px++;
		else if (tbl[px - 1 + py * m->len] == stp - 1)
			px--;
		else if (tbl[px + (py + 1) * m->len] == stp - 1)
			py++;
		else if (tbl[px + (py - 1) * m->len] == stp - 1)
			py--;
	}
	m->SetXY(p1->x, p1->y, M_CAVEFLOOR);

	return true;
}



void XCaveBuilder::CreateDoors()
{
	for (int i = 1; i < m->hgt - 1; i++)
		for (int j = 1; j < m->len - 1; j++)
		{
			if (m->XGetMovability(j, i) == 0 && m->GetRoom(j, i) == 0)
			{
				int a11 = (m->XGetMovability(j - 1, i - 1) == 0);
				int a12 = (m->XGetMovability(j, i - 1) == 0);
				int a13 = (m->XGetMovability(j + 1, i - 1) == 0);
				int a21 = (m->XGetMovability(j - 1, i) == 0);
				int a23 = (m->XGetMovability(j + 1, i) == 0);
				int a31 = (m->XGetMovability(j - 1, i + 1) == 0);
				int a32 = (m->XGetMovability(j , i + 1) == 0);
				int a33 = (m->XGetMovability(j + 1 , i + 1) == 0);

				if (( ((a11 && a13) || (a31 && a33)) && a32 && a12 && !a21 && !a23)
					|| ( ((a11 && a31) || (a13 && a33)) && a21 && a23 && !a12 && !a32))
				{
					if (vRand(3) == 0)
						new XDoor(j, i, 0, location);
					else
						new XDoor(j, i, 1, location);
				}


			}
		}
}
