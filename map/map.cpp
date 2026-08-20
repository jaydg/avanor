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

#include <fstream>

#include <cereal/types/polymorphic.hpp>
#include <sol/sol.hpp>

#include "creature/creature.h"
#include "item/item.h"
#include "map/map.h"
#include "map/map_objects.h"

void XTileType::RegisterLua(sol::state_view& lua)
{
    lua.new_enum("XTileType",
        "GREEN_GRASS", XTileType::GREEN_GRASS,
        "TREE", XTileType::TREE,
        "SAND", XTileType::SAND,
        "WINDOW", XTileType::WINDOW,
        "MAGMA", XTileType::MAGMA,
        "QUARTZ", XTileType::QUARTZ,
        "CAVE_FLOOR", XTileType::CAVE_FLOOR,
        "STONE_FLOOR", XTileType::STONE_FLOOR,
        "PATH", XTileType::PATH,
        "WOOD_WALL", XTileType::WOOD_WALL,
        "STONE_WALL", XTileType::STONE_WALL,
        "WATER", XTileType::WATER,
        "DEEP_WATER", XTileType::DEEP_WATER,
        "LAVA", XTileType::LAVA,
        "HILL", XTileType::HILL,
        "LOW_MOUNTAIN", XTileType::LOW_MOUNTAIN,
        "MOUNTAIN", XTileType::MOUNTAIN,
        "HIGH_MOUNTAIN", XTileType::HIGH_MOUNTAIN,
        "BRIDGE", XTileType::BRIDGE,
        "ROAD", XTileType::ROAD,
        "OBSIDIAN_FLOOR", XTileType::OBSIDIAN_FLOOR,
        "FENCE", XTileType::FENCE,
        "GOLDEN_FLOOR", XTileType::GOLDEN_FLOOR,
        "MARBLE_WALL", XTileType::MARBLE_WALL,
        "BLACK_MARBLE_WALL", XTileType::BLACK_MARBLE_WALL,
        "GOLDEN_FENCE", XTileType::GOLDEN_FENCE,
        "TELEPORT_WHITE", XTileType::TELEPORT_WHITE
    );
}

XTileType std_tile_data[] = {
    {' ', xBLACK, "unknown", XTileType::Movability::UNKNOWN, XTileType::Visibility::UNKNOWN},
    {'.', xGREEN, "green grass", XTileType::Movability::NORMAL, XTileType::Visibility::NORMAL},
    {'T', xGREEN, "large tree", XTileType::Movability::HARD, XTileType::Visibility::HARD},
    {'.', xYELLOW, "sand", XTileType::Movability::SHARD, XTileType::Visibility::NORMAL},
    {'#', xCYAN, "window", XTileType::Movability::WALL, XTileType::Visibility::NORMAL},
    {'#', xDARKGRAY, "magma", XTileType::Movability::WALL, XTileType::Visibility::WALL},
    {'#', xLIGHTGRAY, "quartz", XTileType::Movability::WALL, XTileType::Visibility::WALL},
    {'.', xLIGHTGRAY, "cave floor", XTileType::Movability::NORMAL, XTileType::Visibility::NORMAL},
    {'.', xLIGHTGRAY, "stone floor", XTileType::Movability::NORMAL, XTileType::Visibility::NORMAL},
    {'.', xBROWN, "path", XTileType::Movability::NORMAL, XTileType::Visibility::NORMAL},
    {'#', xBROWN, "wooden wall", XTileType::Movability::WALL, XTileType::Visibility::WALL},
    {'#', xLIGHTGRAY, "stone wall", XTileType::Movability::WALL, XTileType::Visibility::WALL},
    {'=', xLIGHTBLUE, "water", XTileType::Movability::WATER, XTileType::Visibility::NORMAL},
    {'=', xBLUE, "deep water", XTileType::Movability::DEEPWATER, XTileType::Visibility::NORMAL},
    {'=', xRED, "lava", XTileType::Movability::WATER, XTileType::Visibility::NORMAL},
    {'^', xGREEN, "hill", XTileType::Movability::NORMAL, XTileType::Visibility::NORMAL},
    {'^', xBROWN, "low mountains", XTileType::Movability::VHARD, XTileType::Visibility::AHARD},
    {'^', xLIGHTGRAY, "mountains", XTileType::Movability::MOUNTAIN, XTileType::Visibility::HARD},
    {'^', xWHITE, "high mountains", XTileType::Movability::WALL, XTileType::Visibility::VHARD},
    {'=', xBROWN, "bridge", XTileType::Movability::NORMAL, XTileType::Visibility::NORMAL},
    {'.', xYELLOW, "road", XTileType::Movability::NORMAL, XTileType::Visibility::NORMAL},
    {'.', xDARKGRAY, "obsidian floor", XTileType::Movability::NORMAL, XTileType::Visibility::NORMAL},
    {'X', xBROWN, "fence", XTileType::Movability::WALL, XTileType::Visibility::NORMAL},
    {'.', xYELLOW, "golden floor", XTileType::Movability::NORMAL, XTileType::Visibility::NORMAL},
    {'#', xWHITE, "marble wall", XTileType::Movability::WALL, XTileType::Visibility::WALL},
    {'#', xDARKGRAY, "black marble wall", XTileType::Movability::WALL, XTileType::Visibility::WALL},
    {'X', xYELLOW, "golden fence", XTileType::Movability::WALL, XTileType::Visibility::NORMAL},
    {'0', xWHITE, "teleport circle", XTileType::Movability::NORMAL, XTileType::Visibility::NORMAL},
};

