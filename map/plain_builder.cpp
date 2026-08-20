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
#include <cstdlib>

#include "engine/global.h"
#include "game/location.h"
#include "map/map.h"
#include "map/plain_builder.h"

int XPlainBuilder::Rung(const XTileType::Id tile) const
{
    for (size_t i = 0; i < slope.size(); i++) {
        if (slope[i] == tile) {
            return static_cast<int>(i);
        }
    }

    return -1;
}

void XPlainBuilder::Build()
{
    if (slope.empty()) {
        return;
    }

    const XTileType::Id peak = slope.back();

    int lm = 0;
    int rm = w;
    int tm = 0;
    int bm = h;

    int i, j;

    // Create Avanor's plain
    XMap*& map = location->map;

    map = new XMap(w, h);

    for (i = 0; i < map->hgt; i++)
        for (j = 0; j < map->len; j++) {
            if (vRand() % cover_odds) {
                map->SetXY(j, i, ground);
            } else {
                map->SetXY(j, i, cover);
            }
        }

    // creating high mountains
    for (i = lm; i < rm; i++) {
        int z1 = vRand() % ((i % border_depth) + 1) + 1;
        int z2 = vRand() % ((i % border_depth) + 1) + 1;

        for (j = 0; j < z1; j++) {
            map->SetXY(i, tm + j, peak);
        }

        for (j = 0; j < z2; j++) {
            map->SetXY(i, bm - j - 1, peak);
        }
    }

    for (i = tm; i < bm; i++) {
        int z1 = vRand() % ((i % border_depth) + 1) + 1;
        int z2 = vRand() % ((i % border_depth) + 1) + 1;

        for (j = 0; j < z1; j++) {
            map->SetXY(lm + j, i, peak);
        }

        for (j = 0; j < z2; j++) {
            map->SetXY(rm - j - 1, i, peak);
        }
    }

    //evaluate high mountains till hills!
    for (i = 0; i < map->hgt; i++)
        for (j = 0; j < map->len; j++) {
            const int rung = Rung(map->GetXY(j, i));

            if (rung > 0) {
                for (int q = -erosion; q <= erosion; q++)
                    for (int w = -erosion; w <= erosion; w++) {
                        int lower = rung - std::max(abs(q), abs(w));

                        if (lower < 0) {
                            lower = 0;
                        }

                        if (j + q >= 0 && i + w >= 0
                            && j + q < map->len && i + w < map->hgt
                            && Rung(map->GetXY(j + q, i + w)) < lower) {
                            map->SetXY(j + q, i + w, slope[lower]);
                        }
                    }
            }
        }
}
