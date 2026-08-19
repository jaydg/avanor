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
#include <vector>

#include <cereal/archives/json.hpp>
#include <cereal/types/polymorphic.hpp>

#include "creature/skeep_ai.h"
#include "creature/lua_ai.h"
#include "creature/shopkeeper.h"
#include "engine/xgen.h"
#include "game/cbuilder.h"
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

int XLocation::rand_location_count = XLocation::RANDOM;

REGISTER_CLASS(XLocation);
CEREAL_REGISTER_TYPE(XLocation);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XObject, XLocation);

void XLocation::FixupWaysList()
{
    ways_list.clear();

    for (int i = 0; i < map->len * map->hgt; i++) {
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
    for (int y = 0; y < map->hgt; y++) {
        for (int x = 0; x < map->len; x++) {
            auto& cell = map->map[x + y * map->len];

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

XLocation::XLocation(XLocation::Id location)
{
    visited_by_hero = 0;
    map = nullptr;	//map will created by XBuilder...

    for (int i = 0; i < MAX_PLACES; i++) {
        places[i] = nullptr;
    }

    assert(Game.locations[location] == nullptr);
    ln = location;
    Game.locations[location] = std::shared_ptr<XLocation>(this);

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

        if (!result.valid()) {
            return true;
        }

        return result.get<sol::optional<int>>().value_or(0) != 0;
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
        bx = 0;
        by = 0;
        dx = map->len;
        dy = map->hgt;
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

void XLocation::BuildPlain(int w, int h)
{
    int lm = 0;
    int rm = w;
    int tm = 0;
    int bm = h;

    int i, j;

    // Create Avanor's plain
    map = new XMap(w, h);

    for (i = 0; i < map->hgt; i++)
        for (j = 0; j < map->len; j++) {
            if (vRand() % 3) {
                map->SetXY(j, i, XTileType::GREEN_GRASS);
            } else {
                map->SetXY(j, i, XTileType::TREE);
            }
        }

    // creating high mountains
    for (i = lm; i < rm; i++) {
        int z1 = vRand() % ((i & 3) + 1) + 1;
        int z2 = vRand() % ((i & 3) + 1) + 1;

        for (j = 0; j < z1; j++) {
            map->SetXY(i, tm + j, XTileType::HIGH_MOUNTAIN);
        }

        for (j = 0; j < z2; j++) {
            map->SetXY(i, bm - j - 1, XTileType::HIGH_MOUNTAIN);
        }
    }

    for (i = tm; i < bm; i++) {
        int z1 = vRand() % ((i & 3) + 1) + 1;
        int z2 = vRand() % ((i & 3) + 1) + 1;

        for (j = 0; j < z1; j++) {
            map->SetXY(lm + j, i, XTileType::HIGH_MOUNTAIN);
        }

        for (j = 0; j < z2; j++) {
            map->SetXY(rm - j - 1, i, XTileType::HIGH_MOUNTAIN);
        }
    }

    //evaluate high mountains till hills!
    for (i = 0; i < map->hgt; i++)
        for (j = 0; j < map->len; j++) {
            int m = map->GetXY(j, i);

            if (m > XTileType::HILL && m <= XTileType::HIGH_MOUNTAIN) {
                for (int q = -2; q < 3; q++)
                    for (int w = -2; w < 3; w++) {
                        int nm;

                        if (abs(q) >= abs(w)) {
                            nm = m - abs(q);
                        } else {
                            nm = m - abs(w);
                        }

                        if (nm < XTileType::HILL) {
                            nm = XTileType::HILL;
                        }

                        if (j + q >= 0 && i + w >= 0
                            && j + q < map->len && i + w < map->hgt
                            && map->GetXY(j + q, i + w) < nm) {
                            map->SetXY(j + q, i + w, (XTileType::Type)nm);
                        }
                    }
            }
        }
}

namespace {

// 4-directional flood fill of every walkable tile reachable from (sx, sy),
// marking each in visited[] (row-major, same indexing as XMap::map).
// Returns the number of tiles marked (including the seed).
int FloodFillWalkable(XMap* map, int sx, int sy, std::vector<bool>& visited)
{
    std::vector<XPoint> stack;
    stack.emplace_back(sx, sy);
    visited[sx + sy * map->len] = true;
    int count = 1;

    static const int dx[4] = { 1, -1, 0, 0 };
    static const int dy[4] = { 0, 0, 1, -1 };

    while (!stack.empty()) {
        XPoint p = stack.back();
        stack.pop_back();

        for (int i = 0; i < 4; i++) {
            int nx = p.x + dx[i];
            int ny = p.y + dy[i];

            if (nx < 0 || nx >= map->len || ny < 0 || ny >= map->hgt) {
                continue;
            }

            int idx = nx + ny * map->len;

            if (visited[idx] || map->XGetMovability(nx, ny) != 0) {
                continue;
            }

            visited[idx] = true;
            count++;
            stack.emplace_back(nx, ny);
        }
    }

    return count;
}

// True if the map's entire walkable floor is a single connected region -
// i.e. any floor tile can be reached from any other, so a stairway placed
// anywhere on it (chosen afterward, once generation returns) can never
// land in an isolated pocket.
bool IsMapFullyConnected(XMap* map)
{
    int seed_x = -1;
    int seed_y = -1;
    int total_floor = 0;

    for (int y = 0; y < map->hgt; y++) {
        for (int x = 0; x < map->len; x++) {
            if (map->XGetMovability(x, y) == 0) {
                total_floor++;

                if (seed_x < 0) {
                    seed_x = x;
                    seed_y = y;
                }
            }
        }
    }

    if (total_floor == 0) {
        return false;
    }

    std::vector<bool> visited(map->len * map->hgt, false);

    return FloodFillWalkable(map, seed_x, seed_y, visited) == total_floor;
}

// Last-resort fallback if regeneration never produced a fully-connected
// layout within the attempt budget below - carves a straight tunnel from
// one tile of every disconnected pocket to a single hub tile, guaranteeing
// full reachability regardless of how pathological the random layout was,
// rather than silently shipping a map with an unreachable area.
void ConnectAllRegions(XMap* map)
{
    std::vector<bool> visited(map->len * map->hgt, false);
    std::vector<XPoint> component_seeds;

    for (int y = 0; y < map->hgt; y++) {
        for (int x = 0; x < map->len; x++) {
            if (map->XGetMovability(x, y) != 0 || visited[x + y * map->len]) {
                continue;
            }

            component_seeds.emplace_back(x, y);
            FloodFillWalkable(map, x, y, visited);
        }
    }

    if (component_seeds.size() < 2) {
        return;
    }

    const XPoint hub = component_seeds[0];

    for (size_t i = 1; i < component_seeds.size(); i++) {
        int x = component_seeds[i].x;
        int y = component_seeds[i].y;

        while (x != hub.x) {
            map->SetXY(x, y, XTileType::CAVE_FLOOR);
            x += (x < hub.x) ? 1 : -1;
        }

        while (y != hub.y) {
            map->SetXY(x, y, XTileType::CAVE_FLOOR);
            y += (y < hub.y) ? 1 : -1;
        }
    }
}

}

void XLocation::BuildCave()
{
    int cl = 80;
    int ch = 20;

    if (map) {
        cl = map->len;
        ch = map->hgt;
    } else {
        map = new XMap(cl, ch);
    }

    // A random blob-stamp cave has no connectivity guarantee on its own -
    // regenerate from scratch until the whole floor is one reachable
    // region (150 blobs over an 80x20 grid connects within a handful of
    // tries in practice), so a level's stairways - placed afterward, once
    // this returns, via GetFreeXY() over whatever floor exists - can never
    // end up in an isolated pocket the player can't get down/up through.
    constexpr int MAX_ATTEMPTS = 100;
    bool connected = false;

    for (int attempt = 0; attempt < MAX_ATTEMPTS && !connected; attempt++) {
        for (int i = 0; i < map->hgt; i++) {
            for (int j = 0; j < map->len; j++) {
                map->SetXY(j, i, XTileType::MAGMA);
            }
        }

        for (int k = 0; k < 150; k++) {
            int	qx = vRand() % (cl - 7) + 1;
            int qy = vRand() % (ch - 5) + 1;

            for (int q = 0; q < 360; q += 3) {
                for (int w = 0; w < 3; w++) {
                    int tx = qx + (int)(w * cos(q * M_PI / 180.0));
                    int ty = qy + (int)(w * sin(q * M_PI / 180.0));

                    if (tx > 0 && ty > 0 && tx < 79 && ty < 19) {
                        map->SetXY(tx, ty, XTileType::CAVE_FLOOR);
                    }
                }
            }
        }

        connected = IsMapFullyConnected(map);
    }

    if (!connected) {
        ConnectAllRegions(map);
    }
}

void XLocation::BuildLabirint(int create_trap_door_chest)
{
    if (!map) {
        map = new XMap(80, 20);
    }

    XCaveBuilder * xcb = new XCaveBuilder(this, create_trap_door_chest);
    xcb->Build();
    delete xcb;
}

void XLocation::CreateTraps()
{
    if (vRand(3) == 1) {
        XPoint pt;

        for (int i = 0; i < vRand(7); i++) {
            if (const auto pt = GetFreeXY()) {
                new XTrap(pt->x, pt->y, this);
            }
        }
    }
}

void XLocation::CreateChests()
{
    if (vRand(3) == 1) {
        XPoint pt;

        for (int i = 0; i < vRand(4); i++) {
            const auto pt = GetFreeXY();

            if (!pt) {
                continue;
            }

            XChest * ch1 = new XChest(vRand(6) + 1, ItemKind::ITEM, 1, 5000);
            ch1->Drop(this, pt->x, pt->y);
        }
    }
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
        cr->xai->SetArea(rect, ln);
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
        cr->xai->SetArea(rect, ln);
        cr->xai->LearnTraps();
    }

    cr->xai->SetAIFlag((XStandardAI::Flag)(ai_flags));
    return cr;
}

XStairWay* XLocation::NewWay(XLocation::Id target_ln, STAIRWAY_TYPE s_type, XRect * area)
{
    // A location that cannot fit its own stairway is a broken map, not a
    // case to recover from - let the empty optional throw.
    const XPoint pt = GetFreeXY(area).value();

    return NewWay(pt.x, pt.y, target_ln, s_type);
}

XStairWay* XLocation::NewWay(int x, int y, XLocation::Id target_ln, STAIRWAY_TYPE s_type)
{
    XStairWay * pWay = new XStairWay(x, y, this, target_ln, s_type);
    ways_list.push_back(pWay);
    return pWay;
}

void XLocation::CreateShop(unsigned int kind, XRect& rect, const std::string& sk_name, XShop::Door sd)
{
    XShop * shop = new XShop(rect, (ItemKind)kind, this, sd);
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

void XLocation::CreateRandomCave()
{
    int deep = vRand(6) + 5;
    int view = (vRand(2) == 1);
    int start_cr_lvl = vRand(static_cast<int>(CreatureTemplate::Level::AVG));

    XRect tr(115, 60, 180, 80);
    Game.locations[XLocation::MAIN]->NewWay((XLocation::Id)rand_location_count, STW_DOWN, &tr);
    new XRandomLocation(1, view, XLocation::MAIN, rand_location_count + 1, start_cr_lvl);
    int i = 1;

    for (; i < deep - 1; i++) {
        new XRandomLocation(i + 1, view, rand_location_count - 1, rand_location_count + 1, start_cr_lvl + (1 << i));
    }

    new XRandomLocation(i + 1, view, rand_location_count - 1, 0, start_cr_lvl + (1 << i));
}

XRandomLocation::XRandomLocation(int deep, int view, int way_up, int way_down, int cr_lvl) : XLocation((XLocation::Id)(XLocation::rand_location_count))
{
    XLocation::rand_location_count++;
    brief_name = fmt::format("Rnd{}", deep);
    full_name = fmt::format("Random Place Level {}", deep);

    if (view) {
        BuildCave();
    } else {
        map = new XMap(80, 20);
        BuildLabirint(1);
    }

    XPoint pt;

    if (way_up) {
        NewWay((XLocation::Id)way_up, STW_UP, nullptr);
    }

    if (way_down) {
        NewWay((XLocation::Id)way_down, STW_DOWN, nullptr);
    }

    Game.Scheduler.Add(new XUniversalGen(this, CreatureClass::UNDEAD | CreatureClass::BLOB | CreatureClass::INSECT | CreatureClass::REPTILE | CreatureClass::RAT | CreatureClass::ALL_IMPL, static_cast<CreatureTemplate::Level>(cr_lvl), 4, 50000));
}

XLocation* XLocation::current_location = nullptr;
XCreature* XLocation::last_creature = nullptr;
int XLocation::pat_offs_x = 0;
int XLocation::pat_offs_y = 0;

LOCATION_PATTERN XLocation::current_pattern;
std::vector<PALETTE_MAP> XLocation::pattern_translation;


//BuildShop(x, y, 9, 3, ItemKind::ARMOUR + ItemKind::WEAPON + ItemKind::POTION + ItemKind::BOOK + ItemKind::SCROLL + ItemKind::NECK + ItemKind::MISSILE + ItemKind::MISSILEW, 'Toberin, the dwarwen shopkeeper')
// The door side used to be frozen at Door::BUILT_IN here, which is a map
// authoring decision rather than an engine one - script picks it now, and
// still gets Door::BUILT_IN when it says nothing.
void XLocation::BuildShop(int x, int y, int w, int h, int mask, const std::string& keeper_name,
                          sol::optional<int> door)
{
    XRect shop_rect(x, y, x + w, y + h);
    current_location->CreateShop(mask, shop_rect, keeper_name,
        door ? static_cast<XShop::Door>(*door) : XShop::Door::BUILT_IN);
}






std::vector<int>* XLocation::lua_int_buffer = nullptr;
size_t XLocation::lua_int_index = 0;



void XLocation::RegisterLua(sol::state_view& lua)
{
    lua.new_enum("ShopDoor",
        "UP", XShop::Door::UP,
        "LEFT", XShop::Door::LEFT,
        "DOWN", XShop::Door::DOWN,
        "RIGHT", XShop::Door::RIGHT,
        "BUILT_IN", XShop::Door::BUILT_IN
    );

    lua.new_enum("XLocation",
        "MAIN", XLocation::MAIN,
        "MUSHROOMS_CAVE1", XLocation::MUSHROOMS_CAVE1,
        "MUSHROOMS_CAVE2", XLocation::MUSHROOMS_CAVE2,
        "MUSHROOMS_CAVE3", XLocation::MUSHROOMS_CAVE3,
        "MUSHROOMS_CAVE4", XLocation::MUSHROOMS_CAVE4,
        "MUSHROOMS_CAVE5", XLocation::MUSHROOMS_CAVE5,
        "DWARFCITYCAVE1", XLocation::DWARFCITYCAVE1,
        "DWARFCITYCAVE2", XLocation::DWARFCITYCAVE2,
        "DWARFCITYCAVE3", XLocation::DWARFCITYCAVE3,
        "DWARFCITYCAVE4", XLocation::DWARFCITYCAVE4,
        "DWARFCITYCAVE5", XLocation::DWARFCITYCAVE5,
        "DWARFCITYCAVE6", XLocation::DWARFCITYCAVE6,
        "DWARFCITY", XLocation::DWARFCITY,
        "DWARFTREASURE", XLocation::DWARFTREASURE,
        "GASMINE1", XLocation::GASMINE1,
        "GASMINE2", XLocation::GASMINE2,
        "GASMINE3", XLocation::GASMINE3,
        "RATCELLAR", XLocation::RATCELLAR,
        "EXTINCT_VOLCANO", XLocation::EXTINCT_VOLCANO,
        "WIZTOWER_TOP", XLocation::WIZTOWER_TOP,
        "KINGS_TREASURE", XLocation::KINGS_TREASURE,
        "WIZARD_DUNGEON1", XLocation::WIZARD_DUNGEON1,
        "WIZARD_DUNGEON2", XLocation::WIZARD_DUNGEON2,
        "WIZARD_DUNGEON3", XLocation::WIZARD_DUNGEON3,
        "WIZARD_DUNGEON4", XLocation::WIZARD_DUNGEON4,
        "WIZARD_DUNGEON5", XLocation::WIZARD_DUNGEON5,
        "AHKULAN_CASTLE", XLocation::AHKULAN_CASTLE
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
    XLua::Init();
}

void XLocation::CreateNewGame()
{
    XLua::Init();
    sol::state_view lua(XLua::State());
    assert(lua["InitWorld"]().valid());
}

//CreateLocation(L_SMALL_CAVE1, "SmCv:1", "Small Cave Level 1", CAVE)
void XLocation::CreateLocation(int loc_id, const std::string& lbrief, const std::string& lfull, int type)
{
    XLocation::current_location = new XLocation((XLocation::Id)loc_id);
    XLocation::current_location->brief_name = lbrief;
    XLocation::current_location->full_name = lfull;

    if (type == 0) {
        XLocation::current_location->BuildCave();
    } else if (type == 1) {
        XLocation::current_location->BuildLabirint();
    } else {
        XLocation::current_location->BuildPlain(200, 90);
    }
}

//DrawPattern(x, y)
void XLocation::DrawPattern(int x, int y)
{
    pat_offs_x = x;
    pat_offs_y = y;
    XLocation::current_location->PutPalette(x, y);
}

void XLocation::CreateTimerEvent(const std::string& event, int ttm)
{
    XLocation::current_location->event = event;
    XLocation::current_location->ttm = ttm;
    XLocation::current_location->ttmb = XLocation::current_location->ttm;
    Game.Scheduler.Add(XLocation::current_location);
}
