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

#ifndef LOCATION_H
#define LOCATION_H

#include <memory>
#include <string>
#include <optional>
#include <vector>

#include <cereal/types/base_class.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/string.hpp>

#include <sol/sol.hpp>

#include "creature/cr_defs.h"
#include "helpers/point.h"
#include "game/shop.h"
#include "item/itemdef.h"
#include "map/map.h"
#include "map/pattern.h"
#include "map/map_objects.h"
#include "map/xanyplace.h"

enum class LuaEvent {
    MOVE = 1,
    MOVE_IN = 2,
    MOVE_OUT = 3,
    OUTER_USE = 4,
    AI_TURN = 5,     // XCreature::NewMove() - fires before the AI decides what to do this turn
    PRE_MOVE = 6,    // XCreature::Move() - fires before an already-decided move is executed
    CHAT = 10,
    GIVE_ITEM = 11,
    DIE = 12,
    EVENT_SET = 97,
    SAVE = 98,
    LOAD = 99
};

void RegisterLuaEventEnum(sol::state_view& lua);

#define MAX_PLACES 8

class XItem;
class XMap;
class XStairWay;

struct lua_State;

class XLocation : public XObject
{
        std::string event;
    public:

        // How a location's map is laid out when it is created.
        enum class Generator {
            CAVE,
            DUNGEON,
            PLAIN
        };

        // Registers the Lua enums that belong to the location layer.
        static void RegisterLua(sol::state_view& lua);

        bool Run() override;

        bool way_found_flag; //used for recursive way found alg...

        // ways list used for the AI.
        std::vector<XObject*> ways_list;
        // The location's key in Game.locations.
        std::string id;

        // Whether an AI creature that happens to find a stairway leading here
        // may take it. False for a game's hub location, so dungeon monsters do
        // not wander into it; a creature with XStandardAI::ALLOW_MOVE_OUT
        // ignores this and comes anyway.
        //
        // Set from the world script with SetWanderingAllowed().
        bool allow_wandering_in = true;

        DECLARE_CREATOR(XLocation, XObject);
        explicit XLocation(const std::string& location_id);
        XLocation(XLocation * copy)
        {
            assert(0);
        }

        XLocation()
        {
            assert(0);
        }

        XMap* map;
        // A random walkable, unoccupied cell inside area (or the whole map
        // when null), or nullopt when there is none.
        std::optional<XPoint> GetFreeXY(XRect * area = nullptr);
        void AddPlace(XAnyPlace * pl);

        // ways_list holds raw, non-owning XObject* into two different
        // unique_ptr-owned homes (XStairWay via a map cell's
        // pSpecialObject, or an XAnyPlace via `places`) - not something
        // Cereal can track identity for directly, so it's rebuilt
        // structurally after load instead of persisted. This is the
        // same reconstruction the legacy Restore() (since removed)
        // already did for the `places`-derived half; the
        // pSpecialObject-derived half (from NewWay()) was never
        // reconstructed by the old system at all - real persistence,
        // not a mechanical port.
        void FixupWaysList();

        // XMapTile::pMonster/item_list/pSpecialObject round-trip a
        // creature's/item's/special object's own state correctly
        // (XCreature::serialize()/XItem's own serialize()/etc. handle
        // that), but position (x/y, plus a creature's nx/ny) and the
        // owning-location back-reference (`l`, inherited from XMapObject)
        // are deliberately not part of any of them - re-derived here
        // structurally from the cell each is actually found in, same
        // idiom as XMapObject::l/SetLocation() for ordinary map objects.
        // pSpecialObject covers doors, stairways, traps, altars,
        // furniture, and herb bushes - anything scheduled (like a herb
        // bush's periodic Run()) crashes on a null `l` the first time its
        // own turn comes due after a load, not immediately, since that
        // fires off the scheduler rather than this fixup pass itself.
        //
        // Not just cosmetic: XItem::Invalidate() uses `l`/x/y to find and
        // erase itself from its ground cell's item_list - without this,
        // that lookup silently fails (l is null) and the item never
        // leaves the list, which hangs XMapTile::~XMapTile()'s
        // `while (!item_list.empty())` loop forever the first time a
        // loaded location is ever torn down.
        void FixupMapObjectPositions();

        // XShopKeeperAI::shop is a raw, non-owning XShop* - not
        // something Cereal can track identity for directly (XShop
        // itself has no shared/weak-trackable identity of its own).
        // XShop::owner (a weak_ptr<XCreature>, see XAnyPlace::serialize)
        // already round-trips correctly via Cereal's normal pointer
        // tracking, so this resolves `shop` in the other direction from
        // that: for every XShop whose owner is a live, loaded
        // shopkeeper, point its XShopKeeperAI back at it.
        void FixupShops();

