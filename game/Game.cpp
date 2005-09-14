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

#include <time.h>

#include "xdebug.h"
#include "game.h"
#include "xtime.h"
#include "ldebug.h"
#include "dwarfcity.h"
#include "quest.h"
#include "uniquei.h"
#include "xshedule.h"
#include "setting.h"
#include "xtool.h"

#ifndef XLINUX
#include <conio.h>
#endif


char static_buffer[4096];


int XGame::best_cr_level = 0;
XPtr<XCreature> XGame::best_creature;

bool XGame::isAvgPV = true;
bool XGame::isGodMode = false;

XGUID XGame::hero_guid = 0;
int XGame::current_location = 0;

XGame::XGame()
{
	for (int i = 0; i < L_EOF; i++)	locations[i] = NULL;
}

XGame::~XGame()
{
	for (int i = 0; i < L_EOF; i++) 
	{
		if (locations[i]) 
		{
			locations[i]->Invalidate();
			locations[i] = NULL;
		}
	}
}

XCreature * XGame::NewCreature(XCreature * cr, int x, int y, XLocation * loc)
{
	cr->FirstStep(x, y, loc);
	Scheduler.Add(cr);
	return cr;
}

//////////////////////////////////////////////////////////////////////////////
// Create all the necessery objects in memory depending on user's choice    //
//////////////////////////////////////////////////////////////////////////////

void XGame::Create(char type_of_start)
{
	switch (type_of_start)
	{
		case 'R' :
        	vClrScr();
        	vGotoXY((size_x - strlen("Restoring game objects, please wait...")) / 2, size_y / 2);
        	vPutS(MSG_LIGHTGRAY "Restoring game objects, please wait...");
        	vRefresh();
			XLocation::Restoration();
			if (XArchive::RestoreGame())
				break;
			vGotoXY(0, 20);
			vPutS(MSG_YELLOW "There is not a saved game to load. Starting new game.");
			vGotoXY(0, 21);
			vPutS(MSG_CYAN "Press any key...");
			vRefresh();
			vGetch();

		case 'N' :
			XAlchemy::Init();
			_HERBS::Create();
        	vClrScr();
        	vGotoXY((size_x - strlen("Generating game objects, please wait...")) / 2, size_y / 2);
        	vPutS(MSG_LIGHTGRAY "Generating game objects, please wait...");
        	vRefresh();
			CreateLocations();
			CreateHero();
			break;
		case 'T' :
			XAlchemy::Init();
			_HERBS::Create();
        	vClrScr();
        	vGotoXY((size_x - strlen("Preparing for test, please wait...")) / 2, size_y / 2);
        	vPutS(MSG_LIGHTGRAY "Preparing for test, please wait...");
        	vRefresh();

			CreateLocations();
			break;
		case 'D' :
			XAlchemy::Init();
			_HERBS::Create();
        	vClrScr();
        	vGotoXY((size_x - strlen("Preparing for demo, please wait...")) / 2, size_y / 2);
        	vPutS(MSG_LIGHTGRAY "Preparing for demo, please wait...");
        	vRefresh();
			XSettings::isDemo = 1;

			CreateLocations();
			break;

	}
}


void XGame::RunDemo()
{
	int mode = 0;
	while (true)
	{
		for (int i = 0; i < 100; i++)
			Game.Scheduler.Get()->Run();

		if (vKbhit())
		{
			int ch = vGetch();
			if (ch == KEY_ESC)
				break;
			if (ch == ' ')
			{
				vGetch();
			}
			if (ch == 'M')
			{
				msgwin.ShowHistory();
			}

		}
	}

	XObject::InvalidateAllObjects();
}

//////////////////////////////////////////////////////////////////////////////
// Run the game without hero for detecting bugs or benchmarking             //
//////////////////////////////////////////////////////////////////////////////

