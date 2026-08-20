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
    int cl = w;
    int ch = h;

    XMap*& map = location->map;

    if (map) {
        cl = map->len;
        ch = map->hgt;
    } else {
        map = new XMap(cl, ch);
    }

    // A random blob-stamp cave has no connectivity guarantee on its own -
    // regenerate from scratch until the whole floor is one reachable
    // region (Avanor's 150 blobs over an 80x20 grid connect within a
    // handful of tries), so a level's stairways - placed afterward, once
    // this returns, via GetFreeXY() over whatever floor exists - can never
    // end up in an isolated pocket the player can't get down/up through.
    constexpr int MAX_ATTEMPTS = 100;
    bool connected = false;

    for (int attempt = 0; attempt < MAX_ATTEMPTS && !connected; attempt++) {
        for (int i = 0; i < map->hgt; i++) {
            for (int j = 0; j < map->len; j++) {
                map->SetXY(j, i, wall);
            }
        }

        for (int k = 0; k < blobs; k++) {
            // The margins the centres keep from the edge are the
            // original's, expressed through the radius they were chosen
            // for; they are not symmetric, and never were.
            const int qx = vRand() % (cl - (2 * blob_radius + 1)) + 1;
            const int qy = vRand() % (ch - (2 * blob_radius - 1)) + 1;

            for (int angle = 0; angle < 360; angle += 3) {
                for (int r = 0; r < blob_radius; r++) {
                    const int tx = qx + (int)(r * cos(angle * M_PI / 180.0));
                    const int ty = qy + (int)(r * sin(angle * M_PI / 180.0));

                    // Clipped against the map, not against the 80x20 one
                    // this generator was written for.
                    if (tx > 0 && ty > 0 && tx < map->len - 1 && ty < map->hgt - 1) {
                        map->SetXY(tx, ty, floor);
                    }
                }
            }
        }

        connected = map->isFullyConnected();
    }

    if (!connected) {
        map->ConnectAllRegions(floor);
    }
}