        // `location` on the ways/places array, along with every
        // XMapTile::place pointer, is deliberately not persisted directly -
        // re-running XAnyPlace::Setup() for every loaded place (after
        // `places` and `map` are both loaded) re-derives both, the same
        // idiom as XMapObject::l/SetLocation().
        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XObject>(this));
            ar(id);

            if constexpr (Archive::is_loading::value) {
                map = new XMap();
            }

            ar(*map);

            for (auto& p : places) {
                ar(p);
            }

            ar(brief_name, full_name, visited_by_hero, event, allow_wandering_in);

            if constexpr (Archive::is_loading::value) {
                for (auto& p : places) {
                    if (p) {
                        p->Setup(this);
                    }
                }

                FixupWaysList();
                FixupMapObjectPositions();
                FixupShops();
                way_found_flag = false;
            }
        }

    protected:
        // teardown hook, called by XObject::Invalidate()
        void OnInvalidate() override;
    public:

        std::string GetBriefName()
        {
            return brief_name;
        }

        std::string GetFullName()
        {
            return full_name;
        }

        int visited_by_hero;
        void DumpLocation(std::ofstream &file);

        int GetCreatureCount(CreatureClass creature_class); //count of such creatures on this level (need for quests)


        XCreature* NewCreature(CREATURE_NAME cn, int x, int y, GROUP_ID gid = GID_NONE);
        XCreature* NewCreature(CREATURE_NAME cn);
        XCreature* NewCreature(CREATURE_NAME cn, XRect& rect, GROUP_ID gid = GID_NONE, unsigned int ai_flags = 0);
        XCreature* NewCreature(CreatureClass crc);
        XCreature* NewCreature(CreatureClass crc, XRect& rect, GROUP_ID gid = GID_NONE, unsigned int ai_flags = 0);

        XStairWay* NewWay(const std::string& target_ln, XStairWay::Type s_type, XRect * area = nullptr); //creates way at random place
        XStairWay* NewWay(int x, int y, const std::string& target_ln, XStairWay::Type s_type);

        // Checks every location reference in the finished world - each
        // stairway's and teleport's target - against the locations that
        // actually exist, reporting each dangling one. Location ids are
        // free-form strings; this is what catches it, once, with the
        // referring location and coordinates named. new_game additionally
        // requires XGame::start_location to name a real place, which a
        // restored world has no need of. Returns the number of bad
        // references.
        static int ValidateWorld(bool new_game);

        static void CreateNewGame();
        static void Restoration();
        // room_chance, for a DUNGEON, is the chance in a hundred that
        // the level is built with one of the rooms the world script
        // defined through DefineRoom(); levels that say nothing get none.
        static void CreateLocation(const std::string& loc_id, const std::string& lbrief, const std::string& lfull,
                                   Generator generator, sol::optional<int> room_chance);
        static void DrawPattern(int x, int y);
        static void CreateTimerEvent(const std::string& event, int ttm);

        static XLocation* current_location;
        static XCreature* last_creature;
        // The pattern the world script is building with, between its
        // SetPattern() and its DrawPattern().
        static XPattern current_pattern;

        static void BuildShop(int x, int y, int w, int h, int mask, const std::string& keeper_name,
                              sol::optional<int> door);

        // Backing store for the Lua-facing StoreInt/RestoreInt pair (see
        // the .cpp): whichever XCreature/XAnyPlace is currently firing
        // its LuaEvent::SAVE/LuaEvent::LOAD Lua event handler points this at its own
        // lua_ints buffer first (XCreature::NotifyLuaEventHandler(),
        // XAnyPlace::NotifyLuaEvent()), so a script's StoreInt(x)/
        // RestoreInt() calls append to (or sequentially read from) that
        // object's own Cereal-serialized vector<int> instead of a
        // separate raw file.
        static std::vector<int>* lua_int_buffer;
        static size_t lua_int_index;


    protected:
        std::string brief_name; // max. 10 characters
        std::string full_name;  // max. 80 characters
        std::unique_ptr<XAnyPlace> places[MAX_PLACES];


        void CreateShop(unsigned int kind, XRect& rect, const std::string& sk_name, XShop::Door sd = XShop::Door::UP);
};

// Both of XLocation's no-args-shaped constructors assert(0) - route
// construction through DECLARE_CREATOR's DUMMY_STRUCT constructor
// instead. Lives here rather than location.cpp: it gets triggered from
// inside XScheduler::serialize() (a template in xscheduler.h,
// reinstantiated per translation unit), same reasoning as
// XStandardAI/XSpell/XUniversalGen/etc. earlier this session.
CEREAL_LOAD_VIA_DUMMY_CONSTRUCT(XLocation, serialize);


#endif
