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

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <cereal/types/map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/vector.hpp>
#include <sol/forward.hpp>

#include "creature/creature.h"

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

        // Registers this enum as the Lua table XQuest.MEMBER, and the
        // XQuestState usertype (see RegisterLua's definition) as the Lua
        // global QuestState, aliasing the singleton `quest` below.
        static void RegisterLua(sol::state_view& lua);

        XQuest()
        {
            hero_die = 0;
            hero_win = 0;
        };

        std::vector<std::unique_ptr<XQuestRec>> quests;

        void ShowQuests();

        // Generic named quest-progress flags - content (world/*.lua) owns
        // the names and meaning entirely; C++ never branches on a specific
        // key. Replaces what used to be a fixed set of hand-named int
        // fields (roderick_quest, ahk_ulan_killed, etc.), one per quest,
        // that required a C++ recompile to add a new quest.
        int GetFlag(const std::string& name) const;
        void SetFlag(const std::string& name, int value);

        // The hero's win/loss condition gates the main game loop
        // (game/game.cpp's turn loop checks these every turn), so they stay
        // dedicated fields rather than flags-map lookups. hero_die is only
        // ever set from C++ (hero death detection); hero_win is set from
        // Lua dialogue via WinGame().
        int hero_die;
        int hero_win;

        // Ends the game as a win. The message is how the story finished, in
        // the words of whoever ended it - it heads the achievements screen
        // and stands as the epitaph in the high score table, exactly where a
        // death message would.
        void WinGame(const std::string& msg);

        // Generic named creature references, for quests that need to track
        // a specific live object rather than just a scalar flag (e.g.
        // Rotmoth/Giana's kidnapped-girl companion quest) - same idea as
        // the flags map above, just weak_ptr-valued instead of int-valued.
        // Returns nullptr if never set or the referenced creature is gone.
        XCreature* GetCreatureRef(const std::string& name) const;
        void SetCreatureRef(const std::string& name, XCreature* cr);

        static XQuest quest;

        // The quest registered under an id, or nullptr for one no world
        // declares.
        static XQuestRec* Find(const std::string& id);

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(quests);
            ar(flags);
            ar(creature_refs);
            ar(hero_die, hero_win);
        }

    private:
        std::map<std::string, int> flags;
        std::map<std::string, std::weak_ptr<XCreature>> creature_refs;
};

struct XQuestRec {
    // What the quest log shows while the quest is KNOWN, and what the
    // achievements screen shows once it is COMPLETE (the deed done) or
    // CLOSED (done and reported). Either may be empty, in which case that
    // state simply says nothing. All three are written by content, from
    // Quest() in world/quests.lua - the engine never composes quest text.
    std::string know;
    std::string complete;
    std::string closed;

    // What finishing this quest is worth on the achievements screen.
    // Awarded once, on either COMPLETE or CLOSED, so a hero who did the
    // deed but never went back to be thanked still gets the credit.
    int score;

    XQuest::Id status;

    // Which quest this is - the id world/quests.lua registered it under.
    // A string, like every other content id: it is written into saved
    // games, and a hand-kept numbering could be renumbered underneath
    // one, silently re-pointing a hero's progress at another quest.
    std::string quest_id;

    template<class Archive>
    void serialize(Archive& ar)
    {
        ar(know, complete, closed, score, status, quest_id);
    }
};

#endif
