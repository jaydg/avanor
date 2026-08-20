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

#ifndef PLAIN_BUILDER_H
#define PLAIN_BUILDER_H

#include <vector>

#include "map/map.h"

class XLocation;

// Open country: `ground` scattered with `cover`, ringed by a border that
// erodes inward, so the map has an edge the player cannot walk off.
//
// `slope` is that border from the inside out - the gentlest tile first,
// the impassable one last. The ring is raised to the last of them and
// each step outward from a cell drops one rung, which is how a wall of
// mountains turns into foothills. Two rungs make a cliff; six make a
// long climb.
class XPlainBuilder
{
    public:
        XPlainBuilder(XLocation* _location, int _w, int _h, XTileType::Id _ground, XTileType::Id _cover,
                      std::vector<XTileType::Id> _slope)
            : location(_location), w(_w), h(_h), ground(_ground), cover(_cover), slope(std::move(_slope))
        {
        }

        void Build();

    private:
        // Where a tile sits on the slope, or -1 for anything that is not
        // part of it - lower than every rung, so it always gives way.
        [[nodiscard]] int Rung(XTileType::Id tile) const;

        XLocation* location;
        int w, h;
        XTileType::Id ground;
        XTileType::Id cover;
        std::vector<XTileType::Id> slope;
};

#endif
