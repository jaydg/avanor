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

#include "map.h"
#include "item.h"
#include "creature.h"
#include "xarchive.h"
#include "other_misc.h"

xMAP stdmap[] = {
{' ', xBLACK, "unknown", MO_UNKNOWN, VI_UNKNOWN},
{'.', xGREEN, "green grass", MO_NORMAL, VI_NORMAL},
{'T', xGREEN, "large tree", MO_HARD, VI_HARD},
{'.', xYELLOW, "sand", MO_SHARD, VI_NORMAL},
{'#', xCYAN, "window", MO_WALL, VI_NORMAL},
{'#', xDARKGRAY, "magma", MO_WALL, VI_WALL},
{'#', xLIGHTGRAY, "quartz", MO_WALL, VI_WALL},
{'.', xLIGHTGRAY, "cave floor", MO_NORMAL, VI_NORMAL},
{'.', xLIGHTGRAY, "stone floor", MO_NORMAL, VI_NORMAL},
{'.', xBROWN, "path", MO_NORMAL, VI_NORMAL},
{'#', xBROWN, "wooden wall", MO_WALL, VI_WALL},
{'#', xLIGHTGRAY, "stone wall", MO_WALL, VI_WALL},
{'=', xLIGHTBLUE, "water", MO_WATER, VI_NORMAL},
{'=', xBLUE, "deep water", MO_DEEPWATER, VI_NORMAL},
{'^', xGREEN, "hill", MO_NORMAL, VI_NORMAL},
{'^', xBROWN, "low mountains", MO_VHARD, VI_AHARD},
{'^', xLIGHTGRAY, "mountains", MO_MOUNTAIN, VI_HARD},
{'^', xWHITE, "high mountains", MO_WALL, VI_VHARD},
{'=', xBROWN, "bridge", MO_NORMAL, VI_NORMAL},
{'.', xYELLOW, "road", MO_NORMAL, VI_NORMAL},
{'.', xDARKGRAY, "obsidian floor", MO_NORMAL, VI_NORMAL},
{'X', xBROWN, "fence", MO_WALL, VI_NORMAL},
{'.', xYELLOW, "golden floor", MO_NORMAL, VI_NORMAL},
{'#', xWHITE, "marble wall", MO_WALL, VI_WALL},
{'#', xDARKGRAY, "black marble wall", MO_WALL, VI_WALL},
{'X', xYELLOW, "golden fence", MO_WALL, VI_NORMAL},
{'0', xWHITE, "teleport circle", MO_NORMAL, VI_NORMAL},
};

MAP::MAP()
{
	n = M_GREENGRAS;//M_MAGMA;//(STDMAP)(rand() % 3 + 1);
	pMonster = NULL;
	pSpecialObject = NULL;
//	mflag = MF_NONE;
	visible = false;
	known = ' ';
	color = 0;
//	spec = SC_NONE;
	place = NULL; //by default
};

MAP::~MAP()
{
	item_list.KillAll();
	if (pSpecialObject)
	{
		pSpecialObject->Invalidate();
		pSpecialObject = NULL;
	}
}

void MAP::Store(XFile * f)
{
	f->Write(&color, sizeof(char));
	item_list.StoreList(f);
	f->Write(&known, sizeof(char));
	f->Write(&n, sizeof(STDMAP));

	place.Store(f);
	pSpecialObject.Store(f);
	pMonster.Store(f);
	
	f->Write(&visible, sizeof(bool));	
}

void MAP::Restore(XFile * f)
{
	f->Read(&color, sizeof(char));
	item_list.RestoreList(f);
	f->Read(&known, sizeof(char));
	f->Read(&n, sizeof(STDMAP));

	place.Restore(f);
	pSpecialObject.Restore(f);
	pMonster.Restore(f);

	f->Read(&visible, sizeof(bool));	
}

XMap::XMap()
{
	map = NULL;
	hgt = 0;
	len = 0;
	wx = 0;
	wy = 0;
}

XMap::XMap(int l, int h)
{
	map = NULL;
	map = new MAP[l * h];
	assert(map);

	hgt = h;
	len = l;
	wx = 0;
	wy = 0;
}

XMap::~XMap()
{
	delete[] map;
}


void XMap::ResVisible(int x, int y)
{
	if (x >= 0 && x < len && y >= 0 && y < hgt)
		map[x + y * len].visible = false;
}

void XMap::SetVisible(int x, int y)
{
	if (x >= 0 && x < len && y >= 0 && y < hgt)
	{
		map[x + y * len].visible = true;
		map[x + y * len].color = stdmap[map[x + y * len].n].color;
		map[x + y * len].known = stdmap[map[x + y * len].n].view;
	}
}

bool XMap::GetVisible(int x, int y)
{
	if (x >= 0 && x < len && y >= 0 && y < hgt)
		return map[x + y * len].visible;
	else
		return false;
}


