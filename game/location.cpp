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
#include <cctype>
#include <fstream>
#include <iostream>
#include <vector>

#include <cereal/archives/json.hpp>
#include <cereal/types/polymorphic.hpp>

#include "creature/skeep_ai.h"
#include "creature/lua_ai.h"
#include "creature/shopkeeper.h"
#include "engine/xgen.h"
#include "map/pattern_builder.h"
#include "map/cave_builder.h"
#include "map/chambers_builder.h"
#include "map/delve_builder.h"
#include "map/dungeon_builder.h"
#include "map/plain_builder.h"
#include "game/game.h"
#include "game/location.h"
#include "game/quest.h"
#include "game/setting.h"
#include "game/shop.h"
#include "helpers/msgwin.h"
#include "item/itemf.h"
#include "item/item_misc.h"
#include "item/uniquei.h"
#include "item/xherb.h"
#include "item/xpotion.h"
#include "magic/attack_effect_type.h"
#include "map/map_objects.h"

#include <sol/sol.hpp>


REGISTER_CLASS(XLocation);
CEREAL_REGISTER_TYPE(XLocation);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XObject, XLocation);

void XLocation::FixupWaysList()
{
    ways_list.clear();

    for (int i = 0; i < map->CellCount(); i++) {
        if (auto* way = dynamic_cast<XStairWay*>(map->map[i].pSpecialObject.get())) {
            ways_list.push_back(way);
        }
    }

    for (auto& p : places) {
        if (dynamic_cast<XStairWay *>(p.get())) {
            ways_list.push_back(p.get());
        }
    }
}

void XLocation::FixupMapObjectPositions()
{
    for (int i = 0; i < map->CellCount(); i++) {
        {
            const int x = map->CellX(i);
            const int y = map->CellY(i);
            auto& cell = map->map[i];

            if (auto& cr = cell.pMonster) {
                cr->x = x;
                cr->y = y;
                cr->nx = x;
                cr->ny = y;
                cr->SetLocation(this);
            }

            for (auto& item : cell.item_list) {
                item->x = x;
                item->y = y;
                item->SetLocation(this);
            }

            if (auto& spec = cell.pSpecialObject) {
                spec->x = x;
                spec->y = y;
                spec->SetLocation(this);
            }
        }
    }
}

void XLocation::FixupShops()
{
    for (auto& p : places) {
        auto* shop = dynamic_cast<XShop*>(p.get());

        if (!shop) {
            continue;
        }

        if (auto owner = shop->GetOwner().lock()) {
            if (auto* ai = dynamic_cast<XShopKeeperAI*>(owner->xai.get())) {
                ai->SetShop(shop);
            }
        }
    }
}

XLocation::XLocation(const std::string& location_id)
{
    visited_by_hero = 0;
    map = nullptr;	//map will created by XBuilder...

    for (int i = 0; i < MAX_PLACES; i++) {
        places[i] = nullptr;
    }

    // Free-form: any name a script cares to invent. Nothing validates it
    // here - XLocation::ValidateWorld() sweeps the finished world instead,
    // so a typo is reported once, with its referrer, rather than silently
    // creating a second location or failing at the point of use.
    assert(Game.Location(location_id) == nullptr);
    id = location_id;
    Game.locations[id] = std::shared_ptr<XLocation>(this);

    ttmb = 1000000;
    ttm = ttmb;
}

void XLocation::OnInvalidate()
{
    for (int i = 0; i < MAX_PLACES; i++)
        if (places[i]) {
            // No XPtr<XShop> reference exists anywhere anymore to defer
            // to, so XObject::Invalidate()'s own generic logic
            // (reference == 0 && weak_from_this().expired(), both always
            // true for a shop - it's never shared_ptr-wrapped) now
            // deletes the object synchronously, right here. release()
            // (not reset()/= nullptr) is still required: the object is
            // already gone by the time Invalidate() returns, so the
            // unique_ptr must let go of the pointer without also trying
            // to delete it a second time.
            places[i]->Invalidate();
            places[i].release();
        }

    delete map; // map must be the last!!!!!
    map = nullptr;

    XObject::OnInvalidate();
}

