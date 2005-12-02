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

#include "xldebug.h"
#include "xgen.h"
#include "unique.h"
#include "uniquei.h"
#include "incl_i.h"
#include "itemf.h"
#include "shop.h"
#include "skeep_ai.h"
#include "other_misc.h"
#include "game.h"

XLDebug::XLDebug(LOCATION tl) : XLocation(tl)
{
	map = new XMap(80, 20);
	BuildLabirint();
	strcpy(brief_name, "DBGL:1");
	strcpy(full_name, "Debug Level 1");


	map->CreateRoom(0, 0, 80, 20, 0, 0, M_GREENGRAS, M_MAGMA);

	{
	}


//	NewCreature(CN_GIANA, 14, 11)->xai->companion = NewCreature(CN_ROTMOTH, 14, 10);
//	CreateShop(IM_ALL, &XRect(10, 10, 20, 15), "xxx");
//	NewCreature(CN_SHAMAN_KOBOLD, 15, 5);
//	NewCreature(CN_KING_COBRA, 14, 10);
/*	XCreature * tcr = NewCreature(CN_KOBOLD, 15, 10);
	tcr->Wear(ICREATEB(IM_MISSILEW, IT_LONGBOW, 1, 10000));
	tcr->Wear(ICREATEB(IM_MISSILE, IT_ARROW, 1, 10000));*/
/*	for (int i = 0; i < 10; i++)
	{
		NewCreature(CN_GHOST);
		NewCreature(CN_SHAMAN_KOBOLD);
	}*/
/*

	{
   		for (int i = 0; i < 20; i++)
   			for (int j = 0; j < 60; j++)
			{
				if(map->GetXY(j, i) == M_GREENGRAS && map->GetSpecial(j, i) == NULL)
					new XMushSpawn(j, i, this);
			}

	}*/

/*	XRect tr1(30, 5, 40, 10);
	int i;
	for (i = 0; i < 10; i++)
	{
		NewCreature(CN_RAT, &tr1);
	}
*/

/*	map->CreateRoom(10, 10, 5, 5, 12, 15, M_GREENGRAS, M_ROAD);

	XRect r1(10, 10, 15, 15);
	
*/	
/*	for (int i = 0; i < 5; i++)
	{
		XCreature * cr = NewCreature(CN_KOBOLD);
//		cr->xai->SetArea(&r1, tl);
//		cr->xai->SetAIFlag(AIF_GUARD_AREA);
	}*/
/*
	XCreature * cr = NewCreature(CN_KOBOLD);
	
	XQList<SCRIPT_CMD> script;
	SCRIPT_CMD cmd;
	
	cmd.cmd = SCC_MOVE_POINT;
	cmd.pt_x = 5;
	cmd.pt_y = 5;
	cmd.ln = L_DEBUG1;
	script.push_back(cmd);

	cmd.cmd = SCC_MOVE_POINT;
	cmd.pt_x = 15;
	cmd.pt_y = 15;
	cmd.ln = L_DEBUG1;
	script.push_back(cmd);

	cmd.cmd = SCC_MOVE_POINT;
	cmd.pt_x = 15;
	cmd.pt_y = 5;
	cmd.ln = L_DEBUG1;
	script.push_back(cmd);

	
	cr->xai->ExecuteScript(&script);
	cr->color = xRED;

*/
/*	XRect tr2(45, 5, 55, 10);
	for (i = 0; i < 5; i++)
	{
		NewCreature(CN_DOG, &tr2);
	}
*/	
//	Game.NewCreature(XCreatureStorage::Create(CN_RAT), 28, 14, this);
//	Game.NewCreature(XCreatureStorage::Create(CN_RAT), 28, 13, this);

//	Game.NewCreature(XCreatureStorage::Create(CN_KING_COBRA), 8, 15, this);
//	Game.NewCreature(XCreatureStorage::Create(CN_KING_COBRA), 9, 15, this);
//	Game.NewCreature(XCreatureStorage::Create(CN_KING_COBRA), 10, 15, this);
//	Game.NewCreature(XCreatureStorage::Create(CN_KOBOLD), 8, 15, this);
//	Game.NewCreature(XCreatureStorage::Create(CN_KOBOLD), 9, 15, this);
//	Game.NewCreature(XCreatureStorage::Create(CN_SHAMAN_KOBOLD), 10, 15, this);
//	Game.NewCreature(XCreatureStorage::Create(CN_SKELETON), 9, 15, this);
//	Game.NewCreature(XCreatureStorage::Create(CN_GHOST), 8, 15, this);
//	Game.NewCreature(XCreatureStorage::Create(CN_FARMER), 8, 15, this);
//	Game.NewCreature(XCreatureStorage::Create(CN_CITIZEN), 9, 15, this);
//	NewCreature(CN_FARMER, 10, 11);

/*
	XKobold * kobold = new XKobold(CRS_KOBOLD, CRL_ANY, CRE_ANY);
	all->Add(kobold);
	kobold->FirstStep(10, 15, this);
	kobold->xai->SetEnemyClass(CR_ALL);

	XItem * item = ICREATEB(IM_MISSILEW, IT_HEAVYCROSSBOW, 1, 100);

	kobold->CarryItem(item);
	kobold->contain->Add(item);
	item = ICREATEB(IM_MISSILE, IT_QUARREL, 1, 100);
	kobold->CarryItem(item);
	kobold->contain->Add(item);
*/

/*	new XTrap(10, 10, this, TL_RANDOM);
	new XTrap(11, 11, this, TL_RANDOM);
	new XTrap(12, 12, this, TL_RANDOM);
	new XTrap(13, 13, this, TL_RANDOM);
	new XTrap(6, 15, this, TL_RANDOM);
	new XTrap(7, 15, this, TL_RANDOM);
	new XTrap(8, 15, this, TL_RANDOM);
	new XTrap(9, 15, this, TL_RANDOM);
*/

/*	if (tl == L_DEBUG1)
	{
		strcpy(brief_name, "DBGL:1");
		strcpy(full_name, "Debug Level 1");
		NewWay(7, 12, L_DEBUG2, STW_DOWN);
	} else
	{
		strcpy(brief_name, "DBGL:2");
		strcpy(full_name, "Debug Level 2");
		NewWay(7, 12, L_DEBUG1, STW_UP);
	}*/

}


void XLDebug::Init()
{

}