void XGame::RunWithoutHero()
{
	clock_t start_clock = clock();

	while (true)
	{
		for (int i = 0; i < 1000; i++)
		{
			XObject * o = Game.Scheduler.Get();
			o->Run();
		}
		if (vKbhit())
		{
			int ch = vGetch();
			if (ch == KEY_ESC) break;
			if (ch == 'D')
				XObject::DumpAll();

			if (ch == 'L')
			{
				FILE * f = fopen(vMakePath(HOME_DIR, "location.txt"), "w");
				for (int i = 0; i < L_EOF; i++) 
					if (locations[i]) 
						locations[i]->DumpLocation(f);
				fclose(f);
			}
			if (ch == 'I')
			{
				XObject * pItem = XObject::root;
				int count = 0;
				while (pItem)
				{
					if (pItem && pItem->im & IM_ITEM) count++;
					pItem = pItem->next;
				}
				struct TMP {XItem * pI; int val;};
				TMP * ia = new TMP[count];

				pItem = XObject::root;
				int index = 0;
				while (pItem)
				{
					if (pItem && pItem->im & IM_ITEM)
					{
						ia[index].pI = (XItem *)pItem;
						ia[index].val = ia[index].pI->GetValue();
						index++;
					}
					pItem = pItem->next;
				}

				//sort it by value;
				for (int i = 0; i < count; i++)
				{
					for (int j = 0; j < count - 1; j++)
					{
						if (ia[j].val < ia[j + 1].val)
						{
							TMP tmp = ia[j + 1];
							ia[j + 1] = ia[j];
							ia[j] = tmp;
						}
					}
				}
				//dump it
				FILE * f = fopen(vMakePath(HOME_DIR, "items.txt"), "wt");
				for (int k = 0; k < count; k++)
				{
					if (ia[k].pI->im & (IM_VALUEDICE | IM_ARMOUR))
					{
						char buf[256];
						char buf2[256];
						ia[k].pI->Identify(1);
						ia[k].pI->toString(buf);
						sprintf(buf2, "%-70s%d", buf, ia[k].val);
						fprintf(f, "%s\n", buf2);
					}
				}
				fclose(f);
			}
		}
		vClrScr();
		vGotoXY(0, 0);
		char tname[256] = "";
		if (best_creature)
			sprintf(tname, "%s", best_creature->name);
		sprintf(static_buffer, 
			MSG_YELLOW "Testing Avanor - running game without hero ... (press ESC to stop)\n\n"
			MSG_LIGHTGRAY
			"Number of valid objects   : %d\n"
			"Number of invalid objects : %d\n"
			"\n"
			"Number of creatures       : %d\n"
			"Number of items           : %d\n"
			"\n"
            "Best creature             : %s [%d]\n"
			"\n"
			"Turns                     : %d\n"
			"Performance               : %.1lf turns/s",
			XObject::count, 
			XObject::invalid_count,
			total_cr,
			total_it,
            tname, best_cr_level,
            Game.Scheduler.GetTime() / 1000,
			(double)Game.Scheduler.GetTime() * CLOCKS_PER_SEC / (1000. * (clock() - start_clock)));
		vPutS(static_buffer);
		vRefresh();

/*		if (Game.Scheduler.GetTime() / 1000 > 100)
			break;*/
	}

	XObject::InvalidateAllObjects();
}


//////////////////////////////////////////////////////////////////////////////
// Run the game in ordinary mode                                            //
//////////////////////////////////////////////////////////////////////////////

void XGame::Run()
{
	vHideCursor();

	while (!_exit_flag && XQuest::quest.hero_win == 0 && XQuest::quest.hero_die == 0)
		Game.Scheduler.Get()->Run();

	XObject::InvalidateAllObjects();

	vClrScr();
	if (XQuest::quest.hero_win == 1)
	{
		vGotoXY(0, 0);
		vPutS("Congratulations! You are a winner!");
		vGotoXY(0, 1);
		vPutS("Congratulations! You are a winner!");
		vGotoXY(0, 2);
		vPutS("Congratulations! You are a winner!");
	} else
	if (XQuest::quest.hero_die == 1)
	{
		vGotoXY(30, 6);
		vPutS("       #       ");
		vGotoXY(30, 7);
		vPutS("     #####     ");
		vGotoXY(30, 8);
		vPutS("       #       ");
		vGotoXY(30, 9);
		vPutS("    #######    ");
		vGotoXY(30, 10);
		vPutS("   #       #   ");
		vGotoXY(30, 11);
		vPutS("   #  RIP  #   ");
		vGotoXY(30, 12);
		vPutS("   #       \x1F\x04@\x1F\x07   ");
		vGotoXY(30, 13);
		vPutS(" #####\x1F\x04@\x1F\x07####\x1F\x02|\x1F\x07## ");
		vGotoXY(30, 14);
		vPutS("\x1F\x02W\x1F\x07#\x1F\x02WWWWWWWWWWW\x1F\x07#\x1F\x02W");
	} else
	{
		vGotoXY(0, 0);
		vPutS("Goodbye!");
	}
	vRefresh();
#ifndef __XDEBUG_00
	vGetch();
#endif
}

