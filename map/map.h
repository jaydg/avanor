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

#ifndef MAP_H
#define MAP_H

#include <set>

#include "engine/global.h"
#include "helpers/rect.h"
#include "map/xanyplace.h"

#define MAP_MINX 10
#define MAP_MINY 10
#define MAP_MAXX 200
#define MAP_MAXY 200
#define SCR_HGT  (size_y - 5)
#define SCR_LEN  (size_x)
#define SCR_X    0
#define SCR_Y    2

enum MOVABILITY {
    MO_UNKNOWN,
    MO_NORMAL,
    MO_SHARD,
    MO_AHARD,
    MO_HARD,
    MO_VHARD,
    MO_UNWALKABLE,
    MO_WATER,
    MO_DEEPWATER,
    MO_WALL = 80,
    MO_MOUNTAIN
};

enum VISIBILITY {
    VI_UNKNOWN,
    VI_NORMAL,
    VI_SHARD,
    VI_AHARD,
    VI_HARD,
    VI_VHARD,
    VI_WALL = 80
};


struct XTileType {
    enum Type {
        UNKNOWN,
        GREEN_GRAS,
        TREE,
        SAND,
        WINDOW,
        MAGMA,
        QUARTZ,
        CAVE_FLOOR,
        STONE_FLOOR,
        PATH,
        WOOD_WALL,
        STONE_WALL,
        WATER,
        DEEP_WATER,
        LAVA,
        HILL,
        LOW_MOUNTAIN,
        MOUNTAIN,
        HIGH_MOUNTAIN,
        BRIDGE,
        ROAD,
        OBSIDIAN_FLOOR,
        FENCE,
        GOLDEN_FLOOR,
        MARBLE_WALL,
        BLACK_MARBLE_WALL,
        GOLDEN_FENCE,
        TELEPORT_WHITE
    };

    char view;
    char color;
    const char* name;
    MOVABILITY movability; // 0 - normal, 1 -little hard, 80 - UNMOVEABLE
    VISIBILITY visibility; // 0 - normal, 1 ... 80 - wall
};

extern XTileType std_tile_data[];

/* Forward declarations */
class XMapObject;
class XItem;

struct compare {
    bool operator()(const XItem* lhs, const XItem* rhs) const;
};

typedef std::set<XItem*, compare> XItemList;

struct MAP {
    MAP();
    ~MAP();
    void Store(XFile* f);
    void Restore(XFile* f);

    XTileType::Type n;
    XPtr<XCreature> pMonster;        // if null then no monster here
    XItemList item_list;             // list of item in this cell of map. Automatic construct/destruct
    XPtr<XMapObject> pSpecialObject; // door, way, trap door.
    bool visible;                    // visible for HERO!!!
    char known;                      // for hero memory
    char color;                      // for hero memory

    // Pointer to an object which describes this place. It can be NULL, Shop,
    // special room etc. It will be useful to mark all rooms in cave (to create
    // good links between rooms or to create unique room descriptions).
    XPtr<XAnyPlace> place;

    // 0 - no room
    // 1, 2, ... - room ID
    // Currently used to indicate "special" rooms which should not be crossed
    // by passages.
    int room_id;
};

class XMap
{
    public:
        void ForceRecenter(int x, int y);
        int hgt, len;
        int wx, wy;
        MAP* map;
        XMap();
        XMap(int l, int h);
        ~XMap();

        void Put(XCreature* cr) const;
        void Center(int x, int y);
        void PutChar(int x, int y, char c, int color) const;
        [[nodiscard]] int GetMovability(int x, int y) const;
        [[nodiscard]] int XGetMovability(int x, int y) const;
        [[nodiscard]] int GetVisibility(int x, int y) const;
        void SetXY(int x, int y, XTileType::Type std_map) const;
        [[nodiscard]] XTileType::Type GetXY(int x, int y) const;
        void SetRoom(int x, int y, int room_id) const;
        [[nodiscard]] int GetRoom(int x, int y) const;

        void SetVisible(int x, int y) const;
        void ResVisible(int x, int y) const;
        [[nodiscard]] bool GetVisible(int x, int y) const;

        void PutItem(int x, int y, XItem* item) const;
        [[nodiscard]] unsigned int GetItemCount(int x, int y) const;
        [[nodiscard]] XItemList* GetItemList(int x, int y) const;

        void SetMonster(int x, int y, XCreature* monst) const;
        void ResMonster(int x, int y) const;
        [[nodiscard]] XCreature* GetMonster(int x, int y) const;

        void SetPlace(int x, int y, XAnyPlace* place) const;
        [[nodiscard]] XAnyPlace* GetPlace(int x, int y) const;

        void SetKnown(int x, int y) const;
        void ResKnown(int x, int y) const;
        [[nodiscard]] int GetKnown(int x, int y) const;

        [[nodiscard]] const char* GetDescription(int x, int y) const;

        void SetSpecial(int x, int y, XMapObject* spec) const;
        [[nodiscard]] XMapObject* GetSpecial(int x, int y) const;

        void CreateRoom(int x, int y, int l, int h, XTileType::Type m1, XTileType::Type m2) const;
        void CreateRoom(int x, int y, int l, int h, int px, int py, XTileType::Type m1, XTileType::Type m2) const;

        void Store(XFile* f) const;
        void Restore(XFile* f);

        void Dump(FILE* f) const;
};

#endif
