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

#include "location.h"
#include "cbuilder.h"
#include "xarchive.h"
#include "other_misc.h"
#include "item_misc.h"
#include "game.h"
#include "unique.h"

int XLocation::rand_location_count = L_RANDOM;

REGISTER_CLASS(XLocation);
REGISTER_CLASS(XMushroomsCaveLocation);

XLocation::XLocation(LOCATION location)
{
	visited_by_hero = 0;
	map = NULL;	//map will created by XBuilder...
	for (int i = 0; i < MAX_PLACES; i++)
		places[i] = NULL;
	
	assert(Game.locations[location] == NULL);
	ln = location;
	Game.locations[location] = this;

	ttmb = 1000000; 
	ttm = ttmb; 
	im = IM_OTHER;
}

void XLocation::Invalidate()
{
	for (int i = 0; i < MAX_PLACES; i++)
		if (places[i])
		{
			places[i]->Invalidate();
			places[i] = NULL;
		}
	delete map; // map must be the last!!!!!

	XObject::Invalidate();
}

void XLocation::AddPlace(XAnyPlace * pl)
{
	for (int i = 0; i < MAX_PLACES; i++)
		if (places[i] == NULL)
		{
			places[i] = pl;
			return;
		}
	assert(0);
}

void XLocation::GetFreeXY(XPoint * pt, XRect * area)
{
	int f = 10000;
	int tx, ty;
	
	int bx, by, dx, dy;
	if (area)
	{
		bx = area->left;
		by = area->top;
		dx = area->Width();
		dy = area->Hight();
	} else
	{
		bx = 0;
		by = 0;
		dx = map->len;
		dy = map->hgt;
	}

	while(f-- > 0)
	{
		tx = vRand() % dx + bx;
		ty = vRand() % dy + by;
		if (map->XGetMovability(tx, ty) == 0 && map->GetSpecial(tx, ty) == NULL)
		{
			pt->x = tx;
			pt->y = ty;
			return;
		}
	}
	assert(0);
}


void XLocation::BuildCave()
{
	int cl = 80;
	int ch = 20;
	map = new XMap(cl, ch);

	for (int i = 0; i < map->hgt; i++)
	{
		for (int j = 0; j < map->len; j++)
		{
			map->SetXY(j, i, M_MAGMA);
		}
	}

	for (int k = 0; k < 150; k++)
	{
		int	qx = vRand() % (cl - 7) + 1;
		int qy = vRand() % (ch - 5) + 1;

		for (int q = 0; q < 360; q += 3)
		{
			for (int w = 0; w < 3; w++)
			{
				int tx = qx + (int)(w * cos(q * M_PI / 180.0));
				int ty = qy + (int)(w * sin(q * M_PI / 180.0)); 
				if (tx > 0 && ty > 0 && tx < 79 && ty < 19)
				{
					map->SetXY(tx, ty, M_CAVEFLOOR);
				}
			}
		}
	}
}

void XLocation::BuildLabirint(int create_trap_door_chest)
{
     XCaveBuilder * xcb = new XCaveBuilder(map);
     xcb->Build();
     delete xcb;
	 if (create_trap_door_chest)
	 {
		CreateDoors();
		CreateTraps();
		CreateChests();
	 }
}

void XLocation::CreateDoors()
{
	for (int i = 1; i < map->hgt - 1; i++)
		for (int j = 1; j < map->len - 1; j++)
		{
			if (map->XGetMovability(j, i) == 0)
			{
				int a11 = (map->XGetMovability(j - 1, i - 1) == 0);
				int a12 = (map->XGetMovability(j, i - 1) == 0);
				int a13 = (map->XGetMovability(j + 1, i - 1) == 0);
				int a21 = (map->XGetMovability(j - 1, i) == 0);
				int a23 = (map->XGetMovability(j + 1, i) == 0);
				int a31 = (map->XGetMovability(j - 1, i + 1) == 0);
				int a32 = (map->XGetMovability(j , i + 1) == 0);
				int a33 = (map->XGetMovability(j + 1 , i + 1) == 0);

				if (( ((a11 && a13) || (a31 && a33)) && a32 && a12 && !a21 && !a23)
					|| ( ((a11 && a31) || (a13 && a33)) && a21 && a23 && !a12 && !a32))
				{
					if (vRand(3) == 0)
						new XDoor(j, i, 0, this);
					else
						new XDoor(j, i, 1, this);
				}


			}
		}
}

void XLocation::CreateTraps()
{
	if (vRand(3) == 1)
	{
		XPoint pt;
		for (int i = 0; i < vRand(7); i++)
		{
			GetFreeXY(&pt);
			new XTrap(pt.x, pt.y, this, TT_RANDOM);
		}
	}
}

void XLocation::CreateChests()
{
	if (vRand(3) == 1)
	{
		XPoint pt;
		for (int i = 0; i < vRand(4); i++)
		{
			GetFreeXY(&pt);
			XChest * ch1 = new XChest(vRand(6) + 1, IM_ITEM, 1, 5000);
			ch1->Drop(this, pt.x, pt.y);
		}
	}
}

XCreature * XLocation::NewCreature(CREATURE_NAME cn, int x, int y, GROUP_ID gid)
{
	XCreature * cr = XCreatureStorage::Create(cn);
	cr->group_id = gid;
	if (cr->xai->GetAIFlag() & AIF_PEACEFULL)
		cr->xai->SetEnemyClass(CR_NONE); //by default all creatures in pease with others.
	Game.NewCreature(cr, x, y, this);
	return cr;
}

