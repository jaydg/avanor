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

#include <sol/sol.hpp>

#include "creature/anycr.h"
#include "creature/bodypart.h"
#include "creature/cr_defs.h"
#include "creature/deity.h"
#include "creature/std_ai.h"
#include "engine/global.h"
#include "engine/xlua.h"
#include "game/location.h"
#include "lua/api_actor.h"
#include "lua/api_world.h"
#include "game/quest.h"
#include "item/item.h"
#include "item/itemdef.h"
#include "item/xpotion.h"
#include "magic/attack_effect_type.h"
#include "magic/effect.h"
#include "magic/resist.h"
#include "magic/skill.h"
#include "magic/stats.h"
#include "magic/wskills.h"
#include "map/map.h"

lua_State* XLua::L = nullptr;

void XLua::Init()
{
    L = lua_open();
    sol::state_view lua(L);

    XLocation::RegisterLua(lua);
    RegisterLuaEventEnum(lua);
    CreatureTemplate::RegisterLua(lua);
    RegisterCrDefsEnums(lua);
    XItem::RegisterLua(lua);
    RegisterItemDefEnums(lua);
    XPotion::RegisterLua(lua);
    XCreature::RegisterLua(lua);
    XTileType::RegisterLua(lua);
    XStandardAI::RegisterLua(lua);
    XWarSkills::RegisterLua(lua);
    RegisterAttackEffectTypeLua(lua);
    XResistance::RegisterLua(lua);
    RegisterColorEnum(lua);
    RegisterBodyPartEnum(lua);
    XDeity::RegisterLua(lua);
    XReligion::RegisterLua(lua);
    XStats::RegisterLua(lua);
    XSkill::RegisterLua(lua);
    XQuest::RegisterLua(lua);
    XEffect::RegisterLua(lua);

    // Still the only two bindings registered through the raw Lua C API
    // rather than sol2 - they take a lua_State* and hand-roll the stack.

    lua.open_libraries(sol::lib::base, sol::lib::string);

    // Sol2-bound Monster builder - registered before world scripts
    // load below, since world/creatures.lua calls it while loading.
    {
        lua.new_usertype<MonsterBuilder>("Monster",
            sol::constructors<MonsterBuilder(CREATURE_NAME), MonsterBuilder(CREATURE_NAME, CREATURE_NAME)>(),
            "View", &MonsterBuilder::View,
            "Basic", &MonsterBuilder::Basic,
            "Body", &MonsterBuilder::Body,
            "AI", &MonsterBuilder::AI,
            "Stats", &MonsterBuilder::Stats,
            "Resist", &MonsterBuilder::Resist,
            "Combat", &MonsterBuilder::Combat,
            "Main", &MonsterBuilder::Main,
            "Description", &MonsterBuilder::Description,
            "Melee", &MonsterBuilder::Melee,
            "MeleeExtra", &MonsterBuilder::MeleeExtra,
            "LearnSkill", &MonsterBuilder::LearnSkill,
            "LearnSpell", &MonsterBuilder::LearnSpell,
            "Equip", &MonsterBuilder::Equip,
            "EquipCount", &MonsterBuilder::EquipCount,
            "Corpse", &MonsterBuilder::Corpse,
            "CorpseEffect", &MonsterBuilder::CorpseEffect,
            "Unique", &MonsterBuilder::Unique,
            "Register", &MonsterBuilder::Register
        );
    }

    // Sol2-bound location/map-building functions
    // Registered before world scripts load below, since
    // locations.lua/valley.lua call these while loading.
    {
        lua_api::RegisterActorApi(lua);
    lua_api::RegisterWorldApi(lua);

        lua.set_function("BuildShop", &XLocation::BuildShop);
    }

    {









    }

    lua.script_file("./world/init.lua");

    // Catch Lua errors loading data
    assert(lua["LoadScripts"]().valid());
    XCreatureStorage::CreateQuickBase();
}
