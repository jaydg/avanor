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

#ifndef MAP_H
#define MAP_H

#include <string>
#include <vector>

#include <memory>
#include <set>

#include <cereal/archives/json.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/set.hpp>

#include <sol/forward.hpp>

#include "helpers/rect.h"
#include "item/itemlist.h"
#include "map/xanyplace.h"

#define MAP_MINX 10
#define MAP_MINY 10
#define MAP_MAXX 200
#define MAP_MAXY 200
#define SCR_HGT  (size_y - 5)
#define SCR_LEN  (size_x)
#define SCR_X    0
#define SCR_Y    2

struct XTileType {
    // How hard the tile is to cross. Anything from UNWALKABLE upwards
    // cannot be walked at all, which is the comparison the map and the
    // AI both make.
    enum class Movability {
        UNKNOWN,
        NORMAL,
        SHARD,
        AHARD,
        HARD,
        VHARD,
        UNWALKABLE,
        WATER,
        DEEPWATER,
        WALL = 80,
        MOUNTAIN
    };

    // How much the tile blocks sight, on the same scale.
    enum class Visibility {
        UNKNOWN,
        NORMAL,
        SHARD,
        AHARD,
        HARD,
        VHARD,
        WALL = 80
    };

    // A tile is whatever the world script defined, identified by the id
    // DefineTile() handed out in definition order. The engine knows no
    // types of its own - only that id 0 is "nothing here", which is what
    // an undrawn cell and a translation without a tile both hold.
    using Id = int;

    static constexpr Id NONE = 0;

    // Opens the empty Lua table DefineTile() fills, and registers the two
    // scales above as Movability.MEMBER / Visibility.MEMBER.
    static void RegisterLua(sol::state_view& lua);

    // Adds a tile and returns its id. Ids are handed out in definition
    // order, so they belong to one particular tiles.lua - see
    // XTileType::Names() for what that means for saved games.
    static Id Define(const std::string& id_name, char view, char color, const std::string& name,
                     Movability movability, Visibility visibility);

    // The id the script gave that name, or NONE if it never defined it.
    // A lookup per call site is deliberate: ids change when the scripts
    // are reloaded, so nothing may cache one across a game.
    [[nodiscard]] static Id ByName(const std::string& id_name);

    // Every id's name, in id order - written into a save so a game
    // stored under one tiles.lua can be restored under another.
    [[nodiscard]] static std::vector<std::string> Names();

    // Maps the ids in a save onto today's, given the names that save was
    // written with. Empty when nothing moved and no remapping is needed.
    [[nodiscard]] static std::vector<Id> RemapFrom(const std::vector<std::string>& saved_names);

    // Complains if the world script defined no tiles at all, which would
    // leave every map blank. Returns the number of complaints.
    static int ValidateTiles();

    // Dropped with the Lua state that defined them.
    static void ForgetTiles();

    // The name the script defined this tile under.
    std::string id_name;

    char view;
    char color;
    std::string name;
    Movability movability;
    Visibility visibility;
};

// Indexed by XTileType::Id - see XTileType::Define().
extern std::vector<XTileType> std_tile_data;

/* Forward declarations */
class XMapObject;
class XItem;

struct XMapTile {
    XMapTile();
    ~XMapTile();

    XTileType::Id n;
    std::shared_ptr<XCreature> pMonster; // if null then no monster here
    XItemList item_list;             // list of item in this cell of map. Automatic construct/destruct
    // Shared ownership, same reasoning and idiom as pMonster above: a
    // schedulable pSpecialObject (e.g. XHerbBush) is ALSO independently owned
    // by the scheduler's own XScheduler::Entry for as long as it's due to run
    // again, exactly like a creature.
    std::shared_ptr<XMapObject> pSpecialObject; // door, way, trap door.
    bool visible;                    // visible for HERO!!!
    char known;                      // for hero memory
    char color;                      // for hero memory

    // Pointer to an object which describes this place. It can be nullptr, Shop,
    // special room etc. It will be useful to mark all rooms in cave (to create
    // good links between rooms or to create unique room descriptions).
    XAnyPlace* place;

    // 0 - no room
    // 1, 2, ... - room ID
    // Currently used to indicate "special" rooms which should not be crossed
    // by passages.
    int room_id;

