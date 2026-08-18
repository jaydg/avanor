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

#ifndef LUA_API_ACTOR_H
#define LUA_API_ACTOR_H

#include <string>

#include <sol/forward.hpp>

// Script API operating on things that already exist - creatures, items,
// the message window, the quest log - plus the small utility calls.
//
// Deliberately free functions in a namespace rather than statics on
// XLocation, where they used to live: none of them touches a location,
// and none of them is Avanor-specific. Everything here should still make
// sense in a completely different game built on this engine; anything
// that would not belongs in Lua, not in the C++ API.
//
// Its counterpart is lua/api_world.* - the calls that build the location
// currently under construction (XLocation::current_location).
namespace lua_api
{
    void InflictDamage(void* target, int dmg, int resist, sol::optional<std::string> msg);
    void ChangeStats(void* cr, int st, int val);
    int GetStats(void* cr, int st);
    int Rand(int val);
    void SetEventHandler(void* cr, const std::string& event);
    void EnableMoveHandler(void* cr);
    void DisableMoveHandler(void* cr);
    void SetMainCreature(void* cr);
    int GetSkill(void* cr, int skill);
    void LearnSkill(void* cr, int skill, int val);
    int MoneyOperation(void* cr, int val);
    void SetName(void* obj, const std::string& name);
    void SetView(void* obj, const std::string& view, int color);
    std::string GetView(void* obj);
    bool isHero(void* cr);
    bool isEnemy(void* cr1, void* cr2);
    void SetCreatureAI(void* cr, const std::string& lua_class);
    XCreature* AsCreature(void* p);
    XItem* AsItem(void* p);
    int CreatureCountInLocation(int l_id, CreatureClass cc);
    void SetItEnemyFor(void* cr1, void* cr2);
    void SetEnemy(void* cr, int cr_class);
    void* FindCreature(int l_id, const std::string& gid, sol::optional<int> x, sol::optional<int> y, sol::optional<int> w, sol::optional<int> h);
    std::vector<void*> FindCreatures(int l_id, const std::string& gid, sol::optional<int> x, sol::optional<int> y, sol::optional<int> w, sol::optional<int> h);
    void ExecuteCreatureScript(void* cr, sol::table script);
    std::tuple<int, int> GetWayXY(int l_id);
    void AddMessage(const std::string& str);
    std::string AskQuestion(const std::string& msg, const std::string& key, sol::variadic_args va);
    int Gender(void* cr);
    XGUID GetObjectGUID(void* obj);
    std::tuple<int, int, int, int, int, std::string> GetItemParam(void* item);
    void SetItemBrand(void* item, int br);
    int MakeEffect(int effect, void* caller, void* location, int call_x, int call_y, void* target, int target_x, int target_y, int power);
    void DestroyObject(void* item);
    void SetCompanion(void* owner, void* slave, bool flag);
    void GiveObjectToCreature(void* item, void* cr);
    bool GiveAward(void* owner, XGUID aguid, void* target);
    void Quest(int quest_id, int status, const std::string& know, const std::string& complete, const std::string& closed);
    void QuestModify(int id, int status);
    int QuestStatus(int id);
    int StoreInt(lua_State * L);
    int RestoreInt(lua_State * L);
    bool BinaryAND(int v1, int v2);

    void* GetWornItem(void* cr, int bodypart, int slot);
    std::string GetObjectClass(void* obj);

    // Registers every function above under its Lua name.
    void RegisterActorApi(sol::state_view& lua);
}

#endif
