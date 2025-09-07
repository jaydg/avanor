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

#include "game/location.h"
#include "global/xapi.h"
#include "other/other_misc.h"

//******* palletes script langugage ******* //
// Some simbols has default values, but can be overrided
// '.' - GREEN GRASS
// ',' - CAVE FLOOR
// ';' - STONE FLOOR
// '#' - STONE WALL
// '=' - WATER
// '&' - GREEN TREE
// 'X' - FENCE
// '+' - CLOSED DOOR
// '/' - OPENED DOOR
// '^' - TRAP DOOR

STDMAP best_fit_terrain_table[] = {
    M_GREENGRAS,
    M_SAND,
    M_PATH,
    M_ROAD,
    M_CAVEFLOOR,
    M_STONEFLOOR,
    M_OBSIDIANFLOOR,
    M_GOLDENFLOOR,
};

//Location Script Support
extern "C"
{
#include "lauxlib.h"
}

void XLocation::PutPalette(int x, int y)
{
    XQList<XPoint> points_to_resolve;

    for (int i = 0; i < current_pattern.h; i++) {
        for (int j = 0; j < current_pattern.w; j++) {
            bool found_it = false;

            for (XQList<PALETTE_MAP>::iterator it = pattern_translation.begin(); it != pattern_translation.end(); it++) {
                if ((*it).this_view == current_pattern.pattern[i * current_pattern.w + j]) {
                    if ((*it).lua_str[0]) {
                        points_to_resolve.push_back(XPoint(x + j, y + i));
                    } else {
                        map->SetXY(x + j, y + i, (*it).real_view);
                    }

                    found_it = true;
                    break;
                }
            }

            if (!found_it) {
                switch (current_pattern.pattern[i * current_pattern.w + j]) {
                    case '+':
                        new XDoor(x + j, y + i, 0, this);
                        points_to_resolve.push_back(XPoint(x + j, y + i));
                        break;

                    case '/':
                        new XDoor(x + j, y + i, 1, this);
                        points_to_resolve.push_back(XPoint(x + j, y + i));
                        break;

                    case '^':
                        new XTrap(x + j, y + i, this);
                        points_to_resolve.push_back(XPoint(x + j, y + i));
                        break;

                    case '.':
                        map->SetXY(x + j, y + i, M_GREENGRAS);
                        break;

                    case ',':
                        map->SetXY(x + j, y + i, M_CAVEFLOOR);
                        break;

                    case ';':
                        map->SetXY(x + j, y + i, M_STONEFLOOR);
                        break;

                    case '#':
                        map->SetXY(x + j, y + i, M_STONEWALL);
                        break;

                    case '=':
                        map->SetXY(x + j, y + i, M_WATER);
                        break;

                    case '&':
                        map->SetXY(x + j, y + i, M_GREENTREE);
                        break;

                    case 'X':
                        map->SetXY(x + j, y + i, M_FENCE);
                        break;

                    case ' ':
                        break;

                    default:
                        points_to_resolve.push_back(XPoint(x + j, y + i));
                        break;
                }
            }
        }
    }

    for (XQList<XPoint>::iterator it = points_to_resolve.begin(); it != points_to_resolve.end(); it++) {
        XPoint pt = *it;
        int best_fit_index = 0;

        for (int q = -1; q <= 1; q++) {
            for (int w = -1; w <= 1; w++) {
                if (q != 0 || w != 0) {
                    STDMAP tm = map->GetXY(pt.x + q, pt.y + w);

                    for (int i = 0; i < ARRAY_SIZE(best_fit_terrain_table); i++) {
                        if (best_fit_terrain_table[i] == tm && best_fit_index < i) {
                            best_fit_index = i;
                        }
                    }
                }
            }
        }

        map->SetXY(pt.x, pt.y, best_fit_terrain_table[best_fit_index]);

        for (XQList<PALETTE_MAP>::iterator tit = pattern_translation.begin(); tit != pattern_translation.end(); tit++) {
            if ((*tit).this_view == current_pattern.pattern[(pt.y - y) * current_pattern.w + pt.x - x]) {
                char buf[1024];
                sprintf(buf, "local x, y = %d, %d\n %s", pt.x, pt.y, (*tit).lua_str);
                luaL_dostring(L, buf);
            }
        }
    }
}