XMapTile::XMapTile()
{
    n = XTileType::GREEN_GRASS;
    pMonster = nullptr;
    pSpecialObject = nullptr;
    visible = false;
    known = ' ';
    color = 0;
    place = nullptr; // by default
    room_id = 0;
};

XMapTile::~XMapTile()
{
    item_list.InvalidateAll();

    // Unlike XItem, XCreature::Invalidate() doesn't remove itself from
    // wherever it's referenced from (no map-cell back-reference to do
    // that through) - clear pMonster explicitly afterward rather than
    // relying on a side effect. A cell can still be genuinely holding a
    // live creature here (e.g. the whole map being torn down at once,
    // out from under a creature that was simply still standing on it) -
    // this is real, not theoretical: reachable via
    // XLocation::Invalidate()'s `delete map`.
    if (pMonster) {
        pMonster->Invalidate();
        pMonster = nullptr;
    }

    // Move the pointer out BEFORE invalidating: XMapObject::Invalidate()
    // self-evicts via SetSpecial() on its own cell, which must not run
    // against this very cell mid-destruction. With the member already
    // moved-from, Invalidate()'s GetSpecial(x, y) == this check fails
    // harmlessly and the object is released at scope end instead (its
    // deleter sees is_valid already false and plain-deletes).
    if (auto spec = std::move(pSpecialObject)) {
        spec->Invalidate();
    }
}

void XMapTile::SaveCrossRefs(cereal::JSONOutputArchive& ar) const
{
    ar(pMonster, item_list, pSpecialObject);
}

void XMapTile::LoadCrossRefs(cereal::JSONInputArchive& ar)
{
    ar(pMonster, item_list, pSpecialObject);
}

XMap::XMap()
{
    map = nullptr;
    hgt = 0;
    len = 0;
    wx = 0;
    wy = 0;
}

XMap::XMap(const int l, const int h)
{
    map = new XMapTile[l * h];

    hgt = h;
    len = l;
    wx = 0;
    wy = 0;
}

XMap::~XMap()
{
    delete[] map;
}

void XMap::ResVisible(const int x, const int y) const
{
    if (x >= 0 && x < len && y >= 0 && y < hgt) {
        map[x + y * len].visible = false;
    }
}

void XMap::SetVisible(const int x, const int y) const
{
    if (x >= 0 && x < len && y >= 0 && y < hgt) {
        map[x + y * len].visible = true;
        map[x + y * len].color = std_tile_data[map[x + y * len].n].color;
        map[x + y * len].known = std_tile_data[map[x + y * len].n].view;
    }
}

bool XMap::GetVisible(const int x, const int y) const
{
    if (x >= 0 && x < len && y >= 0 && y < hgt) {
        return map[x + y * len].visible;
    }

    return false;
}

void XMap::SetPlace(const int x, const int y, XAnyPlace* place) const
{
    assert(x >= 0 && x < len);
    assert(y >= 0 && y < hgt);

    map[x + y * len].place = place;
}

XAnyPlace* XMap::GetPlace(const int x, const int y) const
{
    assert(x >= 0 && x < len);
    assert(y >= 0 && y < hgt);

    return map[x + y * len].place;
}

void XMap::ResKnown(const int x, const int y) const
{
    assert(x >= 0 && x < len);
    assert(y >= 0 && y < hgt);

    map[x + y * len].known = 0;
}

