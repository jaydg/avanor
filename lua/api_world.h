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

#ifndef LUA_API_WORLD_H
#define LUA_API_WORLD_H

#include <string>
#include <tuple>

#include <sol/forward.hpp>

#include "map/dungeon_builder.h"
#include "map/map_objects.h"

// Script API for building the location currently under construction
// (XLocation::current_location) - terrain patterns, ways, creatures,
// items and the special objects placed on a map.
//
// Free functions in a namespace rather than statics on XLocation, and
// deliberately limited to calls that would still mean something in a
// different game built on this engine. Avanor-specific placement helpers
// are NOT here - see the note in lua/api_world.cpp.
//
// Its counterpart is lua/api_actor.* - the calls that operate on things
// that already exist.
namespace lua_api
{
    void Settle(CreatureClass crc, int crl);
    void* Creature(const std::string& crn, sol::optional<int> x, sol::optional<int> y, sol::optional<int> w, sol::optional<int> h);
    void* Guardian(const std::string& crn, const std::string& gid, int x, int y, sol::optional<int> w, sol::optional<int> h, sol::optional<int> flags);
    void* GuardianClass(CreatureClass crc, const std::string& gid, int x, int y, sol::optional<int> w, sol::optional<int> h, sol::optional<int> flags);
    void Teleport(int x, int y, const std::string& target_loc_id, int dest_x, int dest_y);
    void Way(XStairWay::Type type, const std::string& loc_id, sol::optional<int> x, sol::optional<int> y);
    void* CreateObjectByName(const std::string& name);
    void* CreateObjectByMask(int flag, int min_val, int max_val);
    void* CreateObjectByPotion(int pn);
    void DropItem(void* item, sol::optional<int> x, sol::optional<int> y);
    void DropItemAt(void* item, void* object);
    void SetPattern(int w, int h, const std::string& txt);
    void DefineRoom(int weight, int w, int h, const std::string& pattern, sol::table translations,
                    sol::optional<sol::protected_function> on_drawn);
    void AddTranslation(const std::string& view, sol::object target);
    void SetDefaultTranslations(sol::table translations);
    void SetFloorPriority(sol::table floors);
    void Door(int x, int y, sol::optional<bool> opened);
    void* Furniture(int x, int y, int color, const std::string& view, const std::string& descr);
    void* OuterObject(int color, const std::string& view, const std::string& descr, sol::optional<std::string> event);
    void* OuterObjectAt(int x, int y, int color, const std::string& view, const std::string& descr, sol::optional<std::string> event);
    void Treasure(int x, int y, int val);
    void Chest(int x, int y, sol::optional<int> cnt, sol::optional<int> flg, sol::optional<int> mnval, sol::optional<int> mxval);
    void Trap(int x, int y);
    void SetStartLocation(const std::string& loc_id, sol::optional<int> x, sol::optional<int> y,
                          sol::optional<int> w, sol::optional<int> h);
    void SetWanderingAllowed(const std::string& loc_id, bool allowed);
    void EventPlace(const std::string& event);
    void EventPlaceArea(int x, int y, int w, int h, const std::string& event);

    std::tuple<int, int> GetMapSize(sol::optional<void*> location);
    int GetTile(int x, int y, sol::optional<void*> location);
    bool HasSpecial(int x, int y, sol::optional<void*> location);
    sol::optional<std::tuple<int, int>> GetFreeXY(sol::optional<void*> location);
    sol::object PlaceSpecial(const std::string& class_name, int x, int y,
                             sol::this_state s, sol::optional<void*> location);

    // Registers every function above under its Lua name.
    void RegisterWorldApi(sol::state_view& lua);
}

#endif
