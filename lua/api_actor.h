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

#include <tuple>
#include <string>

#include <sol/forward.hpp>

/**
 * @defgroup lua_api Lua scripting API
 *
 * Every call a world script can make. Two halves, one per file: the
 * calls in lua/api_actor act on things that already exist, and the ones
 * in lua/api_world build the location currently under construction.
 *
 * The rule for what belongs here is the same on both sides - a call
 * should still mean something in a completely different game built on
 * this engine. Anything Avanor-specific is written in Lua instead.
 */

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
/** @addtogroup lua_api
 *  @{ */

    void InflictDamage(void* target, int dmg, const RESISTANCE& resist, sol::optional<std::string> msg);
    int ChangeStats(void* cr, int st, int val);
    void UseSkill(void* cr, int skill, sol::optional<int> amount);
    std::tuple<sol::optional<int>, sol::optional<int>> AskDirection(void* cr);
    sol::optional<void*> SelectItem(void* cr, sol::protected_function predicate);
    std::string DescribeItem(void* item);
    int GetItemWeight(void* item);
    void SetItemWeight(void* item, int weight);
    int GetItemNutrition(void* item);
    void SetItemNutrition(void* item, int nutrio);
    void ToolHold(void* tool, void* item);
    sol::optional<void*> ToolHeld(void* tool);
    void ToolRelease(void* tool);
    void UnCarryItem(void* item);
    void ToolRemember(void* item, const std::string& key, int value);
    int ToolRecall(void* item, const std::string& key);
    int ThrowItemDice(void* item);
    bool isCreatureVisible(void* cr);
    bool isUniqueCreature(void* cr);
    std::string CreatureName(void* cr, int form);
    std::string CreatureVerb(void* cr, const std::string& verb);
    void AddModifier(void* cr, const std::string& modifier, int power);
    void ChangeNutrition(void* cr, double stomachs);
    void SetNutrition(void* cr, double stomachs);
    int GetStats(void* cr, int st);
    void SetStats(void* cr, const std::string& dice);
    void AddStats(void* cr, const std::string& dice);
    void SetMaxStats(void* cr, const std::string& dice);
    void ClampStats(void* cr);
    void SetMoveEnergy(void* cr, const std::string& dice);
    void SetFoodFeeling(void* cr, int ff);
    int Rand(int val);
    double RandRaw();
    void SetWarSkill(void* cr, const COMBAT_SKILL& cs, int level);
    COMBAT_SKILL GetItemWarSkill(void* item);
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

    // Which group a creature was raised into - the same string Guardian()
    // was given and FindCreature()/FindCreatures() match on. Lets a script
    // ask "whose people are these?" about a creature it was handed.
    std::string GetGroupID(void* cr);

    // Where a creature is standing. Mirrors GetWayXY()'s two-value return.
    // Note when calling this from a place's MOVE_IN handler: the event
    // fires before the step commits (XCreature::NewMove), so what comes
    // back is the tile the creature is stepping *off*, which is what tells
    // a handler which way it is travelling.
    std::tuple<int, int> GetCreatureXY(void* cr);

    // The location a creature stands in, as the same handle a location
    // event handler is passed - what the map calls below want when they
    // are asked at runtime rather than while a location is being built.
    // Nothing for a creature that is nowhere.
    sol::optional<void*> GetCreatureLocation(void* cr);

    bool isEnemy(void* cr1, void* cr2);
    void SetCreatureAI(void* cr, const std::string& lua_class);
    XCreature* AsCreature(void* p);
    XItem* AsItem(void* p);
    int CreatureCountInLocation(const std::string& l_id, const std::string& cc);
    void SetItEnemyFor(void* cr1, void* cr2);
    sol::optional<void*> CreatureNear(void* who, const std::string& name);
    void SetAIFlag(void* cr, unsigned int flags);
    void SetEnemy(void* cr, const sol::object& cr_class);
    sol::optional<void*> FindCreature(const std::string& l_id, const std::string& gid, sol::optional<int> x, sol::optional<int> y, sol::optional<int> w, sol::optional<int> h);
    std::vector<void*> FindCreatures(const std::string& l_id, const std::string& gid, sol::optional<int> x, sol::optional<int> y, sol::optional<int> w, sol::optional<int> h);
    void ExecuteCreatureScript(void* cr, sol::table script);
    std::tuple<int, int> GetWayXY(const std::string& l_id);
    void AddMessage(const std::string& str);
    std::string AskQuestion(const std::string& msg, const std::string& key, sol::variadic_args va);
    int Gender(void* cr);
    XGUID GetObjectGUID(void* obj);
    std::tuple<int, std::string, COMBAT_SKILL, ItemType, int, std::string> GetItemParam(void* item);
    bool IsKind(int kind, int wanted);
    bool HasBrand(const std::string& carried, const std::string& ids);
    std::string GetCreatureClass(void* who);
    void SetCreatureClass(void* who, const std::string& cr_class);
    int Favour(void* who, const std::string& deity);
    void ChangeFavour(void* who, const std::string& deity, int delta);
    void SetFavour(void* who, const std::string& deity, int value);
    std::string GetDeityName(const std::string& deity);
    int Sacrifice(void* who, void* item, const std::string& deity);
    void SetItemBrand(void* item, const std::string& br);
    std::string GetItemName(void* item);
    sol::optional<ItemType> MissileForLauncher(void* weapon);
    void SetItemName(void* item, const std::string& name);
    void AddItemToHit(void* item, int bonus);
    void AddItemRange(void* item, int bonus);
    void AddItemDice(void* item, int count, int sides, int bonus);
    int CastEffect(void* caster, const std::string& effect, int power);
    int MakeEffect(const std::string& effect, void* caller, void* location, int call_x, int call_y, void* target, int target_x, int target_y, int power);
    void DestroyObject(void* item);
    void SetCompanion(void* owner, void* slave, bool flag);
    void GiveObjectToCreature(void* item, void* cr);
    bool GiveAward(void* owner, XGUID aguid, void* target);
    void Quest(const std::string& quest_id, int status, const std::string& know, const std::string& complete, const std::string& closed, sol::optional<int> score);
    void QuestModify(const std::string& id, int status);
    int QuestStatus(const std::string& id);
    int StoreInt(lua_State * L);
    int RestoreInt(lua_State * L);

    bool HasBodyPart(void* cr, int bodypart, int slot);
    sol::optional<void*> GetWornItem(void* cr, int bodypart, int slot);
    std::string GetItemId(void* item);
    std::string GetObjectClass(void* obj);

    // Registers every function above under its Lua name.
    void RegisterActorApi(sol::state_view& lua);

/** @} */
}

#endif
