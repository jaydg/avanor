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
#include "other_misc.h"
#include "xgen.h"
#include "unique.h"
#include "game.h"
#include "xtool.h"

XDwarfCityCaveLocation::XDwarfCityCaveLocation(LOCATION l_name) : XLocation(l_name)
{
	map = new XMap(80, 20);
//	location_name = l_name;

	BuildLabirint();

	XPoint pt;
	
	switch (l_name)
	{
		case L_DWARFCITYCAVE1:
			strcpy(brief_name, "PDC1");
			strcpy(full_name, "Path to the Dwarven City Level 1");

			NewWay(L_MAIN, STW_UP);
			NewWay(L_DWARFCITYCAVE2, STW_DOWN);
			Game.Scheduler.Add(new XUniversalGen(this, (CREATURE_CLASS)(CR_UNDEAD | CR_BLOB | CR_INSECT | CR_REPTILE | CR_RAT | CR_FELINE | CR_CANINE), CRL_VERY_LOW, 10, 10000));
			break;

		case L_DWARFCITYCAVE2:
			strcpy(brief_name, "PDC2");
			strcpy(full_name, "Path to the Dwarven City Level 2");

			NewWay(L_DWARFCITYCAVE1, STW_UP);
			NewWay(L_DWARFCITYCAVE3, STW_DOWN);
			Game.Scheduler.Add(new XUniversalGen(this, (CREATURE_CLASS)(CR_UNDEAD | CR_BLOB | CR_INSECT | CR_REPTILE | CR_RAT | CR_FELINE | CR_CANINE), CRL_LOW, 10, 10000));
			break;

		case L_DWARFCITYCAVE3:
			strcpy(brief_name, "PDC3");
			strcpy(full_name, "Path to the Dwarven City Level 3");

			NewWay(L_DWARFCITYCAVE2, STW_UP);
			NewWay(L_DWARFCITYCAVE4, STW_DOWN);
			Game.Scheduler.Add(new XUniversalGen(this, (CREATURE_CLASS)(CR_UNDEAD | CR_BLOB | CR_INSECT | CR_REPTILE | CR_RAT | CR_FELINE | CR_CANINE), CRL_LOW, 10, 10000));
			break;

		case L_DWARFCITYCAVE4:
			strcpy(brief_name, "PDC:4");
			strcpy(full_name, "Path to the Dwarven City Level 4");

			NewWay(L_DWARFCITYCAVE3, STW_UP);
			NewWay(L_DWARFCITYCAVE5, STW_DOWN);
			Game.Scheduler.Add(new XUniversalGen(this, (CREATURE_CLASS)(CR_UNDEAD | CR_BLOB | CR_INSECT | CR_REPTILE | CR_RAT | CR_FELINE | CR_CANINE), CRL_LOW, 10, 10000));
			break;

		case L_DWARFCITYCAVE5:
			strcpy(brief_name, "PDC5");
			strcpy(full_name, "Path to the Dwarven City Level 5");

			NewWay(L_DWARFCITYCAVE4, STW_UP);
			NewWay(L_DWARFCITYCAVE6, STW_DOWN);
			Game.Scheduler.Add(new XUniversalGen(this, (CREATURE_CLASS)(CR_UNDEAD | CR_BLOB | CR_INSECT | CR_REPTILE | CR_RAT | CR_FELINE | CR_CANINE), CRL_LOW, 10, 10000));
			break;

		case L_DWARFCITYCAVE6:
			strcpy(brief_name, "PDC6");
			strcpy(full_name, "Path to the Dwarven City Level 6");

			NewWay(L_DWARFCITYCAVE5, STW_UP);
			NewWay(L_DWARFCITY, STW_DOWN);
			Game.Scheduler.Add(new XUniversalGen(this, (CREATURE_CLASS)(CR_UNDEAD | CR_BLOB | CR_INSECT | CR_REPTILE | CR_RAT | CR_FELINE | CR_CANINE | CR_GOBLIN | CR_KOBOLD), CRL_LOW, 10, 10000));
			
			//add one cooking set to the game
			XCookingSet * cs = new XCookingSet();
			GetFreeXY(&pt);
			cs->Drop(this, pt.x, pt.y);
			break;

	}
}
