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
#include "map/pattern.h"

std::vector<XPattern::Translation> XPattern::default_translations;
std::vector<XTileType::Type> XPattern::floor_priority;

void XPattern::SetDefaults(std::vector<Translation> defaults)
{
    default_translations = std::move(defaults);
}

void XPattern::SetFloorPriority(std::vector<XTileType::Type> floors)
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

void XPattern::Draw(XLocation* location, int x, int y) const
{
    std::vector<XPoint> points_to_resolve;

    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            const char glyph = text[i * w + j];
            const Translation* translation = Lookup(glyph);

            if (translation && !translation->callback.valid()) {
                location->map->SetXY(x + j, y + i, translation->tile);
                continue;
            }

            // A blank is a hole in the pattern: whatever is already on
            // the map there stays. Everything else - a callback, or a
            // character nothing accounts for - is resolved below, once
            // its neighbours are drawn and can be looked at.
            if (!translation && glyph == ' ') {
                continue;
            }

            points_to_resolve.emplace_back(x + j, y + i);
        }
    }

    for (const auto pt: points_to_resolve) {
        if (!floor_priority.empty()) {
            size_t best_fit_index = 0;

            for (int q = -1; q <= 1; q++) {
                for (int w2 = -1; w2 <= 1; w2++) {
                    if (q == 0 && w2 == 0) {
                        continue;
                    }

                    const XTileType::Type tm = location->map->GetXY(pt.x + q, pt.y + w2);

                    for (size_t i = 0; i < floor_priority.size(); i++) {
                        if (floor_priority[i] == tm && best_fit_index < i) {
                            best_fit_index = i;
                        }
                    }
                }
            }

            location->map->SetXY(pt.x, pt.y, floor_priority[best_fit_index]);
        }

        if (const Translation* translation = Lookup(text[(pt.y - y) * w + pt.x - x]);
            translation && translation->callback.valid()) {
            translation->callback(pt.x, pt.y);
        }
    }
}
