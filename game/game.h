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

#ifndef GAME_H
#define GAME_H

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>

#include "creature/xhero.h"
#include "game/location.h"
#include "engine/xscheduler.h"

class XGame
{
        void CreateLocations() const;
        void CreateHero() const;

        bool demo = false;

    public:
        XGame();
        ~XGame();

        static void Run();
        void RunWithoutHero() const;
        static void RunDemo();
        // Sets the world up for a new, restored, test or demo game.
        // False means the world is not playable and Run() must not be
        // called on it - see the 'R' case.
        [[nodiscard]] bool Create(char type_of_start);

        // Whether this game is running itself for show: there is no hero,
        // and the view follows whichever creature stands in for one (see
        // XCreature::main_creature and SetMainCreature()). Set by
        // Create('D') and asked about wherever the hero's own view would
        // otherwise be drawn.
        [[nodiscard]] bool isDemo() const
        {
            return demo;
        }
        XCreature* NewCreature(XCreature * cr, int x, int y, XLocation * loc);
        // Keyed by XLocation::id.
        //
        // Reach for Location() rather than operator[]: subscripting a map
        // would insert an empty entry for an id that does not exist, where
        // the array simply held a null.
        std::unordered_map<std::string, std::shared_ptr<XLocation>> locations;

        // The location with this id, or nullptr when there is none.
        [[nodiscard]] std::shared_ptr<XLocation> Location(const std::string& id) const
        {
            const auto it = locations.find(id);

            return it != locations.end() ? it->second : nullptr;
        }

        // Where a new hero appears, named by the world script through
        // SetStartLocation(). start_area is the part of that location the
        // hero may turn up in; unset means anywhere in it. Both are world
        // definition rather than game state, so neither is saved - a
        // restored game has its hero already.
        static std::string start_location;
        static std::optional<XRect> start_area;

        XScheduler Scheduler;
        static int current_location;
        static XGUID hero_guid;
        static bool isGodMode;

        // God mode's map reveal
        static bool isMapRevealed;
};

extern XGame Game;

#endif
