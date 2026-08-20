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

std::vector<RoomTemplate> room_templates;

bool RoomTemplate::isExit(const int x, const int y) const
{
    const char ch = pattern.pattern[x + y * pattern.w];

    return ch == '+' || ch == '.';
}

// Picks a room by weight. Null when the world script defined no rooms,
// which is not an error - the generator then only makes plain ones.
const RoomTemplate* PickRoom()
{
    int total = 0;

    for (const auto& room : room_templates) {
        total += room.weight;
    }

    if (total <= 0) {
        return nullptr;
    }

    int roll = vRand(total);

    for (const auto& room : room_templates) {
        if (roll < room.weight) {
            return &room;
        }

        roll -= room.weight;
    }

    return nullptr;
}

#define USUAL_CAVE_HGT 4
#define USUAL_CAVE_LEN 4

XCave::XCave(int len, int hgt, const RoomTemplate* _room)
{
    assert(len > 4);
    assert(hgt > 4);

    map_len = len;
    map_hgt = hgt;

    int x, y, l, h;

    room = _room;

    if (!room) { // plain rectangular room

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
    } else { // a room the world script defined
        l = room->pattern.w;
        h = room->pattern.h;
        x = vRand() % (len - l - 6) + 3;
        y = vRand() % (hgt - h - 6) + 3;

        // searching for exits (doors, empty spaces etc.)
        for (int i = 0; i < l; i++) {
            if (room->isExit(i, 0)) {
                exits.push_back(XPoint(i + x, y));
            }

            if (room->isExit(i, h - 1)) {
                exits.push_back(XPoint(i + x, y + h - 1));
            }
        }

        for (int j = 0; j < h; j++) {
            if (room->isExit(0, j)) {
                exits.push_back(XPoint(x, y + j));
            }

            if (room->isExit(l - 1, j)) {
                exits.push_back(XPoint(l - 1 + x, y + j));
            }
        }
    }

    r.Setup(x, y, x + l, y + h);
}

int XCave::Intersect(XCave * xc, int dist)
{
    XRect tr(xc->r);

    return tr.Intersect(&r);
}

void XCave::Draw(XLocation * l)
{
    if (!room) {
        for (int i = r.top; i < r.bottom; i++)
            for (int j = r.left; j < r.right; j++) {
                l->map->SetXY(j, i, XTileType::CAVE_FLOOR);
            }

        if (vRand(10) == 0) {
            int i;

            for (i = 0; i < vRand(5); i++) {
                if (const auto pt = l->GetFreeXY(&r)) {
                    new XTrap(pt->x, pt->y, l, TL_RANDOM);
                }
            }
        }
    } else {
        // The room's own palette resolves its glyphs - tiles directly,
        // and chests, doors or traps through the script callbacks bound
        // to them, the same way a hand-built location's pattern works.
        l->PutPalette(room->pattern, room->translation, r.left, r.top);

        // Marked so the corridor pass routes around the room instead of
        // carving through its walls.
        for (int i = r.top; i < r.bottom; i++) {
            for (int j = r.left; j < r.right; j++) {
                l->map->SetRoom(j, i, 1);
            }
        }

        if (room->on_drawn.valid()) {
            room->on_drawn(r.left, r.top, room->pattern.w, room->pattern.h);
        }
    }
}

bool XCave::GetFreeExit(XPoint * pt)
{
    int attempt = 100;

    while (attempt-- > 0) {
        int n = vRand(exits.size());
        auto it = exits.begin();

        while (n > 0) {
            ++it;
            n--;
        }

        *pt = *it;
        exits.erase(it);

        return true;
    }

    return false;
}