XCreature * XLocation::NewCreature(CREATURE_NAME cn, XRect * rect, GROUP_ID gid, unsigned int ai_flags)
{
	XPoint pt;
	GetFreeXY(&pt, rect);
	XCreature * cr = NewCreature(cn, pt.x, pt.y, gid);
	if (cr->xai->GetAIFlag() & AIF_PEACEFULL)
		cr->xai->SetEnemyClass(CR_NONE); //by default all creatures in pease with others.
	if (rect &&  (ai_flags & AIF_GUARD_ARIAL))
		cr->xai->SetArial(rect, ln);
	cr->xai->SetAIFlag((AI_FLAG)(ai_flags));
	return cr;
}

XCreature * XLocation::NewCreature(CREATURE_CLASS crc, XRect * rect, GROUP_ID gid, unsigned int ai_flags)
{
	XPoint pt;
	GetFreeXY(&pt, rect);
	XCreature * cr = XCreatureStorage::CreateRnd(crc);
	cr->group_id = gid;
	if (cr->xai->GetAIFlag() & AIF_PEACEFULL)
		cr->xai->SetEnemyClass(CR_NONE); //by default all creatures in pease with others.
	Game.NewCreature(cr, pt.x, pt.y, this);
	if (rect &&  (ai_flags & AIF_GUARD_ARIAL))
		cr->xai->SetArial(rect, ln);
	cr->xai->SetAIFlag((AI_FLAG)(ai_flags));
	return cr;
}


XStarWay * XLocation::NewWay(LOCATION target_ln, STARWAYTYPE s_type, XRect * area)
{
	XPoint pt;
	GetFreeXY(&pt, area);
	return NewWay(pt.x, pt.y, target_ln, s_type);
}

XStarWay * XLocation::NewWay(int x, int y, LOCATION target_ln, STARWAYTYPE s_type)
{
	XStarWay * pWay = new XStarWay(x, y, this, target_ln, s_type);
	ways_list.push_back(pWay);
	return pWay;
}



void XLocation::CreateShop(unsigned int im, XRect * rect, char * sk_name, SHOP_DOOR sd)
{
	XShop * shop = new XShop(rect, (ITEM_MASK)im, this, sd);
	AddPlace(shop);
	XCreature * cr = NewCreature(CN_SHOPKEEPER, rect);
	((XShopkeeper *)cr)->SetShop(sk_name, shop);
}

int XLocation::GetCreatureCount(unsigned int creature_class)
{
	int count = 0;
	XObject * o = root;
	while (o)
	{
		if ((o->im & IM_CREATURE) && (((XCreature *)o)->l->xguid == this->xguid) && (((XCreature *)o)->creature_class) & creature_class)
			count++;
		o = o->next;
	}
	return count;
}

void XLocation::Store(XFile * f)
{
	XObject::Store(f);
    map->Store(f);
	for (int i = 0; i < MAX_PLACES; i++) places[i].Store(f);
	f->Write(brief_name, 10);
	f->Write(full_name, 80);
	f->Write(&visited_by_hero);
	f->Write(&ln, sizeof(LOCATION));
}

void XLocation::Restore(XFile * f)
{
	XObject::Restore(f);
	map = new XMap();
	map->Restore(f);
	for (int i = 0; i < MAX_PLACES; i++)
	{
		places[i].Restore(f);
		if (places[i] && places[i]->im & IM_WAY)
			ways_list.push_back(places[i]);
	}
	f->Read(brief_name, 10);
	f->Read(full_name, 80);
	f->Read(&visited_by_hero);
	f->Read(&ln, sizeof(LOCATION));
}

void XLocation::DumpLocation(FILE * f)
{
	fprintf(f, "###### %s ######\n", full_name);
	map->Dump(f);
	fprintf(f, "\n");
	fprintf(f, "\n");
}


void XLocation::CreateRandomCave()
{
	int deep = vRand(6) + 5;
	int view = (vRand(2) == 1);
	int start_cr_lvl = vRand(CRL_AVG);

	XRect tr(115, 60, 180, 80);
	Game.locations[L_MAIN]->NewWay((LOCATION)rand_location_count, STW_DOWN, &tr);
	new XRandomLocation(1, view, L_MAIN, rand_location_count + 1, start_cr_lvl);
	int i = 1;
	for (; i < deep - 1; i++)
	{
		new XRandomLocation(i + 1, view, rand_location_count - 1, rand_location_count + 1, start_cr_lvl + (1 << i));
	}
	new XRandomLocation(i + 1, view, rand_location_count - 1, 0, start_cr_lvl + (1 << i));
}



XRandomLocation::XRandomLocation(int deep, int view, int way_up, int way_down, int cr_lvl) : XLocation((LOCATION)(XLocation::rand_location_count))
{
	XLocation::rand_location_count++;
	sprintf(brief_name, "Rnd%d", deep);
	sprintf(full_name, "Random Place Level %d", deep);

	if (view)
		BuildCave();
	else
	{
		map = new XMap(80, 20);
		BuildLabirint(1);
	}

	XPoint pt;
	if (way_up)
	{
		NewWay((LOCATION)way_up, STW_UP, NULL);
	}

	if (way_down)
	{
		NewWay((LOCATION)way_down, STW_DOWN, NULL);
	}
	Game.Sheduler.Add(new XUniversalGen(this, (CREATURE_CLASS)(CR_UNDEAD | CR_INSECT | CR_REPTILE | CR_RAT | CR_ALL_IMPL), (CREATURE_LEVEL)cr_lvl, 4, 50000));
}