//#define __DEBUG_L

void XGame::CreateLocations()
{
//	Create locations
#ifndef __DEBUG_L
	XLocation::CreateNewGame();
//	(new XMainLocation(L_MAIN))->NewWay(32, 3, (LOCATION)55, STW_DOWN);
	

/*	new XMushroomsCaveLocation(L_MUSHROOMS_CAVE1);
	new XMushroomsCaveLocation(L_MUSHROOMS_CAVE2);
	new XMushroomsCaveLocation(L_MUSHROOMS_CAVE3);
	new XMushroomsCaveLocation(L_MUSHROOMS_CAVE4);
	new XMushroomsCaveLocation(L_MUSHROOMS_CAVE5);*/
/*	new XDwarfCityCaveLocation(L_DWARFCITYCAVE1);
	new XDwarfCityCaveLocation(L_DWARFCITYCAVE2);
	new XDwarfCityCaveLocation(L_DWARFCITYCAVE3);
	new XDwarfCityCaveLocation(L_DWARFCITYCAVE4);
	new XDwarfCityCaveLocation(L_DWARFCITYCAVE5);
	new XDwarfCityCaveLocation(L_DWARFCITYCAVE6);
	new XDwarfCityCaveLocation(L_DWARFCITYCAVE7);*/
//	new XDwarfCityLocation(L_DWARFCITY);
//	new XDwarfTreasureLocation(L_DWARFTREASURE);
//	new XGasMineLocation(L_GASMINE1);
//	new XGasMineLocation(L_GASMINE2);
//	new XGasMineLocation(L_GASMINE3);
//	new XKingsTreasureLocation(L_KINGS_TREASURE);
//	new XExtinctVolcanoLocation(L_EXTINCT_VOLCANO);
//	new XRatCellarLocation(L_RATCELLAR);

/*	new XWizardDungeonLocation(L_WIZARD_DUNGEON1);
	new XWizardDungeonLocation(L_WIZARD_DUNGEON2);
	new XWizardDungeonLocation(L_WIZARD_DUNGEON3);
	new XWizardDungeonLocation(L_WIZARD_DUNGEON4);
	new XWizardDungeonLocation(L_WIZARD_DUNGEON5);
	new XAhkUlanCastleLocation(L_AHKULAN_CASTLE);

	new XYohjiTower(L_WIZTOWER_TOP);

	new XUndeadTombLocation(L_UNDEADS_TOMB1);
*/
/*	XLocation::CreateRandomCave();
	XLocation::CreateRandomCave();
	XLocation::CreateRandomCave();
	XLocation * tl = Game.locations[XLocation::rand_location_count - 1];
	XPoint pt;
	tl->GetFreeXY(&pt);
	(new XEyeOfRaa)->Drop(tl, pt.x, pt.y);*/
#else
	new XLDebug(L_DEBUG1);
	new XLDebug(L_DEBUG2);
#endif

//	Bind ways
	for (int i = 0; i < L_EOF; i++)
	{
		if (locations[i])
		{
			for (XQList<XObject*>::iterator it1 = locations[i]->ways_list.begin(); it1 != locations[i]->ways_list.end(); it1++)
			{
				XStairWay * way = (XStairWay *)(*it1);
				if (way->nx < 0 && way->ny < 0 && locations[way->ln])
				{
					for (XQList<XObject*>::iterator it2 = locations[way->ln]->ways_list.begin(); it2 != locations[way->ln]->ways_list.end(); it2++)
					{
						XStairWay * tmp_way = (XStairWay *)(*it2);
						if (tmp_way->nx < 0 && tmp_way->ny < 0 && tmp_way->ln == (LOCATION)i)
							way->Bind(tmp_way);
					}
				}
			}
		}
	}
}

