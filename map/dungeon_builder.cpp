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
#include <cassert>
#include <cmath>
#include <cstring>
#include <memory>
#include <vector>

#include "engine/global.h"
#include "game/location.h"
#include "creature/std_ai.h"
#include "item/item_misc.h"
#include "map/dungeon_builder.h"
#include "map/map.h"
#include "map/map_objects.h"


std::vector<RoomTemplate> room_templates;

bool RoomTemplate::isExit(const int x, const int y) const
{
    const char ch = pattern.At(x, y);

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

XRoom::XRoom(int len, int hgt, const RoomTemplate* _room)
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
        l = room->pattern.Width();
        h = room->pattern.Height();
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

int XRoom::Intersect(XRoom * other, int dist)
{
    XRect tr(other->r);

    return tr.Intersect(&r);
}

void XRoom::Draw(XLocation * l)
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
        room->pattern.Draw(l, r.left, r.top);

        // Marked so the corridor pass routes around the room instead of
        // carving through its walls.
        for (int i = r.top; i < r.bottom; i++) {
            for (int j = r.left; j < r.right; j++) {
                l->map->SetRoom(j, i, 1);
            }
        }

        if (room->on_drawn.valid()) {
            room->on_drawn(r.left, r.top, room->pattern.Width(), room->pattern.Height());
        }
    }
}

bool XRoom::GetFreeExit(XPoint * pt)
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

XDungeonBuilder::XDungeonBuilder(XLocation * _l, int _room_chance, int create_door_trap_chest)
{
    // Same as the other builders: a location arrives without a map and
    // leaves with one. XLocation::BuildDungeon() used to do this bit.
    if (!_l->map) {
        _l->map = new XMap(80, 20);
    }

    m = _l->map;
    location = _l;
    isCreateDoorTrapChest = create_door_trap_chest > 0;
    room_chance = std::clamp(_room_chance, 0, 100);
}

bool XDungeonBuilder::PlaceRoom(std::vector<std::unique_ptr<XRoom>>& placed, const RoomTemplate* room, int attempts)
{
    while (attempts-- > 0) {
        auto xc = std::make_unique<XRoom>(m->len, m->hgt, room);
        bool clear = true;

        for (size_t q = 0; q < placed.size() && clear; q++) {
            if (placed[q]->Intersect(xc.get(), 0)) {
                clear = false;
            }
        }

        if (clear) {
            xc->Draw(location);
            placed.push_back(std::move(xc));
            return true;
        }
    }

    return false;
}

void XDungeonBuilder::Build()
{
    int i;

    for (i = 0; i < m->hgt; i++)
        for (int j = 0; j < m->len; j++) {
            m->SetXY(j, i, XTileType::MAGMA);
        }

    std::vector<std::unique_ptr<XRoom>> quae;

    int nCave = m->hgt * m->len / 200;

    // One roll for the level: does it get a defined room at all.
    if (isCreateDoorTrapChest && vRand(100) < room_chance) {
        if (const RoomTemplate* room = PickRoom()) {
            PlaceRoom(quae, room, 200);
        }
    }

    // Plain rectangles fill the level up to its room count.
    for (i = static_cast<int>(quae.size()); i < nCave; i++) {
        PlaceRoom(quae, nullptr, 10000);
    }

    for (size_t k = 0; k + 1 < quae.size(); k++) {
        XRoom * tc1 = quae[k].get();
        XRoom * tc2 = quae[k + 1].get();
        XPoint pt1;
        XPoint pt2;

        if (tc1->GetFreeExit(&pt1) && tc2->GetFreeExit(&pt2)) {
            Link(&pt1, &pt2);
        } else {
            assert(0);
        }
    }

    // Every door a template drew should lead somewhere. The chain above
    // uses one exit per room, so the rest would open into solid rock -
    // and because Link() routes corridors *around* rooms, a large room
    // can also cut a level in two. Digging the remaining doors out fixes
    // both: each becomes another way into the level.
    for (const auto& xc : quae) {
        if (!xc->isTemplateRoom()) {
            continue;
        }

        for (const XPoint& door : xc->exits) {
            DigOut(door, xc->r);
        }
    }

    if (isCreateDoorTrapChest) {
        CreateDoors();
    }

    // Whatever the rooms and corridors ended up doing, the level has to
    // be walkable end to end - its stairways are placed afterwards, on
    // whatever free cell turns up.
    if (!m->isFullyConnected()) {
        m->ConnectAllRegions();
    }
}

struct LINK_STACK {
    int pt_count;
    XPoint pa[2000]; // it is seems that will never be more than 2000 points.
    void PushPoint(int x, int y)
    {
        pa[pt_count].x = x;
        pa[pt_count].y = y;
        pt_count++;
        assert(pt_count < 2000);
    }
};

