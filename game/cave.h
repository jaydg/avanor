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

#ifndef CAVE_H
#define CAVE_H

#include "engine/xobject.h"
#include "helpers/point.h"
#include "helpers/rect.h"
#include "game/location.h"
#include "map/map.h"

class XMap;

// A room the dungeon generator can stamp into a level: a pattern and the
// palette that resolves it - exactly what a hand-built location uses -
// plus how often it should turn up relative to the other rooms, and an
// optional hook run once it has been drawn (to post its own guards, say).
//
// A border cell holding '+' or '.' is where a corridor may attach, which
// is the one glyph convention the generator itself relies on.
struct RoomTemplate {
    LOCATION_PATTERN pattern;
    std::vector<PALETTE_MAP> translation;
    sol::protected_function on_drawn;
    int weight;

    [[nodiscard]] bool isExit(int x, int y) const;
};

// Every room the world script has defined through DefineRoom(). The
// engine ships none of its own, so an empty registry simply means levels
// get only the plain rectangular rooms the generator makes up.
extern std::vector<RoomTemplate> room_templates;

class XCave
{
        // The template this room was stamped from, or null for one of the
        // generator's own plain rectangles.
        const RoomTemplate* room;
        int map_len;
        int map_hgt;
    public:
        XRect r;
        std::vector<XPoint> exits;
        XCave(int len, int hgt, bool allow_special_rooms);
        int Intersect(XCave * xc, int dist);
        void Draw(XLocation * l);
        ~XCave() { }

        bool GetFreeExit(XPoint * pt);
};

#endif
