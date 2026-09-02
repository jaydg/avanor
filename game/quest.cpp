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

#include <sol/sol.hpp>

#include "creature/xhero.h"
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

    lua.new_usertype<XQuest>("XQuestState",
        "GetFlag", &XQuest::GetFlag,
        "SetFlag", &XQuest::SetFlag,
        "WinGame", &XQuest::WinGame,
        "GetCreatureRef", &XQuest::GetCreatureRef,
        "SetCreatureRef", &XQuest::SetCreatureRef
    );

    lua["QuestState"] = &XQuest::quest;
}

XQuest XQuest::quest;

int XQuest::GetFlag(const std::string& name) const
{
    auto it = flags.find(name);
    return it != flags.end() ? it->second : 0;
}

void XQuest::SetFlag(const std::string& name, int value)
{
    flags[name] = value;
}

void XQuest::WinGame(const std::string& msg)
{
    hero_win = 1;
    XHero::EndGame(msg.c_str());
}

XCreature* XQuest::GetCreatureRef(const std::string& name) const
{
    auto it = creature_refs.find(name);
    return it != creature_refs.end() ? it->second.lock().get() : nullptr;
}

void XQuest::SetCreatureRef(const std::string& name, XCreature* cr)
{
    creature_refs[name] = XCreature::ToWeakPtr(cr);
}

void XQuest::ShowQuests()
{
    XGuiList list;

    list.SetCaption("<DECORATION>### <VALUE>Current Quests<DECORATION> ###");
    int flag = 1;

    for (auto& quest: quests) {
        if (quest->status == XQuest::KNOWN) {
            list.AddItem(new XGuiItem_Text(quest->know));
            flag = 0;
        }
    }

    if (flag) {
        list.AddItem(new XGuiItem_Text("You have no quests."));
    }

    list.Run();
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
