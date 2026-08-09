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

#ifndef QUEST_H
#define QUEST_H

#include <memory>
#include <string>
#include <vector>

#include <cereal/types/memory.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <sol/forward.hpp>

#include "creature/creature.h"
#include "item/itemdef.h"

class XCreature;
struct XQuestRec;

class XQuest
{
    public:
        enum Id {
            UNKNOWN	= 0,
            KNOWN	= 1,
            COMPLETE	= 2,
            CLOSED	= 3,
            FAIL	= 4,
        };

        // Registers this enum as the Lua table XQuest.MEMBER.
        static void RegisterLua(sol::state_view& lua);

        XQuest()
        {
            beelzvile_killed = 0;
            beelzvile_ordered = 0;
            hero_die = 0;
            hero_win = 0;
            orcs_killed = 0;
            total_orcs_killed = 0;
            guards_get_orc_slay = 0;
            yohjishiro_it_quest = IT_UNKNOWN;
            ahk_ulan_ordered = 0;
            ahk_ulan_killed = 0;
            ahk_ulan_quest = 0;
            roderick_ordered = 0;
            roderick_killed = 0;
            roderick_quest = 0;
            roderick_quest2 = 0;
            torin_quest = 0;
            rotmoth_status = 0;
        };

        std::vector<std::unique_ptr<XQuestRec>> quests;

        void ShowQuests();

        int beelzvile_killed;
        int beelzvile_ordered;

        int ahk_ulan_ordered;
        int ahk_ulan_killed;

        int ahk_ulan_quest;

        int roderick_ordered;
        int roderick_killed;
        int roderick_quest;
        int roderick_quest2;

        int orcs_killed;
        int total_orcs_killed;

        int guards_get_orc_slay;

        int torin_quest;

        ITEM_TYPE yohjishiro_it_quest;

        std::weak_ptr<XCreature> kidnapped_girl;
        int rotmoth_status; // 0 - initial, 1 - killed, 2 - payed;
        int kidnapped_girl_status;

        static XQuest quest;

        int hero_die;
        int hero_win;

        static void Take(int id);
        static void Complete(int id);
        static void Close(int id);
        static Id Status(int id);
        static XQuestRec* Find(int id);

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(quests);
            ar(beelzvile_killed, beelzvile_ordered);
            ar(ahk_ulan_ordered, ahk_ulan_killed, ahk_ulan_quest);
            ar(roderick_ordered, roderick_killed, roderick_quest, roderick_quest2);
            ar(orcs_killed, total_orcs_killed);
            ar(guards_get_orc_slay);
            ar(torin_quest);
            ar(yohjishiro_it_quest);
            ar(kidnapped_girl);
            ar(rotmoth_status, kidnapped_girl_status);
            ar(hero_die, hero_win);
        }
};

struct XQuestRec {
    std::string know;
    std::string complete;
    std::string closed;
    XQuest::Id status;
    int quest_id;

    template<class Archive>
    void serialize(Archive& ar)
    {
        ar(know, complete, closed, status, quest_id);
    }
};

#endif
