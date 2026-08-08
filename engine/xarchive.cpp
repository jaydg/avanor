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

#include <fstream>

#include <cereal/archives/json.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/polymorphic.hpp>

#include "engine/xarchive.h"
#include "game/game.h"
#include "game/quest.h"
#include "game/xtime.h"
#include "item/xamulet.h"
#include "item/xbook.h"
#include "item/xherb.h"
#include "item/xpotion.h"
#include "item/xring.h"
#include "item/xscroll.h"

constexpr unsigned int SAVE_GAME_VERSION = 0x0000047;
constexpr unsigned int SAVE_GAME_CONTROL = 0x9ABCDEF;

int XArchive::StoreGame()
{
    std::ofstream file(vMakePath(HOME_DIR, "avanor.svg"));

    if (!file.is_open()) {
        return 0;
    }

    {
        cereal::JSONOutputArchive ar(file);

        ar(SAVE_GAME_VERSION);
        ar(::guid);

        for (auto& loc : Game.locations) {
            ar(loc);
        }

        ar(XQuest::quest);

        XBook::SaveTable(ar);
        XPotion::SaveTable(ar);
        XScroll::SaveTable(ar);
        XAmulet::SaveTable(ar);
        XRing::SaveTable(ar);
        _HERBS::SaveTable(ar);

        XTime::serialize(ar);

        ar(XGame::hero_guid);
        ar(Game.Scheduler);

        // main_creature is a raw XCreature* (used pervasively as one
        // throughout gameplay code, not worth converting) - saved as a
        // weak_ptr so it resolves via the same shared_ptr identity
        // tracking as everything else in this archive. Must come after
        // Game.locations above: that's what actually registers this
        // creature's shared_ptr id with Cereal.
        ar(XCreature::ToWeakPtr(XCreature::main_creature));

        ar(SAVE_GAME_CONTROL);
    }

    return 1;
}

int XArchive::RestoreGame()
{
    std::ifstream file(vMakePath(HOME_DIR, "avanor.svg"));

    if (!file.is_open()) {
        return 0;
    }

    try {
        cereal::JSONInputArchive ar(file);

        unsigned int version = 0;
        ar(version);

        if (version != SAVE_GAME_VERSION) {
            return 0;
        }

        ar(::guid);

        for (auto& loc : Game.locations) {
            ar(loc);
        }

        ar(XQuest::quest);

        XBook::LoadTable(ar);
        XPotion::LoadTable(ar);
        XScroll::LoadTable(ar);
        XAmulet::LoadTable(ar);
        XRing::LoadTable(ar);
        _HERBS::LoadTable(ar);

        XTime::serialize(ar);

        ar(XGame::hero_guid);
        ar(Game.Scheduler);

        std::weak_ptr<XCreature> main_creature_weak;
        ar(main_creature_weak);
        XCreature::main_creature = main_creature_weak.lock().get();

        unsigned int control = 0;
        ar(control);

        if (control != SAVE_GAME_CONTROL) {
            printf("File corrupted!");
            exit(0);
        }
    } catch (const cereal::Exception&) {
        // Malformed/foreign/truncated file - same graceful "nothing to
        // load" outcome as the version check above, not a hard failure.
        return 0;
    }

    return 1;
}
