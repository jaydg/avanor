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
#include "xgen.h"
#include "unique.h"
#include "std_ai.h"
#include "incl_i.h"
#include "other_misc.h"
#include "itemf.h"
#include "game.h"
#include "item_misc.h"
#include "uniquei.h"

XUndeadTombLocation::XUndeadTombLocation(LOCATION tl) : XLocation(tl)
{
	strcpy(brief_name, "tomb:1");
	strcpy(full_name, "Tomb");

	BuildCave();

	PutPalette(0, 0, PAL_UNDEAD_TOMB1, this);
	NewWay(6, 10, L_MAIN, STW_UP);
	int i;

	XGrave * gr = new XGrave(32, 14, "Frederick III (760-805)", this);
	for (i = 1; i < vRand(10); i++)
		gr->HideItem(new XBone());

	gr = new XGrave(32, 6, "Frederick II (710-790)", this);
	for (i = 1; i < vRand(10); i++)
		gr->HideItem(new XBone());

	gr = new XGrave(55, 16, "Arthur IV (796-854)", this);
	for (i = 1; i < vRand(10); i++)
		gr->HideItem(new XBone());

	gr = new XGrave(55, 4, "Rodomir III (821-894)", this);
	for (i = 1; i < vRand(10); i++)
		gr->HideItem(new XBone());
	gr->HideItem(new XAvanorDefender());
	
	
	XPoint pt;
	for (i = 0; i < 40; i++)
		NewCreature(CN_GHOST);

	for (i = 0; i < 10; i++)
	{
		NewCreature(CN_DREAD);
		NewCreature(CN_SPECTRE);
	}
}