bool XLocation::Run()
{
    if (event.size()) {
        sol::state_view lua(XLua::State());
        sol::protected_function_result result = lua[event]((void*)this);
        ttm = ttmb;

        // Unlike the other handlers, a location event that could not be
        // run answers true: false here would unschedule the location.
        if (!result.valid()) {
            return true;
        }

        return XLua::ResultToBool(result, event);
    }

    return true;
}

void XLocation::AddPlace(XAnyPlace * pl)
{
    for (int i = 0; i < MAX_PLACES; i++)
        if (places[i] == nullptr) {
            places[i].reset(pl);
            return;
        }

    assert(0);
}

std::optional<XPoint> XLocation::GetFreeXY(XRect * area)
{
    int bx, by, dx, dy;

    if (area) {
        bx = area->left;
        by = area->top;
        dx = area->Width();
        dy = area->Height();
    } else {
        // The part of the map this level actually holds - for a floor
        // above another one, looking anywhere else would be looking at
        // the level below, where this level has no room to offer.
        bx = map->stored_x;
        by = map->stored_y;
        dx = map->stored_len;
        dy = map->stored_hgt;
    }

    for (int f = 10000; f-- > 0; ) {
        const int tx = vRand() % dx + bx;
        const int ty = vRand() % dy + by;

        if (map->XGetMovability(tx, ty) == 0 && map->GetSpecial(tx, ty) == nullptr) {
            return XPoint(tx, ty);
        }
    }

    return std::nullopt;
}

XCreature* XLocation::NewCreature(CREATURE_NAME cn, int x, int y, GROUP_ID gid)
{
    XCreature * cr = XCreatureStorage::Create(cn);
    cr->setGroupID(gid);

    if (cr->xai->GetAIFlag() & XStandardAI::PEACEFUL) {
        cr->xai->SetEnemyClass(CreatureClass::NONE); //by default all creatures in pease with others.
    }

    Game.NewCreature(cr, x, y, this);
    return cr;
}

XCreature* XLocation::NewCreature(CREATURE_NAME cn)
{
    const auto pt = GetFreeXY();

    return pt ? NewCreature(cn, pt->x, pt->y) : nullptr;
}

XCreature* XLocation::NewCreature(CREATURE_NAME cn, XRect& rect, GROUP_ID gid, unsigned int ai_flags)
{
    const auto pt = GetFreeXY(&rect);

    if (!pt) {
        return nullptr;
    }

    XCreature * cr = NewCreature(cn, pt->x, pt->y, gid);

    if (ai_flags & XStandardAI::GUARD_AREA) {
        cr->xai->SetArea(rect, id);
        cr->xai->LearnTraps();
    }

    cr->xai->SetAIFlag((XStandardAI::Flag)(ai_flags));

    return cr;
}

XCreature* XLocation::NewCreature(CreatureClass crc)
{
    const auto pt = GetFreeXY();

    if (!pt) {
        return nullptr;
    }

    XCreature * cr = XCreatureStorage::CreateRnd(crc);

    Game.NewCreature(cr, pt->x, pt->y, this);

    return cr;
}

XCreature* XLocation::NewCreature(CreatureClass crc, XRect& rect, GROUP_ID gid, unsigned int ai_flags)
{
    const auto pt = GetFreeXY(&rect);

    if (!pt) {
        return nullptr;
    }

    XCreature * cr = XCreatureStorage::CreateRnd(crc);
    cr->setGroupID(gid);

    if (cr->xai->GetAIFlag() & XStandardAI::PEACEFUL) {
        cr->xai->SetEnemyClass(CreatureClass::NONE); //by default all creatures in pease with others.
    }

    Game.NewCreature(cr, pt->x, pt->y, this);

    if (ai_flags & XStandardAI::GUARD_AREA) {
        cr->xai->SetArea(rect, id);
        cr->xai->LearnTraps();
    }

    cr->xai->SetAIFlag((XStandardAI::Flag)(ai_flags));
    return cr;
}

