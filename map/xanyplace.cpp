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

#include "creature/creature.h"
#include "game/location.h"
#include "map/map.h"
#include "map/xanyplace.h"

REGISTER_CLASS(XAnyPlace);

extern "C"
{
#include "lauxlib.h"
}

XAnyPlace::XAnyPlace(const XRect& _area, XLocation* _loc) : area(_area)
{
    Setup(_loc);
    im = IM_OTHER;
    onEventLua = nullptr;
}

XAnyPlace::XAnyPlace(const XRect& _area, XLocation* _loc, const char* _onEventLua) : area(_area)
{
    Setup(_loc);
    im = IM_OTHER;

    if (_onEventLua) {
        onEventLua = new char[strlen(_onEventLua) + 1];
        strcpy(onEventLua, _onEventLua);
    }
}

XAnyPlace::~XAnyPlace()
{
    delete[] onEventLua;
}

int XAnyPlace::onCreatureMove(XCreature* cr)
{
    if (onEventLua) {
        lua_pushstring(XLocation::L, onEventLua);
        lua_gettable(XLocation::L, LUA_GLOBALSINDEX);
        lua_pushnumber(XLocation::L, LE_MOVE);
        lua_pushlightuserdata(XLocation::L, cr);
        lua_call(XLocation::L, 2, 1);
        const int res = static_cast<int>(lua_tonumber(XLocation::L, 2));
        lua_pop(XLocation::L, 1);

        return res;
    }

    return 0;
}

int XAnyPlace::onCreatureEnter(XCreature* cr)
{
    if (onEventLua) {
        lua_pushstring(XLocation::L, onEventLua);
        lua_gettable(XLocation::L, LUA_GLOBALSINDEX);
        lua_pushnumber(XLocation::L, LE_MOVE_IN);
        lua_pushlightuserdata(XLocation::L, cr);
        lua_call(XLocation::L, 2, 1);
        const int res = static_cast<int>(lua_tonumber(XLocation::L, 2));
        lua_pop(XLocation::L, 1);

        return res;
    }

    return 0;
}

int XAnyPlace::onCreatureLeave(XCreature* cr)
{
    if (onEventLua) {
        lua_pushstring(XLocation::L, onEventLua);
        lua_gettable(XLocation::L, LUA_GLOBALSINDEX);
        lua_pushnumber(XLocation::L, LE_MOVE_OUT);
        lua_pushlightuserdata(XLocation::L, cr);
        lua_call(XLocation::L, 2, 1);
        const int res = static_cast<int>(lua_tonumber(XLocation::L, 2));
        lua_pop(XLocation::L, 1);

        return res;
    }

    return 0;
}


void XAnyPlace::Invalidate()
{
    location = nullptr;
    owner = nullptr;
    XObject::Invalidate();
}

void XAnyPlace::onShowItem(XItem* item, char* buf)
{
    item->toString(buf);
}

void XAnyPlace::Setup(XLocation* _map)
{
    location = _map;

    for (int i = area.left; i < area.right; i++)
        for (int j = area.top; j < area.bottom; j++) {
            location->map->SetPlace(i, j, this);
        }
}

void XAnyPlace::Store(XFile* f)
{
    XObject::Store(f);

    location.Store(f);
    owner.Store(f);
    area.Store(f);
    size_t sz = 0;

    if (onEventLua) {
        sz = strlen(onEventLua) + 1;
    }

    f->Write(&sz);

    if (sz > 0) {
        f->Write(onEventLua, sz);
    }

    if (onEventLua) {
        lua_pushstring(XLocation::L, onEventLua);
        lua_gettable(XLocation::L, LUA_GLOBALSINDEX);
        lua_pushnumber(XLocation::L, LE_SAVE);
        lua_call(XLocation::L, 1, 1);
        lua_tonumber(XLocation::L, 2);
        lua_pop(XLocation::L, 1);
    }
}

void XAnyPlace::Restore(XFile* f)
{
    XObject::Restore(f);

    location.Restore(f);
    owner.Restore(f);
    area.Restore(f);
    size_t sz = 0;
    f->Read(&sz);

    if (sz > 0) {
        onEventLua = new char [sz];
        f->Read(onEventLua, sz);
    } else {
        onEventLua = nullptr;
    }

    if (onEventLua) {
        lua_pushstring(XLocation::L, onEventLua);
        lua_gettable(XLocation::L, LUA_GLOBALSINDEX);
        lua_pushnumber(XLocation::L, LE_LOAD);
        lua_call(XLocation::L, 1, 1);
        lua_tonumber(XLocation::L, 2);
        lua_pop(XLocation::L, 1);
    }
}
