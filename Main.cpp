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

#include <cctype>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <sstream>
#include <typeinfo>
#include <cereal/archives/json.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/set.hpp>

#include "engine/global.h"
#include "engine/xarchive.h"
#include "game/game.h"
#include "helpers/hiscore.h"
#include "helpers/manual.h"
#include "item/xmoney.h"

// Throwaway proofs that the Cereal serialize()/CEREAL_REGISTER_TYPE chain
// added so far (XObject -> XMapObject -> XBaseObject -> XItem -> XMoney)
// actually round-trips real polymorphic/weak_ptr/custom-comparator-set
// object graphs, the riskiest unknowns named by the save/restore rewrite
// plan. Not part of the real save/restore path - remove once the rewrite
// is further along and has its own proper regression coverage.
namespace cereal_pilot {

// Cereal constructs shared_ptr<T> internally using its own plain-delete
// deleter, not XItem::Own()'s Invalidate()-aware one - unlike a normal
// Own()'d object, a cereal-loaded object has no safety net if its last
// shared_ptr reference drops before something calls Invalidate() on it
// explicitly. In the real restore path (Phase 4) every loaded object
// becomes really owned by a real container as part of the same graph
// deserialization, so it'll go through the same explicit
// Invalidate()-before-last-reference-drops discipline as everything else
// already does - but a throwaway orphaned local like these needs that
// same explicit call, or ~XObject()'s invariant trips.
static void RetireOrphan(const std::shared_ptr<XItem>& item)
{
    if (item) {
        item->Invalidate();
    }
}

static bool TestPolymorphicItem()
{
    auto original = XItem::Own(new XMoney(500));

    std::ostringstream oss;
    {
        cereal::JSONOutputArchive archive(oss);
        archive(original);
    }

    std::cout << oss.str() << std::endl;

    std::shared_ptr<XItem> restored;
    {
        std::istringstream iss(oss.str());
        cereal::JSONInputArchive archive(iss);
        archive(restored);
    }

    const bool pass = restored
        && restored.get() != original.get()
        && dynamic_cast<XMoney*>(restored.get()) != nullptr
        && restored->kind == original->kind
        && restored->quantity == original->quantity
        && restored->value == original->value
        && restored->guid() == original->guid();

    RetireOrphan(original);
    RetireOrphan(restored);

    return pass;
}

static bool TestItemList()
{
    XItemList original_list;
    original_list.insert(XItem::Own(new XMoney(100)));
    original_list.insert(XItem::Own(new XMoney(250)));

    // XItemList::insert() (item.h) merges same-category, Compare() == 0
    // items into a single stack rather than adding a second entry - both
    // XMoney(100) and XMoney(250) share IM_MONEY and always compare equal,
    // so this confirms they combine into one element with the summed
    // quantity, not two.
    const size_t original_size = original_list.size();
    const int original_quantity = original_size == 1 ? (*original_list.begin())->quantity : -1;

    std::ostringstream oss;
    {
        cereal::JSONOutputArchive archive(oss);
        archive(original_list);
    }

    XItemList restored_list;
    {
        std::istringstream iss(oss.str());
        cereal::JSONInputArchive archive(iss);
        archive(restored_list);
    }

    bool pass = original_size == 1 && original_quantity == 350
        && restored_list.size() == original_size
        && (*restored_list.begin())->quantity == original_quantity;

    // Heterogeneous lookup via the transparent comparator (find by raw
    // XItem*, not shared_ptr<XItem>) is used throughout the codebase -
    // confirm it still works against a restored set.
    for (const auto& item : restored_list) {
        pass = pass && restored_list.find(item.get()) != restored_list.end();
    }

    for (const auto& item : original_list) {
        RetireOrphan(item);
    }

    for (const auto& item : restored_list) {
        RetireOrphan(item);
    }

    return pass;
}

// Mimics the exact shape XBodyPart::owner will need in Phase 3: an object
// (Owner) holding, via a directly-owned subobject (Sub, analogous to
// XBodyPart living in XCreature::components), a weak_ptr pointing back at
// itself. Proves the underlying Cereal mechanism generically, without
// prematurely building out the real (much larger) XCreature.
struct SelfRefOwner;

struct SelfRefSub {
    std::weak_ptr<SelfRefOwner> owner;

    template<class Archive>
    void serialize(Archive& ar)
    {
        ar(owner);
    }
};

struct SelfRefOwner : public std::enable_shared_from_this<SelfRefOwner> {
    int value{};
    std::unique_ptr<SelfRefSub> sub;