void XMap::SetKnown(const int x, const int y) const
{
    assert(x >= 0 && x < len);
    assert(y >= 0 && y < hgt);

    map[x + y * len].known = 1;
}

int XMap::GetKnown(const int x, const int y) const
{
    assert(x >= 0 && x < len);
    assert(y >= 0 && y < hgt);

    return map[x + y * len].known;
}

void XMap::SetSpecial(const int x, const int y, XMapObject* spec) const
{
    assert(x >= 0 && x < len);
    assert(y >= 0 && y < hgt);

    // Callers only ever pass either a fresh object self-registering into an
    // empty slot, or nullptr to evict the current occupant (now always from
    // inside the occupant's own Invalidate() - see XMapObject::Invalidate())
    // - never a replacement of one live occupant with another.
    if (spec == nullptr) {
        // The cell can be the occupant's only strong owner, and eviction
        // is typically requested from inside one of the occupant's own
        // methods - destroying it synchronously here would delete it out
        // from under its own still-executing code (for Cereal-loaded
        // objects, via a deleter that additionally destructs without
        // Invalidate(), tripping ~XObject's assert). Park the reference
        // in the graveyard instead; it's released between turns.
        XObject::DeferRelease(std::move(map[x + y * len].pSpecialObject));
        map[x + y * len].pSpecialObject = nullptr;
        return;
    }

    // Same idiom as SetMonster(): first placement (nothing owns it yet, e.g. a
    // fresh XHerbBush self-registering from its own constructor) establishes
    // the one master shared_ptr, with a deleter that defers to Invalidate(),
    // since the scheduler may still hold its own reference keeping this alive
    // past this call. Any later placement (already shared_ptr-owned, e.g. via
    // the scheduler) just takes another reference to that same control block.
    if (spec->weak_from_this().expired()) {
        map[x + y * len].pSpecialObject = std::shared_ptr<XMapObject>(spec, [](XMapObject* p) {
            if (p->isValid()) {
                p->Invalidate();
            } else {
                delete p;
            }
        });
    } else {
        map[x + y * len].pSpecialObject = std::static_pointer_cast<XMapObject>(spec->shared_from_this());
    }
}

XMapObject* XMap::GetSpecial(const int x, const int y) const
{
    assert(x >= 0 && x < len);
    assert(y >= 0 && y < hgt);

    return map[x + y * len].pSpecialObject.get();
}

int XMap::GetVisibility(const int x, const int y) const
{
    assert(x >= 0 && x < len);
    assert(y >= 0 && y < hgt);

    XMapObject* spec = map[x + y * len].pSpecialObject.get();
    auto* xdoor = dynamic_cast<XDoor *>(spec);

    if (xdoor && xdoor->isOpened == 0) {
        return 0;
    }

    if (std_tile_data[map[x + y * len].n].visibility == XTileType::Visibility::WALL) {
        return 0;
    }

    return 1;
}

const char* XMap::GetDescription(const int x, const int y) const
{
    assert(x >= 0 && x < len);
    assert(y >= 0 && y < hgt);

    return std_tile_data[map[x + y * len].n].name;
}

XTileType::Movability XMap::GetMovability(const int x, const int y) const
{
    assert(x >= 0 && x < len);
    assert(y >= 0 && y < hgt);

    const XMapTile& _map = map[x + y * len];
    auto* xdoor = dynamic_cast<XDoor *>(_map.pSpecialObject.get());

    if (xdoor && xdoor->isOpened == 0) {
        return XTileType::Movability::WALL;
    }

    return std_tile_data[_map.n].movability;
}

int XMap::XGetMovability(const int x, const int y) const
{
    assert(x >= 0 && x < len);
    assert(y >= 0 && y < hgt);

    const XMapTile* m = &map[x + y * len];

    if (m->pMonster) {
        return 2;
    }

    XMapObject* spec = map[x + y * len].pSpecialObject.get();
    auto* xdoor = dynamic_cast<XDoor *>(spec);

    if (std_tile_data[m->n].movability < XTileType::Movability::UNWALKABLE
        && !(xdoor && xdoor->isOpened == 0)) {
        return 0;
    }

    return 1;
}

void XMap::PutItem(const int x, const int y, XItem* item) const
{
    assert(x >= 0 && x < len);
    assert(y >= 0 && y < hgt);

    item->x = x;
    item->y = y;
    map[x + y * len].item_list.insert(XItem::Own(item));
}