XStairWay* XLocation::NewWay(const std::string& target_ln, const XStairWay::Type s_type, XRect * area)
{
    // A location that cannot fit its own stairway is a broken map, not a
    // case to recover from - let the empty optional throw.
    const XPoint pt = GetFreeXY(area).value();

    return NewWay(pt.x, pt.y, target_ln, s_type);
}

XStairWay* XLocation::NewWay(int x, int y, const std::string& target_ln, const XStairWay::Type s_type)
{
    XStairWay * pWay = new XStairWay(x, y, this, target_ln, s_type);
    ways_list.push_back(pWay);
    return pWay;
}

namespace {

// A tile the generator cannot do without. Missing, it says which
// location and which setting, then leaves the map to show it.
XTileType::Id RequiredTile(const sol::optional<sol::table>& options, const char* key, const std::string& loc_id)
{
    const XTileType::Id tile = options ? options->get_or(key, XTileType::NONE) : XTileType::NONE;

    if (tile == XTileType::NONE) {
        std::cerr << "world: " << loc_id << " is built without a '" << key << "' tile" << std::endl;
    }

    return tile;
}

// A tile a generator can do without, e.g. the water a dry level has none
// of. Absent, it reads as XTileType::NONE and the generator skips
// whatever it was for.
XTileType::Id OptionalTile(const sol::optional<sol::table>& options, const char* key)
{
    return options ? options->get_or(key, XTileType::NONE) : XTileType::NONE;
}

// A setting that is not a whole number, e.g. how round a chamber grows.
double Number(const sol::optional<sol::table>& options, const char* key, const double fallback)
{
    return options ? options->get_or(key, fallback) : fallback;
}

// An ordered list of tiles, e.g. a plain's border from the inside out.
std::vector<XTileType::Id> TileList(const sol::optional<sol::table>& options, const char* key,
                                    const std::string& loc_id)
{
    std::vector<XTileType::Id> tiles;

    if (options) {
        if (const sol::optional<sol::table> list = options->get<sol::optional<sol::table>>(key)) {
            for (size_t i = 1; i <= list->size(); i++) {
                tiles.push_back(static_cast<XTileType::Id>(list->get<int>(i)));
            }
        }
    }

    if (tiles.empty()) {
        std::cerr << "world: " << loc_id << " is built without a '" << key << "' tile list" << std::endl;
    }

    return tiles;
}

int Option(const sol::optional<sol::table>& options, const char* key, const int fallback)
{
    return options ? options->get_or(key, fallback) : fallback;
}

// An inclusive pair, e.g. room_width = { 4, 10 }.
std::pair<int, int> Range(const sol::optional<sol::table>& options, const char* key,
                          const int low, const int high)
{
    if (options) {
        if (const sol::optional<sol::table> pair = options->get<sol::optional<sol::table>>(key)) {
            return {pair->get_or(1, low), pair->get_or(2, high)};
        }
    }

    return {low, high};
}

} // namespace

void XLocation::CreateShop(unsigned int kind, XRect& rect, const std::string& sk_name, XShop::Door sd,
                           const XTileType::Id wall, const XTileType::Id floor,
                           const int min_value, const int max_value)
{
    XShop * shop = new XShop(rect, (ItemKind)kind, this, sd, wall, floor, min_value, max_value);
    AddPlace(shop);
    XCreature * cr = NewCreature(CN_SHOPKEEPER, rect);
    ((XShopkeeper*)cr)->SetShop(sk_name, shop);
}

int XLocation::GetCreatureCount(CreatureClass creature_class)
{
    int count = 0;

    for (const auto& [key, obj] : objects) {
        auto* cr = dynamic_cast<XCreature*>(obj);

        if (cr && !cr->isHero() && cr->l->guid() == this->guid() && cr->creature_class & creature_class) {
            count++;
        }
    }

    return count;
}

