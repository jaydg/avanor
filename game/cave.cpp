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

#include "creature/std_ai.h"
#include "game/cave.h"
#include "item/item_misc.h"
#include "map/map_objects.h"

bool CAVE_DATA::isExit(int x, int y)
{
    char ch = cave[x + y * width];

    if (ch == '+' || ch == '.') {
        return true;
    } else {
        return false;
    }
}

char CAVE_DATA::GetCode(int x, int y)
{
    return cave[x + y * width];
}

CAVE_DATA random_caves[] = {
    // RCT_SIMPLE1
    {
        9, 7,
        CREATE_RANDOM_TRAP_ON_CHEST | CREATE_GUARD_ON_ROOM,
        50,
        "####+####"
        "##.....##"
        "#.##.##.#"
        "+.#~.~#.+"
        "#.##.##.#"
        "##.....##"
        "####+####"
    },
    // RCT_SIMPLE2
    {
        9, 7,
        CREATE_RANDOM_TRAP_ON_CHEST | CREATE_GUARD_ON_ROOM,
        200,
        "####+####"
        "#.......#"
        "#..#+#..#"
        "+..#~#..+"
        "#..###..#"
        "#.......#"
        "####+####"
    },
    // RCT_SIMPLE3
    {
        14, 9,
        CREATE_TRAP_ON_CHEST | CREATE_GUARD_ON_ROOM,
        25,
        "##############"
        "#............#"
        "#.####..####.#"
        "#.#~.#..#.~#.#"
        "+.#..+..+..#.+"
        "#.#~.#..#.~#.#"
        "#.####..####.#"
        "#............#"
        "##############"
    },
    // RCT_SIMPLE4
    {
        17, 9,
        CREATE_TRAP_ON_CHEST | CREATE_GUARD_ON_ROOM,
        25,
        "#################"
        "#...............#"
        "#.#+#####+#####.#"
        "#.#..#~~#..#~~#.#"
        "+.#..#..#..#..#.+"
        "#.#~~#..#~~#..#.#"
        "#.####+#####+##.#"
        "#...............#"
        "#################"
    },
};

#define USUAL_CAVE_HGT 4
#define USUAL_CAVE_LEN 4

XCave::XCave(int len, int hgt, bool isAllowSpecialRooms)
{
    assert(len > 4);
    assert(hgt > 4);

    map_len = len;
    map_hgt = hgt;

    int x, y, l, h;

    if (vRand(20) || !isAllowSpecialRooms) { // random cave
        rct = RCT_USUAL;

        while (1) {
            x = vRand() % (len - USUAL_CAVE_LEN - 2) + 1;
            y = vRand() % (hgt - USUAL_CAVE_HGT - 2) + 1;
            l = vRand() % 7 + USUAL_CAVE_LEN;
            h = vRand() % 3 + USUAL_CAVE_HGT;

            if (x + l < len - 2 && y + h < hgt - 2) {
                break;
            }
        }

        // create from 1 to 4 random exits
        int ec = vRand(2) + 2;
        XPoint tpt;

        while (ec > 0) {
            switch (vRand(4)) { // on which wall the door will placed
                case 0:
                    tpt = XPoint(x + vRand(l - 3) + 1, y);
                    break;

                case 1:
                    tpt = XPoint(x + vRand(l - 3) + 1, y + h - 1);
                    break;

                case 2:
                    tpt = XPoint(x, y + vRand(h - 3) + 1);
                    break;

                case 3:
                    tpt = XPoint(x + l - 1, y + vRand(h - 3) + 1);
                    break;
            }

            if (tpt.x < 2 || tpt.x > map_len - 2 || tpt.y < 2 || tpt.y > map_hgt - 2) {
                continue;
            }

            exits.push_back(tpt);
            ec--;
        }
    } else { // special cave
        static int cave_freq = 0;

        if (cave_freq == 0) { //  run once
            for (int i = 0; i < RCT_USUAL; i++) {
                cave_freq += random_caves[i].freq;
            }
        }

        int rn = vRand(cave_freq);
        rct = RCT_SIMPLE1;

        while (rn - random_caves[rct].freq > 0) {
            rn -= random_caves[rct].freq;
            rct = (RANDOM_CAVE_TYPE)(rct + 1);
        }

        l = random_caves[rct].width;
        h = random_caves[rct].height;
        x = vRand() % (len - l - 6) + 3;
        y = vRand() % (hgt - h - 6) + 3;

        // searching for exits (doors, empty spaces etc.)
        for (int i = 0; i < l; i++) {
            if (random_caves[rct].isExit(i, 0)) {
                exits.push_back(XPoint(i + x, y));
            }

            if (random_caves[rct].isExit(i, h - 1)) {
                exits.push_back(XPoint(i + x, y + h - 1));
            }
        }

        for (int j = 0; j < h; j++) {
            if (random_caves[rct].isExit(0, j)) {
                exits.push_back(XPoint(x, y + j));
            }

            if (random_caves[rct].isExit(l - 1, j)) {
                exits.push_back(XPoint(l - 1 + x, y + j));
            }
        }
    }

    r.Setup(x, y, x + l, y + h);
}

