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

#ifndef __LOCATION_H
#define __LOCATION_H

#include "point.h"
#include "map.h"
#include "xanyplace.h"
#include "cr_defs.h"

enum SHOP_DOOR 
{
	SHOP_DOOR_UP,
	SHOP_DOOR_LEFT,
	SHOP_DOOR_DOWN,
	SHOP_DOOR_RIGHT,
	SHOP_BUILD_IN,
};

enum LOCATION {
L_UNKNOWN = 0,
L_MAIN				= 1,
L_DWARFCITYCAVE1	= 2,
L_DWARFCITYCAVE2	= 3,
L_DWARFCITYCAVE3	= 4,
L_DWARFCITYCAVE4	= 5,
L_DWARFCITYCAVE5	= 6,
L_DWARFCITYCAVE6	= 7,
L_DWARFCITYCAVE7	= 8,
L_DWARFCITY			= 9,
L_DWARFTREASURE		= 10,

L_GASMINE1			= 15,
L_GASMINE2			= 16,
L_GASMINE3			= 17,

L_SMALLCAVE			= 18, 
L_RATCELLAR			= 19, 

L_MUSHROOMS_CAVE1	= 20, //first
L_MUSHROOMS_CAVE2	= 21, //demon
L_MUSHROOMS_CAVE3	= 22, //misc
L_MUSHROOMS_CAVE4	= 23, //kobolds
L_MUSHROOMS_CAVE5	= 24, //mushrooms

L_WIZARD_DUNGEON1	= 30,
L_WIZARD_DUNGEON2	= 31,
L_WIZARD_DUNGEON3	= 32,
L_WIZARD_DUNGEON4	= 33,
L_WIZARD_DUNGEON5	= 34,
L_AHKULAN_CASTLE	= 35,

L_UNDEADS_TOMB1		= 40,
L_UNDEADS_TOMB2		= 41,
L_UNDEADS_TOMB3		= 42,
L_UNDEADS_TOMB4		= 43,
L_UNDEADS_TOMB5		= 44,

L_EXTINCT_VOLCANO	= 45,

L_KINGS_TREASURE	= 46,

L_WIZTOWER_TOP		= 50,
L_SMALL_CAVE_1		= 55,
L_SMALL_CAVE_2		= 56,

L_DEBUG1			= 90,
L_DEBUG2			= 91,
L_RANDOM = 100,
L_EOF = 200,
};


enum LUA_EVENT
{
	LE_MOVE				= 1,
	LE_MOVE_IN			= 2,
	LE_MOVE_OUT			= 3,
	LE_OUTER_USE		= 4,
	LE_CHAT				= 10,
	LE_GIVE_ITEM		= 11,
	LE_EVENT_SET		= 97,
	LE_SAVE				= 98,
	LE_LOAD				= 99
};

enum PALETTE 
{
	PAL_UNKNOWN			= 0x0000,
	PAL_SMALL_TOWN		= 0x0001,
	PAL_SMALL_VILLAGE	= 0x0002,
	PAL_CITY			= 0x0003,
	PAL_DWARF_CITY		= 0x0004,
	PAL_WIZARD_TOWER	= 0x0005,
	PAL_AHKULAN_CASTLE	= 0x0006,
	PAL_RAT_CELLAR		= 0x0007,
	PAL_TOWER_RUINS		= 0x0008,
	PAL_DWARF_TREASURE	= 0x0009,
	PAL_KINGS_TREASURE	= 0x000A,
	PAL_WIZTOWER_TOP,
	PAL_UNDEAD_TOMB0,
	PAL_UNDEAD_TOMB1,
	PAL_EXTINCT_VOLCANO
};

enum STDMAP;
struct PALETTE_MAP
{
	char this_view;
	STDMAP	real_view;
	char lua_str[512];
};

struct LOCATION_PATTERN
{
	char * pattern;
	int w;
	int h;
};

#define MAX_PLACES 8

class XMap;
class XStairWay;


struct lua_State;

class XLocation : public XObject
{
public:
	bool way_found_flag; //used for recursive way found alg...

	XQList<XObject*> ways_list; //ways list used for AI...
	LOCATION ln;

	static int rand_location_count;
	DECLARE_CREATOR(XLocation, XObject);
	XLocation(LOCATION location);
	XLocation(XLocation * copy) {assert(0);}
	XLocation() {assert(0);}

	XMap * map;
	void GetFreeXY(XPoint * pt, XRect * area = NULL);
	void AddPlace(XAnyPlace * pl);

	virtual void Store(XFile * f);
	virtual void Restore(XFile * f);
	virtual void Invalidate();
	char * GetBriefName() {return brief_name;}
	char * GetFullName() {return full_name;}

	int visited_by_hero;
	void DumpLocation(FILE * f);
	
	int GetCreatureCount(unsigned int creature_class); //count of such creatures on this level (need for quests)

	static void CreateRandomCave();