    // `place` is a raw, non-owning pointer into the owning XLocation's
    // places[MAX_PLACES] (which owns the XAnyPlace instances via
    // unique_ptr) - deliberately not persisted here; XLocation
    // re-derives every cell's `place` structurally, by re-running
    // XAnyPlace::Setup() for each loaded place once the whole map grid
    // is loaded, the same idiom as XMapObject::l/SetLocation().
    //
    // pMonster/item_list/pSpecialObject go through non-template,
    // concrete-archive-typed helpers (defined in map.cpp, where XCreature
    // and XItem are fully visible) rather than directly in this template -
    // same reasoning as XCreature::SaveModifier/LoadModifier: pulling
    // creature.h/item.h's full definitions into this widely-included
    // header would be circular.
    void SaveCrossRefs(cereal::JSONOutputArchive& ar) const;
    void LoadCrossRefs(cereal::JSONInputArchive& ar);

    template<class Archive>
    void serialize(Archive& ar)
    {
        ar(n, visible, known, color, room_id);

        if constexpr (Archive::is_loading::value) {
            LoadCrossRefs(ar);
        } else {
            SaveCrossRefs(ar);
        }
    }
};

class XMap
{
    public:
        void ForceRecenter(int x, int y);
        int hgt, len;
        int wx, wy;
        XMapTile* map;
        XMap();
        XMap(int l, int h);
        ~XMap();

        void Put(XCreature* cr) const;
        void Center(int x, int y);
        void PutChar(int x, int y, char c, int color) const;
        [[nodiscard]] XTileType::Movability GetMovability(int x, int y) const;
        [[nodiscard]] int XGetMovability(int x, int y) const;
        [[nodiscard]] int GetVisibility(int x, int y) const;
        void SetXY(int x, int y, XTileType::Id std_map) const;
        [[nodiscard]] XTileType::Id GetXY(int x, int y) const;
        // True when the whole walkable floor is one connected region, so
        // anything placed on it afterwards - a stairway, the hero - can be
        // reached from anywhere else.
        [[nodiscard]] bool isFullyConnected() const;

        // Last-resort repair: carves a straight tunnel from every isolated
        // pocket to a single hub, so no floor is left unreachable however
        // pathological the layout was.
        void ConnectAllRegions(XTileType::Id floor);

        // Translates every cell's tile id through remap, for a map that
        // was saved when the ids meant something else.
        void RemapTiles(const std::vector<XTileType::Id>& remap) const;

        void SetRoom(int x, int y, int room_id) const;
        [[nodiscard]] int GetRoom(int x, int y) const;

        void SetVisible(int x, int y) const;
        void ResVisible(int x, int y) const;
        [[nodiscard]] bool GetVisible(int x, int y) const;

        void PutItem(int x, int y, XItem* item) const;
        [[nodiscard]] unsigned int GetItemCount(int x, int y) const;
        [[nodiscard]] XItemList* GetItemList(int x, int y) const;

        void SetMonster(int x, int y, XCreature* monst) const;
        void ResMonster(int x, int y) const;
        [[nodiscard]] XCreature* GetMonster(int x, int y) const;

        void SetPlace(int x, int y, XAnyPlace* place) const;
        [[nodiscard]] XAnyPlace* GetPlace(int x, int y) const;

        void SetKnown(int x, int y) const;
        void ResKnown(int x, int y) const;
        [[nodiscard]] int GetKnown(int x, int y) const;

        [[nodiscard]] const char* GetDescription(int x, int y) const;

        void SetSpecial(int x, int y, XMapObject* spec) const;
        [[nodiscard]] XMapObject* GetSpecial(int x, int y) const;

        void CreateRoom(int x, int y, int l, int h, XTileType::Id m1, XTileType::Id m2) const;
        void CreateRoom(int x, int y, int l, int h, int px, int py, XTileType::Id m1, XTileType::Id m2) const;

        void Dump(std::ofstream &file) const;

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(len, hgt, wx, wy);

            if constexpr (Archive::is_loading::value) {
                map = new XMapTile[len * hgt];
            }

            for (int i = 0; i < len * hgt; i++) {
                ar(map[i]);
            }
        }
};

#endif
