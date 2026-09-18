/*
This file is part of "Avanor, the Land of Mystery" roguelike game

Copyright (C) 2000-2006 Vadim Gaidukevich
Copyright (C) 2025,2026 Joachim de Groot

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
// to be something solid - the rock a vault is cut out of, say. A floor
// built over another level is the exception: there, nothing at all
// (XTileType::NONE) is the point, since every cell the pattern leaves
// alone is a hole the level below shows through.
class XPatternBuilder
{
    public:
        // `w` by `h` is the coordinate space the level lives in; the
        // level itself covers `part_w` by `part_h` of it, at (`at_x`,
        // `at_y`). For a level standing on its own those are the same
        // thing, and it starts at the origin.
        XPatternBuilder(XLocation* _location, int _w, int _h, int _at_x, int _at_y,
                        int _part_w, int _part_h, XTileType::Id _fill)
            : location(_location), w(_w), h(_h), at_x(_at_x), at_y(_at_y),
              part_w(_part_w), part_h(_part_h), fill(_fill)
        {
        }

        void Build() const;

    private:
        XLocation* location;
        int w, h;
        int at_x, at_y;
        int part_w, part_h;
        XTileType::Id fill;
};

#endif
