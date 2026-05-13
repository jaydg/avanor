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

#include <fstream>

#include "creature/creature.h"
#include "item/item.h"
#include "map/map.h"
#include "map/map_objects.h"

XTileType std_tile_data[] = {
    {' ', xBLACK, "unknown", MO_UNKNOWN, VI_UNKNOWN},
    {'.', xGREEN, "green grass", MO_NORMAL, VI_NORMAL},
    {'T', xGREEN, "large tree", MO_HARD, VI_HARD},
    {'.', xYELLOW, "sand", MO_SHARD, VI_NORMAL},
    {'#', xCYAN, "window", MO_WALL, VI_NORMAL},
    {'#', xDARKGRAY, "magma", MO_WALL, VI_WALL},
    {'#', xLIGHTGRAY, "quartz", MO_WALL, VI_WALL},
    {'.', xLIGHTGRAY, "cave floor", MO_NORMAL, VI_NORMAL},
    {'.', xLIGHTGRAY, "stone floor", MO_NORMAL, VI_NORMAL},
    {'.', xBROWN, "path", MO_NORMAL, VI_NORMAL},
    {'#', xBROWN, "wooden wall", MO_WALL, VI_WALL},
    {'#', xLIGHTGRAY, "stone wall", MO_WALL, VI_WALL},
    {'=', xLIGHTBLUE, "water", MO_WATER, VI_NORMAL},
    {'=', xBLUE, "deep water", MO_DEEPWATER, VI_NORMAL},
    {'=', xRED, "lava", MO_WATER, VI_NORMAL},
    {'^', xGREEN, "hill", MO_NORMAL, VI_NORMAL},
    {'^', xBROWN, "low mountains", MO_VHARD, VI_AHARD},
    {'^', xLIGHTGRAY, "mountains", MO_MOUNTAIN, VI_HARD},
    {'^', xWHITE, "high mountains", MO_WALL, VI_VHARD},
    {'=', xBROWN, "bridge", MO_NORMAL, VI_NORMAL},
    {'.', xYELLOW, "road", MO_NORMAL, VI_NORMAL},
    {'.', xDARKGRAY, "obsidian floor", MO_NORMAL, VI_NORMAL},
    {'X', xBROWN, "fence", MO_WALL, VI_NORMAL},
    {'.', xYELLOW, "golden floor", MO_NORMAL, VI_NORMAL},
    {'#', xWHITE, "marble wall", MO_WALL, VI_WALL},
    {'#', xDARKGRAY, "black marble wall", MO_WALL, VI_WALL},
    {'X', xYELLOW, "golden fence", MO_WALL, VI_NORMAL},
    {'0', xWHITE, "teleport circle", MO_NORMAL, VI_NORMAL},
};

MAP::MAP()
{
    n = XTileType::GREEN_GRAS;
    pMonster = nullptr;
    pSpecialObject = nullptr;
    visible = false;
    known = ' ';
    color = 0;
    place = nullptr; // by default
    room_id = 0;
};

MAP::~MAP()
{
    for (const auto item : item_list) {
        item->Invalidate();
    }

    if (pSpecialObject) {
        pSpecialObject->Invalidate();
        pSpecialObject = nullptr;
    }
}

void MAP::Store(XFile * f)
{
    f->Write(&color, sizeof(char));

    // FIXME: Implement when porting saving/restoring to Cereal
    // item_list.StoreList(f);

    f->Write(&known, sizeof(char));
    f->Write(&n, sizeof(XTileType::Type));

    place.Store(f);
    pSpecialObject.Store(f);
    pMonster.Store(f);

    f->Write(&visible, sizeof(bool));
}

void MAP::Restore(XFile * f)
{
    f->Read(&color, sizeof(char));

    // FIXME: Implement when porting saving/restoring to Cereal
    // item_list.RestoreList(f);

    f->Read(&known, sizeof(char));
    f->Read(&n, sizeof(XTileType::Type));

    place.Restore(f);
    pSpecialObject.Restore(f);
    pMonster.Restore(f);

    f->Read(&visible, sizeof(bool));
}

XMap::XMap()
{
    map = nullptr;
    hgt = 0;
    len = 0;
    wx = 0;
    wy = 0;
}

XMap::XMap(const int l, const int h)
{
    map = new MAP[l * h];

    hgt = h;
    len = l;
    wx = 0;
    wy = 0;
}

XMap::~XMap()
{
    delete[] map;
}

void XMap::ResVisible(const int x, const int y) const
{
    if (x >= 0 && x < len && y >= 0 && y < hgt) {
        map[x + y * len].visible = false;
    }
}

void XMap::SetVisible(const int x, const int y) const
{
    if (x >= 0 && x < len && y >= 0 && y < hgt) {
        map[x + y * len].visible = true;
        map[x + y * len].color = std_tile_data[map[x + y * len].n].color;
        map[x + y * len].known = std_tile_data[map[x + y * len].n].view;
    }
}

bool XMap::GetVisible(const int x, const int y) const
{
    if (x >= 0 && x < len && y >= 0 && y < hgt) {
        return map[x + y * len].visible;
    }

    return false;
}