    template<class Archive>
    void serialize(Archive& ar)
    {
        ar(value, sub);
    }
};

static bool TestSelfReferentialWeakPtr()
{
    auto owner = std::make_shared<SelfRefOwner>();
    owner->value = 42;
    owner->sub = std::make_unique<SelfRefSub>();
    owner->sub->owner = owner;

    std::ostringstream oss;
    {
        cereal::JSONOutputArchive archive(oss);
        archive(owner);
    }

    std::shared_ptr<SelfRefOwner> restored;
    {
        std::istringstream iss(oss.str());
        cereal::JSONInputArchive archive(iss);
        archive(restored);
    }

    return restored
        && restored->value == owner->value
        && restored->sub
        && restored->sub->owner.lock() == restored;
}

// The real thing: spawns an actual game world (same init path as the
// established -test soak mode) and round-trips a real, fully-populated
// XCreature - not a synthetic stand-in. Exercises everything the
// XCreature/XBodyPart/magic-modifier/skills work depends on: polymorphic
// dispatch through the full creature hierarchy, xai's ai_owner fixup,
// XBodyPart's self-referential owner, and worn items staying resident in
// (and round-tripping identically with) contain.
static bool TestRealCreature()
{
    Game.Create('T');

    std::shared_ptr<XCreature> original;

    for (int li = 0; li < XLocation::COUNT && !original; li++) {
        auto& loc = Game.locations[li];

        if (!loc || !loc->map) {
            continue;
        }

        for (int y = 0; y < loc->map->hgt && !original; y++) {
            for (int x = 0; x < loc->map->len; x++) {
                if (auto& cr = loc->map->map[x + y * loc->map->len].pMonster) {
                    original = cr;
                    break;
                }
            }
        }
    }

    if (!original) {
        std::cout << "TestRealCreature: no creature found to test with" << std::endl;
        return false;
    }

    std::cout << "TestRealCreature: runtime type is " << typeid(*original).name() << std::endl;

    const auto original_guid = original->guid();
    const auto original_contain_size = original->contain.size();
    const auto original_components_size = original->components.size();

    // A worn item (if any) must stay resident in contain by identity -
    // note whether there is one, to check the same invariant Wear()
    // enforces at runtime survives the round trip.
    bool had_worn_item = false;
    for (auto& bp : original->components) {
        if (bp->Item()) {
            had_worn_item = true;
            break;
        }
    }

    std::ostringstream oss;
    {
        cereal::JSONOutputArchive archive(oss);
        archive(original);
    }

    std::shared_ptr<XCreature> restored;
    {
        std::istringstream iss(oss.str());
        cereal::JSONInputArchive archive(iss);
        archive(restored);
    }

    bool pass = restored
        && restored.get() != original.get()
        && restored->guid() == original_guid
        && restored->contain.size() == original_contain_size
        && restored->components.size() == original_components_size;

    if (had_worn_item) {
        bool found_worn_in_restored_contain = false;

        for (auto& bp : restored->components) {
            if (auto item = bp->Item()) {
                found_worn_in_restored_contain = restored->contain.find(item) != restored->contain.end();
                break;
            }
        }

        pass = pass && found_worn_in_restored_contain;
    }

    std::cout
        << "  guid " << original_guid << " -> " << (restored ? restored->guid() : 0)
        << ", contain " << original_contain_size << " -> " << (restored ? restored->contain.size() : 0)
        << ", components " << original_components_size << " -> " << (restored ? restored->components.size() : 0)
        << std::endl;

    // `original` is a real, live creature still referenced elsewhere in
    // the running game (the map tile, scheduler, etc.) - must NOT be
    // invalidated here, unlike every other orphaned test object in this
    // file. `restored` is a genuine orphan (this function is its only
    // reference) and needs the usual explicit Invalidate(), same as
    // TestPolymorphicItem()'s original/restored.
    if (restored) {
        restored->Invalidate();
    }

    return pass;
}

static bool TestRealLocation()
{
    std::shared_ptr<XLocation> original = Game.locations[XLocation::MAIN];

    if (!original || !original->map) {
        std::cout << "TestRealLocation: XLocation::MAIN not built, nothing to test with" << std::endl;
        return false;
    }

    const auto original_guid = original->guid();
    const auto original_ln = original->ln;
    const auto original_len = original->map->len;
    const auto original_hgt = original->map->hgt;
    const auto original_ways_count = original->ways_list.size();

    std::set<XAnyPlace*> original_places;
    int original_monster_count = 0;
    int original_item_count = 0;

    for (int i = 0; i < original_len * original_hgt; i++) {
        if (original->map->map[i].pMonster) {
            original_monster_count++;
        }

        original_item_count += static_cast<int>(original->map->map[i].item_list.size());
    }

    for (int y = 0; y < original_hgt; y++) {
        for (int x = 0; x < original_len; x++) {
            if (auto* p = original->map->GetPlace(x, y)) {
                original_places.insert(p);
            }
        }
    }

    // A creature reachable from this location's map can carry a custom
    // Lua event handler that, via NotifyLuaEventHandler(LE_SAVE/LE_LOAD)
    // inside XCreature::serialize(), calls back into Location.StoreInt/
    // RestoreInt - Lua bindings backed by XLocation::lua_int_buffer, a
    // pointer NotifyLuaEventHandler/NotifyLuaEvent point at that object's
    // own lua_ints member before firing. No special setup needed here:
    // it's just another field in the normal archive stream below.
    std::ostringstream oss;
    {
        cereal::JSONOutputArchive archive(oss);
        archive(original);
    }

    std::shared_ptr<XLocation> restored;
    {
        std::istringstream iss(oss.str());
        cereal::JSONInputArchive archive(iss);
        archive(restored);
    }

    bool pass = restored
        && restored.get() != original.get()
        && restored->guid() == original_guid
        && restored->ln == original_ln
        && restored->map
        && restored->map->len == original_len
        && restored->map->hgt == original_hgt
        && restored->ways_list.size() == original_ways_count;

    std::set<XAnyPlace*> restored_places;
    int restored_monster_count = 0;
    int restored_item_count = 0;

    if (restored && restored->map) {
        for (int i = 0; i < restored->map->len * restored->map->hgt; i++) {
            if (restored->map->map[i].pMonster) {
                restored_monster_count++;
            }

            restored_item_count += static_cast<int>(restored->map->map[i].item_list.size());
        }

        for (int y = 0; y < restored->map->hgt; y++) {
            for (int x = 0; x < restored->map->len; x++) {
                if (auto* p = restored->map->GetPlace(x, y)) {
                    restored_places.insert(p);

                    // Setup() must have re-derived `location` (and, per
                    // cell, XMapTile::place) after load, exactly like the
                    // real constructors do - not persisted directly.
                    pass = pass && (p->location == restored.get());
                }
            }
        }
    }

    pass = pass
        && (restored_places.size() == original_places.size())
        && (restored_monster_count == original_monster_count)
        && (restored_item_count == original_item_count);

    std::cout
        << "  guid " << original_guid << " -> " << (restored ? restored->guid() : 0)
        << ", places " << original_places.size() << " -> " << restored_places.size()
        << ", ways " << original_ways_count << " -> " << (restored ? restored->ways_list.size() : 0)
        << ", monsters " << original_monster_count << " -> " << restored_monster_count
        << ", items " << original_item_count << " -> " << restored_item_count
        << std::endl;

    // `original` is Game.locations[XLocation::MAIN] itself, still live and
    // referenced there - must NOT be invalidated. `restored` is a
    // genuine orphan (this function is its only reference).
    if (restored) {
        restored->Invalidate();
    }

    return pass;
}

static bool TestRealScheduler()
{
    // A fresh, isolated scheduler rather than Game.Scheduler itself -
    // XScheduler::Get() has real side effects (it can advance time,
    // reschedule entries whose timer hasn't yet expired) that would be
    // unsafe to trigger against the live, shared one mid-test.
    XScheduler local_sched;
    local_sched.SetTime(12345);

    // Add() takes ownership via XItem::Own() (see its own comment) for
    // XItem-derived entries - matches how real code schedules things
    // (e.g. Game.Scheduler.Add(new XCorpse(...))), not something this
    // test needs to manage itself.
    local_sched.Add(new XMoney(500));

    std::ostringstream oss;
    {
        cereal::JSONOutputArchive archive(oss);
        archive(local_sched);
    }

    XScheduler restored;
    {
        std::istringstream iss(oss.str());
        cereal::JSONInputArchive archive(iss);
        archive(restored);
    }

    bool pass = restored.GetTime() == local_sched.GetTime();

    // ttm/ttmb round-trip via XObject::serialize() same as every other
    // field - a freshly-constructed XMoney's ttm is 0, which Place()
    // turns negative (expired) the moment it's re-placed one slot
    // ahead, so a single Get() call is enough to retrieve it back.
    auto entry = restored.Get();
    pass = pass && entry && dynamic_cast<XMoney*>(entry.get()) != nullptr;

    std::cout
        << "  scheduler time " << local_sched.GetTime() << " -> " << restored.GetTime()
        << ", entry " << (entry ? typeid(*entry).name() : "none")
        << std::endl;

    // Genuine orphan (Cereal's plain-delete deleter, no Invalidate()
    // safety net) - Get() doesn't remove it from `restored`'s own
    // internal data[], so invalidating it here is enough: restored's
    // own destructor will find it already invalid and just drop the
    // reference, not double-invalidate it.
    if (entry) {
        entry->Invalidate();
    }

    return pass;
}

} // namespace cereal_pilot

