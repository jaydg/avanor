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

#include <algorithm>
#include <ctime>
#include <fstream>
#include <string>
#include <vector>

#include <fmt/format.h>

#include "engine/xarchive.h"
#include "game/game.h"
#include "game/quest.h"
#include "game/setting.h"
#include "helpers/msgwin.h"
#include "item/xherb.h"
#include "map/map_objects.h"

#ifndef XLINUX
    #include <conio.h>
#endif

bool XGame::isGodMode = false;
XGUID XGame::hero_guid = 0;
int XGame::current_location = 0;

XGame::XGame()
{
    // Nothing to clear - an empty map already means "no locations".
}

XGame::~XGame()
{
    // XObject::InvalidateAllObjects() (called from Run() before exiting) has
    // already invalidated every location. Deletion itself is now entirely
    // shared_ptr-driven: dropping this map's reference deletes a location
    // only once every other reference to it (the Scheduler's weak_ptr
    // doesn't count) is also gone.
    locations.clear();
}

XCreature* XGame::NewCreature(XCreature * cr, const int x, const int y, XLocation* loc)
{
    cr->FirstStep(x, y, loc);
    Scheduler.Add(cr);
    return cr;
}

//////////////////////////////////////////////////////////////////////////////
// Create all the necessary objects in memory depending on user's choice    //
//////////////////////////////////////////////////////////////////////////////

void XGame::Create(const char type_of_start) const
{
    switch (type_of_start) {
        case 'R' :
            vClrScr();
            vGotoXY((size_x - strlen("Restoring game objects, please wait...")) / 2, size_y / 2);
            vPutS(MSG_LIGHTGRAY "Restoring game objects, please wait...");
            vRefresh();
            XLocation::Restoration();

            if (XArchive::RestoreGame()) {
                break;
            }

            vGotoXY(0, 20);
            vPutS(MSG_YELLOW "There is not a saved game to load. Starting new game.");
            vGotoXY(0, 21);
            vPutS(MSG_CYAN "Press any key...");
            vRefresh();
            vGetch();

        case 'N' :
            XAlchemy::Init();
            PlantDefinition::Create();
            vClrScr();
            vGotoXY((size_x - strlen("Generating game objects, please wait...")) / 2, size_y / 2);
            vPutS(MSG_LIGHTGRAY "Generating game objects, please wait...");
            vRefresh();
            CreateLocations();
            CreateHero();
            break;

        case 'T' :
            XAlchemy::Init();
            PlantDefinition::Create();
            vClrScr();
            vGotoXY((size_x - strlen("Preparing for test, please wait...")) / 2, size_y / 2);
            vPutS(MSG_LIGHTGRAY "Preparing for test, please wait...");
            vRefresh();

            CreateLocations();
            break;

        case 'D' :
            XAlchemy::Init();
            PlantDefinition::Create();
            vClrScr();
            vGotoXY((size_x - strlen("Preparing for demo, please wait...")) / 2, size_y / 2);
            vPutS(MSG_LIGHTGRAY "Preparing for demo, please wait...");
            vRefresh();
            XSettings::isDemo = 1;

            CreateLocations();
            break;
    }
}

void XGame::RunDemo()
{
    while (true) {
        for (int i = 0; i < 100; i++) {
            Game.Scheduler.Get()->Run();
            XObject::DrainDeferred();
        }

        if (vKbhit()) {
            const int ch = vGetch();

            if (ch == KEY_ESC) {
                break;
            }

            if (ch == ' ') {
                vGetch();
            }

            if (ch == 'M') {
                msgwin.ShowHistory();
            }

        }
    }

    XObject::InvalidateAllObjects();
}

//////////////////////////////////////////////////////////////////////////////
// Run the game without hero for detecting bugs or benchmarking             //
//////////////////////////////////////////////////////////////////////////////

void XGame::RunWithoutHero() const
{
    clock_t start_clock = clock();

    while (true) {
        for (int i = 0; i < 1000; i++) {
            auto o = Game.Scheduler.Get();
            o->Run();
            XObject::DrainDeferred();
        }

        if (vKbhit()) {
            int ch = vGetch();

            if (ch == KEY_ESC) {
                break;
            }

            if (ch == 'L') {
                std::ofstream f(vMakePath(HOME_DIR, "location.txt"));

                for (const auto& [key, location] : locations) {
                    if (location) {
                        location->DumpLocation(f);
                    }
                }
            }

            if (ch == 'I') {
                struct TMP {
                    XItem* pI;
                    int val;
                };

                std::vector<TMP> ia;

                for (const auto& [key, pItem] : XObject::objects) {
                    if (auto* it = dynamic_cast<XItem*>(pItem); it && it->kind & ItemKind::ITEM) {
                        TMP entry{};
                        entry.pI = it;
                        entry.val = entry.pI->GetValue();
                        ia.push_back(entry);
                    }
                }

                // sort by value (descending)
                std::sort(ia.begin(), ia.end(), [](const TMP& a, const TMP& b) {
                    return a.val > b.val;
                });

                // dump it
                std::ofstream f(vMakePath(HOME_DIR, "items.txt"));

                for (const auto&[pI, val] : ia) {
                    if (pI->kind & (ItemKind::VALUEDICE | ItemKind::ARMOUR)) {
                        pI->Identify(1);
                        f << fmt::format("{:<70}{}\n", pI->toString(), val);
                    }
                }
            }
        }

        vClrScr();
        vGotoXY(0, 0);

        std::string status = fmt::format(
            MSG_YELLOW "Testing Avanor - running game without hero ... (press ESC to stop)\n\n"
            MSG_LIGHTGRAY
            "Number of valid objects   : {}\n"
            "Number of invalid objects : {}\n"
            "\n"
            "Number of creatures       : {}\n"
            "Number of items           : {}\n"
            "\n"
            "Turns                     : {}\n"
            "Performance               : {:.1f} turns/s",
            XObject::objects.size(),
            XObject::invalid_count,
            total_cr,
            total_it,
            Game.Scheduler.GetTime() / 1000,
            static_cast<double>(Game.Scheduler.GetTime()) * CLOCKS_PER_SEC
                / (1000. * (clock() - start_clock)));

        vPutS(status.c_str());
        vRefresh();
    }

    XObject::InvalidateAllObjects();
}


