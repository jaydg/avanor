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

#include <cereal/archives/json.hpp>
#include <cereal/types/polymorphic.hpp>

#include "creature/creature.h"
#include "game/location.h"
#include "map/map.h"
#include "map/xanyplace.h"

#include <sol/sol.hpp>

REGISTER_CLASS(XAnyPlace);
CEREAL_REGISTER_TYPE(XAnyPlace);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XObject, XAnyPlace);

void XAnyPlace::NotifyLuaEvent(bool is_load)
{
    XLocation::lua_int_buffer = &lua_ints;
    XLocation::lua_int_index = 0;

    if (!onEventLua) {
        return;
    }

    sol::state_view lua(XLocation::L);
    lua[onEventLua](is_load ? LE_LOAD : LE_SAVE);
}

XAnyPlace::XAnyPlace(const XRect& _area, XLocation* _loc) : area(_area)
{
    Setup(_loc);
    im = IM_UNKNOWN;
    onEventLua = nullptr;
}

XAnyPlace::XAnyPlace(const XRect& _area, XLocation* _loc, const char* _onEventLua) : area(_area)
{
    Setup(_loc);
    im = IM_UNKNOWN;

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
    if (!onEventLua) {
        return 0;
    }

    // cr stays void*, not XCreature* - Sol2 pushes void* as light userdata
    sol::state_view lua(XLocation::L);
    sol::protected_function_result result = lua[onEventLua](LE_MOVE, (void*)cr);

    if (!result.valid()) {
        return 0;
    }

    return result.get<sol::optional<int>>().value_or(0);
}

int XAnyPlace::onCreatureEnter(XCreature* cr)
{
    if (!onEventLua) {
        return 0;
    }

    sol::state_view lua(XLocation::L);
    sol::protected_function_result result = lua[onEventLua](LE_MOVE_IN, (void*)cr);

    if (!result.valid()) {
        return 0;
    }

    return result.get<sol::optional<int>>().value_or(0);
}

int XAnyPlace::onCreatureLeave(XCreature* cr)
{
    if (!onEventLua) {
        return 0;
    }

    sol::state_view lua(XLocation::L);
    sol::protected_function_result result = lua[onEventLua](LE_MOVE_OUT, (void*)cr);

    if (!result.valid()) {
        return 0;
    }

    return result.get<sol::optional<int>>().value_or(0);
}

void XAnyPlace::Invalidate()
{
    location = nullptr;
    owner.reset();
    XObject::Invalidate();
}

std::string XAnyPlace::onShowItem(XItem* item)
{
    return item->toString();
}

void XAnyPlace::Setup(XLocation* _map)
{
    location = _map;

    for (int i = area.left; i < area.right; i++)
        for (int j = area.top; j < area.bottom; j++) {
            location->map->SetPlace(i, j, this);
        }
}

