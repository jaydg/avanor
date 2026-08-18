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
#include <vector>

#include <cereal/types/base_class.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/string.hpp>

#include <sol/sol.hpp>

#include "creature/cr_defs.h"
#include "helpers/point.h"
#include "item/itemdef.h"
#include "map/map.h"
#include "map/xanyplace.h"

enum STAIRWAY_TYPE {
    STW_UNKNOWN,
    STW_UP,
    STW_DOWN
};

enum SHOP_DOOR {
    SHOP_DOOR_UP,
    SHOP_DOOR_LEFT,
    SHOP_DOOR_DOWN,
    SHOP_DOOR_RIGHT,
    SHOP_BUILD_IN,
};

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

enum PALETTE {
    PAL_UNKNOWN	= 0x0000,
    PAL_SMALL_TOWN	= 0x0001,
    PAL_SMALL_VILLAGE	= 0x0002,
    PAL_CITY	= 0x0003,
    PAL_DWARF_CITY	= 0x0004,
    PAL_WIZARD_TOWER	= 0x0005,
    PAL_AHKULAN_CASTLE	= 0x0006,
    PAL_RAT_CELLAR	= 0x0007,
    PAL_TOWER_RUINS	= 0x0008,
    PAL_DWARF_TREASURE	= 0x0009,
    PAL_KINGS_TREASURE	= 0x000A,
    PAL_WIZTOWER_TOP,
    PAL_UNDEAD_TOMB0,
    PAL_UNDEAD_TOMB1,
    PAL_EXTINCT_VOLCANO
};

struct PALETTE_MAP {
    char this_view;
    XTileType::Type real_view;

    // Set instead of real_view when this translation resolves
    // to a spawn callback rather than a plain tile type.
    sol::protected_function callback;
};

struct LOCATION_PATTERN {
    std::string pattern;
    int w = 0;
    int h = 0;
};

#define MAX_PLACES 8

class XItem;
class XMap;
class XStairWay;

struct lua_State;

class XLocation : public XObject
{
        std::string event;
    public:
        enum Id {
            UNKNOWN = 0,
            MAIN	= 1,
            DWARFCITYCAVE1	= 2,
            DWARFCITYCAVE2	= 3,
            DWARFCITYCAVE3	= 4,
            DWARFCITYCAVE4	= 5,
            DWARFCITYCAVE5	= 6,
            DWARFCITYCAVE6	= 7,
            DWARFCITYCAVE7	= 8,
            DWARFCITY	= 9,
            DWARFTREASURE	= 10,

            GASMINE1	= 15,
            GASMINE2	= 16,
            GASMINE3	= 17,

            SMALLCAVE	= 18,
            RATCELLAR	= 19,

            MUSHROOMS_CAVE1	= 20, //first
            MUSHROOMS_CAVE2	= 21, //demon
            MUSHROOMS_CAVE3	= 22, //misc
            MUSHROOMS_CAVE4	= 23, //kobolds
            MUSHROOMS_CAVE5	= 24, //mushrooms

            WIZARD_DUNGEON1	= 30,
            WIZARD_DUNGEON2	= 31,
            WIZARD_DUNGEON3	= 32,
            WIZARD_DUNGEON4	= 33,
            WIZARD_DUNGEON5	= 34,
            AHKULAN_CASTLE	= 35,

            UNDEADS_TOMB1	= 40,
            UNDEADS_TOMB2	= 41,
            UNDEADS_TOMB3	= 42,
            UNDEADS_TOMB4	= 43,
            UNDEADS_TOMB5	= 44,

            EXTINCT_VOLCANO	= 45,

            KINGS_TREASURE	= 46,

            WIZTOWER_TOP	= 50,
            SMALL_CAVE_1	= 55,
            SMALL_CAVE_2	= 56,

            DEBUG1	= 90,
            DEBUG2	= 91,
            RANDOM = 100,
            COUNT = 200,
        };

        // Registers this enum as the Lua table XLocation.MEMBER.
        static void RegisterLua(sol::state_view& lua);

        bool Run() override;

        bool way_found_flag; //used for recursive way found alg...

        // ways list used for the AI.
        std::vector<XObject*> ways_list;
        Id ln;

        static int rand_location_count;
        DECLARE_CREATOR(XLocation, XObject);
        XLocation(Id location);
        XLocation(XLocation * copy)
        {
            assert(0);
        }

        XLocation()
        {
            assert(0);
        }

        XMap* map;
        void GetFreeXY(XPoint * pt, XRect * area = nullptr);
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
            ar(ln);

            if constexpr (Archive::is_loading::value) {
                map = new XMap();
            }

            ar(*map);

            for (auto& p : places) {
                ar(p);
            }

            ar(brief_name, full_name, visited_by_hero, event);

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

        static void CreateRandomCave();

        XCreature* NewCreature(CREATURE_NAME cn, int x, int y, GROUP_ID gid = GID_NONE);
        XCreature* NewCreature(CREATURE_NAME cn);
        XCreature* NewCreature(CREATURE_NAME cn, XRect& rect, GROUP_ID gid = GID_NONE, unsigned int ai_flags = 0);
        XCreature* NewCreature(CreatureClass crc);
        XCreature* NewCreature(CreatureClass crc, XRect& rect, GROUP_ID gid = GID_NONE, unsigned int ai_flags = 0);

        XStairWay* NewWay(Id target_ln, STAIRWAY_TYPE s_type, XRect * area = nullptr); //creates way at random place
        XStairWay* NewWay(int x, int y, Id target_ln, STAIRWAY_TYPE s_type);

        static void CreateNewGame();
        static void Restoration();
        static void CreateLocation(int loc_id, const std::string& lbrief, const std::string& lfull, int type);
        static void DrawPattern(int x, int y);
        static void CreateTimerEvent(const std::string& event, int ttm);

        static XLocation* current_location;
        static XCreature* last_creature;
        static LOCATION_PATTERN current_pattern;
        static std::vector<PALETTE_MAP> pattern_translation;
        static int pat_offs_x;
        static int pat_offs_y;

        static void BuildShop(int x, int y, int w, int h, int mask, const std::string& keeper_name);
        static void Altar(int x, int y, int deity);
        static void CreateMushroom(void* location);

        // Roderick's ancestral-sword recognition check (both BP_HAND slots)
        // - kept as one bespoke predicate rather than decomposed into
        // generic Lua-side body-part/guid primitives, since that's a
        // separate, larger "expose item identity generically" project.
        static bool IsWearingAvanorDefender(void* cr);

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

        void PutPalette(int x, int y);

        void BuildCave();
        void BuildLabirint(int create_trap_door_chest = 1);
        void BuildPlain(int w, int h);
        void CreateTraps();
        void CreateChests();

        void CreateShop(unsigned int kind, XRect& rect, char* sk_name, SHOP_DOOR sd = SHOP_DOOR_UP);
};

// Both of XLocation's no-args-shaped constructors assert(0) - route
// construction through DECLARE_CREATOR's DUMMY_STRUCT constructor
// instead. Lives here rather than location.cpp: it gets triggered from
// inside XScheduler::serialize() (a template in xscheduler.h,
// reinstantiated per translation unit), same reasoning as
// XStandardAI/XSpell/XUniversalGen/etc. earlier this session.
CEREAL_LOAD_VIA_DUMMY_CONSTRUCT(XLocation, serialize);

class XRandomLocation : public XLocation
{
    public:
        XRandomLocation(int deep, int view, int way_up, int way_down, int cr_lvl); //view 0 - labirinth, 1 - cave
};

#endif
