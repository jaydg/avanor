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
#include "xgen.h"
#include "unique.h"
#include "game.h"


class XMagicMCPlace : public XAnyPlace
{
public:
	DECLARE_CREATOR(XMagicMCPlace, XAnyPlace);
	XMagicMCPlace(XRect * _arial, XLocation * _loc) : XAnyPlace(_arial, _loc) {}
	int onCreatureMove(XCreature * cr)
	{
		if (cr->isHero() && vRand(15) == 0)
		{
			msgwin.Add("You fill power swirling the air...");
		}
		if (vRand(30) == 0)
		{
			STATS st = (STATS)vRand(S_EOF); //random stats;
			if (cr->GetStats(st) > 5)
				cr->GainAttr(st, -1);
		}
		return 1;
	}
};

REGISTER_CLASS(XMagicMCPlace);

//           000  entr cave(1)
//            |
//           000  demon cave (2)
//          / |  
// misc(3)000 | 
//         | 000 mushroom cave (5)
//         |
//        000 kobold cave (4)


XMushroomsCaveLocation::XMushroomsCaveLocation(LOCATION loc) : XLocation(loc)
{
	BuildCave();

	if (loc == 	L_MUSHROOMS_CAVE1)
	{
		strcpy(brief_name, "MC:1");
		strcpy(full_name, "Mushroom Caves Level 1");
		
		NewWay(L_MAIN, STW_UP);
		NewWay(L_MUSHROOMS_CAVE2, STW_DOWN);

//		Game.Sheduler.Add(new XUniversalGen(this, (CREATURE_CLASS)(CR_INSECT | CR_REPTILE), CRL_VERY_LOW, 10, 15000));
	}
	if (loc == L_MUSHROOMS_CAVE2)
	{
		strcpy(brief_name, "MC:2");
		strcpy(full_name, "Mushroom Caves Level 2");
		NewWay(L_MUSHROOMS_CAVE1, STW_UP);
		NewWay(L_MUSHROOMS_CAVE3, STW_DOWN);
		NewWay(L_MUSHROOMS_CAVE5, STW_DOWN);
		NewCreature(CN_BEELZEVILE, NULL);
	}
	if (loc == L_MUSHROOMS_CAVE3)
	{
		strcpy(brief_name, "KC:1");
		strcpy(full_name, "Kobolds Cavern Level 1");
		NewWay(L_MUSHROOMS_CAVE2, STW_UP);
		NewWay(L_MUSHROOMS_CAVE4, STW_DOWN);
		Game.Sheduler.Add(new XUniversalGen(this, (CREATURE_CLASS)(CR_KOBOLD), (CREATURE_LEVEL)(CRL_LOW | CRL_VERY_LOW), 10, 25000));
	}
	if (loc == L_MUSHROOMS_CAVE4)
	{
		strcpy(brief_name, "KC:2");
		strcpy(full_name, "Kobolds Cavern Level 2");
		NewWay(L_MUSHROOMS_CAVE3, STW_UP);

		Game.Sheduler.Add(new XUniversalGen(this, (CREATURE_CLASS)(CR_KOBOLD), (CREATURE_LEVEL)(CRL_LOW | CRL_VERY_LOW), 10, 25000));
	}
	if (loc == L_MUSHROOMS_CAVE5)
	{
		strcpy(brief_name, "MC:3");
		strcpy(full_name, "Mushroom Caves Level 3");
		NewWay(L_MUSHROOMS_CAVE2, STW_UP);
		XRect t_arial(0, 0, 80, 20);
		XAnyPlace * place = new XMagicMCPlace(&t_arial, this);
		AddPlace(place);
		ttmb = 1000;
		ttm = 1000;
		Game.Sheduler.Add(Game.locations[L_MUSHROOMS_CAVE5].get());
		
	}
}

int XMushroomsCaveLocation::Run()
{
	ttm = 45000;
	XPoint pt;
	GetFreeXY(&pt);
	new XMushSpawn(pt.x, pt.y, this);
	return 1;
}

