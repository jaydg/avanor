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

#include "other_misc.h"
#include "creatures.h"
#include "unique.h"
#include "skeep_ai.h"
#include "xgen.h"
#include "game.h"
#include "item_misc.h"

XMainLocation::XMainLocation(LOCATION tl) : XLocation(tl)
{
	strcpy(brief_name, "Valley");
	strcpy(full_name, "Valley of Avanor");

	int lm = 0;
	int rm = 200;
	int tm = 0;
	int bm = 90;
	map = new XMap(rm, bm);
	int i,j;

	for (i = 0; i < map->hgt; i++)
		for (j = 0; j < map->len; j++)
		{
			if (vRand() % 3)
				map->SetXY(j, i, M_GREENGRAS);
			else
				map->SetXY(j, i, M_GREENTREE);
		}
		
// creating high mountains
	for (i = lm; i < rm; i++)
	{
		int z1 = vRand() % ((i & 3) + 1) + 1;
		int z2 = vRand() % ((i & 3) + 1) + 1;
		for (j = 0; j < z1; j++)
		{
			map->SetXY(i, tm + j, M_HIGHMOUNTAIN);
		}
		for (j = 0; j < z2; j++)
		{
			map->SetXY(i, bm - j - 1, M_HIGHMOUNTAIN);
		}
	}
	
	for (i = tm; i < bm; i++)
	{
		int z1 = vRand() % ((i & 3) + 1) + 1;
		int z2 = vRand() % ((i & 3) + 1) + 1;
		for (j = 0; j < z1; j++)
		{
			map->SetXY(lm + j, i, M_HIGHMOUNTAIN);
		}
		for (j = 0; j < z2; j++)
		{
			map->SetXY(rm - j - 1, i, M_HIGHMOUNTAIN);
		}
	}
	
//evaluate high mountains till hills!
	for (i = 0; i < map->hgt; i++)
		for (j = 0; j < map->len; j++)
		{
			int m = map->GetXY(j, i);
			if (m > M_HILL && m <= M_HIGHMOUNTAIN)
			{
				for (int q = -2; q < 3; q++)
					for (int w = -2; w < 3; w++)
					{
						int nm;
						if (abs(q) >= abs(w))
							nm = m - abs(q);
						else
							nm = m - abs(w);
						if (nm < M_HILL) nm = M_HILL;
						if (j + q >= 0 && i + w >= 0
							&& j + q < map->len && i + w < map->hgt
							&& map->GetXY(j + q, i + w) < nm)
							map->SetXY(j + q, i + w, (STDMAP)nm);
					}
			}
			
		}
		
		
//Small village		
///		PutPalette(0, 0, PAL_SMALL_VILLAGE, this);

///		NewWay(3, 6, L_MUSHROOMS_CAVE1, STW_DOWN);

///		XRect village_area(8, 6, 34, 16);
///		for (i = 0; i < 5; i++)
///		{
///			NewCreature(CN_FARMER, &village_area, GID_SMALL_VILLAGE_FARMER, AIF_GUARD_AREA);
///			NewCreature(CN_GOODWIFE, &village_area, GID_SMALL_VILLAGE_FARMER, AIF_GUARD_AREA);
///		}


///		XRect elder_area(19, 9, 25, 11);
///		NewCreature(CN_ELDER_GRIDOR, &elder_area, GID_SMALL_VILLAGE_FARMER, AIF_GUARD_AREA);
///		new XFurniture(24, 8, xBROWN, '~', "plain bed", this);

///		XRect shop_rect1(8, 7, 18, 11);
///		CreateShop(IM_FOOD, &shop_rect1, "Nobel, the human shopkeeper");
	

//way to dwarven caves
///		NewWay(5, 29, L_DWARFCITYCAVE1, STW_DOWN);


//master thief
///		XRect master_thief_area(40, 10, 45, 14);
///		map->CreateRoom(40, 10, 5, 4, 42, 10, M_SAND, M_WOODWALL);
///		NewCreature(CN_JORGUS, &master_thief_area, GID_FOREST_BROTHER, AIF_GUARD_AREA);

//bandits area
///		XRect bandit_area(35, 4, 47, 16);
///		for (i = 0; i < 10; i++)
///			NewCreature(CN_BANDIT, &bandit_area, GID_FOREST_BROTHER, AIF_GUARD_AREA | AIF_PROTECT_AREA | AIF_PEACEFUL);


//small town
///		PutPalette(10, 40, PAL_SMALL_TOWN, this);
///		XRect small_town_area(16, 42, 30, 48);

///		for (i = 0; i < 8; i++)
///			NewCreature(CN_ROYAL_GUARD, &small_town_area, GID_GUARDIAN, AIF_GUARD_AREA)->xai->SetEnemyClass(CR_ORC);

///		NewCreature(CN_GEKTA, &small_town_area, GID_GUARDIAN, AIF_GUARD_AREA)->xai->SetEnemyClass(CR_ORC);
		
///		XRect ozrect(24, 50, 27, 52);
///		NewCreature(CN_OZORIK, &ozrect, GID_GUARDIAN, AIF_GUARD_AREA)->xai->SetEnemyClass(CR_ORC);
//		new XFurniture(26, 51, xLIGHTRED, '~', "bed", this);
//		new XFurniture(24, 51, xBROWN, '~', "desk", this);

/*		new XFurniture(12, 46, xBROWN, '~', "plain bed", this);
		new XFurniture(14, 46, xBROWN, '~', "plain bed", this);
		new XFurniture(12, 42, xBROWN, '~', "plain bed", this);
		new XFurniture(14, 42, xBROWN, '~', "plain bed", this);
		new XFurniture(18, 42, xBROWN, '~', "plain bed", this);
		new XFurniture(20, 42, xBROWN, '~', "plain bed", this);
		new XFurniture(24, 42, xBROWN, '~', "plain bed", this);
		new XFurniture(26, 42, xBROWN, '~', "plain bed", this);
		new XFurniture(30, 42, xBROWN, '~', "plain bed", this);
		new XFurniture(32, 42, xBROWN, '~', "plain bed", this);
		new XFurniture(36, 42, xBROWN, '~', "plain bed", this);
		new XFurniture(38, 42, xBROWN, '~', "plain bed", this);
		new XFurniture(42, 42, xBROWN, '~', "plain bed", this);
		new XFurniture(44, 42, xBROWN, '~', "plain bed", this);
		new XFurniture(48, 42, xBROWN, '~', "plain bed", this);
		new XFurniture(50, 42, xBROWN, '~', "plain bed", this);

		new XFurniture(30, 46, xBROWN, '~', "table", this);
		new XFurniture(30, 48, xBROWN, '~', "table", this);
		new XFurniture(32, 46, xBROWN, '~', "table", this);
		new XFurniture(32, 48, xBROWN, '~', "table", this);
		new XFurniture(34, 46, xBROWN, '~', "table", this);
		new XFurniture(34, 48, xBROWN, '~', "table", this);
		new XFurniture(36, 46, xBROWN, '~', "table", this);
		new XFurniture(36, 48, xBROWN, '~', "table", this);
		new XFurniture(38, 46, xBROWN, '~', "table", this);
		new XFurniture(38, 48, xBROWN, '~', "table", this);
*/
///		NewWay(13, 42, L_RATCELLAR, STW_DOWN);

///		XRect shop_rect2(11, 49, 22, 54);
///		CreateShop(IM_ARMOUR | IM_WEAPON | IM_MISSILE | IM_MISSILEW, &shop_rect2, "Noberik, the human shopkeeper");

//orcs!
		XRect orc_area(10, 70, 30, 80);
		for (i = 0; i < 20; i++)
			NewCreature(CR_ORC, &orc_area, GID_ORCS_WARPARTY, AIF_GUARD_AREA);

		Game.Scheduler.Add(new XMainLocationGen(this));

//Yohjishiro
///		PutPalette(45, 25, PAL_WIZARD_TOWER, this);
///		NewWay(55, 31, L_WIZTOWER_TOP, STW_UP);

//Large City		
///		XRect city_area(139, 2, 157, 26);
///		PutPalette(130, 2, PAL_CITY, this);

///		XRect shop_rect3(147, 18, 153, 24);
///		CreateShop(IM_BOOK | IM_SCROLL | IM_POTION, &shop_rect3, "Toberik, the human shopkeeper", SHOP_DOOR_RIGHT);

///		XRect magic_guild_area(163, 12, 166, 15);
///		NewCreature(CN_GEFEON, &magic_guild_area, GID_RODERICK_GUARDIAN, AIF_GUARD_AREA);
///		new XFurniture(165, 13, xLIGHTRED, '~', "eternal flame", this);

//Roderik palace
///		XRect roderick_area(182, 4, 183, 5);
///		NewCreature(CN_RODERIK, &roderick_area, GID_RODERICK_GUARDIAN, (AIF_GUARD_AREA | AIF_NO_SWAP));
///		new XFurniture(182, 4, xYELLOW, '~', "the throne of Avanor", this);
///		new XFurniture(172, 9, xLIGHTRED, '~', "royal bed", this);
///		new XFurniture(189, 6, xBROWN, '~', "round table", this);
///		new XFurniture(191, 12, xBROWN, '~', "dinner table", this);
/***		{
			XRect gr(180, 7, 181, 8);
			NewCreature(CN_ROYAL_GUARD, &gr, GID_RODERICK_GUARDIAN, AIF_GUARD_AREA);
		}

		{
			XRect gr(180, 6, 181, 7);
			NewCreature(CN_ROYAL_GUARD, &gr, GID_RODERICK_GUARDIAN, AIF_GUARD_AREA);		
		}

		{
			XRect gr(184, 7, 185, 8);
			NewCreature(CN_ROYAL_GUARD, &gr, GID_RODERICK_GUARDIAN, AIF_GUARD_AREA);
		}

		{
			XRect gr(184, 6, 185, 7);
			NewCreature(CN_ROYAL_GUARD, &gr, GID_RODERICK_GUARDIAN, AIF_GUARD_AREA);
		}

		{
			//treasure guardian
			XRect gr(175, 15, 176, 16);
			NewCreature(CN_ROYAL_GUARD, &gr, GID_RODERICK_GUARDIAN, AIF_GUARD_AREA | AIF_NO_SWAP); 
			NewWay(175, 15, L_KINGS_TREASURE, STW_DOWN);
		}

		{
			XRect gr(172, 14, 173, 15);
			NewCreature(CN_ROYAL_GUARD, &gr, GID_RODERICK_GUARDIAN, AIF_GUARD_AREA);
		}
		{
			XRect gr(172, 15, 173, 16);
			NewCreature(CN_ROYAL_GUARD, &gr, GID_RODERICK_GUARDIAN, AIF_GUARD_AREA);
		}
***/
//Lage city townee
///		for (i = 0; i < 5; i++)
///		{
///			NewCreature(CN_CITIZEN, &city_area, GID_RODERICK_GUARDIAN, AIF_GUARD_AREA);
///			NewCreature(CN_FCITIZEN, &city_area, GID_RODERICK_GUARDIAN, AIF_GUARD_AREA);
///		}

// teleports from small village to town and back
		new XTeleport(23, 20, this, L_MAIN, 154, 13);
		new XTeleport(154, 13, this, L_MAIN, 23, 20);

///		new XAltar(154, 4, D_LIFE, this);
///		{
///			XRect gr(153, 4, 156, 7);
///			NewCreature(CN_HIGHPRIEST, &gr, GID_RODERICK_GUARDIAN, AIF_GUARD_AREA);
///		}
///		new XFurniture(152,5,xBROWN,'~',"pew",this);
///		new XFurniture(152,6,xBROWN,'~',"pew",this);
///		new XFurniture(153,5,xBROWN,'~',"pew",this);
///		new XFurniture(153,6,xBROWN,'~',"pew",this);
///		new XFurniture(155,5,xBROWN,'~',"pew",this);
///		new XFurniture(155,6,xBROWN,'~',"pew",this);
///		new XFurniture(156,5,xBROWN,'~',"pew",this);
///		new XFurniture(156,6,xBROWN,'~',"pew",this);

//Black wizard ruins
///		PutPalette(155, 44, PAL_TOWER_RUINS, this);
///		NewWay(165, 49, L_WIZARD_DUNGEON1, STW_DOWN);

//tomb
///		PutPalette(100, 50, PAL_UNDEAD_TOMB0, this);
///		NewWay(118, 54, L_UNDEADS_TOMB1, STW_DOWN);

///		XRect tomb_area(104, 51, 117, 57);
		
///		for (i = 0; i < 20; i++)
///			NewCreature(CN_SKELETON, &tomb_area);

//Extint Volcano
///		PutPalette(40, 72, PAL_EXTINCT_VOLCANO, this);
///		NewWay(46, 75, L_EXTINCT_VOLCANO, STW_DOWN);

   		for (i = 0; i < map->hgt; i++)
   			for (j = 0; j < map->len; j++)
   			{
         		if (vRand(18) == 0)
				{
					if(map->GetXY(j, i) == M_GREENGRAS && map->GetSpecial(j, i) == NULL)
						new XHerbBush(j, i, this);
				}

   			}


}

