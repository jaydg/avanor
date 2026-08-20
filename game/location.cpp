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

        connected = map->isFullyConnected();
    }

    if (!connected) {
        map->ConnectAllRegions();
    }
}

void XLocation::BuildDungeon(int room_chance, int create_trap_door_chest)
{
    if (!map) {
        map = new XMap(80, 20);
    }

    XCaveBuilder * xcb = new XCaveBuilder(this, room_chance, create_trap_door_chest);
    xcb->Build();
    delete xcb;
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
    lua.new_enum("XLocation",
        "CAVE", Generator::CAVE,
        "DUNGEON", Generator::DUNGEON,
        "PLAIN", Generator::PLAIN
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
    XLua::Init();
    ValidateWorld(false);
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

        for (int i = 0; i < loc->map->len * loc->map->hgt; i++) {
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
            std::cerr << "world: " << key << " at " << (i % loc->map->len) << ","
                      << (i / loc->map->len) << " leads to '" << target
                      << "', which is not a location" << std::endl;
        }
    }

    return bad;
}

void XLocation::CreateNewGame()
{
    XLua::Init();
    sol::state_view lua(XLua::State());
    assert(lua["InitWorld"]().valid());
    ValidateWorld(true);
}

//CreateLocation(L_SMALL_CAVE1, "SmCv:1", "Small Cave Level 1", CAVE)
void XLocation::CreateLocation(const std::string& loc_id, const std::string& lbrief, const std::string& lfull,
                               const Generator generator, sol::optional<int> room_chance)
{
    XLocation::current_location = new XLocation(loc_id);
    XLocation::current_location->brief_name = lbrief;
    XLocation::current_location->full_name = lfull;

    switch (generator) {
        case Generator::CAVE:
            XLocation::current_location->BuildCave();
            break;

        case Generator::DUNGEON:
            XLocation::current_location->BuildDungeon(room_chance.value_or(0));
            break;

        case Generator::PLAIN:
            XLocation::current_location->BuildPlain(200, 90);
            break;
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
