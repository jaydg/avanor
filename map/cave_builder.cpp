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

#include <cmath>

#include "engine/global.h"
#include "game/location.h"
#include "map/cave_builder.h"
#include "map/map.h"

void XCaveBuilder::Build()
{
    int cl = 80;
    int ch = 20;

    XMap*& map = location->map;

    if (map) {
        cl = map->len;
        ch = map->hgt;
    } else {
        map = new XMap(cl, ch);
    }

    // A random blob-stamp cave has no connectivity guarantee on its own -
    // regenerate from scratch until the whole floor is one reachable
    // region (150 blobs over an 80x20 grid connects within a handful of
    // tries in practice), so a level's stairways - placed afterward, once
    // this returns, via GetFreeXY() over whatever floor exists - can never
    // end up in an isolated pocket the player can't get down/up through.
    constexpr int MAX_ATTEMPTS = 100;
    bool connected = false;

    for (int attempt = 0; attempt < MAX_ATTEMPTS && !connected; attempt++) {
        for (int i = 0; i < map->hgt; i++) {
            for (int j = 0; j < map->len; j++) {
                map->SetXY(j, i, XTileType::MAGMA);
            }
        }

        for (int k = 0; k < 150; k++) {
            int	qx = vRand() % (cl - 7) + 1;
            int qy = vRand() % (ch - 5) + 1;

            for (int q = 0; q < 360; q += 3) {
                for (int w = 0; w < 3; w++) {
                    int tx = qx + (int)(w * cos(q * M_PI / 180.0));
                    int ty = qy + (int)(w * sin(q * M_PI / 180.0));

                    if (tx > 0 && ty > 0 && tx < 79 && ty < 19) {
                        map->SetXY(tx, ty, XTileType::CAVE_FLOOR);
                    }
                }
            }
        }

        connected = map->isFullyConnected();
    }

    if (!connected) {
        map->ConnectAllRegions();
    }
}