/*XYohjiTower::XYohjiTower(LOCATION tl) : XLocation(tl)
{
	strcpy(brief_name, "WzTwr");
	strcpy(full_name, "Yohjishiro's Tower");

	map = new XMap(80, 20);
	int i,j;

	for (i = 0; i < map->hgt; i++)
		for (j = 0; j < map->len; j++)
		{
			if (vRand() % 3)
				map->SetXY(j, i, M_GREENGRAS);
			else
				map->SetXY(j, i, M_GREENTREE);
		}

	PutPalette(30, 5, PAL_WIZTOWER_TOP, this);
	NewWay(40, 11, L_MAIN, STW_DOWN);

	// It's her house...
	XRect wizard_tower_area(37, 7, 44, 12);
	NewCreature(CN_YOHJISHIRO, &wizard_tower_area, GID_NONE, AIF_GUARD_AREA);

	new XFurniture(37, 10, xLIGHTRED, '~', "nice bed", this);
}


XKingsTreasureLocation::XKingsTreasureLocation(LOCATION tl) : XLocation(tl)
{
	strcpy(brief_name, "RoyalTr");
	strcpy(full_name, "Royal Treasure");

	BuildCave();

	PutPalette(0, 0, PAL_KINGS_TREASURE, this);
	NewWay(10, 9, L_MAIN, STW_UP);	

	int i;
	XPoint pt;
	XRect area(1, 1, 20, 7);
	for (i = 0; i < 16; i++)
	{
		GetFreeXY(&pt, &area);
		new XTrap(pt.x, pt.y, this);
	}

	for (i = 0; i < 9; i++)
	{
		GetFreeXY(&pt, &area);
		XChest * tchest = new XChest(5, IM_ITEM, 200, 15000);
		tchest->Drop(this, pt.x, pt.y);
	}

	XItem * it = new XAncientMachinePart();
	it->Drop(this, 2, 3);

	it = new XAncientMachinePart();
	it->Drop(this, 2, 4);

	for (i = 1; i < 20; i++)
	{
		for (int j = 1; j < 7; j++)
		{
			if (map->XGetMovability(i, j) == 0 && map->GetItemCount(i, j) == 0)
			{
				XMoney * money = new XMoney(vRand() % 200 + 50);
				money->Drop(this, i, j);
			}
		}
	}
}
*/

XExtinctVolcanoLocation::XExtinctVolcanoLocation(LOCATION tl) : XLocation(tl)
{
	BuildCave();

	/* Random lava! */
/*	for (int k = 0; k < 150; k++)
	{
		int	qx = vRand() % (80 - 7) + 1;
		int qy = vRand() % (20 - 5) + 1;

		map->SetXY(qx, qy, M_LAVA);
	}
	strcpy(brief_name, "Volcano");
	strcpy(full_name, "Crater of an Extinct Volcano");
	NewWay(L_MAIN, STW_UP);

	NewCreature(CN_XSHEE_VOO);*/
}