//////////////////////////////////////////////////////////////////////////////
// Run the game in ordinary mode                                            //
//////////////////////////////////////////////////////////////////////////////

void XGame::Run()
{
    vHideCursor();

    while (!_exit_flag && XQuest::quest.hero_win == 0 && XQuest::quest.hero_die == 0) {
        const auto o = Game.Scheduler.Get();

        if (!o->Run()) {
            // object is dead!
            Game.Scheduler.Remove();
        }

        // Between turns, nothing is mid-call on any game object - the
        // one safe point to actually release objects that evicted
        // themselves from the map during this turn (see
        // XObject::DeferRelease()).
        XObject::DrainDeferred();
    }

    XObject::InvalidateAllObjects();

    vClrScr();

    if (XQuest::quest.hero_win == 1) {
        vGotoXY(0, 0);
        vPutS("Congratulations! You are a winner!");
        vGotoXY(0, 1);
        vPutS("Congratulations! You are a winner!");
        vGotoXY(0, 2);
        vPutS("Congratulations! You are a winner!");
    } else if (XQuest::quest.hero_die == 1) {
        vGotoXY(30, 6);
        vPutS("       #       ");
        vGotoXY(30, 7);
        vPutS("     #####     ");
        vGotoXY(30, 8);
        vPutS("       #       ");
        vGotoXY(30, 9);
        vPutS("    #######    ");
        vGotoXY(30, 10);
        vPutS("   #       #   ");
        vGotoXY(30, 11);
        vPutS("   #  RIP  #   ");
        vGotoXY(30, 12);
        vPutS("   #       \x1F\x04@\x1F\x07   ");
        vGotoXY(30, 13);
        vPutS(" #####\x1F\x04@\x1F\x07####\x1F\x02|\x1F\x07## ");
        vGotoXY(30, 14);
        vPutS("\x1F\x02W\x1F\x07#\x1F\x02WWWWWWWWWWW\x1F\x07#\x1F\x02W");
    } else {
        vGotoXY(0, 0);
        vPutS("Goodbye!");
    }

    vRefresh();
    vGetch();
}

void XGame::CreateLocations() const
{
    //	Create locations
    XLocation::CreateNewGame();

    //	Bind ways
    for (const auto& [key, loc] : locations) {
        if (loc) {
            for (const auto it1: loc->ways_list) {
                auto way = dynamic_cast<XStairWay*>(it1);

                if (way->dest_x < 0 && way->dest_y < 0 && Location(way->ln)) {
                    for (const auto it2: Location(way->ln)->ways_list) {
                        auto tmp_way = dynamic_cast<XStairWay*>(it2);

                        if (tmp_way->dest_x < 0 && tmp_way->dest_y < 0 && tmp_way->ln == loc->ln) {
                            way->Bind(tmp_way);
                        }
                    }
                }
            }
        }
    }
}

void XGame::CreateHero() const
{
    XRect hero_rect(26, 4, 32, 9);

    // The one caller that cannot carry on without a cell: a starting
    // location with nowhere to stand is an unusable world, so let the
    // empty optional throw rather than inventing a coordinate.
    const XPoint hero_point = Location(XLocation::MAIN)->GetFreeXY(&hero_rect).value();

    const auto hero = new XHero(1);
    Game.NewCreature(hero, hero_point.x, hero_point.y, Location(XLocation::MAIN).get());
    hero->MoneyOp(2000);

    // If hero is a bard, then create a dog for them...
    if (strstr(hero->GetProfessionStr(), "bard")) {
        XRect tr(hero->x - 1, hero->y - 1, hero->x + 1, hero->y + 1);

        // Only the eight cells around the hero, which can legitimately all
        // be taken - then the bard simply starts without the dog.
        if (const auto dog_point = hero->l->GetFreeXY(&tr)) {
            const XCreature* cr = hero->l->NewCreature(CN_DOG, dog_point->x, dog_point->y);
            cr->xai->SetCompanion(hero);
            cr->xai->SetAIFlag(XStandardAI::ALLOW_MOVE_OUT);
            cr->xai->SetAIFlag(XStandardAI::PEACEFUL);
            cr->xai->SetEnemyClass(CreatureClass::KOBOLD | CreatureClass::GOBLIN | CreatureClass::UNDEAD | CreatureClass::INSECT | CreatureClass::BLOB | CreatureClass::CANINE | CreatureClass::FELINE | CreatureClass::RAT | CreatureClass::REPTILE | CreatureClass::ORC);
        }
    }

    XCreature::main_creature = hero;
}