void XLocation::DumpLocation(std::ofstream &file)
{
    file << fmt::format("###### {} ######\n", full_name);
    map->Dump(file);

    file << "\n\n";
}


XLocation* XLocation::current_location = nullptr;
XCreature* XLocation::last_creature = nullptr;
XPattern XLocation::current_pattern;


//BuildShop(x, y, 9, 3, ItemKind.FOOD, 'Nobel', { wall = XTileType.STONE_WALL, floor = XTileType.STONE_FLOOR })
// options: `door` picks the side its entrance is on, defaulting to
// Door::BUILT_IN (the pattern already drew one); `wall` and `floor` are
// what it is built of, which is the script's call, not the engine's.
void XLocation::BuildShop(int x, int y, int w, int h, int mask, const std::string& keeper_name,
                          sol::optional<sol::table> options)
{
    XRect shop_rect(x, y, x + w, y + h);
    const auto door = options ? options->get_or("door", static_cast<int>(XShop::Door::BUILT_IN))
                              : static_cast<int>(XShop::Door::BUILT_IN);

    // `min_value`/`max_value` bound what the shop will stock, in gold - see
    // XShop::MinValue(). Absent, it stocks anything, which is what every
    // shop did before the option existed.
    const auto min_value = options ? options->get_or("min_value", 0) : 0;
    const auto max_value = options ? options->get_or("max_value", 10000) : 10000;

    current_location->CreateShop(mask, shop_rect, keeper_name, static_cast<XShop::Door>(door),
        RequiredTile(options, "wall", current_location->id),
        RequiredTile(options, "floor", current_location->id),
        min_value, max_value);
}


std::vector<int>* XLocation::lua_int_buffer = nullptr;
size_t XLocation::lua_int_index = 0;


void XLocation::RegisterLua(sol::state_view& lua)
{
    lua.new_enum("XLocation",
        "CAVE", Generator::CAVE,
        "CHAMBERS", Generator::CHAMBERS,
        "DELVE", Generator::DELVE,
        "DUNGEON", Generator::DUNGEON,
        "PLAIN", Generator::PLAIN,
        "PATTERN", Generator::PATTERN
    );

    lua.new_enum("XDelve",
        "CUBEROOT", XDelveBuilder::Pull::CUBEROOT,
        "ALL", XDelveBuilder::Pull::ALL,
        "BOTTOM", XDelveBuilder::Pull::BOTTOM
    );

    lua.new_enum("XDelveStore",
        "PERM", XDelveBuilder::Store::PERM,
        "CW", XDelveBuilder::Store::CW,
        "CCW", XDelveBuilder::Store::CCW
    );

    lua.new_enum("ShopDoor",
        "UP", XShop::Door::UP,
        "LEFT", XShop::Door::LEFT,
        "DOWN", XShop::Door::DOWN,
        "RIGHT", XShop::Door::RIGHT,
        "BUILT_IN", XShop::Door::BUILT_IN
    );

}

void RegisterLuaEventEnum(sol::state_view& lua)
{
    lua.new_enum("LuaEvent",
        "MOVE", LuaEvent::MOVE,
        "MOVE_IN", LuaEvent::MOVE_IN,
        "MOVE_OUT", LuaEvent::MOVE_OUT,
        "OUTER_USE", LuaEvent::OUTER_USE,
        "AI_TURN", LuaEvent::AI_TURN,
        "PRE_MOVE", LuaEvent::PRE_MOVE,
        "CHAT", LuaEvent::CHAT,
        "GIVE_ITEM", LuaEvent::GIVE_ITEM,
        "DIE", LuaEvent::DIE,
        "EVENT_SET", LuaEvent::EVENT_SET,
        "SAVE", LuaEvent::SAVE,
        "LOAD", LuaEvent::LOAD
    );
}


void XLocation::Restoration()
{
    // Only what has to exist *before* a saved world can be read: the
    // tile table, the creature templates, the map alphabet. The world
    // itself arrives afterwards, in XArchive::RestoreGame(), which
    // validates and links it once every location is back - doing either
    // here would sweep an empty Game.locations and link nothing.
    XLua::Init();
}

