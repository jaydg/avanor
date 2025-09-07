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

#include "other/other_misc.h"
#include "creature/unique.h"
#include "creature/skeep_ai.h"
#include "other/xgen.h"
#include "game/game.h"
#include "item/item_misc.h"

XMainLocation::XMainLocation(LOCATION tl) : XLocation(tl)
{
    strcpy(brief_name, "Valley");
    strcpy(full_name, "Valley of Avanor");

    int lm = 0;
    int rm = 200;
    int tm = 0;
    int bm = 90;
    map = new XMap(rm, bm);
    int i, j;

    for (i = 0; i < map->hgt; i++)
        for (j = 0; j < map->len; j++) {
            if (vRand() % 3) {
                map->SetXY(j, i, M_GREENGRAS);
            } else {
                map->SetXY(j, i, M_GREENTREE);
            }
        }

    // creating high mountains
    for (i = lm; i < rm; i++) {
        int z1 = vRand() % ((i & 3) + 1) + 1;
        int z2 = vRand() % ((i & 3) + 1) + 1;

        for (j = 0; j < z1; j++) {
            map->SetXY(i, tm + j, M_HIGHMOUNTAIN);
        }

        for (j = 0; j < z2; j++) {
            map->SetXY(i, bm - j - 1, M_HIGHMOUNTAIN);
        }
    }

    for (i = tm; i < bm; i++) {
        int z1 = vRand() % ((i & 3) + 1) + 1;
        int z2 = vRand() % ((i & 3) + 1) + 1;

        for (j = 0; j < z1; j++) {
            map->SetXY(lm + j, i, M_HIGHMOUNTAIN);
        }

        for (j = 0; j < z2; j++) {
            map->SetXY(rm - j - 1, i, M_HIGHMOUNTAIN);
        }
    }

    // evaluate high mountains till hills!
    for (i = 0; i < map->hgt; i++)
        for (j = 0; j < map->len; j++) {
            int m = map->GetXY(j, i);

            if (m > M_HILL && m <= M_HIGHMOUNTAIN) {
                for (int q = -2; q < 3; q++)
                    for (int w = -2; w < 3; w++) {
                        int nm;

                        if (abs(q) >= abs(w)) {
                            nm = m - abs(q);
                        } else {
                            nm = m - abs(w);
                        }

                        if (nm < M_HILL) {
                            nm = M_HILL;
                        }

                        if (j + q >= 0 && i + w >= 0
                            && j + q < map->len && i + w < map->hgt
                            && map->GetXY(j + q, i + w) < nm) {
                            map->SetXY(j + q, i + w, (STDMAP)nm);
                        }
                    }
            }
        }

    //orcs!
    XRect orc_area(10, 70, 30, 80);

    for (i = 0; i < 20; i++) {
        NewCreature(CR_ORC, orc_area, GID_ORCS_WARPARTY, AIF_GUARD_AREA);
    }

    Game.Scheduler.Add(new XMainLocationGen(this));

    // teleports from small village to town and back
    new XTeleport(23, 20, this, L_MAIN, 154, 13);
    new XTeleport(154, 13, this, L_MAIN, 23, 20);

    for (i = 0; i < map->hgt; i++) {
        for (j = 0; j < map->len; j++) {
            if (vRand(18) == 0) {
                if (map->GetXY(j, i) == M_GREENGRAS && map->GetSpecial(j, i) == NULL) {
                    new XHerbBush(j, i, this);
                }
            }
        }
    }
}

XExtinctVolcanoLocation::XExtinctVolcanoLocation(LOCATION tl) : XLocation(tl)
{
    BuildCave();
}