void XMap::SetPlace(const int x, const int y, XAnyPlace* place) const
{
    assert(x >= 0 && x < len);
    assert(y >= 0 && y < hgt);

    map[x + y * len].place = place;
}

XAnyPlace* XMap::GetPlace(const int x, const int y) const
{
    assert(x >= 0 && x < len);
    assert(y >= 0 && y < hgt);

    return map[x + y * len].place.get();
}

void XMap::ResKnown(const int x, const int y) const
{
    assert(x >= 0 && x < len);
    assert(y >= 0 && y < hgt);

    map[x + y * len].known = 0;
}

void XMap::SetKnown(const int x, const int y) const
{
    assert(x >= 0 && x < len);
    assert(y >= 0 && y < hgt);

    map[x + y * len].known = 1;
}

int XMap::GetKnown(const int x, const int y) const
{
    assert(x >= 0 && x < len);
    assert(y >= 0 && y < hgt);

    return map[x + y * len].known;
}

void XMap::SetSpecial(const int x, const int y, XMapObject* spec) const
{
    assert(x >= 0 && x < len);
    assert(y >= 0 && y < hgt);

    map[x + y * len].pSpecialObject = spec;
}

XMapObject* XMap::GetSpecial(const int x, const int y) const
{
    assert(x >= 0 && x < len);
    assert(y >= 0 && y < hgt);

    return map[x + y * len].pSpecialObject.get();
}

int XMap::GetVisibility(const int x, const int y) const
{
    assert(x >= 0 && x < len);
    assert(y >= 0 && y < hgt);

    XMapObject* spec = map[x + y * len].pSpecialObject.get();

    if (spec && spec->im & IM_DOOR && dynamic_cast<XDoor *>(spec)->isOpened == 0) {
        return 0;
    }

    if (std_tile_data[map[x + y * len].n].visibility == VI_WALL) {
        return 0;
    }

    return 1;
}

const char* XMap::GetDescription(const int x, const int y) const
{
    assert(x >= 0 && x < len);
    assert(y >= 0 && y < hgt);

    return std_tile_data[map[x + y * len].n].name;
}

int XMap::GetMovability(const int x, const int y) const
{
    assert(x >= 0 && x < len);
    assert(y >= 0 && y < hgt);

    const MAP& _map = map[x + y * len];

    if (_map.pSpecialObject && (_map.pSpecialObject->im & IM_DOOR) &&
        dynamic_cast<XDoor *>(_map.pSpecialObject.get())->isOpened == 0) {
        return MO_WALL;
    }

    return std_tile_data[_map.n].movability;
}

int XMap::XGetMovability(const int x, const int y) const
{
    assert(x >= 0 && x < len);
    assert(y >= 0 && y < hgt);

    const MAP* m = &map[x + y * len];

    if (m->pMonster) {
        return 2;
    }

    XMapObject* spec = map[x + y * len].pSpecialObject.get();

    if (std_tile_data[m->n].movability < MO_UNWALKABLE
        && !(spec && spec->im & IM_DOOR && dynamic_cast<XDoor *>(spec)->isOpened == 0)) {
        return 0;
    }

    return 1;
}

void XMap::PutItem(const int x, const int y, XItem* item) const
{
    assert(x >= 0 && x < len);
    assert(y >= 0 && y < hgt);

    item->x = x;
    item->y = y;
    map[x + y * len].item_list.insert(item);
}

XItemList* XMap::GetItemList(const int x, const int y) const
{
    assert(x >= 0 && x < len);
    assert(y >= 0 && y < hgt);

    return &map[x + y * len].item_list;
}

unsigned int XMap::GetItemCount(const int x, const int y) const
{
    assert(x >= 0 && x < len);
    assert(y >= 0 && y < hgt);

    return map[x + y * len].item_list.size();
}

void XMap::SetMonster(const int x, const int y, XCreature* monst) const
{
    assert(x >= 0 && x < len);
    assert(y >= 0 && y < hgt);

    map[x + y * len].pMonster = monst;
}

void XMap::ResMonster(const int x, const int y) const
{
    assert(x >= 0 && x < len);
    assert(y >= 0 && y < hgt);

    map[x + y * len].pMonster = nullptr;
}

XCreature* XMap::GetMonster(const int x, const int y) const
{
    assert(x >= 0 && x < len);
    assert(y >= 0 && y < hgt);

    return map[x + y * len].pMonster.get();
}

void XMap::PutChar(const int x, const int y, const char c, const int color) const
{
    if (x >= wx && x < wx + SCR_LEN && y >= wy && y < wy + SCR_HGT) {
        vPutCh(x - wx + SCR_X, y - wy + SCR_Y, c, color);
    }
}

