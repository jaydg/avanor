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
    lua_register(L, "StoreInt", XLocation::StoreInt);
    lua_register(L, "RestoreInt", XLocation::RestoreInt);

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
        lua.set_function("CreateLocation", &XLocation::CreateLocation);
        lua.set_function("Settle", &XLocation::Settle);
        lua.set_function("Creature", &XLocation::Creature);
        lua.set_function("Guardian", &XLocation::Guardian);
        lua.set_function("GuardianClass", &XLocation::GuardianClass);
        lua.set_function("Teleport", &XLocation::Teleport);
        lua.set_function("ScatterHerbBushes", &XLocation::ScatterHerbBushes);
        lua.set_function("Way", &XLocation::Way);
        lua.set_function("CreateObject", sol::overload(&XLocation::CreateObjectByName, &XLocation::CreateObjectByMask, &XLocation::CreateObjectByPotion));
        lua.set_function("DropItem", sol::overload(&XLocation::DropItem, &XLocation::DropItemAt));
        lua.set_function("SetPattern", &XLocation::SetPattern);
        lua.set_function("AddTranslation", &XLocation::AddTranslation);
        lua.set_function("DrawPattern", &XLocation::DrawPattern);
        lua.set_function("BuildShop", &XLocation::BuildShop);
        lua.set_function("Furniture", &XLocation::Furniture);
        lua.set_function("OuterObject", sol::overload(&XLocation::OuterObject, &XLocation::OuterObjectAt));
        lua.set_function("Altar", &XLocation::Altar);
        lua.set_function("Treasure", &XLocation::Treasure);
        lua.set_function("Chest", &XLocation::Chest);
        lua.set_function("Trap", &XLocation::Trap);
        lua.set_function("EventPlace", sol::overload(&XLocation::EventPlace, &XLocation::EventPlaceArea));
        lua.set_function("CreateMushroom", &XLocation::CreateMushroom);
    }

    {
        lua.set_function("isHero", &XLocation::isHero);
        lua.set_function("isEnemy", &XLocation::isEnemy);
        lua.set_function("SetCreatureAI", &XLocation::SetCreatureAI);
        lua.set_function("AsCreature", &XLocation::AsCreature);
        lua.set_function("AsItem", &XLocation::AsItem);
        lua.set_function("GetCreatureCount", &XLocation::CreatureCountInLocation);
        lua.set_function("IsWearingAvanorDefender", &XLocation::IsWearingAvanorDefender);
        lua.set_function("FindCreature", &XLocation::FindCreature);
        lua.set_function("FindCreatures", &XLocation::FindCreatures);
        lua.set_function("ExecuteCreatureScript", &XLocation::ExecuteCreatureScript);
        lua.set_function("GetWayXY", &XLocation::GetWayXY);

        lua.set_function("AddMessage", &XLocation::AddMessage);
        lua.set_function("AskQuestion", &XLocation::AskQuestion);

        lua.set_function("SetItEnemyFor", &XLocation::SetItEnemyFor);
        lua.set_function("SetEnemy", &XLocation::SetEnemy);
        lua.set_function("ChangeStats", &XLocation::ChangeStats);
        lua.set_function("GetStats", &XLocation::GetStats);
        lua.set_function("InflictDamage", &XLocation::InflictDamage);
        lua.set_function("Rand", &XLocation::Rand);
        lua.set_function("SetEventHandler", &XLocation::SetEventHandler);
        lua.set_function("EnableMoveHandler", &XLocation::EnableMoveHandler);
        lua.set_function("DisableMoveHandler", &XLocation::DisableMoveHandler);
        lua.set_function("SetMainCreature", &XLocation::SetMainCreature);
        lua.set_function("CreateTimerEvent", &XLocation::CreateTimerEvent);

        lua.set_function("GetSkill", &XLocation::GetSkill);
        lua.set_function("LearnSkill", &XLocation::LearnSkill);
        lua.set_function("MoneyOperation", &XLocation::MoneyOperation);

        lua.set_function("SetName", &XLocation::SetName);
        lua.set_function("SetView", &XLocation::SetView);
        lua.set_function("GetView", &XLocation::GetView);

        lua.set_function("GetObjectGUID", &XLocation::GetObjectGUID);
        lua.set_function("GetItemParam", &XLocation::GetItemParam);
        lua.set_function("SetItemBrand", &XLocation::SetItemBrand);
        lua.set_function("GiveObjectToCreature", &XLocation::GiveObjectToCreature);
        lua.set_function("GiveAward", &XLocation::GiveAward);

        lua.set_function("MakeEffect", &XLocation::MakeEffect);
        lua.set_function("DestroyObject", &XLocation::DestroyObject);
        lua.set_function("SetCompanion", &XLocation::SetCompanion);

        lua.set_function("Quest", &XLocation::Quest);
        lua.set_function("QuestModify", &XLocation::QuestModify);
        lua.set_function("QuestStatus", &XLocation::QuestStatus);
        lua.set_function("Gender", &XLocation::Gender);

        lua.set_function("BinaryAND", &XLocation::BinaryAND);

    }

    lua.script_file("./world/init.lua");

    // Catch Lua errors loading data
    assert(lua["LoadScripts"]().valid());
    XCreatureStorage::CreateQuickBase();
}
