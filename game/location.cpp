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


//Location Language
//see description in location.txt

XLocation * XLocation::current_location = NULL;
XCreature * XLocation::last_creature = NULL;
int XLocation::current_script_line = 0;
XQList<CONSTANT_REGISTER> XLocation::constants;

#define AVANOR_LOG(x) \
{ FILE * ff = fopen("avanor.log", "at"); fprintf(ff, "ERROR LINE [%d]:", current_script_line); fprintf(ff, "%s\n", x); fclose(ff); }


const char * XLocation::GetToken(const char * line, const char ** token, int * token_len) //returns ptr just behinde the token
{
	int pos = 0;
	while (line[pos] > 0 && line[pos] < 33) pos++;
	if (line[pos] == '"')
	{
		pos++;
		int token_begin = pos;
		*token = line + pos;
		while (line[pos] > 32 && line[pos] != '"') pos++;
		*token_len = pos - token_begin;
		return line + pos;
	} else
	{
		int token_begin = pos;
		*token = line + pos;
		while (line[pos] > 32) pos++;
		*token_len = pos - token_begin;
		return line + pos;
	}
}

const char * XLocation::GetNumber(const char * line, int * num)
{
	const char * token;
	int token_sz;
	const char * res = GetToken(line, &token, &token_sz);
	if (isdigit(token[0]))
		*num = atoi(token);
	else if (token_sz > 0)
	{
		for (XQList<CONSTANT_REGISTER>::iterator it = constants.begin(); it != constants.end(); it++)
		{
			if (strncmp(token, (*it).constant, token_sz) == 0)
			{
				*num = (*it).val;
				return res;
			}
		}
		AVANOR_LOG("Identifer not found.");
	} else
		*num = 0;
	return res;
}

const char * XLocation::GetString(const char * line, char * buf)
{
	const char * token;
	int token_sz;
	const char * res = GetToken(line, &token, &token_sz);
	strncpy(buf, token, token_sz);
	buf[token_sz] = 0;
	return res;
}

bool XLocation::ParseLine(const char * line)
{
	if (line[0] == ';' || line[0] == '\n' || line[0] == '\r')
		return true;

	const char * token;
	int token_sz;
	line = GetToken(line, &token, &token_sz);
	if (strncmp("BEGIN", token, 5) == 0)
	{
		if (current_location)
			AVANOR_LOG("BEGIN before END.");
		int location_number;
		line = GetNumber(line, &location_number);
		current_location = new XLocation((LOCATION)location_number);
		return true;
	} else
	if (strncmp("END", token, 3) == 0)
	{
		if (!current_location)
			AVANOR_LOG("END before BEGIN.");
		current_location = NULL;
		return true;
	} else
	if (strncmp("DESCR", token, 5) == 0)
	{
		line = GetString(line, current_location->brief_name);
		line = GetString(line, current_location->full_name);
		return true;
	} else
	if (strncmp("BUILD", token, 5) == 0)
	{
		const char * ts;
		int txsz;
		line = GetToken(line, &ts, &txsz);
		int tx;
		int ty;
		line = GetNumber(line, &tx);
		line = GetNumber(line, &ty);
		if (tx == 0 || ty == 0)
		{
			tx = 80;
			ty = 20;
		}
		current_location->map = new XMap(tx, ty);
		if (strncmp("CAVE", ts, 4) == 0)
		{
			current_location->BuildCave();
			return true;
		} else
		if (strncmp("DUNGEON", ts, 7) == 0)
		{
			current_location->BuildLabirint();
			return true;
		} else
			return false;
	} else
	if (strncmp("WAY", token, 3) == 0)
	{
		const char * ts;
		int txsz;
		line = GetToken(line, &ts, &txsz);

		STAIRWAYTYPE stw_type = STW_UP;
		if (strncmp("DOWN", ts, 4) == 0)
			stw_type = STW_DOWN;
		
		int val;
		int tx;
		int ty;
		line = GetNumber(line, &val);
		line = GetNumber(line, &tx);
		line = GetNumber(line, &ty);
		if (tx == 0 || ty == 0)
			current_location->NewWay((LOCATION)val, stw_type);
		else
			current_location->NewWay(tx, ty, (LOCATION)val, stw_type);
		return true;
	}else if (strncmp("SETTLE", token, 6) == 0)
	{
		int crc = 0;
		int crl = 0;
		int tmp = 0;
		line = GetNumber(line, &crl);
		do 
		{
			line = GetNumber(line, &tmp);
			crc |= tmp;
		} while (tmp > 0);
		Game.Scheduler.Add(new XUniversalGen(current_location, (CREATURE_CLASS)(crc), (CREATURE_LEVEL)(crl), 5, 25000));
		return true;
	} else if (strncmp("SET", token, 3) == 0)
	{
		char buf[256];
		line = GetString(line, buf);
		int val;
		line = GetNumber(line, &val);
		ConstantRegister(buf, val);
		return true;
	} else if (strncmp("CREATURE", token, 8) == 0)
	{
		int crn;
		line = GetNumber(line, &crn);
		int tx;
		int ty;
		line = GetNumber(line, &tx);
		line = GetNumber(line, &ty);
		if (tx == 0 || ty == 0)
			last_creature = current_location->NewCreature((CREATURE_NAME)crn);
		else
			last_creature = current_location->NewCreature((CREATURE_NAME)crn, tx, ty);
		return true;
	}

	return false;
}

