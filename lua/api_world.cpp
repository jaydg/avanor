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
#include "creature/shopkeeper.h"
#include "engine/xgen.h"
#include "engine/xlua.h"
#include "game/cbuilder.h"
#include "game/game.h"
#include "game/location.h"
#include "game/shop.h"
#include "item/itemf.h"
#include "item/item_misc.h"
#include "item/uniquei.h"
#include "item/xherb.h"
#include "item/xmoney.h"
#include "item/xpotion.h"
#include "lua/api_world.h"
#include "map/map_objects.h"

// Left behind in game/location.cpp on purpose, not overlooked: Altar
// (bound to XDeity), ScatterHerbBushes and CreateMushroom (named after
// two specific plants), BuildShop (drags in XShop and its C++ keeper AI)
// and IsWearingAvanorDefender (an engine global named after a single
// artifact). Those are Avanor content wearing an engine API hat; moving
// them here would bless them as part of the generic interface.
//
// CreateObject's potion overload is a borderline case kept for now only
// because sol::overload has to register all three together - it should
// collapse into one id-based call once object identity is data rather
// than a compiled enum.

namespace lua_api
{

// CreateLocation, DrawPattern and CreateTimerEvent stayed on XLocation:
// each reaches into its private state (brief_name/BuildCave/BuildPlain,
// pat_offs_x/PutPalette, event/ttm respectively), so they are location
// implementation rather than engine API. They still register here, and
// would move once XLocation grows a public seam for them.



//Settle(CreatureClass.RAT + CreatureClass.FELINE + CreatureClass.INSECT, CreatureTemplate.VERY_LOW)
void Settle(CreatureClass crc, int crl)
{
    Game.Scheduler.Add(new XUniversalGen(XLocation::current_location, crc, static_cast<CreatureTemplate::Level>(crl), 5, 25000));
}

//cr = Creature("rotmoth")
//cr = Creature("rat", [x, y, [w, h]])
void* Creature(const std::string& crn, sol::optional<int> x, sol::optional<int> y, sol::optional<int> w, sol::optional<int> h)
{
    XCreature * cr = nullptr;

    if (!x) {
        cr = XLocation::current_location->NewCreature(crn);
    } else {
        int tx = *x;
        int ty = *y;
        XRect rect = w ? XRect(tx, ty, tx + *w, ty + *h) : XRect(tx, ty, tx + 1, ty + 1);
        cr = XLocation::current_location->NewCreature(crn, rect);
    }

    return cr;
}

//cr = Guardian("dwarf_guard", GID_DWARVEN_GUARDIAN, x, y, [len,  hgt], [flags])
void* Guardian(const std::string& crn, const std::string& gid, int x, int y, sol::optional<int> w, sol::optional<int> h, sol::optional<int> flags)
{
    XRect rect = w ? XRect(x, y, x + *w, y + *h) : XRect(x, y, x + 1, y + 1);
    int flag = XStandardAI::GUARD_AREA;

    if (flags) {
        flag |= *flags;
    }

    XCreature * cr = XLocation::current_location->NewCreature(crn, rect, gid, flag);

    // NewCreature() already set enemy_class to NONE for a PEACEFUL
    // creature (see its own comment) - respect that instead of
    // unconditionally overwriting it here. Without this check, every
    // Guardian()-spawned PEACEFUL creature (farmers, goodwives, and
    // anyone else inheriting their template) ends up hostile to every
    // non-human/humanoid class regardless, silently defeating the whole
    // point of being flagged PEACEFUL.
    if (!(cr->xai->GetAIFlag() & XStandardAI::PEACEFUL)) {
        cr->xai->SetEnemyClass(CreatureClass::ALL ^ (CreatureClass::HUMAN | CreatureClass::HUMANOID));
    }

    return cr;
}

//GuardianClass(CreatureClass.ORC, "orcs_war_party", 10, 70, 20, 10, XStandardAI.GUARD_AREA)
void* GuardianClass(CreatureClass crc, const std::string& gid, int x, int y, sol::optional<int> w, sol::optional<int> h, sol::optional<int> flags)
{
    XRect rect = w ? XRect(x, y, x + *w, y + *h) : XRect(x, y, x + 1, y + 1);
    int flag = XStandardAI::GUARD_AREA;

    if (flags) {
        flag |= *flags;
    }

    return XLocation::current_location->NewCreature(crc, rect, gid, flag);
}

//Teleport(23, 20, XLocation.MAIN, 154, 13)
void Teleport(int x, int y, int target_loc_id, int dest_x, int dest_y)
{
    new XTeleport(x, y, XLocation::current_location, (XLocation::Id)target_loc_id, dest_x, dest_y);
}

//Way(DOWN, L_SMALL_CAVE2)
//Way(DOWN, L_SMALL_CAVE2, x, y)
void Way(int type, int loc_id, sol::optional<int> x, sol::optional<int> y)
{
    if (x) {
        XLocation::current_location->NewWay(*x, *y, (XLocation::Id)loc_id, (STAIRWAY_TYPE)type);
    } else {
        XLocation::current_location->NewWay((XLocation::Id)loc_id, (STAIRWAY_TYPE)type);
    }
}

//CreateObject("XCookingSet")
void* CreateObjectByName(const std::string& name)
{
    return XClassFactory::CreateNew((char*)name.c_str());
}

//CreateObject(ItemKind::ITEM - ItemKind::FOOD, 20, 500)
void* CreateObjectByMask(int flag, int min_val, int max_val)
{
    return ICREATE((ItemKind)(flag), min_val, max_val);
}

//CreateObject(PotionName.HEALING)
void* CreateObjectByPotion(int pn)
{
    return new XPotion(static_cast<PotionName>(pn));
}

//DropItem(item, 0, 0)
//DropItem(item)
void DropItem(void* item, sol::optional<int> x, sol::optional<int> y)
{
    XItem * pItem = (XItem*)item;
    int tx;
    int ty;

    if (x) {
        tx = *x;
        ty = *y;
    } else {
        XPoint pt;
        XLocation::current_location->GetFreeXY(&pt);
        tx = pt.x;
        ty = pt.y;
    }

    if (pItem) {
        pItem->Drop(XLocation::current_location, tx, ty);
    }
}

//DropItem(item, object)
void DropItemAt(void* item, void* object)
{
    XItem * pItem = (XItem*)item;
    XMapObject * pMO = (XMapObject*)object;
    pItem->Drop(pMO->l, pMO->x, pMO->y);
}

//SetPattern(width, height,
// "###" ..
// "#.#" ..
// "###")
void SetPattern(int w, int h, const std::string& txt)
{
    XLocation::current_pattern.w = w;
    XLocation::current_pattern.h = h;
    XLocation::current_pattern.pattern = txt;
    XLocation::pattern_translation.clear();
}

//AddTranslation("1", GOLDEN_FLOOR)
//AddTranslation("A", function(x, y) Guardian('dwarf_guard', GID_DWARVEN_GUARDIAN, x, y) end)
void AddTranslation(const std::string& view, sol::object target)
{
    PALETTE_MAP pm;
    pm.this_view = view[0];

    if (target.get_type() == sol::type::function) {
        pm.callback = target.as<sol::protected_function>();
        pm.real_view = XTileType::UNKNOWN;
    } else {
        pm.real_view = (XTileType::Type)target.as<int>();
    }

    XLocation::pattern_translation.push_back(pm);
}


//Furniture(x, y, xLIGHTRED, '~', 'a royal bad')
void* Furniture(int x, int y, int color, const std::string& view, const std::string& descr)
{
    return new XFurniture(x, y, color, view[0], (char*)descr.c_str(), XLocation::current_location);
}

//OuterObject(xLIGHTRED, '~', 'a royal bad', 'EventHandler')
void* OuterObject(int color, const std::string& view, const std::string& descr, sol::optional<std::string> event)
{
    XPoint pt;
    XLocation::current_location->GetFreeXY(&pt);
    return new XOuterObject(pt.x, pt.y, color, view[0], (char*)descr.c_str(), XLocation::current_location, event ? event->c_str() : nullptr);
}

//OuterObject(x, y, xLIGHTRED, '~', 'a royal bad', 'EventHandler')
void* OuterObjectAt(int x, int y, int color, const std::string& view, const std::string& descr, sol::optional<std::string> event)
{
    return new XOuterObject(x, y, color, view[0], (char*)descr.c_str(), XLocation::current_location, event ? event->c_str() : nullptr);
}

void Treasure(int x, int y, int val)
{
    XMoney * money = new XMoney(vRand(val) + val);
    money->Drop(XLocation::current_location, x, y);
}

void Chest(int x, int y, sol::optional<int> cnt, sol::optional<int> flg, sol::optional<int> mnval, sol::optional<int> mxval)
{
    XChest * tchest = new XChest(cnt.value_or(5), (ItemKind)flg.value_or(static_cast<int>(ItemKind::ITEM)), mnval.value_or(100), mxval.value_or(25000));
    tchest->Drop(XLocation::current_location, x, y);
}

void Trap(int x, int y)
{
    new XTrap(x, y, XLocation::current_location);
}

//EventPlace('MushroomCaveEvent')
void EventPlace(const std::string& event)
{
    XRect area(0, 0, XLocation::current_location->map->len, XLocation::current_location->map->hgt);
    new XAnyPlace(area, XLocation::current_location, event);
}

//EventPlace(x, y, 5, 2, 'SmallCaveEvent')
void EventPlaceArea(int x, int y, int w, int h, const std::string& event)
{
    XRect area(x, y, x + w, y + h);
    new XAnyPlace(area, XLocation::current_location, event);
}


void RegisterWorldApi(sol::state_view& lua)
{
    lua.set_function("CreateLocation", &XLocation::CreateLocation);
        lua.set_function("Settle", &lua_api::Settle);
        lua.set_function("Creature", &lua_api::Creature);
        lua.set_function("Guardian", &lua_api::Guardian);
        lua.set_function("GuardianClass", &lua_api::GuardianClass);
        lua.set_function("Teleport", &lua_api::Teleport);
        lua.set_function("Way", &lua_api::Way);
        lua.set_function("CreateObject", sol::overload(&lua_api::CreateObjectByName, &lua_api::CreateObjectByMask, &lua_api::CreateObjectByPotion));
        lua.set_function("DropItem", sol::overload(&lua_api::DropItem, &lua_api::DropItemAt));
        lua.set_function("SetPattern", &lua_api::SetPattern);
        lua.set_function("AddTranslation", &lua_api::AddTranslation);
        lua.set_function("DrawPattern", &XLocation::DrawPattern);
        lua.set_function("Furniture", &lua_api::Furniture);
        lua.set_function("OuterObject", sol::overload(&lua_api::OuterObject, &lua_api::OuterObjectAt));
        lua.set_function("Treasure", &lua_api::Treasure);
        lua.set_function("Chest", &lua_api::Chest);
        lua.set_function("Trap", &lua_api::Trap);
        lua.set_function("EventPlace", sol::overload(&lua_api::EventPlace, &lua_api::EventPlaceArea));
        lua.set_function("CreateTimerEvent", &XLocation::CreateTimerEvent);
}

} // namespace lua_api
