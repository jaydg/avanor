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

#include <vector>

#include "game/location.h"
#include "helpers/point.h"
#include "map/map.h"
#include "map/pattern.h"

std::vector<XPattern::Translation> XPattern::default_translations;
std::vector<XTileType::Id> XPattern::floor_priority;

void XPattern::SetDefaults(std::vector<Translation> defaults)
{
    default_translations = std::move(defaults);
}

void XPattern::SetFloorPriority(std::vector<XTileType::Id> floors)
{
    floor_priority = std::move(floors);
}

void XPattern::ForgetScriptPalette()
{
    default_translations.clear();
    floor_priority.clear();
}

const XPattern::Translation* XPattern::Lookup(const char glyph) const
{
    for (const auto& t : translations) {
        if (t.glyph == glyph) {
            return &t;
        }
    }

    for (const auto& t : default_translations) {
        if (t.glyph == glyph) {
            return &t;
        }
    }

    return nullptr;
}

int XPattern::Draw(XLocation* location, int x, int y) const
{
    std::vector<XPoint> points_to_resolve;
    int outside = 0;

    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            const char glyph = text[i * w + j];
            const Translation* translation = Lookup(glyph);

            // A blank is a hole in the pattern: whatever is already on
            // the map there stays. Everything else - a callback, or a
            // character nothing accounts for - is resolved below, once
            // its neighbours are drawn and can be looked at. Blanks are
            // answered first, so a pattern whose border draws nothing may
            // hang over the edge of its level without complaint.
            if (!translation && glyph == ' ') {
                continue;
            }

            // This cell is not one the level holds - past the edge of the
            // map, or, for a floor above another level, past the corner
            // of it this floor covers. Drawing it would be writing into
            // another level's cells or into none at all, so leave it and
            // count it: the caller names the level that drew outside
            // itself.
            if (!location->map->StoredCell(x + j, y + i)) {
                outside++;
                continue;
            }

            if (translation && !translation->callback.valid()) {
                location->map->SetXY(x + j, y + i, translation->tile);
                continue;
            }

            points_to_resolve.emplace_back(x + j, y + i);
        }
    }

    for (const auto pt: points_to_resolve) {
        size_t best_fit_index = 0;
        bool copied_a_neighbour = false;

        for (int q = -1; q <= 1; q++) {
            for (int w2 = -1; w2 <= 1; w2++) {
                if (q == 0 && w2 == 0) {
                    continue;
                }

                // A cell on the border of the map has neighbours that are
                // nowhere at all; they simply say nothing about what the
                // ground here is.
                const XMapTile* neighbour = location->map->Cell(pt.x + q, pt.y + w2);

                if (!neighbour) {
                    continue;
                }

                for (size_t i = 0; i < floor_priority.size(); i++) {
                    if (floor_priority[i] == neighbour->n && (!copied_a_neighbour || best_fit_index < i)) {
                        best_fit_index = i;
                        copied_a_neighbour = true;
                    }
                }
            }
        }

        // Nothing around says what the ground is - a treasure alcove cut
        // into solid rock, where every neighbour is wall and every cell
        // of the alcove itself is waiting here in this same list. The
        // level's own floor answers that; without one, the first of the
        // world's floors does, which is what a level had before.
        if (copied_a_neighbour) {
            location->map->SetXY(pt.x, pt.y, floor_priority[best_fit_index]);
        } else if (location->default_floor != XTileType::NONE) {
            location->map->SetXY(pt.x, pt.y, location->default_floor);
        } else if (!floor_priority.empty()) {
            location->map->SetXY(pt.x, pt.y, floor_priority[0]);
        }

        if (const Translation* translation = Lookup(text[(pt.y - y) * w + pt.x - x]);
            translation && translation->callback.valid()) {
            translation->callback(pt.x, pt.y);
        }
    }

    return outside;
}