static void RunCerealPilotTest()
{
    const bool item_pass = cereal_pilot::TestPolymorphicItem();
    const bool list_pass = cereal_pilot::TestItemList();
    const bool selfref_pass = cereal_pilot::TestSelfReferentialWeakPtr();
    const bool creature_pass = cereal_pilot::TestRealCreature();
    const bool location_pass = cereal_pilot::TestRealLocation();
    const bool scheduler_pass = cereal_pilot::TestRealScheduler();

    std::cout << "polymorphic item:      " << (item_pass ? "PASS" : "FAIL") << std::endl;
    std::cout << "XItemList round-trip:  " << (list_pass ? "PASS" : "FAIL") << std::endl;
    std::cout << "self-referential weak: " << (selfref_pass ? "PASS" : "FAIL") << std::endl;
    std::cout << "real creature:         " << (creature_pass ? "PASS" : "FAIL") << std::endl;
    std::cout << "real location:         " << (location_pass ? "PASS" : "FAIL") << std::endl;
    std::cout << "scheduler round-trip:  " << (scheduler_pass ? "PASS" : "FAIL") << std::endl;
    std::cout << "CEREAL PILOT: " << ((item_pass && list_pass && selfref_pass && creature_pass && location_pass && scheduler_pass) ? "PASS" : "FAIL") << std::endl;

    // TestRealCreature() calls Game.Create('T'), populating a whole real
    // game world - unlike -test/-demo mode, this mode returns from main()
    // right after this function without ever tearing that world down, so
    // static/global destructors at process exit would otherwise run
    // against a world nothing ever Invalidate()'d. -test/-demo mode
    // always runs the scheduler for a while before their own equivalent
    // teardown call - a completely fresh, never-ticked world segfaulted
    // inside InvalidateAllObjects() (confirmed via gdb: a virtual call
    // through a corrupted vtable pointer, i.e. a stale/dangling entry in
    // the object registry), so mirror that by running a few ticks first.
    for (int i = 0; i < 100; i++) {
        Game.Scheduler.Get()->Run();
    }

    XObject::InvalidateAllObjects();
}

