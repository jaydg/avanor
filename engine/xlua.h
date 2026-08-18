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

#ifndef XLUA_H
#define XLUA_H

struct lua_State;

// The game's single Lua runtime.
class XLua
{
    public:
        // Creates the state, registers every binding and enum, then runs
        // world/init.lua. Called once per game start (new or restored).
        static void Init();

        // The raw state, for the `sol::state_view lua(XLua::State())` call
        // sites scattered through the creature/map/location code.
        static lua_State* State()
        {
            return L;
        }

    private:
        static lua_State* L;
};

#endif
