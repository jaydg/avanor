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
#include "item/item_misc.h"
#include "creature/bodypart.h"
#include "creature/cr_defs.h"
#include "creature/deity.h"
#include "creature/std_ai.h"
#include "engine/global.h"
#include "engine/xlua.h"
#include "map/dungeon_builder.h"
#include "map/pattern.h"
#include "game/location.h"
#include "lua/api_actor.h"
#include "lua/api_world.h"
#include "game/quest.h"
#include "item/item.h"
#include "item/itemdef.h"
#include "item/xcorpse.h"
#include "item/xpotion.h"
#include "magic/attack_effect_type.h"
#include "magic/effect.h"
#include "magic/resist.h"
#include "magic/skill.h"
#include "magic/stats.h"
#include "magic/wskills.h"
#include <iostream>

#include "map/map.h"

lua_State* XLua::L = nullptr;

namespace {

// An enum table the engine registered answers an unknown member with an
// error rather than with nil. Lua hands nil back for a missing table
// field, sol2 turns that nil into 0 for an enum-typed parameter, and
// whatever was built from it - a creature with no class, a tile that is
// "nothing here" - is quietly wrong, surfacing far from the line that
// made it or not until someone is hours into a game. With this, a
// misspelt member stops the world script at the line that misspelt it.
void MakeStrict(sol::state_view& lua, const char* name)
{
    sol::optional<sol::table> table = lua[name];

    if (!table) {
        std::cerr << "lua: no table '" << name << "' to guard against typos" << std::endl;

        return;
    }

    // The members are not in the table itself - sol2 keeps a read-only
    // enum's entries behind its metatable's __index - so the guard has
    // to ask that first and complain only when it has no answer, rather
    // than replace it and take every member with it.
    sol::object inherited = sol::lua_nil;

    if (sol::object meta_object = (*table)[sol::metatable_key]; meta_object.is<sol::table>()) {
        inherited = meta_object.as<sol::table>()["__index"];
    }

    sol::table meta = lua.create_table();
    const std::string table_name = name;

    meta.set_function(sol::meta_function::index,
        [table_name, inherited](sol::this_state state, sol::table self, sol::object key) {
            sol::object value = sol::lua_nil;

            if (inherited.is<sol::table>()) {
                value = inherited.as<sol::table>().get<sol::object>(key);
            } else if (inherited.is<sol::protected_function>()) {
                if (auto result = inherited.as<sol::protected_function>()(self, key); result.valid()) {
                    value = result;
                }
            }

            if (value.valid() && value != sol::lua_nil) {
                return value;
            }

            const std::string member = key.is<std::string>() ? key.as<std::string>() : "?";

            luaL_error(state, "%s.%s does not exist", table_name.c_str(), member.c_str());

            return sol::object(sol::lua_nil);
        });

    (*table)[sol::metatable_key] = meta;
}

} // namespace


void XLua::Init()
{
    // The rooms and the map alphabet both hold sol::protected_functions
    // belonging to the state being replaced here, so neither can outlive
    // it - the world scripts fill both again on the next load.
    room_templates.clear();
    XPattern::ForgetScriptPalette();
    XTileType::ForgetTiles();

    L = lua_open();
    sol::state_view lua(L);

    XLocation::RegisterLua(lua);
    XStairWay::RegisterLua(lua);
    RegisterLuaEventEnum(lua);
    CreatureTemplate::RegisterLua(lua);
    RegisterCrDefsEnums(lua);
    XItem::RegisterLua(lua);
    RegisterItemDefEnums(lua);
    XPotion::RegisterLua(lua);
    RegisterCorpseEffectEnum(lua);
    RegisterSpellNameEnum(lua);
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

    // Every table above holds a fixed set of names the world scripts
    // spell out by hand, so a name that is not in one is a typo, not a
    // value. XTileType is here too: it is filled by DefineTile() as
    // world/tiles.lua runs, and read by name everywhere after that.
    for (const char* enum_table : {
            "AttackEffectType", "BodyPart", "CorpseEffectType", "CreatureClass",
            "CreatureSize", "CreatureTemplate", "Gender", "ItemKind", "ItemType", "LuaEvent",
            "Movability", "PersonType", "PotionName", "ScriptCommand", "ShopDoor", "Spell",
            "Visibility", "xColor", "XDeity", "XEffect", "XLocation", "XQuest",
            "XResistance", "XSkill", "XStairWay", "XStandardAI", "XStats",
            "XTileType", "XWarSkills"
        }) {
        MakeStrict(lua, enum_table);
    }

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

    // Sol2-bound Food builder, registered here for the same reason as the
    // Monster builder above: world/items.lua calls it while loading.
    {
        lua.new_usertype<FoodBuilder>("Food",
            sol::constructors<FoodBuilder(std::string)>(),
            "View", &FoodBuilder::View,
            "Basic", &FoodBuilder::Basic,
            "Nutrition", &FoodBuilder::Nutrition,
            "Register", &FoodBuilder::Register
        );
    }

    // Sol2-bound location/map-building functions
    // Registered before world scripts load below, since
    // locations.lua/valley.lua call these while loading.
    {
        lua_api::RegisterActorApi(lua);
    lua_api::RegisterWorldApi(lua);

    }

    {









    }

    lua.script_file("./world/init.lua");

    // Catch Lua errors loading data.
    // The message is what says which file and line went wrong.
    if (const sol::protected_function_result result = lua["LoadScripts"]();
        !result.valid()) {
        const sol::error err = result;
        std::cerr << "world: " << err.what() << std::endl;
        assert(false && "LoadScripts() failed - see the message above");
    }

    // Nothing can be drawn or walked on before this holds.
    XTileType::ValidateTiles();
    XCreatureStorage::CreateQuickBase();
}

bool XLua::ResultToBool(const sol::protected_function_result& result,
                        const std::string_view handler)
{
    if (!result.valid()) {
        return false;
    }

    if (const sol::optional<bool> answer = result.get<sol::optional<bool>>()) {
        return *answer;
    }

    // See the declaration in engine/xlua.h for why a number has to be
    // called out rather than quietly taken for a "no".
    if (result.get_type() == sol::type::number) {
        std::cerr << "world: " << handler
                  << " answered with a number - event handlers answer true or false"
                  << std::endl;
    }

    return false;
}