int XLocation::ValidateWorld(const bool new_game)
{
    int bad = 0;

    // Only a new game needs a start location: a restored one is built
    // from the save, and its hero already stands somewhere.
    if (new_game) {
        const auto start = Game.Location(XGame::start_location);

        if (XGame::start_location.empty()) {
            bad++;
            std::cerr << "world: no start location - the world script must call"
                         " SetStartLocation()" << std::endl;
        } else if (!start) {
            bad++;
            std::cerr << "world: the hero starts in '" << XGame::start_location
                      << "', which is not a location" << std::endl;
        } else if (const auto& area = XGame::start_area;
                   area && (area->left < 0 || area->top < 0
                            || area->right > start->map->len
                            || area->bottom > start->map->hgt)) {
            bad++;
            std::cerr << "world: the hero's start area " << area->left << "," << area->top
                      << " to " << area->right << "," << area->bottom << " reaches outside "
                      << XGame::start_location << std::endl;
        }
    }

    for (const auto& [key, loc] : Game.locations) {
        if (!loc || !loc->map) {
            continue;
        }

        for (int i = 0; i < loc->map->CellCount(); i++) {
            const auto& spec = loc->map->map[i].pSpecialObject;

            if (!spec) {
                continue;
            }

            std::string target;

            if (const auto* way = dynamic_cast<XStairWay*>(spec.get())) {
                target = way->ln;
            } else if (const auto* pad = dynamic_cast<XTeleport*>(spec.get())) {
                target = pad->ln;
            } else {
                continue;
            }

            if (Game.Location(target)) {
                continue;
            }

            bad++;
            std::cerr << "world: " << key << " at " << loc->map->CellX(i) << ","
                      << loc->map->CellY(i) << " leads to '" << target
                      << "', which is not a location" << std::endl;
        }
    }

    return bad;
}

int XLocation::LinkLevels()
{
    int bad = 0;

    for (const auto& [key, loc] : Game.locations) {
        if (!loc) {
            continue;
        }

        loc->below_location = nullptr;

        if (loc->below.empty()) {
            continue;
        }

        const auto under = Game.Location(loc->below);

        if (!under || under.get() == loc.get()) {
            bad++;
            std::cerr << "world: " << key << " is built over '" << loc->below
                      << "', which is not a location it can stand on" << std::endl;
            continue;
        }

        // A floor above shares the coordinate space of the level it
        // stands on, and must sit within it.
        if (loc->map->len != under->map->len || loc->map->hgt != under->map->hgt
            || loc->map->stored_x < 0 || loc->map->stored_y < 0
            || loc->map->stored_x + loc->map->stored_len > under->map->len
            || loc->map->stored_y + loc->map->stored_hgt > under->map->hgt) {
            bad++;
            std::cerr << "world: " << key << " sits at " << loc->map->stored_x << ","
                      << loc->map->stored_y << " on " << loc->below
                      << ", which reaches past its edge" << std::endl;
            continue;
        }

        loc->below_location = under.get();
        loc->map->below = under->map;
    }

    return bad;
}

int XLocation::ValidateWays()
{
    int bad = 0;

    // Stairways are paired one for one after the world is built, and each
    // end then knows where the other stands. One that finds no partner
    // keeps dest_x/dest_y = -1, and walking into it would read off the
    // front of the target's map - so ask each way, once the pairing pass
    // has had its turn, whether it found its other end.
    for (const auto& [key, loc] : Game.locations) {
        if (!loc) {
            continue;
        }

        for (const auto* obj : loc->ways_list) {
            const auto* way = dynamic_cast<const XStairWay*>(obj);

            if (!way || way->dest_x >= 0 || way->dest_y >= 0) {
                continue;
            }

            bad++;
            std::cerr << "world: the stairway in " << key << " at " << way->x << "," << way->y
                      << " leads to '" << way->ln << "', but nothing there leads back - "
                      << "it comes out nowhere" << std::endl;
        }
    }

    return bad;
}

