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

#include <ctype.h>
#include "location.h"
#include "cbuilder.h"
#include "xarchive.h"
#include "other_misc.h"
#include "item_misc.h"
#include "game.h"
#include "unique.h"
#include "quest.h"

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

	if (map)
	{
		cl = map->len;
		ch = map->hgt;
	} else
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
	if (!map)
		map = new XMap(80, 20);
     XCaveBuilder * xcb = new XCaveBuilder(this, create_trap_door_chest);
     xcb->Build();
     delete xcb;
//	 if (create_trap_door_chest)
//	 {
//		CreateDoors();
//		CreateTraps();
//		CreateChests();
//	 }
}


void XLocation::CreateTraps()
{
	if (vRand(3) == 1)
	{
		XPoint pt;
		for (int i = 0; i < vRand(7); i++)
		{
			GetFreeXY(&pt);
			new XTrap(pt.x, pt.y, this);
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
	if (cr->xai->GetAIFlag() & AIF_PEACEFUL)
		cr->xai->SetEnemyClass(CR_NONE); //by default all creatures in pease with others.
	Game.NewCreature(cr, x, y, this);
	return cr;
}

XCreature * XLocation::NewCreature(CREATURE_NAME cn, XRect * rect, GROUP_ID gid, unsigned int ai_flags)
{
	XPoint pt;
	GetFreeXY(&pt, rect);
	XCreature * cr = NewCreature(cn, pt.x, pt.y, gid);
	if (cr->xai->GetAIFlag() & AIF_PEACEFUL)
		cr->xai->SetEnemyClass(CR_NONE); //by default all creatures in pease with others.
	if (rect &&  (ai_flags & AIF_GUARD_AREA))
	{
		cr->xai->SetArea(rect, ln);
		cr->xai->LearnTraps();
	}
	cr->xai->SetAIFlag((AI_FLAG)(ai_flags));
	return cr;
}

XCreature * XLocation::NewCreature(CREATURE_CLASS crc, XRect * rect, GROUP_ID gid, unsigned int ai_flags)
{
	XPoint pt;
	GetFreeXY(&pt, rect);
	XCreature * cr = XCreatureStorage::CreateRnd(crc);
	cr->group_id = gid;
	if (cr->xai->GetAIFlag() & AIF_PEACEFUL)
		cr->xai->SetEnemyClass(CR_NONE); //by default all creatures in pease with others.
	Game.NewCreature(cr, pt.x, pt.y, this);
	if (rect &&  (ai_flags & AIF_GUARD_AREA))
	{
		cr->xai->SetArea(rect, ln);
		cr->xai->LearnTraps();
	}
	cr->xai->SetAIFlag((AI_FLAG)(ai_flags));
	return cr;
}


XStairWay * XLocation::NewWay(LOCATION target_ln, STAIRWAYTYPE s_type, XRect * area)
{
	XPoint pt;
	GetFreeXY(&pt, area);
	return NewWay(pt.x, pt.y, target_ln, s_type);
}

XStairWay * XLocation::NewWay(int x, int y, LOCATION target_ln, STAIRWAYTYPE s_type)
{
	XStairWay * pWay = new XStairWay(x, y, this, target_ln, s_type);
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
	Game.Scheduler.Add(new XUniversalGen(this, (CREATURE_CLASS)(CR_UNDEAD | CR_BLOB | CR_INSECT | CR_REPTILE | CR_RAT | CR_ALL_IMPL), (CREATURE_LEVEL)cr_lvl, 4, 50000));
}


//Location Script Support
extern "C"
{
	#include "./lua/include/lauxlib.h"
	#include "./lua/include/lualib.h"
}

XLocation * XLocation::current_location = NULL;
XCreature * XLocation::last_creature = NULL;

LOCATION_PATTERN XLocation::current_pattern;
XQList<PALETTE_MAP> XLocation::pattern_translation;

//CreateLocation(L_SMALL_CAVE1, "SmCv:1", "Small Cave Level 1", CAVE)
int XLocation::CreateLocation(lua_State * L)
{
	int loc_id = lua_tonumber(L, 1);
	const char * lbrief = lua_tostring(L, 2);
	const char * lfull = lua_tostring(L, 3);
	int type = lua_tonumber(L, 4);

	current_location = new XLocation((LOCATION)loc_id);
	strcpy(current_location->brief_name, lbrief);
	strcpy(current_location->full_name, lfull);

	if (type == 0)
		current_location->BuildCave();
	else
		current_location->BuildLabirint();

	return 0;
}

//Settle(CR_RAT + CR_FELINE + CR_INSECT, CRL_VERY_LOW)
int XLocation::Settle(lua_State * L)
{
	int crc = lua_tonumber(L, 1);
	int crl = lua_tonumber(L, 2);
	Game.Scheduler.Add(new XUniversalGen(current_location, (CREATURE_CLASS)(crc), (CREATURE_LEVEL)(crl), 5, 25000));
	return 0;
}


//cr = Creature(CN_ROTMOTH)
int XLocation::Creature(lua_State * L)
{
	int crn = lua_tonumber(L, 1);
	current_location->NewCreature((CREATURE_NAME)crn);
	return 0;
}

//cr = Guardian(CN_DWARF_GUARD, GID_DWARVEN_GUARDIAN, x, y)
int XLocation::Guardian(lua_State * L)
{
	CREATURE_NAME crn = (CREATURE_NAME)lua_tonumber(L, 1);
	GROUP_ID gid = (GROUP_ID)lua_tonumber(L, 2);
	XRect rect;
	int tx = lua_tonumber(L, 3);
	int ty = lua_tonumber(L, 4);
	int n = lua_gettop(L);
	if (n == 4)
	{
		rect = XRect(tx, ty, tx + 1, ty + 1);
	} else
	{
		int tw = lua_tonumber(L, 5);
		int th = lua_tonumber(L, 6);
		rect = XRect(tx, ty, tx + tw, ty + th);
	}

	XCreature * cr = current_location->NewCreature(crn, &rect, gid, AIF_GUARD_AREA);
	cr->xai->SetEnemyClass((CREATURE_CLASS)(CR_ALL ^ (CR_HUMAN | CR_HUMANOID)));
	lua_pushlightuserdata(L, cr);
	return 1;
}


//Way(DOWN, L_SMALL_CAVE2)
//Way(DOWN, L_SMALL_CAVE2, x, y)
int XLocation::Way(lua_State * L)
{
	int type = lua_tonumber(L, 1);
	int loc_id = lua_tonumber(L, 2);
	int n = lua_gettop(L);
	if (n == 4)
	{
		int tx = lua_tonumber(L, 3);
		int ty = lua_tonumber(L, 4);
		current_location->NewWay(tx, ty, (LOCATION)loc_id, (STAIRWAYTYPE)type);
	} else
		current_location->NewWay((LOCATION)loc_id, (STAIRWAYTYPE)type);

	return 0;
}

//CreateObject("XCookingSet")
int XLocation::CreateObject(lua_State * L)
{
	const char * name = lua_tostring(L, 1);
	XObject * p = XClassFactory::CreateNew((char *)name);
	lua_pushlightuserdata(L, p);
	return 1;
}


//DropItem(item, 0, 0)
int XLocation::DropItem(lua_State * L)
{
	XItem * pItem = (XItem *)lua_topointer(L, 1);
	int n = lua_gettop(L);
	
	int tx;
	int ty;
	if (n == 3)
	{
		tx = lua_tonumber(L, 2);
		ty = lua_tonumber(L, 3);
	} else
	{
		XPoint pt;
		current_location->GetFreeXY(&pt);
		tx = pt.x;
		ty = pt.y;
	}

	if (pItem)
	{
		pItem->Drop(current_location, tx, ty);
	}
	return 0;
}

//SetPattern(width, height,
//           "###" ..
//           "#.#" ..
//           "###")
int XLocation::SetPattern(lua_State * L)
{
	current_pattern.w = lua_tonumber(L, 1);
	current_pattern.h = lua_tonumber(L, 2);
	current_pattern.pattern = lua_tostring(L, 3);
	pattern_translation.clear();
	return 0;
}

//AddTranslation("1", M_GOLDENFLOOR)
int XLocation::AddTranslation(lua_State * L)
{
	PALETTE_MAP pm;
	pm.this_view = (lua_tostring(L, 1))[0];
	if (lua_isnumber(L, 2))
	{
		pm.real_view = (STDMAP)lua_tonumber(L, 2);
		pm.lua_str[0] = 0;
	} else
	{
		strcpy(pm.lua_str, lua_tostring(L, 2));
		pm.real_view = M_UNKNOWN;
	}
	pattern_translation.push_back(pm);
	return 0;
}

//DrawPattern(x, y)
int XLocation::DrawPattern(lua_State * L)
{
	int tx = lua_tonumber(L, 1);
	int ty = lua_tonumber(L, 2);
	current_location->PutPalette(tx, ty);
	return 0;
}

//BuildShop(x, y, 9, 3, IM_ARMOUR + IM_WEAPON + IM_POTION + IM_BOOK + IM_SCROLL + IM_NECK + IM_MISSILE + IM_MISSILEW, 'Toberin, the dwarwen shopkeeper')
int XLocation::BuildShop(lua_State * L)
{
	int tx = lua_tonumber(L, 1);
	int ty = lua_tonumber(L, 2);
	int tw = lua_tonumber(L, 3);
	int th = lua_tonumber(L, 4);
	XRect shop_rect(tx, ty, tx + tw, ty + th);
	int mask = lua_tonumber(L, 5);
	const char * keeper_name = lua_tostring(L, 6);
	current_location->CreateShop(mask, &shop_rect, (char *)keeper_name, SHOP_BUILD_IN);
	return 0;
}

//Furniture(x, y, xLIGHTRED, '~', 'a royal bad')
int XLocation::Furniture(lua_State * L)
{
	int tx = lua_tonumber(L, 1);
	int ty = lua_tonumber(L, 2);
	int tc = lua_tonumber(L, 3);
	const char * tv = lua_tostring(L, 4);
	const char * subscr = lua_tostring(L, 5);
	XFurniture * p = new XFurniture(tx, ty, tc, tv[0], (char *)subscr, current_location);
	lua_pushlightuserdata(L, p);
	return 1;
}


//Furniture(x, y, xLIGHTRED, '~', 'a royal bad', 'EventHandler')
int XLocation::OuterObject(lua_State * L)
{
	int tx = lua_tonumber(L, 1);
	int ty = lua_tonumber(L, 2);
	int tc = lua_tonumber(L, 3);
	const char * tv = lua_tostring(L, 4);
	const char * subscr = lua_tostring(L, 5);
	const char * event = lua_tostring(L, 6);
	XOuterObject * p = new XOuterObject(tx, ty, tc, tv[0], (char *)subscr, current_location, event);
	lua_pushlightuserdata(L, p);
	return 1;
}


//Altar(x, y, D_LIFE)
int XLocation::Altar(lua_State * L)
{
	int tx = lua_tonumber(L, 1);
	int ty = lua_tonumber(L, 2);
	int deity = lua_tonumber(L, 3);
	new XAltar(tx, ty, (DEITY)deity, current_location);
	return 0;
}

int XLocation::Treasure(lua_State * L)
{
	int tx = lua_tonumber(L, 1);
	int ty = lua_tonumber(L, 2);
	int val = lua_tonumber(L, 3);
	XMoney * money = new XMoney(vRand(val) + val);
	money->Drop(current_location, tx, ty);
	return 0;
}

int XLocation::Chest(lua_State * L)
{
	int tx = lua_tonumber(L, 1);
	int ty = lua_tonumber(L, 2);

	int cnt = 5;
	int flg = IM_ITEM;
	int mnval = 100;
	int mxval = 25000;

	int n = lua_gettop(L);

	if (n > 2)
		cnt = lua_tonumber(L, 3);
	if (n > 3)
		flg = lua_tonumber(L, 4);
	if (n > 4)
	{
		mnval = lua_tonumber(L, 5);
		mxval = lua_tonumber(L, 6);
	}
	XChest * tchest = new XChest(cnt, (ITEM_MASK)flg, mnval, mxval);
	tchest->Drop(current_location, tx, ty);
	return 0;
}

int XLocation::Trap(lua_State * L)
{
	int tx = lua_tonumber(L, 1);
	int ty = lua_tonumber(L, 2);
	new XTrap(tx, ty, current_location);
	return 0;
}


//
int XLocation::EventPlace(lua_State * L)
{
	int n = lua_gettop(L);
	XRect area(0, 0, current_location->map->len, current_location->map->hgt);
	const char * event = NULL;
	if (n > 1)
	{
		area.left = lua_tonumber(L, 1);
		area.top = lua_tonumber(L, 2);
		area.right = area.left + lua_tonumber(L, 3);
		area.bottom = area.top + lua_tonumber(L, 4);
		event = lua_tostring(L, 5);
	} else
	{
		event = lua_tostring(L, 1);
	}
	XAnyPlace * place = new XAnyPlace(&area, current_location, (char *)event);
	return 0;
}


//InflictDamage (target, dmg, RESISTANCE_TYPE, creature ["msg"])
int XLocation::InflictDamage(lua_State * L)
{
	XCreature * p = (XCreature *)lua_topointer(L, 1);
	int dmg = lua_tonumber(L, 2);
	int resist = lua_tonumber(L, 3);
	dmg = p->onMagicDamage(dmg, (RESISTANCE)resist);
	p->_HP -= dmg;
	if (p->_HP < 0)
	{
		if (lua_isstring(L, 4))
		{
			const char * str = lua_tostring(L, 4);
			XFakeCreature * tcr = new XFakeCreature((char *)str);
			p->Die(tcr);
			tcr->Invalidate();
		} else
		{
		}
	}
	return 0;
}


int XLocation::ChangeStats(lua_State * L)
{
	return 0;
}

int XLocation::Rand(lua_State * L)
{
	int val = lua_tonumber(L, 1);
	lua_pushnumber(L, vRand(val));
	return 1;
}


int XLocation::isHero(lua_State * L)
{
	XCreature * p = (XCreature *)lua_topointer(L, 1);
	lua_pushboolean(L, p->isHero());
	return 1;
}

int XLocation::isEnemy(lua_State * L)
{
	XCreature * p1 = (XCreature *)lua_topointer(L, 1);
	XCreature * p2 = (XCreature *)lua_topointer(L, 2);
	if (p1 && p2)
		lua_pushboolean(L, p1->xai->isEnemy(p2));
	else
		lua_pushboolean(L, false);
	return 1;
}

int XLocation::FindCreature(lua_State * L)
{
	int l_id = lua_tonumber(L, 1);
	int gid = lua_tonumber(L, 2);
	int n = lua_gettop(L);
	
	XRect rect(0, 0,  Game.locations[l_id]->map->len, Game.locations[l_id]->map->hgt);
	if (n == 6)
	{
		rect.left = lua_tonumber(L, 3);
		rect.top = lua_tonumber(L, 4);
		rect.right = rect.left + lua_tonumber(L, 5);
		rect.bottom = rect.top + lua_tonumber(L, 6);
	}
	for (int i = rect.left; i < rect.right; i++)
		for (int j = rect.top; j < rect.bottom; j++)
		{
			XCreature * cr = Game.locations[l_id]->map->GetMonster(i, j);
			if (cr && cr->group_id == gid)
			{
				lua_pushlightuserdata(L, cr);
				return 1;
			}
		}
	lua_pushlightuserdata(L, NULL);
	return 1;
}


int XLocation::SetItEnemyFor(lua_State * L)
{
	XCreature * p1 = (XCreature *)lua_topointer(L, 1);
	if (lua_islightuserdata(L, 2))
	{
		XCreature * p2 = (XCreature *)lua_topointer(L, 2);
		if (p2 && p1)
		{
			p2->xai->AddPersonalEnemy(p1);
			p2->xai->SetGroupEnemy(p1);
		}
	} else
	{
//		int gid = lua_tonumber(L, 2);
//		assert(0);
	}

	return 0;
}


int XLocation::AddMessage(lua_State * L)
{
	const char * str = lua_tostring(L, 1);
	msgwin.Add(str);
	return 0;
}



XFile * XLocation::svg_file = NULL;
int XLocation::StoreInt(lua_State * L)
{
	int tx = lua_tonumber(L, 1);
	svg_file->Write(&tx);
	return 0;
}

int XLocation::RestoreInt(lua_State * L)
{
	int tx = 0;
	svg_file->Read(&tx);
	lua_pushnumber(L, tx);
	return 1;
}

#define LUA_REG(x) { char buf[256]; sprintf(buf, #x "=%d", x); lua_dostring(L, buf); }

lua_State * XLocation::L = NULL;



void XLocation::CommonLuaInitialization()
{
	L = lua_open();

	LUA_REG(L_MAIN);

	LUA_REG(L_MUSHROOMS_CAVE1);
	LUA_REG(L_MUSHROOMS_CAVE2);
	LUA_REG(L_MUSHROOMS_CAVE3);
	LUA_REG(L_MUSHROOMS_CAVE4);
	LUA_REG(L_MUSHROOMS_CAVE5);

	LUA_REG(L_DWARFCITYCAVE1);
	LUA_REG(L_DWARFCITYCAVE2);
	LUA_REG(L_DWARFCITYCAVE3);
	LUA_REG(L_DWARFCITYCAVE4);
	LUA_REG(L_DWARFCITYCAVE5);
	LUA_REG(L_DWARFCITYCAVE6);
	LUA_REG(L_DWARFCITY);
	LUA_REG(L_DWARFTREASURE);
	LUA_REG(L_GASMINE1);
	LUA_REG(L_GASMINE2);
	LUA_REG(L_GASMINE3);
	

	LUA_REG(CR_RAT);
	LUA_REG(CR_FELINE);
	LUA_REG(CR_CANINE);
	LUA_REG(CR_REPTILE);
	LUA_REG(CR_INSECT);
	LUA_REG(CR_HUMAN);
	LUA_REG(CR_ORC);
	LUA_REG(CR_GIANT);
	LUA_REG(CR_KOBOLD);
	LUA_REG(CR_UNDEAD);
	LUA_REG(CR_GOBLIN);
	LUA_REG(CR_DEMON);
	LUA_REG(CR_HUMANOID);
	LUA_REG(CR_BLOB);

	LUA_REG(CRL_VERY_LOW);
	LUA_REG(CRL_LOW);
	LUA_REG(CRL_AVG);
	LUA_REG(CRL_ABOVE_AVG);
	LUA_REG(CRL_HI);
	LUA_REG(CRL_ABOVE_HI);
	LUA_REG(CRL_VERY_HI);
	LUA_REG(CRL_EXTREM_HI);
	LUA_REG(CRL_UNIQUE);
	LUA_REG(CRL_ANY);
	LUA_REG(CRL_VL);
	LUA_REG(CRL_LA);
	LUA_REG(CRL_AH);
	LUA_REG(CRL_HVH);

	
	LUA_REG(CN_DWARF);
	LUA_REG(CN_DWARF_GUARD);
	LUA_REG(CN_TORIN);
	LUA_REG(CN_TODIN);

	LUA_REG(CN_BEELZEVILE);
	LUA_REG(CN_MAGNUSH);
	LUA_REG(CN_ROTMOTH);
	LUA_REG(CN_GIANA);

	LUA_REG(M_GREENGRAS);
	LUA_REG(M_GREENTREE);
	LUA_REG(M_SAND);
	LUA_REG(M_WINDOW);
	LUA_REG(M_MAGMA);
	LUA_REG(M_QUARTZ);
	LUA_REG(M_CAVEFLOOR);
	LUA_REG(M_STONEFLOOR);
	LUA_REG(M_PATH);
	LUA_REG(M_WOODWALL);
	LUA_REG(M_STONEWALL);
	LUA_REG(M_WATER);
	LUA_REG(M_DEEPWATER);
	LUA_REG(M_LAVA);
	LUA_REG(M_HILL);
	LUA_REG(M_LOWMOUNTAIN);
	LUA_REG(M_MOUNTAIN);
	LUA_REG(M_HIGHMOUNTAIN);
	LUA_REG(M_BRIDGE);
	LUA_REG(M_ROAD);
	LUA_REG(M_OBSIDIANFLOOR);
	LUA_REG(M_FENCE);
	LUA_REG(M_GOLDENFLOOR);
	LUA_REG(M_MARBLEWALL);
	LUA_REG(M_BLACKMARBLEWALL);
	LUA_REG(M_GOLDENFENCE);
	LUA_REG(M_TELEPORTWHITE);
	


	LUA_REG(GID_ORCS_WARPARTY);
	LUA_REG(GID_FOREST_BROTHER);
	LUA_REG(GID_GUARDIAN);
	LUA_REG(GID_SMALL_VILLAGE_FARMER);
	LUA_REG(GID_TOWNEE_1);
	LUA_REG(GID_DWARVEN_GUARDIAN);
	LUA_REG(GID_AHKULAN_GUARDIAN);
	LUA_REG(GID_RODERICK_GUARDIAN);


	LUA_REG(IM_HAT);
	LUA_REG(IM_NECK);
	LUA_REG(IM_BODY);
	LUA_REG(IM_CLOAK);
	LUA_REG(IM_WEAPON);
	LUA_REG(IM_SHIELD);
	LUA_REG(IM_HAND);
	LUA_REG(IM_GLOVES);
	LUA_REG(IM_RING);
	LUA_REG(IM_BOOTS);
	LUA_REG(IM_MISSILEW);
	LUA_REG(IM_MISSILE);
	LUA_REG(IM_POTION);
	LUA_REG(IM_SCROLL);
	LUA_REG(IM_BOOK);
	LUA_REG(IM_WAND);
	LUA_REG(IM_FOOD);
	LUA_REG(IM_HERB);
	LUA_REG(IM_LIGHTSOURCE);
	LUA_REG(IM_TOOL);
	LUA_REG(IM_GEM);
	LUA_REG(IM_MONEY);
	LUA_REG(IM_STACKABLE);
	LUA_REG(IM_CHEST);
	LUA_REG(IM_ARMOUR);
	LUA_REG(IM_ITEM);


	LUA_REG(R_WHITE);
	LUA_REG(R_BLACK);
	LUA_REG(R_FIRE);
	LUA_REG(R_WATER);
	LUA_REG(R_AIR);
	LUA_REG(R_EARTH);
	LUA_REG(R_ACID);
	LUA_REG(R_COLD);
	LUA_REG(R_POISON);
	LUA_REG(R_DISEASE);
	LUA_REG(R_PARALYSE);
	LUA_REG(R_STUN);
	LUA_REG(R_CONFUSE);
	LUA_REG(R_BLIND);
	LUA_REG(R_LIGHT);
	LUA_REG(R_DARKNESS);
	LUA_REG(R_INVISIBLE);
	LUA_REG(R_SEEINVISIBLE);


	LUA_REG(xBLACK);
	LUA_REG(xBLUE); 
	LUA_REG(xGREEN); 
	LUA_REG(xCYAN); 
	LUA_REG(xRED); 
	LUA_REG(xMAGENTA); 
	LUA_REG(xBROWN); 
	LUA_REG(xLIGHTGRAY); 
	LUA_REG(xDARKGRAY); 
	LUA_REG(xLIGHTBLUE); 
	LUA_REG(xLIGHTGREEN); 
	LUA_REG(xLIGHTCYAN); 
	LUA_REG(xLIGHTRED); 
	LUA_REG(xLIGHTMAGENTA); 
	LUA_REG(xYELLOW); 
	LUA_REG(xWHITE);

	LUA_REG(D_LIFE);
	LUA_REG(D_DEATH);

	LUA_REG(LE_MOVE);
	LUA_REG(LE_MOVE_IN);
	LUA_REG(LE_MOVE_OUT);
	LUA_REG(LE_OUTER_USE);
	LUA_REG(LE_SAVE);
	LUA_REG(LE_LOAD);


	LUA_REG(Q_UNKNOWN);
	LUA_REG(Q_KNOWN);
	LUA_REG(Q_COMPLETE);
	LUA_REG(Q_CLOSED);

	lua_register(L, "CreateLocation", CreateLocation);
	lua_register(L, "Way", Way);
	lua_register(L, "Creature", Creature);
	lua_register(L, "Guardian", Guardian);
	lua_register(L, "Settle", Settle);
	lua_register(L, "CreateObject", CreateObject);
	lua_register(L, "DropItem", DropItem);
	lua_register(L, "SetPattern", SetPattern);
	lua_register(L, "AddTranslation", AddTranslation);
	lua_register(L, "DrawPattern", DrawPattern);
	lua_register(L, "BuildShop", BuildShop);
	lua_register(L, "Furniture", Furniture);
	lua_register(L, "OuterObject", OuterObject);
	lua_register(L, "Altar", Altar);
	lua_register(L, "Trap", Trap);
	lua_register(L, "Chest", Chest);
	lua_register(L, "Treasure", Treasure);
	lua_register(L, "EventPlace", EventPlace);
	lua_register(L, "isHero", isHero);
	lua_register(L, "isEnemy", isEnemy);
	lua_register(L, "FindCreature", FindCreature);
	lua_register(L, "AddMessage", AddMessage);
	lua_register(L, "SetItEnemyFor", SetItEnemyFor);
	lua_register(L, "ChangeStats", ChangeStats);
	lua_register(L, "InflictDamage", InflictDamage);
	lua_register(L, "Rand", Rand);
	

	lua_register(L, "StoreInt", StoreInt);
	lua_register(L, "RestoreInt", RestoreInt);
	
	luaopen_base(L);
	lua_dofile(L, "location.lua");

}

void XLocation::Restoration()
{
	CommonLuaInitialization();
}

void XLocation::CreateNewGame()
{
	CommonLuaInitialization();
	lua_dostring(L, "MakeDwarvenCity()");
}

