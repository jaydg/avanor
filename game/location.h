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
        static XLocation* current_location;
        static XCreature* last_creature;
        static LOCATION_PATTERN current_pattern;
        static std::vector<PALETTE_MAP> pattern_translation;
        static int pat_offs_x;
        static int pat_offs_y;

        static void CreateLocation(int loc_id, const std::string& lbrief, const std::string& lfull, int type);
        static void Settle(CreatureClass crc, int crl);

        static void* Creature(const std::string& crn, sol::optional<int> x, sol::optional<int> y, sol::optional<int> w, sol::optional<int> h);
        static void* Guardian(const std::string& crn, const std::string& gid, int x, int y, sol::optional<int> w, sol::optional<int> h, sol::optional<int> flags);

        // Like Guardian(), but spawns a random creature of crc (see
        // XCreatureStorage::CreateRnd) instead of one fixed name - for a
        // group with several interchangeable member templates (e.g. the
        // orc war party's 7 orc variants) where any one of them filling
        // a given spot is fine.
        static void* GuardianClass(CreatureClass crc, const std::string& gid, int x, int y, sol::optional<int> w, sol::optional<int> h, sol::optional<int> flags);

        // Places an instant same-or-cross-location jump pad at (x, y) in
        // the current location, landing at (dest_x, dest_y) in target_loc_id.
        static void Teleport(int x, int y, int target_loc_id, int dest_x, int dest_y);

        // Scatters XHerbBush onto roughly 1-in-18 GREEN_GRASS tiles across
        // the whole current location (skipping tiles that already have a
        // special object) - a one-shot world-gen batch, not a per-tile Lua
        // loop, since it's an 18000-tile scan for a 200x90 map like the
        // Valley.
        static void ScatterHerbBushes();

        static void Way(int type, int loc_id, sol::optional<int> x, sol::optional<int> y);
        static void* CreateObjectByName(const std::string& name);
        static void* CreateObjectByMask(int flag, int min_val, int max_val);

        // pn is really PotionName - kept as a plain int here, same
        // reasoning as CreateObjectByMask's ItemKind-as-int flag: pulling
        // item/xpotion.h into this header breaks XLocation's own forward-
        // declaration ordering for downstream includers.
        static void* CreateObjectByPotion(int pn);
        static void DropItem(void* item, sol::optional<int> x, sol::optional<int> y);
        static void DropItemAt(void* item, void* object);
        static void SetPattern(int w, int h, const std::string& txt);
        static void AddTranslation(const std::string& view, sol::object target);
        static void DrawPattern(int x, int y);
        static void BuildShop(int x, int y, int w, int h, int mask, const std::string& keeper_name);
        static void* Furniture(int x, int y, int color, const std::string& view, const std::string& descr);
        static void* OuterObject(int color, const std::string& view, const std::string& descr, sol::optional<std::string> event);
        static void* OuterObjectAt(int x, int y, int color, const std::string& view, const std::string& descr, sol::optional<std::string> event);
        static void Altar(int x, int y, int deity);
        static void Treasure(int x, int y, int val);
        static void Chest(int x, int y, sol::optional<int> cnt, sol::optional<int> flg, sol::optional<int> mnval, sol::optional<int> mxval);
        static void Trap(int x, int y);
        static void EventPlace(const std::string& event);
        static void EventPlaceArea(int x, int y, int w, int h, const std::string& event);
        static void CreateMushroom(void* location);

        static void InflictDamage(void* target, int dmg, int resist, sol::optional<std::string> msg);
        static void ChangeStats(void* cr, int st, int val);
        static int GetStats(void* cr, int st);
        static int Rand(int val);
        static void SetEventHandler(void* cr, const std::string& event);

        // Opt-in gate for LuaEvent::AI_TURN/PRE_MOVE (see XCreature::
        // wants_move_hook) - call once, alongside SetEventHandler, for any
        // creature whose Lua handler wants a callback on every move (these
        // fire far more often than Chat/Die/GiveItem, so they're not
        // dispatched to every event_handler-bearing creature by default).
        static void EnableMoveHandler(void* cr);

        // Turns EnableMoveHandler() back off - for a handler that only
        // needs the per-turn callback for a bounded stretch of gameplay,
        // not for the rest of the game.
        static void DisableMoveHandler(void* cr);

        // Flags cr as the creature the display follows in demo/attract mode
        // ("-demo") in place of a real hero (see XCreature::NewMove()/Move()) -
        // a no-op outside demo mode, so it's safe for any world script to
        // call unconditionally on whichever creature should play that role.
        static void SetMainCreature(void* cr);
        static void CreateTimerEvent(const std::string& event, int ttm);

        static int GetSkill(void* cr, int skill);
        static void LearnSkill(void* cr, int skill, int val);
        static int MoneyOperation(void* cr, int val);

        static void SetName(void* obj, const std::string& name);
        static void SetView(void* obj, const std::string& view, int color);
        static std::string GetView(void* obj);

        static bool isHero(void* cr);
        static bool isEnemy(void* cr1, void* cr2);

        // Swaps a freshly-created creature's AI to a Lua-driven XLuaAI
        // (see creature/lua_ai.h), naming a global Lua table with optional
        // isEnemy/onWasAttacked/onDie/onSteal hooks. Call right after
        // creation, before anything else touches xai (SetAIFlag etc. set
        // on the old XStandardAI would otherwise be lost).
        static void SetCreatureAI(void* cr, const std::string& lua_class);

        // Cast bridges from the existing opaque void* handles (event_handler
        // dispatch, FindCreature/FindCreatures, ...) to the real
        // XCreature/XItem usertypes (see XCreature::RegisterLua,
        // XItem::RegisterLua) - lets Lua opt into the rich method/property
        // API on an object it already has a void* handle for, without
        // changing what type that handle actually is under the hood.
        static XCreature* AsCreature(void* p);
        static XItem* AsItem(void* p);

        // Location-scoped creature count, e.g. for a "is this crypt clear
        // of undead yet" quest check. Named distinctly from the instance
        // method below (same name, different signature) - taking &XLocation
        // ::GetCreatureCount is otherwise ambiguous for sol2's set_function.
        static int CreatureCountInLocation(int l_id, CreatureClass cc);

        // Roderick's ancestral-sword recognition check (both BP_HAND slots)
        // - kept as one bespoke predicate rather than decomposed into
        // generic Lua-side body-part/guid primitives, since that's a
        // separate, larger "expose item identity generically" project.
        static bool IsWearingAvanorDefender(void* cr);

        static void SetItEnemyFor(void* cr1, void* cr2);
        static void SetEnemy(void* cr, int cr_class);
        static void* FindCreature(int l_id, const std::string& gid, sol::optional<int> x, sol::optional<int> y, sol::optional<int> w, sol::optional<int> h);
        static std::vector<void*> FindCreatures(int l_id, const std::string& gid, sol::optional<int> x, sol::optional<int> y, sol::optional<int> w, sol::optional<int> h);

        // Builds a SCRIPT_CMD queue from `script` (an array of tables,
        // each {cmd = ScriptCommand.X, pt_x = .., pt_y = .., ln = .., kind = ..}
        // - only the fields the given cmd actually uses need to be set)
        // and hands it to `cr`'s XStandardAI via ExecuteScript(), same as
        // any other AI-flag-driven behavior (see XStandardAI::EXECUTE_SCRIPT).
        static void ExecuteCreatureScript(void* cr, sol::table script);

        // Position of the first entry in a location's ways_list (e.g. the
        // stairway Way(UP, ...) auto-places at a random free spot when
        // called without explicit x/y) - there's no other way for a
        // script to learn where that ended up.
        static std::tuple<int, int> GetWayXY(int l_id);

        static void AddMessage(const std::string& str);
        static std::string AskQuestion(const std::string& msg, const std::string& key, sol::variadic_args va);
        static int Gender(void* cr);

        static XGUID GetObjectGUID(void* obj);
        static std::tuple<int, int, int, int, int, std::string> GetItemParam(void* item);
        static void SetItemBrand(void* item, int br);

        static int MakeEffect(int effect, void* caller, void* location, int call_x, int call_y, void* target, int target_x, int target_y, int power);
        static void DestroyObject(void* item);

        static void SetCompanion(void* owner, void* slave, bool flag);

        static void GiveObjectToCreature(void* item, void* cr);
        static bool GiveAward(void* owner, XGUID aguid, void* target);

        static void Quest(int quest_id, int status, const std::string& know, const std::string& complete, const std::string& closed);
        static void QuestModify(int id, int status);
        static int QuestStatus(int id);

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
        static int StoreInt(lua_State * L);
        static int RestoreInt(lua_State * L);
        static bool BinaryAND(int v1, int v2);

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
