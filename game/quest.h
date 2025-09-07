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

#ifndef __QUEST_H
#define __QUEST_H

#include "creature/creature.h"
#include "helpers/xstr.h"
#include "item/itemdef.h"

class XFile;
class XCreature;

enum QUEST {
    Q_UNKNOWN	= 0,
    Q_KNOWN	= 1,
    Q_COMPLETE	= 2,
    Q_CLOSED	= 3,
    Q_FAIL	= 4,
};

struct QUEST_REC {
    XStr know;
    XStr complete;
    XStr closed;
    QUEST status;
    int quest_id;
};

class XQuest
{
    public:
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

        XQList<QUEST_REC*> quests;

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

        XPtr<XCreature> kidnapped_girl;
        int rotmoth_status; // 0 - initial, 1 - killed, 2 - payed;
        int kidnapped_girl_status;

        static XQuest quest;

        int hero_die;
        int hero_win;


        static void Take(int id);
        static void Complete(int id);
        static void Close(int id);
        static QUEST Status(int id);
        static QUEST_REC* Find(int id);

        void Store(XFile * f);
        void Restore(XFile * f);
};

#endif