void XGame::CreateHero()
{
#ifndef __DEBUG_L
    XRect hero_rect(26, 4, 32, 9);
	XPoint hero_point;

	locations[L_MAIN]->GetFreeXY(&hero_point, &hero_rect);

	XHero * hero = new XHero(1);
	hero_point.x = 25;
	hero_point.y = 50;
	Game.NewCreature(hero, hero_point.x, hero_point.y, locations[L_MAIN]);
	
	XItem * it = new XWeapon(IT_LONGSWORD);
	it->brt = BR_ORCSLAYER;
	hero->ContainItem(it);


//	Game.NewCreature(hero, 77, 11, locations[L_DWARFCITY]);

/*	int lll = L_EXTINCT_VOLCANO;
	locations[lll]->GetFreeXY(&hero_point);
	Game.NewCreature(hero, hero_point.x, hero_point.y, locations[lll]);
*/	

/*	hero_point.x = 40;
	hero_point.y = 10;
	Game.NewCreature(hero, hero_point.x, hero_point.y, locations[L_RATCELLAR]);
	locations[L_RATCELLAR]->map->Center(hero->x, hero->y);
	locations[L_RATCELLAR]->visited_by_hero = 1;

	locations[L_MAIN]->map->Center(hero->x, hero->y);
	locations[L_MAIN]->visited_by_hero = 1;
*/
	//if hero is a bard, than create a dog for him...
	if (strstr(hero->GetRaceStr(), "bard"))
	{
		XRect tr(hero_point.x - 1, hero_point.y - 1, hero_point.x + 1, hero_point.y + 1);
		locations[L_MAIN]->GetFreeXY(&hero_point, &tr);
		XCreature * cr = locations[L_MAIN]->NewCreature(CN_DOG, hero_point.x, hero_point.y);
		cr->xai->companion = hero;
		cr->xai->SetAIFlag(AIF_ALLOW_MOVE_OUT);
		cr->xai->SetAIFlag(AIF_PEACEFUL);
		cr->xai->SetEnemyClass((CREATURE_CLASS)(CR_KOBOLD | CR_GOBLIN | CR_UNDEAD | CR_INSECT | CR_BLOB | CR_CANINE | CR_FELINE | CR_RAT | CR_REPTILE | CR_ORC));
	}
#else
	XPoint pt;
	locations[L_DEBUG1]->GetFreeXY(&pt);
	XHero * hero = new XHero(1);
	Game.NewCreature(hero, pt.x, pt.y, locations[L_DEBUG1]);
	locations[L_DEBUG1]->map->Center(hero->x, hero->y);

	XRect gr(pt.x + 2, pt.y + 2, pt.x + 3, pt.y + 3);
	locations[L_DEBUG1]->NewCreature(CN_DWARF_GUARD, &gr, GID_DWARVEN_GUARDIAN, AIF_GUARD_AREA)->xai->AddPersonalEnemy(hero);

//	new XTrap(pt.x, pt.y, locations[L_DEBUG1], TL_RANDOM, TT_ARROW);
//	XItem *it = new XBlackClub();
//	it->Drop(Game.locations[L_DEBUG1], hero->x, hero->y);

//	XCreature * cr = locations[L_DEBUG1]->NewCreature(CN_WOLF, 4, 15);
//	cr->xai->companion = hero;

#endif
	XCreature::main_creature = hero;

/*

	XCreature * cr = XCreatureFactory::CreateCreature(
		CR_REPTILE, CRS_ANY, CRL_ANY);
	XPoint pt;
	locations[L_MAIN]->GetFreeXY(&pt, &hero_rect);
	Game.NewCreature(cr, pt.x, pt.y, locations[L_MAIN]);
	cr->xai->SetAIFlag(AIF_ALLOW_MOVE_WAY_DOWN);	
*/
}