extern "C"
{
#include "./lua/include/lauxlib.h"
}

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

int XLocation::Settle(lua_State * L)
{
	int crc = lua_tonumber(L, 1);
	int crl = lua_tonumber(L, 2);
	Game.Scheduler.Add(new XUniversalGen(current_location, (CREATURE_CLASS)(crc), (CREATURE_LEVEL)(crl), 5, 25000));
	return 0;
}


int XLocation::Creature(lua_State * L)
{
	int crn = lua_tonumber(L, 1);
	current_location->NewCreature((CREATURE_NAME)crn);
	return 0;
}

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


#define LUA_REG(x) { char buf[256]; sprintf(buf, #x "=%d", x); lua_dostring(L, buf); }

void XLocation::CreateFromFile(char * file_name)
{

	ConstantRegister("L_MAIN", L_MAIN);

	ConstantRegister("L_MUSHROOMS_CAVE1", L_MUSHROOMS_CAVE1);
	ConstantRegister("L_MUSHROOMS_CAVE2", L_MUSHROOMS_CAVE2);
	ConstantRegister("L_MUSHROOMS_CAVE3", L_MUSHROOMS_CAVE3);
	ConstantRegister("L_MUSHROOMS_CAVE4", L_MUSHROOMS_CAVE4);
	ConstantRegister("L_MUSHROOMS_CAVE5", L_MUSHROOMS_CAVE5);

	ConstantRegister("CR_RAT", CR_RAT);
	ConstantRegister("CR_FELINE", CR_FELINE);
	ConstantRegister("CR_CANINE", CR_CANINE);
	ConstantRegister("CR_REPTILE", CR_REPTILE);
	ConstantRegister("CR_INSECT", CR_INSECT);
	ConstantRegister("CR_HUMAN", CR_HUMAN);
	ConstantRegister("CR_ORC", CR_ORC);
	ConstantRegister("CR_GIANT", CR_GIANT);
	ConstantRegister("CR_KOBOLD", CR_KOBOLD);
	ConstantRegister("CR_UNDEAD", CR_UNDEAD);
	ConstantRegister("CR_GOBLIN", CR_GOBLIN);
	ConstantRegister("CR_DEMON", CR_DEMON);
	ConstantRegister("CR_HUMANOID", CR_HUMANOID);
	ConstantRegister("CR_BLOB", CR_BLOB);

	ConstantRegister("CRL_VERY_LOW", CRL_VERY_LOW);
	ConstantRegister("CRL_LOW", CRL_LOW);
	ConstantRegister("CRL_AVG", CRL_AVG);
	ConstantRegister("CRL_ABOVE_AVG", CRL_ABOVE_AVG);
	ConstantRegister("CRL_HI", CRL_HI);
	ConstantRegister("CRL_ABOVE_HI", CRL_ABOVE_HI);
	ConstantRegister("CRL_VERY_HI", CRL_VERY_HI);
	ConstantRegister("CRL_EXTREM_HI", CRL_EXTREM_HI);
	ConstantRegister("CRL_UNIQUE", CRL_UNIQUE);
	ConstantRegister("CRL_ANY", CRL_ANY);
	ConstantRegister("CRL_VL", CRL_VL);
	ConstantRegister("CRL_LA", CRL_LA);
	ConstantRegister("CRL_AH", CRL_AH);
	ConstantRegister("CRL_HVH", CRL_HVH);


	ConstantRegister("CN_BEELZEVILE", CN_BEELZEVILE);
	ConstantRegister("CN_MAGNUSH", CN_MAGNUSH);
	ConstantRegister("CN_ROTMOTH", CN_ROTMOTH);
	ConstantRegister("CN_GIANA", CN_GIANA);

/*	FILE * f = fopen(file_name, "rt");
	char buf[8192];
	current_script_line = 1;
	while (fgets(buf, 8192, f))
	{
		if (!ParseLine(buf))
		{
			AVANOR_LOG("Something wrong...");
		}
		current_script_line++;
	}
	fclose(f);*/


	lua_State* L;
	L = lua_open();

	lua_register(L, "CreateLocation", CreateLocation);
	lua_register(L, "Way", Way);
	lua_register(L, "Creature", Creature);
	lua_register(L, "Settle", Settle);

//	LUA_REG(L_MAIN);
//	LUA_REG(CN_ROTMOTH);
	for (XQList<CONSTANT_REGISTER>::iterator it = constants.begin(); it != constants.end(); it++)
	{
		char buf[256];
		sprintf(buf, "%s=%d", (*it).constant, (*it).val);
		lua_dostring(L, buf);
//		LUA_REG((*it))
	}
	
	lua_dofile(L, "location.lua");
	lua_close(L);
}

void XLocation::ConstantRegister(char * cnst, int val)
{
	for (XQList<CONSTANT_REGISTER>::iterator it = constants.begin(); it != constants.end(); it++)
	{
		if (strcmp(cnst, (*it).constant) == 0)
		{
			(int)((*it).val) = val;
			return;
		}
	}

	CONSTANT_REGISTER rec;
	strcpy(rec.constant, cnst);
	rec.val = val;
	constants.push_back(rec);
}