//linking of room is using standard flood-fill algorithm
bool XDungeonBuilder::Link(XPoint * p1, XPoint * p2)
{
    //create and reset table equal with map
    std::vector<int> tbl(m->hgt * m->len, 0);

    //create stack of points (we will exchange them to increase perfomance)
    LINK_STACK st1;
    st1.pt_count = 0;
    LINK_STACK st2;
    st2.pt_count = 0;
    LINK_STACK * current = &st1;
    LINK_STACK * other = &st2;

    current->pa[current->pt_count].x = p1->x;
    current->pa[current->pt_count].y = p1->y;
    current->pt_count = 1;
    int step = 1;
    tbl[p1->x + p1->y * m->len] = step;

    bool flag = true;

    while (flag) {
        for (int i = 0; i < current->pt_count; i++) {
            int tx = current->pa[i].x;
            int ty = current->pa[i].y;

            if (tx > 0 && tx < m->len - 1 && ty > 0 && ty < m->hgt - 1) {
                if (tbl[tx + 1 + ty * m->len] == 0) {
                    if (tx + 1 == p2->x && ty == p2->y) {
                        tbl[tx + 1 + ty * m->len] = step;
                        flag = false;
                        break;
                    }

                    if (m->GetRoom(tx + 1, ty) == 0) {
                        other->PushPoint(tx + 1, ty);
                        tbl[tx + 1 + ty * m->len] = step;
                    }
                }

                if (tbl[tx - 1 + ty * m->len] == 0) {
                    if (tx - 1 == p2->x && ty == p2->y) {
                        tbl[tx - 1 + ty * m->len] = step;
                        flag = false;
                        break;
                    }

                    if (m->GetRoom(tx - 1, ty) == 0) {
                        other->PushPoint(tx - 1, ty);
                        tbl[tx - 1 + ty * m->len] = step;
                    }
                }

                if (tbl[tx + (ty + 1) * m->len] == 0) {

                    if (tx == p2->x && ty + 1 == p2->y) {
                        tbl[tx + (ty + 1) * m->len] = step;
                        flag = false;
                        break;
                    }

                    if (m->GetRoom(tx, ty + 1) == 0) {
                        other->PushPoint(tx, ty + 1);
                        tbl[tx + (ty + 1) * m->len] = step;
                    }
                }

                if (tbl[tx + (ty - 1) * m->len] == 0) {
                    if (tx == p2->x && ty - 1 == p2->y) {
                        tbl[tx + (ty - 1) * m->len] = step;
                        flag = false;
                        break;
                    }

                    if (m->GetRoom(tx, ty - 1) == 0) {
                        other->PushPoint(tx, ty - 1);
                        tbl[tx + (ty - 1) * m->len] = step;
                    }
                }
            }
        }

        LINK_STACK * tmp = other;
        other = current;
        current = tmp;
        other->pt_count = 0;
        step++;

        if (current->pt_count == 0) {
            return false;
        }
    }

    // we have found the way! yahoo!
    int px = p2->x;
    int py = p2->y;
    int stp = tbl[px + py * m->len];

    while (stp > 1) {
        stp = tbl[px + py * m->len];
        m->SetXY(px, py, XTileType::CAVE_FLOOR);

        if (tbl[px + 1 + py * m->len] == stp - 1) {
            px++;
        } else if (tbl[px - 1 + py * m->len] == stp - 1) {
            px--;
        } else if (tbl[px + (py + 1) * m->len] == stp - 1) {
            py++;
        } else if (tbl[px + (py - 1) * m->len] == stp - 1) {
            py--;
        }
    }

    m->SetXY(p1->x, p1->y, XTileType::CAVE_FLOOR);

    return true;
}

void XDungeonBuilder::CreateDoors()
{
    for (int i = 1; i < m->hgt - 1; i++)
        for (int j = 1; j < m->len - 1; j++) {
            if (m->XGetMovability(j, i) == 0 && m->GetRoom(j, i) == 0) {
                int a11 = (m->XGetMovability(j - 1, i - 1) == 0);
                int a12 = (m->XGetMovability(j, i - 1) == 0);
                int a13 = (m->XGetMovability(j + 1, i - 1) == 0);
                int a21 = (m->XGetMovability(j - 1, i) == 0);
                int a23 = (m->XGetMovability(j + 1, i) == 0);
                int a31 = (m->XGetMovability(j - 1, i + 1) == 0);
                int a32 = (m->XGetMovability(j, i + 1) == 0);
                int a33 = (m->XGetMovability(j + 1, i + 1) == 0);

                if ((((a11 && a13) || (a31 && a33)) && a32 && a12 && !a21 && !a23)
                    || (((a11 && a31) || (a13 && a33)) && a21 && a23 && !a12 && !a32)) {
                    if (vRand(3) == 0) {
                        new XDoor(j, i, 0, location);
                    } else {
                        new XDoor(j, i, 1, location);
                    }
                }
            }
        }
}

bool XDungeonBuilder::DigOut(const XPoint& door, const XRect& room)
{
    // Breadth-first out from the door, through rock and around every
    // other room, until it meets floor belonging to the rest of the
    // level - then carve the path it came by.
    std::vector<int> came_from(m->len * m->hgt, -1);
    std::vector<int> queue;
    const int start = door.x + door.y * m->len;

    queue.push_back(start);
    came_from[start] = start;

    static const int dx[4] = { 1, -1, 0, 0 };
    static const int dy[4] = { 0, 0, 1, -1 };

    for (size_t head = 0; head < queue.size(); head++) {
        const int cx = queue[head] % m->len;
        const int cy = queue[head] / m->len;

        for (int d = 0; d < 4; d++) {
            const int nx = cx + dx[d];
            const int ny = cy + dy[d];

            if (nx < 1 || ny < 1 || nx >= m->len - 1 || ny >= m->hgt - 1) {
                continue;
            }

            const int next = nx + ny * m->len;

            if (came_from[next] >= 0) {
                continue;
            }

            // Inside the room this door belongs to: not an escape, and
            // digging back through its own walls would ruin the art.
            if (nx >= room.left && nx < room.right && ny >= room.top && ny < room.bottom) {
                continue;
            }

            came_from[next] = queue[head];

            if (m->GetMovability(nx, ny) < XTileType::Movability::UNWALKABLE || m->GetRoom(nx, ny) != 0) {
                // Reached the level. Carve back to the door.
                for (int at = came_from[next]; at != start; at = came_from[at]) {
                    m->SetXY(at % m->len, at / m->len, XTileType::CAVE_FLOOR);
                }

                return true;
            }

            // Another room's wall is not something to tunnel through.
            if (m->GetRoom(nx, ny) == 0) {
                queue.push_back(next);
            }
        }
    }

    return false;
}
