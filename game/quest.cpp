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
	int sz = quests.size();
	f->Write(&sz);
	for (XQList<QUEST_REC *>::iterator it = quests.begin(); it != quests.end(); it++)
	{
		int idx = (*it)->quest_id;
		f->Write(&idx);
		int status = (*it)->status;
		f->Write(&status);
		(*it)->know.Store(f);
		(*it)->closed.Store(f);
		(*it)->complete.Store(f);
	}
	f->Write(&beelzvile_killed);
	f->Write(&beelzvile_ordered);
	f->Write(&hero_die);
	f->Write(&hero_win);
	f->Write(&orcs_killed);
	f->Write(&total_orcs_killed);
	f->Write(&guards_get_orc_slay);
	f->Write(&yohjishiro_it_quest, sizeof(ITEM_TYPE));
	f->Write(&ahk_ulan_ordered);
	f->Write(&ahk_ulan_killed, sizeof(int));
	f->Write(&ahk_ulan_quest);
	f->Write(&roderick_ordered);
	f->Write(&roderick_killed);
	f->Write(&roderick_quest);
	f->Write(&roderick_quest2);
	f->Write(&torin_quest);
	f->Write(&rotmoth_status);
	XObject::StorePointer(f, (XObject *)kidnapped_girl.get());
}

void XQuest::Restore(XFile * f)
{
	int sz;
	f->Read(&sz);
	while (sz > 0)
	{
		QUEST_REC * qr = new QUEST_REC;
		int status;
		f->Read(&qr->quest_id);
		f->Read(&status);
		qr->status = (QUEST)status;
		qr->know.Restore(f);
		qr->closed.Restore(f);
		qr->complete.Restore(f);
		quests.push_back(qr);
		sz--;
	}

	f->Read(&beelzvile_killed, sizeof(int));
	f->Read(&beelzvile_ordered, sizeof(int));
	f->Read(&hero_die, sizeof(int));
	f->Read(&hero_win, sizeof(int));
	f->Read(&orcs_killed, sizeof(int));
	f->Read(&total_orcs_killed, sizeof(int));
	f->Read(&guards_get_orc_slay, sizeof(int));
	f->Read(&yohjishiro_it_quest, sizeof(ITEM_TYPE));
	f->Read(&ahk_ulan_ordered, sizeof(int));
	f->Read(&ahk_ulan_killed, sizeof(int));
	f->Read(&ahk_ulan_quest);
	f->Read(&roderick_ordered);
	f->Read(&roderick_killed);
	f->Read(&roderick_quest);
	f->Read(&roderick_quest2);
	f->Read(&torin_quest);
	f->Read(&rotmoth_status);
	kidnapped_girl = (XCreature *)(XObject::RestorePointer(f, NULL));
}

void XQuest::ShowQuests()
{
	XGuiList list;

	list.SetCaption(MSG_BROWN "### " MSG_YELLOW "Current Quests" MSG_BROWN " ###");
	int flag = 1;

	for (XQList<QUEST_REC *>::iterator it = quests.begin(); it != quests.end(); it++)
	{
		if ((*it)->status == Q_KNOWN)
		{
			list.AddItem(new XGuiItem_Text((*it)->know.c_str()));
			flag = 0;
		}
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
			"Roderick, the King of Avanor has asked you to find an artifact called the 'Eye of Raa'"));
		flag = 0;
	}

	if (roderick_quest2 == 1)
	{
		list.AddItem(new XGuiItem_Text(
			"Roderick, the King of Avanor has asked you to cleanse his family crypt."));
		flag = 0;
	}


	if (ahk_ulan_quest)
	{
		char buf[256];
		sprintf(buf, "Ahk-Ulan asked you to bring 3 parts of ancient machine. There are %d more parts left.", 4 - ahk_ulan_quest);
		list.AddItem(new XGuiItem_Text(buf));
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


void XQuest::Take(int id)
{
	QUEST_REC * qr = Find(id);
	qr->status = Q_KNOWN;
}

void XQuest::Complete(int id)
{
	QUEST_REC * qr = Find(id);
	qr->status = Q_COMPLETE;
}

void XQuest::Close(int id)
{
	QUEST_REC * qr = Find(id);
	qr->status = Q_CLOSED;
}

QUEST XQuest::Status(int id)
{
	QUEST_REC * qr = Find(id);
	return qr->status;
}

QUEST_REC * XQuest::Find(int id)
{
	for (XQList<QUEST_REC *>::iterator it = XQuest::quest.quests.begin(); it != XQuest::quest.quests.end(); it++)
	{
		if ((*it)->quest_id == id)
			return (*it);
	}
	return NULL;
}