void XMap::SetPlace(int x, int y, XAnyPlace * place)
{
	assert(x >= 0 && x < len && y >= 0 && y < hgt);
	map[x + y * len].place = place;
}

XAnyPlace * XMap::GetPlace(int x, int y)
{
	assert(x >= 0 && x < len && y >= 0 && y < hgt);
	return map[x + y * len].place.get();
}

/*
MFLAG XMap::GetMFlag(int x, int y)
{
	if (x >= 0 && x < len && y >= 0 && y < hgt)
		return   map[x + y * len].mflag;
	else
		return MF_NONE;
}

void XMap::ResMFlag(int x, int y, MFLAG mf)
{
	if (x >= 0 && x < len && y >= 0 && y < hgt)
	{
		map[x + y * len].mflag = (MFLAG)(map[x + y * len].mflag | mf);
		map[x + y * len].mflag = (MFLAG)(map[x + y * len].mflag ^ mf);
	}
}

void XMap::SetMFlag(int x, int y, MFLAG mf)
{
	if (x >= 0 && x < len && y >= 0 && y < hgt)
		map[x + y * len].mflag = (MFLAG)(map[x + y * len].mflag | mf);
}
*/
void XMap::ResKnown(int x, int y)
{
	if (x >= 0 && x < len && y >= 0 && y < hgt)
		map[x + y * len].known = 0;
}

void XMap::SetKnown(int x, int y)
{
	if (x >= 0 && x < len && y >= 0 && y < hgt)
		map[x + y * len].known = 1;
}

int XMap::GetKnown(int x, int y)
{
	if (x >= 0 && x < len && y >= 0 && y < hgt)
		return map[x + y * len].known;
	else
		return 0;
}


void XMap::SetSpecial(int x, int y, XMapObject * spec)
{
	if (x >= 0 && x < len && y >= 0 && y < hgt)
	{
		map[x + y * len].pSpecialObject = spec; 
	}
}

XMapObject * XMap::GetSpecial(int x, int y)
{
	if (x >= 0 && x < len && y >= 0 && y < hgt)
		return map[x + y * len].pSpecialObject.get();
	else
		return NULL;
}

int XMap::GetVisibility(int x, int y)
{
	if (x >= 0 && x < len && y >= 0 && y < hgt)
	{
		XMapObject * spec = map[x + y * len].pSpecialObject.get();
		if (spec && spec->im & IM_DOOR && ((XDoor *)spec)->isOpened == 0) 
			return 0;
		if (stdmap[map[x + y * len].n].visiable == VI_WALL)
			return 0;
		else
			return 1;
	}
	else
		return 0;
}

char * XMap::GetDescription(int x, int y)
{
	if (x >= 0 && x < len && y >= 0 && y < hgt)
	{
		return stdmap[map[x + y * len].n].name;
	}
	else
		return "";
}


int XMap::GetMovability(int x, int y)
{
	if (x >= 0 && x < len && y >= 0 && y < hgt)
	{
		MAP & _map = map[x + y * len];
	  	if (_map.pSpecialObject && (_map.pSpecialObject->im & IM_DOOR) && 
  			((XDoor *)_map.pSpecialObject.get())->isOpened == 0) return MO_WALL;
  		return stdmap[_map.n].moveable;
	} else
		return MO_WALL;
}

int XMap::XGetMovability(int x, int y)
{
	if (x >= 0 && x < len && y >= 0 && y < hgt)
	{
		MAP * m = &map[x + y * len];
		if (m->pMonster) return 2;
		XMapObject * spec = map[x + y * len].pSpecialObject.get();
		if (stdmap[m->n].moveable < MO_WALL 
			&& !(spec && spec->im & IM_DOOR && ((XDoor *)spec)->isOpened == 0))
			return 0;
		else
			return 1;
	} else return 1;
}



void XMap::PutItem(int x, int y, XItem * item)
{
	if (x >= 0 && x < len && y >= 0 && y < hgt)
	{
		item->x = x;
		item->y = y;
		map[x + y * len].item_list.Add(item);
	}
	else
		assert(0);
}


XItemList * XMap::GetItemList(int x, int y)
{
	if (x >= 0 && x < len && y >= 0 && y < hgt)
	{
		return &map[x + y * len].item_list;
	}
	else
		assert(0);
	return NULL;
}

unsigned int XMap::GetItemCount(int x, int y)
{
	if (x >= 0 && x < len && y >= 0 && y < hgt)
	{
		return map[x + y * len].item_list.size();
	} else
		return 0;
}



void XMap::SetMonster(int x, int y, XCreature * monstr)
{
	if (x >= 0 && x < len && y >= 0 && y < hgt)
		map[x + y * len].pMonster = monstr;
	else
		assert(0);
}

void XMap::ResMonster(int x, int y)
{
	if (x >= 0 && x < len && y >= 0 && y < hgt)
		map[x + y * len].pMonster = NULL;
	else
		assert(0);
}

