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

#include "xarchive.h"
#include "game.h"
#include "quest.h"
#include "xtime.h"
#include "xherb.h"

const unsigned int SAVE_GAME_VERSION = 0x0000045;
const unsigned int SAVE_GAME_CONTROL = 0x9ABCDEF;

int XArchive::StoreGame()
{
	XFile file;
	XLocation::svg_file = &file;
	if (!file.Open(vMakePath(HOME_DIR, "avanor.svg"), "wb"))
		return 0;
	if (!file.Write((void *)&SAVE_GAME_VERSION, sizeof(unsigned int)))
		return 0;

	XObject::StoreAllObjects(&file);

	file.Write(&::guid, sizeof(XGUID));
	XQuest::quest.Store(&file);
	XBook::StoreTable(&file);
	XPotion::StoreTable(&file);
	XScroll::StoreTable(&file);
	XAmulet::StoreTable(&file);
	XRing::StoreTable(&file);
	XTime::Store(&file);
	XAlchemy::Store(&file);
	_HERBS::Store(&file);
	file.Write(&XGame::hero_guid, sizeof(int));
	Game.Scheduler.Store(&file);

	for (int i = 0; i < L_EOF; i++)
      Game.locations[i].Store(&file);

	XObject::StorePointer(&file, XCreature::main_creature);
	unsigned int tmp = SAVE_GAME_CONTROL;
	file.Write(&tmp, sizeof(unsigned int));
	file.Close();
	XObject::FreeTable();
	return 1;
}

int XArchive::RestoreGame()
{
	XFile file;
	XLocation::svg_file = &file;

	if (!file.Open(vMakePath(HOME_DIR, "avanor.svg"), "rb"))
	{
		return 0;
	}
	unsigned int tmp = 0;
	file.Read(&tmp, sizeof(unsigned int));
	if (tmp != SAVE_GAME_VERSION)
	{
		return 0;
	}

	XObject::RestoreAllObjects(&file);

	file.Read(&::guid, sizeof(XGUID));
	XQuest::quest.Restore(&file);
	XBook::RestoreTable(&file);
	XPotion::RestoreTable(&file);
	XScroll::RestoreTable(&file);
	XAmulet::RestoreTable(&file);
	XRing::RestoreTable(&file);
	XTime::Restore(&file);
	XAlchemy::Restore(&file);
	_HERBS::Restore(&file);
	file.Read(&XGame::hero_guid, sizeof(int));
	Game.Scheduler.Restore(&file);

	for (int i = 0; i < L_EOF; i++)
      Game.locations[i].Restore(&file);

	XCreature::main_creature = (XCreature *)XObject::RestorePointer(&file, NULL);
	file.Read(&tmp, sizeof(unsigned int));
	if (tmp != SAVE_GAME_CONTROL)
	{
		printf("File corrupted!");
		exit(0);
	}
	file.Close();
	XObject::FreeTable();
	return 1;
}
