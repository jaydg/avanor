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

#include "engine/xlua.h"
#include <cereal/types/polymorphic.hpp>
#include <sol/sol.hpp>

#include "creature/lua_ai.h"
#include "game/location.h"

CEREAL_REGISTER_TYPE(XLuaAI);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XStandardAI, XLuaAI);

XLuaAI::XLuaAI(XCreature* cr, const std::string& lc) : XStandardAI(cr), lua_class(lc)
{
    ResolveHooks();
}

void XLuaAI::ResolveHooks()
{
    sol::state_view lua(XLua::State());
    sol::object obj = lua[lua_class];

    // lua[lua_class] is nil (not a table) both for a genuinely missing/
    // mistyped class name and - critically - for the Cereal placeholder
    // construction that happens on every load (see
    // CEREAL_LOAD_VIA_PLACEHOLDER_CONSTRUCT in engine/xobject.h): it
    // constructs via this same real constructor with lua_class="" before
    // serialize() has populated the actual saved name. sol::table's own
    // constructor would type-check-panic on a non-table value here, so
    // check the type first rather than converting straight to sol::table.
    if (obj.get_type() != sol::type::table) {
        has_isEnemy = false;
        has_onWasAttacked = false;
        has_onDie = false;
        has_onSteal = false;
        return;
    }

    sol::table cls = obj;

    has_isEnemy = cls["isEnemy"].valid();
    has_onWasAttacked = cls["onWasAttacked"].valid();
    has_onDie = cls["onDie"].valid();
    has_onSteal = cls["onSteal"].valid();
}

bool XLuaAI::isEnemy(XCreature* cr)
{
    if (has_isEnemy) {
        sol::state_view lua(XLua::State());
        sol::protected_function_result result = lua[lua_class]["isEnemy"](ai_owner, cr);

        if (result.valid()) {
            if (sol::optional<bool> ret = result; ret) {
                return *ret;
            }
        }
    }

    return XStandardAI::isEnemy(cr);
}

void XLuaAI::onWasAttacked(XCreature* attacker)
{
    if (has_onWasAttacked) {
        sol::state_view lua(XLua::State());
        sol::protected_function_result result = lua[lua_class]["onWasAttacked"](ai_owner, attacker);

        if (result.valid()) {
            if (sol::optional<bool> handled = result; handled && *handled) {
                return;
            }
        }
    }

    XStandardAI::onWasAttacked(attacker);
}

void XLuaAI::onDie(XCreature* killer)
{
    if (has_onDie) {
        sol::state_view lua(XLua::State());
        sol::protected_function_result result = lua[lua_class]["onDie"](ai_owner, killer);

        if (result.valid()) {
            if (sol::optional<bool> handled = result; handled && *handled) {
                return;
            }
        }
    }

    XStandardAI::onDie(killer);
}

void XLuaAI::onSteal(XCreature* rogue)
{
    if (has_onSteal) {
        sol::state_view lua(XLua::State());
        sol::protected_function_result result = lua[lua_class]["onSteal"](ai_owner, rogue);

        if (result.valid()) {
            if (sol::optional<bool> handled = result; handled && *handled) {
                return;
            }
        }
    }

    XStandardAI::onSteal(rogue);
}