XCreature * XMap::GetMonster(int x, int y)
{
	if (x >= 0 && x < len && y >= 0 && y < hgt)
	{
		return map[x + y * len].pMonster.get();
	}
	else
		return NULL;
}

void XMap::PutChar(int x, int y, char c, int color)
{
	if (x >= wx && x < wx + SCR_LEN && y >= wy && y < wy + SCR_HGT)
		vPutCh(x - wx + SCR_X, y - wy + SCR_Y, c, color);
}

void XMap::Put(XCreature * cr)
{
	for (int i = 0; i < SCR_HGT && wy + i < hgt; i++)
		for (int j = 0; j < SCR_LEN  && wx + j < len; j++)
		{
			MAP * tmap = &map[(i + wy) * len + j + wx];
			if (tmap->visible)
			{
				if (tmap->pSpecialObject && !(tmap->pSpecialObject->im == IM_TRAP && !((XTrap *)tmap->pSpecialObject.get())->isVisible(NULL)))
				{
					vPutCh(j + SCR_X, i + SCR_Y, tmap->pSpecialObject->view, tmap->pSpecialObject->color);
					tmap->color = tmap->pSpecialObject->color;
					tmap->known = tmap->pSpecialObject->view;
				} else
				if (!tmap->item_list.empty())
				{
					XItem * item = tmap->item_list.begin();
					vPutCh(j + SCR_X, i + SCR_Y, item->view, item->color);
					tmap->color = item->color;
					tmap->known = item->view;
				} else
				{
					//int tn = (i + wy) * len + j + wx;
					int n = tmap->n;
					vPutCh(j + SCR_X, i + SCR_Y, stdmap[n].view, stdmap[n].color);
				}
				if (tmap->pMonster && cr->isCreatureVisible(tmap->pMonster))
				{
					XCreature * xb = tmap->pMonster;
					vPutCh(xb->x - wx + SCR_X, xb->y - wy + SCR_Y, xb->view, xb->color);
				}
			} else
				vPutCh(j + SCR_X, i + SCR_Y, ' ', xBLACK);
			if (tmap->known && !tmap->visible)
			{
				vPutCh(j + SCR_X, i + SCR_Y, tmap->known, tmap->color);
			}
		}
}

void XMap::Center(int x, int y)
{
	if (x <= wx + 2 || x >= wx + SCR_LEN - 2)
	{
		wx = x - SCR_LEN / 2;
		if (wx + SCR_LEN > len) wx = len - SCR_LEN;
		if (wx < 0) wx = 0;
	}
	if (y <= wy + 2 || y >= wy + SCR_HGT - 2)
	{
		wy = y - SCR_HGT / 2;
		if (wy + SCR_HGT > hgt) wy = hgt - SCR_HGT;
		if (wy < 0) wy = 0;
	}
}

void XMap::SetXY(int x, int y, STDMAP stdm)
{
	assert(x >= 0 && x < len);
	assert(y >= 0 && y < hgt);

	map[x + y * len].n = stdm;
}

STDMAP XMap::GetXY(int x, int y)
{
	assert(x >= 0 && x < len);
	assert(y >= 0 && y < hgt);

	return map[x + y * len].n;
}

void XMap::CreateRoom(int x, int y, int l, int h, int px, int py, STDMAP m1, STDMAP m2)
{
	CreateRoom(x, y, l, h, m1, m2);
	SetXY(px, py, m1);
}

void XMap::CreateRoom(int x, int y, int l, int h, STDMAP m1, STDMAP m2)
{
	for (int i = 0; i < l; i++)
		for (int j = 0; j < h; j++)
		{
			if (i == 0 || i == l - 1 || j == 0 || j == h - 1)
			{
				SetXY(i + x, j + y, m2);
			}
			else
				SetXY(i + x, j + y, m1);

		}
}


void XMap::Store(XFile * f)
{
	f->Write(&len, sizeof(int));
	f->Write(&hgt, sizeof(int));

	for (int i = 0; i < hgt; i++)
		for (int j = 0; j < len; j++)
			map[j + i * len].Store(f);
}

void XMap::Restore(XFile * f)
{
	f->Read(&len, sizeof(int));
	f->Read(&hgt, sizeof(int));
	map = new MAP[len * hgt];

	for (int i = 0; i < hgt; i++)
		for (int j = 0; j < len; j++)
			map[j + i * len].Restore(f);

}

void XMap::Dump(FILE * f)
{
	for (int i = 0; i < hgt; i++)
	{
		for (int j = 0; j < len; j++)
		{
			MAP * tmap = &map[i * len + j];
			int n = tmap->n;
			char vch = stdmap[n].view;
		
			if (tmap->pSpecialObject)
				vch = tmap->pSpecialObject->view;
			
			if (!tmap->item_list.empty())
				vch = tmap->item_list.begin()->view;

			if (tmap->pMonster)
				vch = tmap->pMonster->view;

			fprintf(f, "%c", vch);
		}
		fprintf(f, "\n");
	}
}