int XCave::Compare(XObject * o)
{
    XCave * xc = (XCave*)o;

    if (xc->r.left < r.left && xc->r.top <= r.top) {
        return -1;
    } else {
        return 1;
    }
}

int XCave::Intersect(XCave * xc, int dist)
{
    XRect tr(xc->r);

    return tr.Intersect(&r);
}

void XCave::Draw(XLocation * l)
{
    if (rct == RCT_USUAL) {
        for (int i = r.top; i < r.bottom; i++)
            for (int j = r.left; j < r.right; j++) {
                l->map->SetXY(j, i, M_CAVEFLOOR);
            }

        if (vRand(10) == 0) {
            int i;

            for (i = 0; i < vRand(5); i++) {
                XPoint pt;
                l->GetFreeXY(&pt, &r);
                new XTrap(pt.x, pt.y, l, TL_RANDOM);
            }
        }
    } else {
        STDMAP sm = M_CAVEFLOOR;

        for (int i = r.top; i < r.bottom; i++) {
            for (int j = r.left; j < r.right; j++) {
                char ch = random_caves[rct].GetCode(j - r.left, i - r.top);

                switch (ch) {
                    case '#':
                        sm = M_MAGMA;
                        break;

                    case '.':
                        sm = M_CAVEFLOOR;
                        break;

                    case '+':
                        sm = M_CAVEFLOOR;
                        new XDoor(j, i, 0, l);
                        break;

                    case '~': {
                        sm = M_CAVEFLOOR;
                        XChest * ch1 = new XChest(vRand(6) + 1, IM_ITEM, 1, 5000);
                        ch1->Drop(l, j, i);

                        if ((random_caves[rct].cf & CREATE_TRAP_ON_CHEST)
                            || (random_caves[rct].cf & CREATE_RANDOM_TRAP_ON_CHEST && vRand(3) == 0)) {
                            new XTrap(j, i, l, TL_RANDOM);
                        }
                    }
                    break;

                    default:
                        sm = M_CAVEFLOOR;
                        break;
                }

                l->map->SetXY(j, i, sm);
                l->map->SetRoom(j, i, 1);
            }
        }

        if (random_caves[rct].cf & CREATE_GUARD_ON_ROOM) {
            CREATURE_CLASS crc = CR_UNDEAD;

            switch (vRand(3)) {
                case 0:
                    crc = CR_UNDEAD;
                    break;

                case 1:
                    crc = CR_GOBLIN;
                    break;

                case 2:
                    crc = CR_KOBOLD;
                    break;
            }

            for (int i = 0; i < 10; i++) {
                l->NewCreature(crc, r, GID_RANDOM_GUARDIAN, AIF_GUARD_AREA);
            }
        }
    }
}

bool XCave::GetFreeExit(XPoint * pt)
{
    int attempt = 100;

    while (attempt-- > 0) {
        int n = vRand(exits.size());
        XQList<XPoint>::iterator it = exits.begin();

        while (n > 0) {
            it++;
            n--;
        }

        *pt = *it;
        exits.erase(it);
        return true;
    }

    return false;
}
