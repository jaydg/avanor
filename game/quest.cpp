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

#include "quest.h"
#include "xfile.h"
#include "global.h"
#include "xgui.h"

XQuest XQuest::quest;

void XQuest::Store(XFile * f)
{
	f->Write(&beelzvile_killed, sizeof(int));
	f->Write(&beelzvile_ordered, sizeof(int));
	f->Write(&hero_die, sizeof(int));
	f->Write(&hero_win, sizeof(int));
	f->Write(&orcs_killed, sizeof(int));
	f->Write(&total_orcs_killed, sizeof(int));
	f->Write(&gurads_get_orc_slay, sizeof(int));
	f->Write(&yohjishiro_it_quest, sizeof(ITEM_TYPE));
	f->Write(&ahk_ulan_ordered, sizeof(int));
	f->Write(&ahk_ulan_killed, sizeof(int));
	f->Write(&ahk_ulan_quest);
	f->Write(&roderick_ordered);
	f->Write(&roderick_killed);
	f->Write(&roderick_quest);
	f->Write(&roderick_quest2);
	f->Write(&torin_quest);
}

void XQuest::Restore(XFile * f)
{
	f->Read(&beelzvile_killed, sizeof(int));
	f->Read(&beelzvile_ordered, sizeof(int));
	f->Read(&hero_die, sizeof(int));
	f->Read(&hero_win, sizeof(int));
	f->Read(&orcs_killed, sizeof(int));
	f->Read(&total_orcs_killed, sizeof(int));
	f->Read(&gurads_get_orc_slay, sizeof(int));
	f->Read(&yohjishiro_it_quest, sizeof(ITEM_TYPE));
	f->Read(&ahk_ulan_ordered, sizeof(int));
	f->Read(&ahk_ulan_killed, sizeof(int));
	f->Read(&ahk_ulan_quest);
	f->Read(&roderick_ordered);
	f->Read(&roderick_killed);
	f->Read(&roderick_quest);
	f->Read(&roderick_quest2);
	f->Read(&torin_quest);
}

void XQuest::ShowQuests()
{
	XGuiList list;

	list.SetCaption(MSG_BROWN "### " MSG_YELLOW "Current Quests" MSG_BROWN " ###");
	int flag = 1;
	if (beelzvile_ordered && !beelzvile_killed)
	{
		list.AddItem(new XGuiItem_Text(
			"The Village Elder asked you to kill the demon who atttacks villagers "
			"and has occupied the caves to the west of the village.\n\n"));
		flag = 0;
	}

	if (ahk_ulan_ordered)
	{
		list.AddItem(new XGuiItem_Text(
			"Gefeon asked you to kill Ahk-Ulan."));
		flag = 0;
	}

	if (roderick_ordered)
	{
		list.AddItem(new XGuiItem_Text(
			"Ahk-Ulan asked you to kill Roderick."));
		flag = 0;
	}

	if (roderick_quest == 1)
	{
		list.AddItem(new XGuiItem_Text(
			"Roderick, the King of Avanor asked you to find artifact called 'Eye of Raa'"));
		flag = 0;
	}

	if (roderick_quest2 == 1)
	{
		list.AddItem(new XGuiItem_Text(
			"Roderick, the King of Avanor asked you to cleanse his family crypt."));
		flag = 0;
	}


	if (ahk_ulan_quest)
	{
		char buf[256];
		sprintf(buf, "Ahk-Ulan asked you to bring 3 parts of ancient machine. You left to bring %d.", 4 - ahk_ulan_quest);
		list.AddItem(new XGuiItem_Text(buf));
		flag = 0;
	}

	if (torin_quest == 1)
	{
		list.AddItem(new XGuiItem_Text(
			"Torin, the Dwarven King asked you to switch on gas pump at the bottom of gold mine."));
		flag = 0;
	}




	if (yohjishiro_it_quest != IT_UNKNOWN)
	{
		if (yohjishiro_it_quest == IT_RATTAIL)
			list.AddItem(new XGuiItem_Text("Yohjishiro asked you bring a rat tail."));
		else
			list.AddItem(new XGuiItem_Text("Yohjishiro asked you bring a bat wing."));
		flag = 0;
	}



	if (flag)
	{
		list.AddItem(new XGuiItem_Text("You have no quests."));
	}

	list.Run();
}