	XCreature * NewCreature(CREATURE_NAME cn, int x, int y, GROUP_ID gid = GID_NONE);
	XCreature * NewCreature(CREATURE_NAME cn, XRect * rect = NULL, GROUP_ID gid = GID_NONE, unsigned int ai_flags = 0);
	XCreature * NewCreature(CREATURE_CLASS crc, XRect * rect = NULL, GROUP_ID gid = GID_NONE, unsigned int ai_flags = 0);

	XStairWay * NewWay(LOCATION target_ln, STAIRWAYTYPE s_type, XRect * area = NULL); //creates way at random place
	XStairWay * NewWay(int x, int y, LOCATION target_ln, STAIRWAYTYPE s_type);

	
	//Location Script Language
	static const char * GetToken(const char * line, const char ** token, int * token_len);
	static const char * GetNumber(const char * line, int * num);
	static const char * GetString(const char * line, char * buf);

	static void CreateNewGame();
	static void Restoration();
	static void CommonLuaInitialization();
	static XLocation * current_location;
	static XCreature * last_creature;
	static lua_State * L;
	static LOCATION_PATTERN current_pattern;
	static XQList<PALETTE_MAP> pattern_translation;
	static int pat_offs_x;
	static int pat_offs_y;

	static int CreateLocation(lua_State * L);
	static int Settle(lua_State * L);
	
	static int Creature(lua_State * L);
	static int Guardian(lua_State * L);

	static int Way(lua_State * L);
	static int CreateObject(lua_State * L);
	static int DropItem(lua_State * L);
	static int SetPattern(lua_State * L);
	static int AddTranslation(lua_State * L);
	static int DrawPattern(lua_State * L);
	static int BuildShop(lua_State * L);
	static int Furniture(lua_State * L);
	static int OuterObject(lua_State * L);
	static int Altar(lua_State * L);
	static int Treasure(lua_State * L);
	static int Chest(lua_State * L);
	static int Trap(lua_State * L);
	static int EventPlace(lua_State * L);
	static int InflictDamage(lua_State * L);
	static int ChangeStats(lua_State * L);
	static int GetStats(lua_State * L);
	static int Rand(lua_State * L);
	static int SetEventHandler(lua_State * L);

	static int SetName(lua_State * L);
	static int SetView(lua_State * L);
	static int GetView(lua_State * L);


	static int isHero(lua_State * L);
	static int isEnemy(lua_State * L);
	static int SetItEnemyFor(lua_State * L);
	static int SetEnemy(lua_State * L);
	static int FindCreature(lua_State * L);
	static int AddMessage(lua_State * L);

	static int GetObjectGUID(lua_State * L);
	static int GiveObjectToCreature(lua_State * L);
	static int GiveAward(lua_State * L);


	static XFile * svg_file;
	static int StoreInt(lua_State * L);
	static int RestoreInt(lua_State * L);
	static int StoreObject(lua_State * L);
	static int RestoreObject(lua_State * L);
	
	
	
protected:
	char brief_name[10];
	char full_name[80];
	XPtr<XAnyPlace> places[MAX_PLACES];

	void PutPalette(int x, int y, PALETTE pal, XLocation * l);
	void PutPalette(int x, int y);
	char ** Resolve(PALETTE pal, int * size, PALETTE_MAP ** pm, int * pm_size);

	void BuildCave();
	void BuildLabirint(int create_trap_door_chest = 1);
	void BuildPlain(int w, int h);
	void CreateTraps();
	void CreateChests();

	void CreateShop(unsigned int im, XRect * rect, char * sk_name, SHOP_DOOR sd = SHOP_DOOR_UP);
};

class XRandomLocation : public XLocation
{
public:
	XRandomLocation(int deep, int view, int way_up, int way_down, int cr_lvl); //view 0 - labirinth, 1 - cave
};




////////////// ALL OTHER LOCATIONS /////////////////////

class XUndeadTombLocation : public XLocation
{
public:
	XUndeadTombLocation(LOCATION tl);
};


class XRatCellarLocation : public XLocation
{
public:
	XRatCellarLocation(LOCATION tl);
};


class XMushroomsCaveLocation : public XLocation
{
public:
	DECLARE_CREATOR(XMushroomsCaveLocation, XLocation);
	XMushroomsCaveLocation(LOCATION loc);
	XMushroomsCaveLocation() {assert(0);}
	int Run();
};

class XWizardDungeonLocation : public XLocation
{
public:
	XWizardDungeonLocation(LOCATION l_name);
};

class XAhkUlanCastleLocation : public XLocation
{
public:
	XAhkUlanCastleLocation(LOCATION tl);
};

class XDwarfCityCaveLocation : public XLocation
{
public:
	XDwarfCityCaveLocation(LOCATION l_name);
};

class XMainLocation : public XLocation
{
public:
	XMainLocation(LOCATION tl);
};

class XYohjiTower : public XLocation
{
public:
	XYohjiTower(LOCATION tl);
};

class XKingsTreasureLocation : public XLocation
{
public:
	XKingsTreasureLocation(LOCATION tl);
};

class XExtinctVolcanoLocation : public XLocation
{
public:
	XExtinctVolcanoLocation(LOCATION tl);
};


#endif