const char* logo_text[] = {
    MSG_CYAN
    "  .oooooo  oo      oo    .oooooo  oo     oo    .oooo.    ooooooo. ",
    " dP'   88  88      88   dP'   88  88b    88   dP'  `Yb   88    `Yb",
    "d8     88  88      88  d8     88  88Yb   88  88      88  88     88",
    "88     88  Y8      8Y  88     88  88 Yb  88  88      88  88     88",
    "88ooooo88   Yb    dY   88ooooo88  88  Yb 88  88      88  88oooodP ",
    "88     88    Yb  dY    88     88  88   Yb88  88      88  88   Yb  ",
    "88     88     YbdY     88     88  88    Y88  `8b    d8'  88    Yb ",
    "88     88      YY      88     88  88     88   `Y8bd8P    88     Yb",
    MSG_WHITE,
    "                T h e  L a n d  o f  M y s t e r y",
    "",
    MSG_GREEN
    "                      version " GAME_VERSION "\n",
    MSG_YELLOW,
    "              (c) 2000 - 2006 by Vadim Gaidukevich",
    "",
    MSG_YELLOW
    "                      http://www.avanor.com",
    "",
    "",
    " "
    MSG_LIGHTGRAY "[" MSG_WHITE "R" MSG_LIGHTGRAY "] - Restore game "
    MSG_LIGHTGRAY "[" MSG_WHITE "N" MSG_LIGHTGRAY "] - New game "
    MSG_LIGHTGRAY "[" MSG_WHITE "?" MSG_LIGHTGRAY "] - read Manual "
    MSG_LIGHTGRAY "[" MSG_WHITE "Esc" MSG_LIGHTGRAY "] - Exit",
};

