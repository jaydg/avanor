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

#ifndef LUA_AI_H
#define LUA_AI_H

#include <string>

#include <cereal/cereal.hpp>
#include <sol/forward.hpp>

#include "creature/std_ai.h"

// A single, generic XStandardAI subclass that lets Lua "subclass" AI
// behavior by name, instead of every unique-monster AI override needing
// its own hand-written C++ class (compare XBanditAI/XRotmothAI/
// XShopKeeperAI, each with their own CEREAL_REGISTER_TYPE/
// CEREAL_LOAD_VIA_PLACEHOLDER_CONSTRUCT boilerplate). `lua_class` names a
// global Lua table (e.g. "BanditAI") with optional isEnemy/onWasAttacked/
// onDie/onSteal functions, called as LuaClass.hookName(self_creature,
// ...args). isEnemy returns true/false to answer definitively, or nil to
// defer to XStandardAI's own isEnemy(); the void hooks return true to
// suppress XStandardAI's default behavior entirely, or false/nil to run
// it afterward.
//
// Whether each hook exists is resolved once (ctor, and once after Cereal
// load) into a handful of bools rather than re-looked-up per call:
// isEnemy() in particular sits on the hottest path in the AI engine (once
// per visible creature per turn via the line-of-sight sweep, plus a
// 225-cell nested loop in the hero's auto-target code - see
// creature/xhero.cpp), so a creature whose Lua AI class doesn't override
// it must pay effectively nothing beyond the four bool checks.
// Note: XLuaAI is deliberately NOT given its own sol2 usertype/RegisterLua.
// The hook mechanism below (isEnemy/onWasAttacked/onDie/onSteal) is plain
// C++ virtual dispatch and works regardless - Lua never needs to know
// XLuaAI exists as a distinct type for that. The one thing this rules out
// is Lua code calling AsCreature(cr).xai:someCreatureAIMethod() on a
// creature whose AI was swapped via SetCreatureAI (xai's static type is
// XStandardAI*, but sol2 tracks runtime type for classes with virtual
// methods, so a XLuaAI instance pushed through that property needs its
// own registration to carry CreatureAI's methods) - not needed by any
// current Lua AI class, so left out for now rather than chasing it.
class XLuaAI : public XStandardAI
{
    public:
        XLuaAI() = delete;
        XLuaAI(XCreature* cr, const std::string& lua_class);

        bool isEnemy(XCreature* cr) override;
        void onWasAttacked(XCreature* attacker) override;
        void onDie(XCreature* killer) override;
        void onSteal(XCreature* rogue) override;

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XStandardAI>(this));
            ar(lua_class);

            if constexpr (Archive::is_loading::value) {
                ResolveHooks();
            }
        }

    private:
        std::string lua_class;
        bool has_isEnemy = false;
        bool has_onWasAttacked = false;
        bool has_onDie = false;
        bool has_onSteal = false;

        void ResolveHooks();
};

// Must live here, not in lua_ai.cpp: XLuaAI's construction gets
// instantiated from inside XCreature::load() itself, wherever that gets
// reinstantiated - same cross-TU visibility reasoning as XStandardAI's
// own CEREAL_LOAD_VIA_PLACEHOLDER_CONSTRUCT (see std_ai.h).
CEREAL_LOAD_VIA_PLACEHOLDER_CONSTRUCT(XLuaAI, serialize, nullptr, "");

#endif
