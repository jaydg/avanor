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
#include <cereal/archives/json.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/set.hpp>

#include "engine/global.h"
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
        && restored->im == original->im
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

    // The custom comparator orders XItemList purely by `im` (item mask),
    // so two items sharing an `im` are equivalent as far as std::set is
    // concerned - this is pre-existing container behaviour, not anything
    // to do with Cereal. Round-trip whatever the set actually ends up
    // holding rather than assuming a particular size.
    const size_t original_size = original_list.size();

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

    bool pass = restored_list.size() == original_size;

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

} // namespace cereal_pilot

static void RunCerealPilotTest()
{
    const bool item_pass = cereal_pilot::TestPolymorphicItem();
    const bool list_pass = cereal_pilot::TestItemList();
    const bool selfref_pass = cereal_pilot::TestSelfReferentialWeakPtr();

    std::cout << "polymorphic item:      " << (item_pass ? "PASS" : "FAIL") << std::endl;
    std::cout << "XItemList round-trip:  " << (list_pass ? "PASS" : "FAIL") << std::endl;
    std::cout << "self-referential weak: " << (selfref_pass ? "PASS" : "FAIL") << std::endl;
    std::cout << "CEREAL PILOT: " << ((item_pass && list_pass && selfref_pass) ? "PASS" : "FAIL") << std::endl;
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