void XLocation::CreateNewGame()
{
    XLua::Init();
    sol::state_view lua(XLua::State());
    if (const sol::protected_function_result result = lua["InitWorld"]();
        !result.valid()) {
        const sol::error err = result;
        std::cerr << "world: " << err.what() << std::endl;
        assert(false && "InitWorld() failed - see the message above");
    }
    ValidateWorld(true);
}

//CreateLocation(L_SMALL_CAVE1, "SmCv:1", "Small Cave Level 1", CAVE)
void XLocation::CreateLocation(const std::string& loc_id, const std::string& lbrief, const std::string& lfull,
                               const Generator generator, sol::optional<sol::table> options)
{
    XLocation::current_location = new XLocation(loc_id);
    XLocation::current_location->brief_name = lbrief;
    XLocation::current_location->full_name = lfull;

    const int width = Option(options, "width", 80);
    const int height = Option(options, "height", 20);

    XLocation::current_location->sight_range = Option(options, "sight", 0);

    // The ground this level is floored with, for a pattern that has to
    // invent some (a door in a wall of a hand-drawn cave has no floored
    // neighbour to copy). A plain has none - open country says what its
    // ground is cell by cell.
    XLocation::current_location->default_floor =
        options ? options->get_or("floor", XTileType::NONE) : XTileType::NONE;

    // A floor above another level, and where it sits on it.
    XLocation::current_location->below = options ? options->get_or<std::string>("below", "") : "";
    const auto [origin_x, origin_y] = Range(options, "origin", 0, 0);
    XLocation::current_location->origin_x = origin_x;
    XLocation::current_location->origin_y = origin_y;

    switch (generator) {
        case Generator::CAVE:
            XCaveBuilder(XLocation::current_location, width, height,
                         RequiredTile(options, "wall", loc_id),
                         RequiredTile(options, "floor", loc_id),
                         Option(options, "blobs", 150),
                         Option(options, "blob_radius", 3)).Build();
            break;

        case Generator::CHAMBERS: {
            const auto [min_areas, max_areas] = Range(options, "areas", 9, 19);
            const auto [min_size, max_size] = Range(options, "area_size", 11, 14);
            const XChambersBuilder::Shape shape = {min_areas, max_areas, min_size, max_size,
                                                   Option(options, "scale", 3),
                                                   Number(options, "gamma", 4.0),
                                                   Option(options, "corridors", 30),
                                                   Option(options, "corridor_left", 35),
                                                   Option(options, "loop_odds", 25),
                                                   Option(options, "roughness", 40),
                                                   Option(options, "roughness_grain", 6),
                                                   Option(options, "smooth", 1)};
            const XChambersBuilder::Water water = {OptionalTile(options, "water"),
                                                   OptionalTile(options, "deep_water"),
                                                   Option(options, "water_level", 14),
                                                   Option(options, "deep_level", 40),
                                                   Option(options, "water_grain", 24)};

            XChambersBuilder(XLocation::current_location, width, height,
                             RequiredTile(options, "wall", loc_id),
                             RequiredTile(options, "floor", loc_id),
                             shape, water).Build();
            break;
        }

        case Generator::DELVE: {
            // The digging table itself comes from the world scripts - see
            // world/delve_patterns.lua - either written out as one of
            // Kusigrosz's description strings or asked for by neighbour
            // count. Nothing here knows any pattern by name.
            const std::string desc =
                options ? options->get_or<std::string>("desc", "") : "";
            const auto [ngb_min, ngb_max] = Range(options, "ngb", 0, 0);

            XDelveBuilder(XLocation::current_location, width, height,
                          RequiredTile(options, "wall", loc_id),
                          RequiredTile(options, "floor", loc_id),
                          desc, ngb_min, ngb_max,
                          Option(options, "conmil", 0),
                          Option(options, "cells", 0),
                          static_cast<XDelveBuilder::Pull>(
                              Option(options, "pull", static_cast<int>(XDelveBuilder::Pull::CUBEROOT))),
                          static_cast<XDelveBuilder::Store>(
                              Option(options, "store", static_cast<int>(XDelveBuilder::Store::PERM)))).Build();
            break;
        }

        case Generator::DUNGEON: {
            const auto [min_w, max_w] = Range(options, "room_width", 4, 10);
            const auto [min_h, max_h] = Range(options, "room_height", 4, 6);
            const auto [min_exits, max_exits] = Range(options, "room_exits", 2, 3);
            const RoomShape shape = {min_w, max_w, min_h, max_h, min_exits, max_exits,
                                     Option(options, "trap_odds", 10),
                                     Option(options, "max_traps", 5)};

            XDungeonBuilder(XLocation::current_location, width, height,
                            RequiredTile(options, "wall", loc_id),
                            RequiredTile(options, "floor", loc_id),
                            Option(options, "room_chance", 0),
                            Option(options, "cells_per_room", 200),
                            Option(options, "door_odds", 3),
                            shape).Build();
            break;
        }

        case Generator::PLAIN:
            XPlainBuilder(XLocation::current_location, Option(options, "width", 200), Option(options, "height", 90),
                          RequiredTile(options, "ground", loc_id),
                          RequiredTile(options, "cover", loc_id),
                          TileList(options, "slope", loc_id),
                          Option(options, "cover_odds", 3),
                          Option(options, "border_depth", 4),
                          Option(options, "erosion", 2)).Build();
            break;

        case Generator::PATTERN: {
            // A level drawn by hand: a blank map of the size asked for,
            // and the script's own pattern is all there ever is on it.
            XLocation* here = XLocation::current_location;

            // A floor above another level shares that level's coordinate
            // space and covers only its own corner of it, so that a
            // position means the same place on both.
            int space_len = width;
            int space_hgt = height;

            if (!here->below.empty()) {
                const auto under = Game.Location(here->below);

                if (!under || !under->map) {
                    std::cerr << "world: " << loc_id << " is built over '" << here->below
                              << "', which has to be built before it" << std::endl;
                } else {
                    space_len = under->map->len;
                    space_hgt = under->map->hgt;
                }
            }

            // Nothing at all is a real answer here, and the usual one for
            // a floor above: every cell its pattern leaves alone is a
            // hole. A level standing on its own wants something solid.
            const XTileType::Id fill = options ? options->get_or("fill", XTileType::NONE)
                                               : XTileType::NONE;

            if (fill == XTileType::NONE && here->below.empty()) {
                std::cerr << "world: " << loc_id << " is built without a 'fill' tile" << std::endl;
            }

            XPatternBuilder(here, space_len, space_hgt, here->origin_x, here->origin_y,
                            width, height, fill).Build();
            break;
        }
    }
}

//DrawPattern(x, y)
void XLocation::DrawPattern(int x, int y)
{
    XLocation* here = XLocation::current_location;
    const XMap* map = here->map;
    const int outside = XLocation::current_pattern.Draw(here, x, y);

    // The pattern is the whole of a hand-drawn level, so one that does
    // not fit is a level with cells nothing ever drew - and, before the
    // cells were counted rather than written, a write past the end of the
    // map. ValidateWorld() cannot catch this: the world is built by the
    // time it runs, and this happens while it is being built.
    if (outside > 0) {
        std::cerr << "world: " << here->id << " draws a "
                  << XLocation::current_pattern.Width() << "x"
                  << XLocation::current_pattern.Height() << " pattern at " << x << "," << y
                  << ", which reaches past the " << map->stored_len << "x" << map->stored_hgt
                  << " it holds at " << map->stored_x << "," << map->stored_y << " - "
                  << outside << " cells are left undrawn" << std::endl;
    }
}

void XLocation::CreateTimerEvent(const std::string& event, int ttm)
{
    XLocation::current_location->event = event;
    XLocation::current_location->ttm = ttm;
    XLocation::current_location->ttmb = XLocation::current_location->ttm;
    Game.Scheduler.Add(XLocation::current_location);
}
