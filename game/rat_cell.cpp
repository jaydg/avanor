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
#include "std_ai.h"
#include "uniquei.h"
#include "game.h"
#include "item_misc.h"

XRatCellarLocation::XRatCellarLocation(LOCATION tl) : XLocation(tl)
{
	strcpy(brief_name, "RC:1");
	strcpy(full_name, "Rat's cellar");

	map = new XMap(80, 20);
	PutPalette(0, 0, PAL_RAT_CELLAR, this);
	NewWay(40, 10, L_MAIN, STW_UP);	
	
	int tx = vRand() % 2 * 70 + 5;
	int ty = vRand() % 2 * 16 + 2;
	XForestBrotherCloak * cloak = new XForestBrotherCloak();
	cloak->Drop(this, tx, ty);

	int i;
	XRect r1(1, 1, 10, 5);
	XRect r2(1, 14, 10, 19);
	XRect r3(69, 1, 79, 5);
	XRect r4(69, 14, 79, 19);
	for (i = 0; i < 10; i++)
	{
		NewCreature(CN_RAT, &r1);
		NewCreature(CN_RAT, &r2);
		NewCreature(CN_RAT, &r3);
		NewCreature(CN_RAT, &r4);
	}

	XRect r5(8, 8, 11, 10);
	for (i = 0; i < 2; i++)
		NewCreature(CN_GHOST, &r5);

	XChest * ch1 = new XChest(6, (ITEM_MASK)(IM_WEAPON | IM_ARMOUR), 100, 1500);
	ch1->Drop(this, 8, 9);

	XChest * ch2 = new XChest(6, (ITEM_MASK)(IM_BOOK | IM_SCROLL), 100, 4500);
	ch2->Drop(this, 8, 10);
}

