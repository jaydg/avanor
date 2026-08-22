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

#ifndef PATTERN_BUILDER_H
#define PATTERN_BUILDER_H

#include "map/map.h"

class XLocation;

// The blank map a hand-drawn level's pattern is painted on: exactly the
// size asked for, every cell `fill`, and nothing invented. What the level
// looks like is then entirely the script's own pattern (XPattern, which
// this builder only makes room for - it does not draw it), which is why a
// level built this way is as large as it draws and no larger.
//
// `fill` is what remains wherever the pattern draws nothing, so it wants
// to be something solid - the rock a vault is cut out of, say. It is
// never the tile a level shows by accident: tile id 0 would be, and that
// one is walkable and see-through.
class XPatternBuilder
{
    public:
        XPatternBuilder(XLocation* _location, int _w, int _h, XTileType::Id _fill)
            : location(_location), w(_w), h(_h), fill(_fill)
        {
        }

        void Build() const;

    private:
        XLocation* location;
        int w, h;
        XTileType::Id fill;
};

#endif
