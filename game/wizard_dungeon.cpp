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

XWizardDungeonLocation::XWizardDungeonLocation(LOCATION l_name) : XLocation(l_name)
{
	map = new XMap(80, 20);
	BuildLabirint();

	XPoint pt;

	switch (l_name)
	{
		case L_WIZARD_DUNGEON1:
			strcpy(brief_name, "AD1");
			strcpy(full_name, "Ahk-Ulan's dungeons level 1");
			NewWay(L_MAIN, STW_UP);
			NewWay(L_WIZARD_DUNGEON2, STW_DOWN);
			Game.Sheduler.Add(new XUniversalGen(this, (CREATURE_CLASS)(CR_UNDEAD | CR_INSECT | CR_REPTILE | CR_RAT), CRL_VERY_LOW, 10, 10000));
			break;

		case L_WIZARD_DUNGEON2:
			strcpy(brief_name, "AD2");
			strcpy(full_name, "Ahk-Ulan's dungeons level 2");
			NewWay(L_WIZARD_DUNGEON1, STW_UP);
			NewWay(L_WIZARD_DUNGEON3, STW_DOWN);
			Game.Sheduler.Add(new XUniversalGen(this, (CREATURE_CLASS)(CR_UNDEAD | CR_INSECT | CR_REPTILE | CR_RAT), CRL_LOW, 10, 10000));
			break;

		case L_WIZARD_DUNGEON3:
			strcpy(brief_name, "AD3");
			strcpy(full_name, "Ahk-Ulan's dungeons level 3");
			NewWay(L_WIZARD_DUNGEON2, STW_UP);
			NewWay(L_WIZARD_DUNGEON4, STW_DOWN);
			Game.Sheduler.Add(new XUniversalGen(this, (CREATURE_CLASS)(CR_UNDEAD | CR_INSECT | CR_REPTILE | CR_RAT ), CRL_LOW, 10, 10000));
			break;

		case L_WIZARD_DUNGEON4:
			strcpy(brief_name, "AD4");
			strcpy(full_name, "Ahk-Ulan's dungeons level 4");
			NewWay(L_WIZARD_DUNGEON3, STW_UP);
			NewWay(L_WIZARD_DUNGEON5, STW_DOWN);
			Game.Sheduler.Add(new XUniversalGen(this, (CREATURE_CLASS)(CR_UNDEAD | CR_INSECT | CR_REPTILE | CR_RAT ), CRL_LOW, 10, 10000));
			break;

		case L_WIZARD_DUNGEON5:
			strcpy(brief_name, "AD5");
			strcpy(full_name, "Ahk-Ulan's dungeons level 5");
			NewWay(L_WIZARD_DUNGEON4, STW_UP);
			NewWay(L_AHKULAN_CASTLE, STW_DOWN);
			Game.Sheduler.Add(new XUniversalGen(this, (CREATURE_CLASS)(CR_UNDEAD | CR_INSECT | CR_REPTILE | CR_RAT ), CRL_LOW, 10, 10000));
			break;
	}
}


XAhkUlanCastleLocation::XAhkUlanCastleLocation(LOCATION tl) : XLocation(tl)
{
	strcpy(brief_name, "AC");
	strcpy(full_name, "Ahk-Ulan's castle");
	
	BuildCave();

	PutPalette(0, 0, PAL_AHKULAN_CASTLE, this);
	NewWay(4, 9, L_WIZARD_DUNGEON5, STW_UP);

	XPoint pt;
	{
		XRect gr(39, 8, 40, 9);
		NewCreature(CN_DEATH_KNIGHT, &gr, GID_AHKULAN_GUARDIAN, AIF_GUARD_ARIAL)->xai->SetEnemyClass((CREATURE_CLASS)(CR_ALL ^ (CR_HUMAN | CR_HUMANOID)));
	}

	{
		XRect gr(39, 10, 40, 11);
		NewCreature(CN_DEATH_KNIGHT, &gr, GID_AHKULAN_GUARDIAN, AIF_GUARD_ARIAL)->xai->SetEnemyClass((CREATURE_CLASS)(CR_ALL ^ (CR_HUMAN | CR_HUMANOID)));
	}

	{
		XRect gr(47, 8, 48, 9);
		NewCreature(CN_DEATH_KNIGHT, &gr, GID_AHKULAN_GUARDIAN, AIF_GUARD_ARIAL)->xai->SetEnemyClass((CREATURE_CLASS)(CR_ALL ^ (CR_HUMAN | CR_HUMANOID)));
	}

	{
		XRect gr(48, 8, 49, 9);
		NewCreature(CN_DEATH_KNIGHT, &gr, GID_AHKULAN_GUARDIAN, AIF_GUARD_ARIAL)->xai->SetEnemyClass((CREATURE_CLASS)(CR_ALL ^ (CR_HUMAN | CR_HUMANOID)));
	}

	{
		XRect gr(49, 8, 50, 9);
		NewCreature(CN_DEATH_KNIGHT, &gr, GID_AHKULAN_GUARDIAN, AIF_GUARD_ARIAL)->xai->SetEnemyClass((CREATURE_CLASS)(CR_ALL ^ (CR_HUMAN | CR_HUMANOID)));
	}

	{
		XRect gr(47, 10, 48, 11);
		NewCreature(CN_DEATH_KNIGHT, &gr, GID_AHKULAN_GUARDIAN, AIF_GUARD_ARIAL)->xai->SetEnemyClass((CREATURE_CLASS)(CR_ALL ^ (CR_HUMAN | CR_HUMANOID)));
	}

	{
		XRect gr(48, 10, 49, 11);
		NewCreature(CN_DEATH_KNIGHT, &gr, GID_AHKULAN_GUARDIAN, AIF_GUARD_ARIAL)->xai->SetEnemyClass((CREATURE_CLASS)(CR_ALL ^ (CR_HUMAN | CR_HUMANOID)));
	}

	{
		XRect gr(49, 10, 50, 11);
		NewCreature(CN_DEATH_KNIGHT, &gr, GID_AHKULAN_GUARDIAN, AIF_GUARD_ARIAL)->xai->SetEnemyClass((CREATURE_CLASS)(CR_ALL ^ (CR_HUMAN | CR_HUMANOID)));
	}

	XRect gr(50, 9, 51, 10);
	NewCreature(CN_AHKULAN, &gr, GID_AHKULAN_GUARDIAN, AIF_GUARD_ARIAL)->xai->SetEnemyClass((CREATURE_CLASS)(CR_ALL ^ (CR_HUMAN | CR_HUMANOID)));

	new XAltar(43, 9, D_DEATH, this);
}

