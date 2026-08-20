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

#ifndef DUNGEON_BUILDER_H
#define DUNGEON_BUILDER_H

#include <memory>
#include <vector>

#include "helpers/point.h"
#include "helpers/rect.h"
#include "map/map.h"
#include "map/pattern.h"

class XMap;
class XLocation;

// A room the dungeon generator can stamp into a level: a pattern and the
// palette that resolves it - exactly what a hand-built location uses -
// plus how often it should turn up relative to the other rooms, and an
// optional hook run once it has been drawn (to post its own guards, say).
//
// A border cell holding '+' or '.' is where a corridor may attach, which
// is the one glyph convention the generator itself relies on.
struct RoomTemplate {
    XPattern pattern;
    sol::protected_function on_drawn;
    int weight;

    [[nodiscard]] bool isExit(int x, int y) const;
};

// Every room the world script has defined through DefineRoom(). The
// engine ships none of its own, so an empty registry simply means levels
// get only the plain rectangular rooms the generator makes up.
extern std::vector<RoomTemplate> room_templates;

// Picks a room by weight, or null when the script defined none.
const RoomTemplate* PickRoom();

// One room of a generated dungeon level: where it sits, where a corridor
// may attach to it, and how it puts itself on the map. Either a plain
// rectangle the generator made up, or one of the rooms above.
class XRoom
{
        // The template this room was stamped from, or null for one of the
        // generator's own plain rectangles.
        const RoomTemplate* room;
        int map_len;
        int map_hgt;
    public:
        XRect r;

        // Border cells a corridor may be dug to. Consumed as they are
        // used, so what is left over are the ways in nothing took.
        std::vector<XPoint> exits;

        XRoom(int len, int hgt, const RoomTemplate* room);

        ~XRoom() { }

        [[nodiscard]] bool isTemplateRoom() const
        {
            return room != nullptr;
        }

        int Intersect(XRoom * other, int dist);
        void Draw(XLocation * l, XTileType::Id floor);
        bool GetFreeExit(XPoint * pt);
};

// Builds a level out of rooms joined by corridors.
class XDungeonBuilder
{
        bool isCreateDoorTrapChest;

        // The chance in a hundred that this level is built with one of the
        // rooms the world script defined. Per level setting allows variety.
        int room_chance;

        // What the level is cut out of, and what is left where it is cut.
        XTileType::Id wall;
        XTileType::Id floor;

        // How much floor each room is worth - a level gets its area
        // divided by this many rooms - and how often a corridor junction
        // gets a door, one time in door_odds.
        int cells_per_room;
        int door_odds;
    public:
        XMap* m;
        XLocation* location;
        XDungeonBuilder(XLocation * _l, int _w, int _h, XTileType::Id _wall, XTileType::Id _floor,
                        int _room_chance, int _cells_per_room, int _door_odds,
                        int create_door_trap_chest = 1);
        void Build();
        bool Link(XPoint * p1, XPoint * p2);
        void CreateDoors();

        [[nodiscard]] XTileType::Id Floor() const
        {
            return floor;
        }

    private:
        // Draws room (null for a plain one) at a random free spot, giving
        // up after `attempts` tries. Returns whether it went down.
        bool PlaceRoom(std::vector<std::unique_ptr<XRoom>>& placed, const RoomTemplate* room, int attempts);

        // Digs from a template room's unused door out to the nearest floor
        // that is not part of that room.
        bool DigOut(const XPoint& door, const XRect& room);
};

#endif