void ShowLogo()
{
    int logo_height = sizeof(logo_text) / sizeof(char*), logo_width = 0, i;

    for (i = 0; i < logo_height; i++)
        if (x_strlen(logo_text[i]) > logo_width) {
            logo_width = x_strlen(logo_text[i]);
        }

    int shift_x = (size_x - logo_width) / 2;
    int shift_y = (size_y - logo_height) / 2;

    vClrScr();

    for (i = 0; i < logo_height; i++) {
        vGotoXY(shift_x, i + shift_y);
        vPutS(logo_text[i]);
    }

    vRefresh();
}

XGame Game;


int main(int argc, char* argv[])
{
    vRandSeed(static_cast<unsigned long>(time(nullptr)));
    vInit();
    vClrScr();
    vHideCursor();

    ShowLogo();

    if (argc > 1 && strcmp(argv[1], "-test-cereal") == 0) {
        RunCerealPilotTest();
        vFinit();
        return 0;
    } else if (argc > 1 && strcmp(argv[1], "-test-save") == 0) {
        // End-to-end proof of the real XArchive::StoreGame()/RestoreGame()
        // entry points, as two separate process invocations (this one,
        // then -test-load in a fresh process) rather than one process
        // doing both: RestoreGame() populates Game.locations[]/Scheduler
        // directly, which is only safe against an empty, freshly-started
        // world (exactly how production uses it, via XGame::Create('R')
        // right at startup) - calling it against this process's own
        // already-live Game.Create('T') world would silently orphan the
        // live locations via plain shared_ptr reassignment, without ever
        // Invalidate()ing them first.
        if (argc > 2) {
            vRandSeed(atoi(argv[2]));
        }

        Game.Create('T');

        for (int i = 0; i < 100; i++) {
            Game.Scheduler.Get()->Run();
        }

        const int ok = XArchive::StoreGame();
        std::cout << "StoreGame: " << (ok ? "PASS" : "FAIL") << std::endl;

        for (int i = 0; i < 100; i++) {
            Game.Scheduler.Get()->Run();
        }

        XObject::InvalidateAllObjects();
        vFinit();
        return ok ? 0 : 1;
    } else if (argc > 1 && strcmp(argv[1], "-test-load") == 0) {
        XLocation::Restoration();
        const int ok = XArchive::RestoreGame();
        std::cout << "RestoreGame: " << (ok ? "PASS" : "FAIL") << std::endl;

        if (ok) {
            int location_count = 0;
            int monster_count = 0;
            int item_count = 0;

            for (auto& loc : Game.locations) {
                if (!loc || !loc->map) {
                    continue;
                }

                location_count++;

                for (int i = 0; i < loc->map->len * loc->map->hgt; i++) {
                    if (loc->map->map[i].pMonster) {
                        monster_count++;
                    }

                    item_count += static_cast<int>(loc->map->map[i].item_list.size());
                }
            }

            std::cout
                << "  locations " << location_count
                << ", monsters " << monster_count
                << ", items " << item_count
                << ", hero " << (XCreature::main_creature ? "present" : "none")
                << std::endl;

            for (int i = 0; i < 100; i++) {
                Game.Scheduler.Get()->Run();
            }
        }

        XObject::InvalidateAllObjects();
        vFinit();
        return ok ? 0 : 1;
    } else if (argc > 1 && strcmp(argv[1], "-test") == 0) {
        if (argc > 2) {
            vRandSeed(atoi(argv[2]));
        }

        Game.Create('T');
        Game.RunWithoutHero();
    } else if (argc > 1 && strcmp(argv[1], "-demo") == 0) {
        Game.Create('D');
        Game.RunDemo();
    } else {
        if (argc > 1 && strcmp(argv[1], "-god") == 0) {
            Game.isGodMode = true;
        } else {
            Game.isGodMode = false;
        }

        char ch;

        while (true) {
            ch = toupper(vGetch());

            if (ch == '?') {
                XManual man;
                man.Run();
                ShowLogo();
            }

            if (ch == KEY_ESC || ch == 'Z') {
                vFinit();
                return 0;
            }

            if (ch == 'R' || ch == 'N') {
                break;
            }
        }

        Game.Create(ch);
        Game.Run();
    }

    vFinit();
    return 0;
}