void XMap::Put(XCreature * cr) const
{
    for (int i = 0; i < SCR_HGT && wy + i < hgt; i++)
        for (int j = 0; j < SCR_LEN && wx + j < len; j++) {
            MAP * tmap = &map[(i + wy) * len + j + wx];

            if (tmap->visible) {
                if (tmap->pSpecialObject && !(tmap->pSpecialObject->im == IM_TRAP && !dynamic_cast<XTrap *>(tmap->pSpecialObject.get())->isVisible(nullptr))) {
                    vPutCh(j + SCR_X, i + SCR_Y, tmap->pSpecialObject->view, tmap->pSpecialObject->color);
                    tmap->color = tmap->pSpecialObject->color;
                    tmap->known = tmap->pSpecialObject->view;
                } else if (!tmap->item_list.empty()) {
                    const XItem* item = *(tmap->item_list.begin());

                    vPutCh(j + SCR_X, i + SCR_Y, item->view, item->color);
                    tmap->color = item->color;
                    tmap->known = item->view;
                } else {
                    //int tn = (i + wy) * len + j + wx;
                    int n = tmap->n;
                    vPutCh(j + SCR_X, i + SCR_Y, std_tile_data[n].view, std_tile_data[n].color);
                }

                if (tmap->pMonster && cr->isCreatureVisible(tmap->pMonster)) {
                    XCreature * xb = tmap->pMonster;
                    vPutCh(xb->x - wx + SCR_X, xb->y - wy + SCR_Y, xb->view, xb->color);
                }
            } else {
                vPutCh(j + SCR_X, i + SCR_Y, ' ', xBLACK);
            }

            if (tmap->known && !tmap->visible) {
                vPutCh(j + SCR_X, i + SCR_Y, tmap->known, tmap->color);
            }
        }
}

void XMap::Center(const int x, const int y)
{
    if (x <= wx + 2 || x >= wx + SCR_LEN - 2) {
        wx = x - SCR_LEN / 2;

        if (wx + SCR_LEN > len) {
            wx = len - SCR_LEN;
        }

        if (wx < 0) {
            wx = 0;
        }
    }

    if (y <= wy + 2 || y >= wy + SCR_HGT - 2) {
        wy = y - SCR_HGT / 2;

        if (wy + SCR_HGT > hgt) {
            wy = hgt - SCR_HGT;
        }

        if (wy < 0) {
            wy = 0;
        }
    }
}

void XMap::SetXY(const int x, const int y, const XTileType::Type std_map) const
{
    assert(x >= 0 && x < len);
    assert(y >= 0 && y < hgt);

    map[x + y * len].n = std_map;
}

XTileType::Type XMap::GetXY(const int x, const int y) const
{
    assert(x >= 0 && x < len);
    assert(y >= 0 && y < hgt);

    return map[x + y * len].n;
}


void XMap::SetRoom(const int x, const int y, const int room_id) const
{
    assert(x >= 0 && x < len);
    assert(y >= 0 && y < hgt);

    map[x + y * len].room_id = room_id;
}

int XMap::GetRoom(const int x, const int y) const
{
    assert(x >= 0 && x < len);
    assert(y >= 0 && y < hgt);

    return map[x + y * len].room_id;
}

void XMap::CreateRoom(const int x, const int y, const int l, const int h, const int px, const int py, const XTileType::Type m1, const XTileType::Type m2) const
{
    CreateRoom(x, y, l, h, m1, m2);
    SetXY(px, py, m1);
}

void XMap::CreateRoom(const int x, const int y, const int l, const int h, const XTileType::Type m1, const XTileType::Type m2) const
{
    for (int i = 0; i < l; i++) {
        for (int j = 0; j < h; j++) {
            if (i == 0 || i == l - 1 || j == 0 || j == h - 1) {
                SetXY(i + x, j + y, m2);
            } else {
                SetXY(i + x, j + y, m1);
            }
        }
    }
}

void XMap::Store(XFile* f) const
{
    f->Write(&len, sizeof(int));
    f->Write(&hgt, sizeof(int));

    for (int i = 0; i < hgt; i++)
        for (int j = 0; j < len; j++) {
            map[j + i * len].Store(f);
        }
}

void XMap::Restore(XFile* f)
{
    f->Read(&len, sizeof(int));
    f->Read(&hgt, sizeof(int));
    map = new MAP[len * hgt];

    for (int i = 0; i < hgt; i++)
        for (int j = 0; j < len; j++) {
            map[j + i * len].Restore(f);
        }
}

void XMap::Dump(std::ofstream &file) const
{
    for (int i = 0; i < hgt; i++) {
        for (int j = 0; j < len; j++) {
            MAP* tmap = &map[i * len + j];
            int n = tmap->n;
            char vch = std_tile_data[n].view;

            if (tmap->pSpecialObject) {
                vch = tmap->pSpecialObject->view;
            }

            if (!tmap->item_list.empty()) {
                const auto item = *(tmap->item_list.begin());
                vch = item->view;
            }

            if (tmap->pMonster) {
                vch = tmap->pMonster->view;
            }

            file << vch;
        }

        file << "\n";
    }
}

void XMap::ForceRecenter(const int x, const int y)
{
    wx = x - SCR_LEN / 2;

    if (wx + SCR_LEN > len) {
        wx = len - SCR_LEN;
    }

    if (wx < 0) {
        wx = 0;
    }

    wy = y - SCR_HGT / 2;

    if (wy + SCR_HGT > hgt) {
        wy = hgt - SCR_HGT;
    }

    if (wy < 0) {
        wy = 0;
    }
}