XItemList* XMap::GetItemList(const int x, const int y) const
{
    assert(x >= 0 && x < len);
    assert(y >= 0 && y < hgt);

    return &map[x + y * len].item_list;
}

unsigned int XMap::GetItemCount(const int x, const int y) const
{
    assert(x >= 0 && x < len);
    assert(y >= 0 && y < hgt);

    return map[x + y * len].item_list.size();
}

void XMap::SetMonster(const int x, const int y, XCreature* monst) const
{
    assert(x >= 0 && x < len);
    assert(y >= 0 && y < hgt);

    // First time this creature is ever placed on the map (birth): nothing
    // owns it yet, so this is the one place its master shared_ptr gets
    // constructed. Every later move just transfers a fresh shared_ptr from
    // the same control block via shared_from_this() - never a second,
    // independent one.
    if (monst->weak_from_this().expired()) {
        // This can be the last shared_ptr/weak_ptr reference going away
        // without the creature ever having gone through Die() (e.g. the
        // whole map being torn down at once at full teardown, out from
        // under a creature that was simply still standing on it) - make
        // sure Invalidate() always runs first regardless, since it does
        // real cleanup (deregistering from the object registry, invalidating
        // carried items, ...), not just bookkeeping. No XPtr<XCreature>
        // cross-reference exists anywhere anymore (all migrated to
        // weak_ptr), so nothing can still be holding a legacy reference by
        // the time we get here - safe to delete unconditionally once
        // Invalidate() has run (or already had, on a previous pass).
        map[x + y * len].pMonster = std::shared_ptr<XCreature>(monst, [](XCreature* p) {
            if (p->isValid()) {
                p->Invalidate();
            } else {
                delete p;
            }
        });
    } else {
        map[x + y * len].pMonster = std::static_pointer_cast<XCreature>(monst->shared_from_this());
    }
}

void XMap::ResMonster(const int x, const int y) const
{
    assert(x >= 0 && x < len);
    assert(y >= 0 && y < hgt);

    map[x + y * len].pMonster = nullptr;
}

XCreature* XMap::GetMonster(const int x, const int y) const
{
    assert(x >= 0 && x < len);
    assert(y >= 0 && y < hgt);

    return map[x + y * len].pMonster.get();
}

void XMap::PutChar(const int x, const int y, const char c, const int color) const
{
    if (x >= wx && x < wx + SCR_LEN && y >= wy && y < wy + SCR_HGT) {
        vPutCh(x - wx + SCR_X, y - wy + SCR_Y, c, color);
    }
}

void XMap::Put(XCreature * cr) const
{
    for (int i = 0; i < SCR_HGT && wy + i < hgt; i++)
        for (int j = 0; j < SCR_LEN && wx + j < len; j++) {
            XMapTile * tmap = &map[(i + wy) * len + j + wx];

            if (tmap->visible) {
                auto* trap = dynamic_cast<XTrap *>(tmap->pSpecialObject.get());

                if (tmap->pSpecialObject && !(trap && !trap->isVisible(nullptr))) {
                    vPutCh(j + SCR_X, i + SCR_Y, tmap->pSpecialObject->view, tmap->pSpecialObject->color);
                    tmap->color = tmap->pSpecialObject->color;
                    tmap->known = tmap->pSpecialObject->view;
                } else if (!tmap->item_list.empty()) {
                    const XItem* item = tmap->item_list.begin()->get();

                    vPutCh(j + SCR_X, i + SCR_Y, item->view, item->color);
                    tmap->color = item->color;
                    tmap->known = item->view;
                } else {
                    //int tn = (i + wy) * len + j + wx;
                    int n = tmap->n;
                    vPutCh(j + SCR_X, i + SCR_Y, std_tile_data[n].view, std_tile_data[n].color);
                }

                if (tmap->pMonster && cr->isCreatureVisible(tmap->pMonster.get())) {
                    XCreature * xb = tmap->pMonster.get();
                    vPutCh(xb->x - wx + SCR_X, xb->y - wy + SCR_Y, xb->view, xb->color);
                }
            } else {
                vPutCh(j + SCR_X, i + SCR_Y, ' ', xBLACK);
            }

            if (tmap->known && !tmap->visible) {
                vPutCh(j + SCR_X, i + SCR_Y, tmap->known, tmap->color);
            }
        }
}

