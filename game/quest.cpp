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

#include <fmt/format.h>
#include <sol/sol.hpp>

#include "engine/global.h"
#include "game/quest.h"
#include "helpers/xgui.h"

void XQuest::RegisterLua(sol::state_view& lua)
{
    lua.new_enum("XQuest",
        "UNKNOWN", XQuest::UNKNOWN,
        "KNOWN", XQuest::KNOWN,
        "COMPLETE", XQuest::COMPLETE,
        "CLOSED", XQuest::CLOSED,
        "FAIL", XQuest::FAIL
    );
}

XQuest XQuest::quest;

void XQuest::ShowQuests()
{
    XGuiList list;

    list.SetCaption(MSG_BROWN "### " MSG_YELLOW "Current Quests" MSG_BROWN " ###");
    int flag = 1;

    for (auto& quest: quests) {
        if (quest->status == XQuest::KNOWN) {
            list.AddItem(new XGuiItem_Text(quest->know.c_str()));
            flag = 0;
        }
    }

    if (ahk_ulan_ordered) {
        list.AddItem(new XGuiItem_Text(
            "Gefeon asked you to kill Ahk-Ulan."));
        flag = 0;
    }

    if (roderick_ordered) {
        list.AddItem(new XGuiItem_Text(
            "Ahk-Ulan asked you to kill Roderick."));
        flag = 0;
    }

    if (roderick_quest == 1) {
        list.AddItem(new XGuiItem_Text(
            "Roderick, the King of Avanor has asked you to find an artifact called the 'Eye of Raa'"));
        flag = 0;
    }

    if (roderick_quest2 == 1) {
        list.AddItem(new XGuiItem_Text(
            "Roderick, the King of Avanor has asked you to cleanse his family crypt."));
        flag = 0;
    }

    if (ahk_ulan_quest) {
        std::string quest_str = fmt::format(
            "Ahk-Ulan asked you to bring 3 parts of ancient machine. There are {} more parts left.",
            4 - ahk_ulan_quest);
        list.AddItem(new XGuiItem_Text(quest_str));
        flag = 0;
    }

    if (yohjishiro_it_quest != IT_UNKNOWN) {
        if (yohjishiro_it_quest == IT_RATTAIL) {
            list.AddItem(new XGuiItem_Text("Yohjishiro asked you bring a rat tail."));
        } else {
            list.AddItem(new XGuiItem_Text("Yohjishiro asked you bring a bat wing."));
        }

        flag = 0;
    }

    if (flag) {
        list.AddItem(new XGuiItem_Text("You have no quests."));
    }

    list.Run();
}

void XQuest::Take(int id)
{
    XQuestRec * qr = Find(id);
    qr->status = XQuest::KNOWN;
}

void XQuest::Complete(int id)
{
    XQuestRec * qr = Find(id);
    qr->status = XQuest::COMPLETE;
}

void XQuest::Close(int id)
{
    XQuestRec * qr = Find(id);
    qr->status = XQuest::CLOSED;
}

XQuest::Id XQuest::Status(int id)
{
    XQuestRec * qr = Find(id);
    return qr->status;
}

XQuestRec* XQuest::Find(const int id)
{
    for (const auto& it: XQuest::quest.quests) {
        if (it->quest_id == id) {
            return it.get();
        }
    }

    return nullptr;
}
