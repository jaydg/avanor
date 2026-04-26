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

#ifndef CAVE_H
#define CAVE_H

#include "engine/xobject.h"
#include "helpers/point.h"
#include "helpers/rect.h"
#include "map/map.h"

class XMap;

enum RANDOM_CAVE_TYPE {
    RCT_SIMPLE1 = 0,
    RCT_SIMPLE2,
    RCT_SIMPLE3,
    RCT_SIMPLE4,
    RCT_USUAL, //just a simple random XY cave
};

enum CAVE_FLAGS {
    CREATE_RANDOM_TRAP_ON_CHEST	= 0x00000001,
    CREATE_TRAP_ON_CHEST	= 0x00000002,
    CREATE_GUARD_ON_ROOM	= 0x00000004,
};

struct CAVE_DATA {
    int width;
    int height;
    unsigned int cf;
    int freq;
    const char* cave;

    bool isExit(int x, int y);
    char GetCode(int x, int y);
};

class XCave
{
        RANDOM_CAVE_TYPE rct;
        int map_len;
        int map_hgt;
    public:
        XRect r;
        std::vector<XPoint> exits;
        XCave(int len, int hgt, bool isAllowSpecialRooms);
        int Intersect(XCave * xc, int dist);
        void Draw(XLocation * l);
        ~XCave() { }

        bool GetFreeExit(XPoint * pt);
        int Compare(XObject * o);
};

#endif