void XMap::Center(const int x, const int y)
{
    if (x <= wx + 2 || x >= wx + SCR_LEN - 2) {
        wx = x - SCR_LEN / 2;

        if (wx + SCR_LEN > len) {
            wx = len - SCR_LEN;
        }

        if (wx < 0) {
            wx = 0;
        }
    }

    if (y <= wy + 2 || y >= wy + SCR_HGT - 2) {
        wy = y - SCR_HGT / 2;

        if (wy + SCR_HGT > hgt) {
            wy = hgt - SCR_HGT;
        }

        if (wy < 0) {
            wy = 0;
        }
    }
}

void XMap::SetXY(const int x, const int y, const XTileType::Type std_map) const
{
    assert(x >= 0 && x < len);
    assert(y >= 0 && y < hgt);

    map[x + y * len].n = std_map;
}

XTileType::Type XMap::GetXY(const int x, const int y) const
{
    assert(x >= 0 && x < len);
    assert(y >= 0 && y < hgt);

    return map[x + y * len].n;
}

void XMap::SetRoom(const int x, const int y, const int room_id) const
{
    assert(x >= 0 && x < len);
    assert(y >= 0 && y < hgt);

    map[x + y * len].room_id = room_id;
}

int XMap::GetRoom(const int x, const int y) const
{
    assert(x >= 0 && x < len);
    assert(y >= 0 && y < hgt);

    return map[x + y * len].room_id;
}

void XMap::CreateRoom(const int x, const int y, const int l, const int h, const int px, const int py, const XTileType::Type m1, const XTileType::Type m2) const
{
    CreateRoom(x, y, l, h, m1, m2);
    SetXY(px, py, m1);
}

void XMap::CreateRoom(const int x, const int y, const int l, const int h, const XTileType::Type m1, const XTileType::Type m2) const
{
    for (int i = 0; i < l; i++) {
        for (int j = 0; j < h; j++) {
            if (i == 0 || i == l - 1 || j == 0 || j == h - 1) {
                SetXY(i + x, j + y, m2);
            } else {
                SetXY(i + x, j + y, m1);
            }
        }
    }
}

void XMap::Dump(std::ofstream &file) const
{
    for (int i = 0; i < hgt; i++) {
        for (int j = 0; j < len; j++) {
            XMapTile* tmap = &map[i * len + j];
            int n = tmap->n;
            char vch = std_tile_data[n].view;

            if (tmap->pSpecialObject) {
                vch = tmap->pSpecialObject->view;
            }

            if (!tmap->item_list.empty()) {
                const auto item = *(tmap->item_list.begin());
                vch = item->view;
            }

            if (tmap->pMonster) {
                vch = tmap->pMonster->view;
            }

            file << vch;
        }

        file << "\n";
    }
}

void XMap::ForceRecenter(const int x, const int y)
{
    wx = x - SCR_LEN / 2;

    if (wx + SCR_LEN > len) {
        wx = len - SCR_LEN;
    }

    if (wx < 0) {
        wx = 0;
    }

    wy = y - SCR_HGT / 2;

    if (wy + SCR_HGT > hgt) {
        wy = hgt - SCR_HGT;
    }

    if (wy < 0) {
        wy = 0;
    }
}

namespace {

// Whether the terrain itself can be walked on, ignoring whatever
// happens to be standing there: a closed door opens, and a creature
// moves. Connectivity is a property of the map, not of its occupants.
bool isWalkableTerrain(const XMap* map, int x, int y)
{
    return std_tile_data[map->GetXY(x, y)].movability < XTileType::Movability::UNWALKABLE;
}

// 4-directional flood fill of every walkable tile reachable from (sx, sy),
// marking each in visited[] (row-major, same indexing as XMap::map).
// Returns the number of tiles marked (including the seed).
int FloodFillWalkable(const XMap* map, int sx, int sy, std::vector<bool>& visited)
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

            if (visited[idx] || !isWalkableTerrain(map, nx, ny)) {
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
} // namespace

bool XMap::isFullyConnected() const
{
    const XMap* map = this;

    int seed_x = -1;
    int seed_y = -1;
    int total_floor = 0;

    for (int y = 0; y < map->hgt; y++) {
        for (int x = 0; x < map->len; x++) {
            if (isWalkableTerrain(map, x, y)) {
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
void XMap::ConnectAllRegions()
{
    XMap* map = this;

    std::vector<bool> visited(map->len * map->hgt, false);
    std::vector<XPoint> component_seeds;

    for (int y = 0; y < map->hgt; y++) {
        for (int x = 0; x < map->len; x++) {
            if (!isWalkableTerrain(map, x, y) || visited[x + y * map->len]) {
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
