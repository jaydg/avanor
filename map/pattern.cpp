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

#include <vector>

#include "game/location.h"
#include "helpers/point.h"
#include "map/map.h"
#include "map/map_objects.h"
#include "map/pattern.h"

namespace {

// The characters a pattern gets for free, so most maps need no palette
// of their own. A translation added for the same character wins.
//
//   '.' green grass   ',' cave floor    ';' stone floor
//   '#' stone wall    '=' water         '&' tree
//   'X' fence         '+' closed door   '/' open door
//   '^' trap          ' ' left as it is
//
// The list is Avanor's own map alphabet, which is the one piece of this
// file that is content rather than mechanism.

// What a callback cell - and any character with no meaning at all - ends
// up standing on: whichever of these its neighbours suggest, latest in
// the list winning, so a door in a stone corridor gets stone floor and
// one in a field gets grass.
XTileType::Type best_fit_terrain_table[] = {
    XTileType::GREEN_GRASS,
    XTileType::SAND,
    XTileType::PATH,
    XTileType::ROAD,
    XTileType::CAVE_FLOOR,
    XTileType::STONE_FLOOR,
    XTileType::OBSIDIAN_FLOOR,
    XTileType::GOLDEN_FLOOR,
};

} // namespace

void XPattern::Draw(XLocation* location, int x, int y) const
{
    std::vector<XPoint> points_to_resolve;

    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            bool found_it = false;

            for (const auto& [glyph, tile, callback]: translations) {
                if (glyph == text[i * w + j]) {
                    if (callback.valid()) {
                        points_to_resolve.emplace_back(x + j, y + i);
                    } else {
                        location->map->SetXY(x + j, y + i, tile);
                    }

                    found_it = true;
                    break;
                }
            }

            if (!found_it) {
                switch (text[i * w + j]) {
                    case '+':
                        new XDoor(x + j, y + i, 0, location);
                        points_to_resolve.emplace_back(x + j, y + i);
                        break;

                    case '/':
                        new XDoor(x + j, y + i, 1, location);
                        points_to_resolve.emplace_back(x + j, y + i);
                        break;

                    case '^':
                        new XTrap(x + j, y + i, location);
                        points_to_resolve.emplace_back(x + j, y + i);
                        break;

                    case '.':
                        location->map->SetXY(x + j, y + i, XTileType::GREEN_GRASS);
                        break;

                    case ',':
                        location->map->SetXY(x + j, y + i, XTileType::CAVE_FLOOR);
                        break;

                    case ';':
                        location->map->SetXY(x + j, y + i, XTileType::STONE_FLOOR);
                        break;

                    case '#':
                        location->map->SetXY(x + j, y + i, XTileType::STONE_WALL);
                        break;

                    case '=':
                        location->map->SetXY(x + j, y + i, XTileType::WATER);
                        break;

                    case '&':
                        location->map->SetXY(x + j, y + i, XTileType::TREE);
                        break;

                    case 'X':
                        location->map->SetXY(x + j, y + i, XTileType::FENCE);
                        break;

                    case ' ':
                        break;

                    default:
                        points_to_resolve.emplace_back(x + j, y + i);
                        break;
                }
            }
        }
    }

    for (const auto pt: points_to_resolve) {
        int best_fit_index = 0;

        for (int q = -1; q <= 1; q++) {
            for (int w = -1; w <= 1; w++) {
                if (q != 0 || w != 0) {
                    XTileType::Type tm = location->map->GetXY(pt.x + q, pt.y + w);

                    for (int i = 0; i < std::size(best_fit_terrain_table); i++) {
                        if (best_fit_terrain_table[i] == tm && best_fit_index < i) {
                            best_fit_index = i;
                        }
                    }
                }
            }
        }

        location->map->SetXY(pt.x, pt.y, best_fit_terrain_table[best_fit_index]);

        for (const auto& tit: translations) {
            if (tit.glyph == text[(pt.y - y) * w + pt.x - x] && tit.callback.valid()) {
                tit.callback(pt.x, pt.y);
            }
        }
    }
}
