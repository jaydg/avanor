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
#include <fstream>

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

void XLocation::Invalidate()
{
    if (!isValid()) {
        return;
    }

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

    XObject::Invalidate();
}

bool XLocation::Run()
{
    if (event.size()) {
        sol::state_view lua(XLocation::L);
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

void XLocation::GetFreeXY(XPoint * pt, XRect * area)
{
    int f = 10000;
    int tx, ty;

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

    while (f-- > 0) {
        tx = vRand() % dx + bx;
        ty = vRand() % dy + by;

        if (map->XGetMovability(tx, ty) == 0 && map->GetSpecial(tx, ty) == nullptr) {
            pt->x = tx;
            pt->y = ty;
            return;
        }
    }

    assert(0);
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
            GetFreeXY(&pt);
            new XTrap(pt.x, pt.y, this);
        }
    }
}

void XLocation::CreateChests()
{
    if (vRand(3) == 1) {
        XPoint pt;

        for (int i = 0; i < vRand(4); i++) {
            GetFreeXY(&pt);
            XChest * ch1 = new XChest(vRand(6) + 1, ItemKind::ITEM, 1, 5000);
            ch1->Drop(this, pt.x, pt.y);
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
    XPoint pt;
    GetFreeXY(&pt, nullptr);
    return NewCreature(cn, pt.x, pt.y);
}

XCreature* XLocation::NewCreature(CREATURE_NAME cn, XRect& rect, GROUP_ID gid, unsigned int ai_flags)
{
    XPoint pt;
    GetFreeXY(&pt, &rect);
    XCreature * cr = NewCreature(cn, pt.x, pt.y, gid);

    if (ai_flags & XStandardAI::GUARD_AREA) {
        cr->xai->SetArea(rect, ln);
        cr->xai->LearnTraps();
    }

    cr->xai->SetAIFlag((XStandardAI::Flag)(ai_flags));

    return cr;
}

XCreature* XLocation::NewCreature(CreatureClass crc)
{
    XPoint pt;
    GetFreeXY(&pt, nullptr);
    XCreature * cr = XCreatureStorage::CreateRnd(crc);

    Game.NewCreature(cr, pt.x, pt.y, this);

    return cr;
}

XCreature* XLocation::NewCreature(CreatureClass crc, XRect& rect, GROUP_ID gid, unsigned int ai_flags)
{
    XPoint pt;
    GetFreeXY(&pt, &rect);
    XCreature * cr = XCreatureStorage::CreateRnd(crc);
    cr->setGroupID(gid);

    if (cr->xai->GetAIFlag() & XStandardAI::PEACEFUL) {
        cr->xai->SetEnemyClass(CreatureClass::NONE); //by default all creatures in pease with others.
    }

    Game.NewCreature(cr, pt.x, pt.y, this);

    if (ai_flags & XStandardAI::GUARD_AREA) {
        cr->xai->SetArea(rect, ln);
        cr->xai->LearnTraps();
    }

    cr->xai->SetAIFlag((XStandardAI::Flag)(ai_flags));
    return cr;
}

XStairWay* XLocation::NewWay(XLocation::Id target_ln, STAIRWAY_TYPE s_type, XRect * area)
{
    XPoint pt;
    GetFreeXY(&pt, area);
    return NewWay(pt.x, pt.y, target_ln, s_type);
}

XStairWay* XLocation::NewWay(int x, int y, XLocation::Id target_ln, STAIRWAY_TYPE s_type)
{
    XStairWay * pWay = new XStairWay(x, y, this, target_ln, s_type);
    ways_list.push_back(pWay);
    return pWay;
}

void XLocation::CreateShop(unsigned int kind, XRect& rect, char* sk_name, SHOP_DOOR sd)
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

//CreateLocation(L_SMALL_CAVE1, "SmCv:1", "Small Cave Level 1", CAVE)
void XLocation::CreateLocation(int loc_id, const std::string& lbrief, const std::string& lfull, int type)
{
    current_location = new XLocation((XLocation::Id)loc_id);
    current_location->brief_name = lbrief;
    current_location->full_name = lfull;

    if (type == 0) {
        current_location->BuildCave();
    } else if (type == 1) {
        current_location->BuildLabirint();
    } else {
        current_location->BuildPlain(200, 90);
    }
}

//Settle(CreatureClass.RAT + CreatureClass.FELINE + CreatureClass.INSECT, CreatureTemplate.VERY_LOW)
void XLocation::Settle(CreatureClass crc, int crl)
{
    Game.Scheduler.Add(new XUniversalGen(current_location, crc, static_cast<CreatureTemplate::Level>(crl), 5, 25000));
}

//cr = Creature("rotmoth")
//cr = Creature("rat", [x, y, [w, h]])
void* XLocation::Creature(const std::string& crn, sol::optional<int> x, sol::optional<int> y, sol::optional<int> w, sol::optional<int> h)
{
    XCreature * cr = nullptr;

    if (!x) {
        cr = current_location->NewCreature(crn);
    } else {
        int tx = *x;
        int ty = *y;
        XRect rect = w ? XRect(tx, ty, tx + *w, ty + *h) : XRect(tx, ty, tx + 1, ty + 1);
        cr = current_location->NewCreature(crn, rect);
    }

    return cr;
}

//cr = Guardian("dwarf_guard", GID_DWARVEN_GUARDIAN, x, y, [len,  hgt], [flags])
void* XLocation::Guardian(const std::string& crn, const std::string& gid, int x, int y, sol::optional<int> w, sol::optional<int> h, sol::optional<int> flags)
{
    XRect rect = w ? XRect(x, y, x + *w, y + *h) : XRect(x, y, x + 1, y + 1);
    int flag = XStandardAI::GUARD_AREA;

    if (flags) {
        flag |= *flags;
    }

    XCreature * cr = current_location->NewCreature(crn, rect, gid, flag);

    // NewCreature() already set enemy_class to NONE for a PEACEFUL
    // creature (see its own comment) - respect that instead of
    // unconditionally overwriting it here. Without this check, every
    // Guardian()-spawned PEACEFUL creature (farmers, goodwives, and
    // anyone else inheriting their template) ends up hostile to every
    // non-human/humanoid class regardless, silently defeating the whole
    // point of being flagged PEACEFUL.
    if (!(cr->xai->GetAIFlag() & XStandardAI::PEACEFUL)) {
        cr->xai->SetEnemyClass(CreatureClass::ALL ^ (CreatureClass::HUMAN | CreatureClass::HUMANOID));
    }

    return cr;
}

//GuardianClass(CreatureClass.ORC, "orcs_war_party", 10, 70, 20, 10, XStandardAI.GUARD_AREA)
void* XLocation::GuardianClass(CreatureClass crc, const std::string& gid, int x, int y, sol::optional<int> w, sol::optional<int> h, sol::optional<int> flags)
{
    XRect rect = w ? XRect(x, y, x + *w, y + *h) : XRect(x, y, x + 1, y + 1);
    int flag = XStandardAI::GUARD_AREA;

    if (flags) {
        flag |= *flags;
    }

    return current_location->NewCreature(crc, rect, gid, flag);
}

//Teleport(23, 20, XLocation.MAIN, 154, 13)
void XLocation::Teleport(int x, int y, int target_loc_id, int nx, int ny)
{
    new XTeleport(x, y, current_location, (XLocation::Id)target_loc_id, nx, ny);
}

//ScatterHerbBushes()
void XLocation::ScatterHerbBushes()
{
    for (int i = 0; i < current_location->map->hgt; i++) {
        for (int j = 0; j < current_location->map->len; j++) {
            if (vRand(18) == 0) {
                if (current_location->map->GetXY(j, i) == XTileType::GREEN_GRASS
                    && current_location->map->GetSpecial(j, i) == nullptr) {
                    new XHerbBush(j, i, current_location);
                }
            }
        }
    }
}

//Way(DOWN, L_SMALL_CAVE2)
//Way(DOWN, L_SMALL_CAVE2, x, y)
void XLocation::Way(int type, int loc_id, sol::optional<int> x, sol::optional<int> y)
{
    if (x) {
        current_location->NewWay(*x, *y, (XLocation::Id)loc_id, (STAIRWAY_TYPE)type);
    } else {
        current_location->NewWay((XLocation::Id)loc_id, (STAIRWAY_TYPE)type);
    }
}

//CreateObject("XCookingSet")
void* XLocation::CreateObjectByName(const std::string& name)
{
    return XClassFactory::CreateNew((char*)name.c_str());
}

//CreateObject(ItemKind::ITEM - ItemKind::FOOD, 20, 500)
void* XLocation::CreateObjectByMask(int flag, int min_val, int max_val)
{
    return ICREATE((ItemKind)(flag), min_val, max_val);
}

//CreateObject(PotionName.HEALING)
void* XLocation::CreateObjectByPotion(int pn)
{
    return new XPotion(static_cast<POTION_NAME>(pn));
}

//DropItem(item, 0, 0)
//DropItem(item)
void XLocation::DropItem(void* item, sol::optional<int> x, sol::optional<int> y)
{
    XItem * pItem = (XItem*)item;
    int tx;
    int ty;

    if (x) {
        tx = *x;
        ty = *y;
    } else {
        XPoint pt;
        current_location->GetFreeXY(&pt);
        tx = pt.x;
        ty = pt.y;
    }

    if (pItem) {
        pItem->Drop(current_location, tx, ty);
    }
}

//DropItem(item, object)
void XLocation::DropItemAt(void* item, void* object)
{
    XItem * pItem = (XItem*)item;
    XMapObject * pMO = (XMapObject*)object;
    pItem->Drop(pMO->l, pMO->x, pMO->y);
}

//SetPattern(width, height,
// "###" ..
// "#.#" ..
// "###")
void XLocation::SetPattern(int w, int h, const std::string& txt)
{
    current_pattern.w = w;
    current_pattern.h = h;
    current_pattern.pattern = txt;
    pattern_translation.clear();
}

//AddTranslation("1", GOLDEN_FLOOR)
//AddTranslation("A", function(x, y) Guardian('dwarf_guard', GID_DWARVEN_GUARDIAN, x, y) end)
void XLocation::AddTranslation(const std::string& view, sol::object target)
{
    PALETTE_MAP pm;
    pm.this_view = view[0];

    if (target.get_type() == sol::type::function) {
        pm.callback = target.as<sol::protected_function>();
        pm.real_view = XTileType::UNKNOWN;
    } else {
        pm.real_view = (XTileType::Type)target.as<int>();
    }

    pattern_translation.push_back(pm);
}

//DrawPattern(x, y)
void XLocation::DrawPattern(int x, int y)
{
    pat_offs_x = x;
    pat_offs_y = y;
    current_location->PutPalette(x, y);
}

//BuildShop(x, y, 9, 3, ItemKind::ARMOUR + ItemKind::WEAPON + ItemKind::POTION + ItemKind::BOOK + ItemKind::SCROLL + ItemKind::NECK + ItemKind::MISSILE + ItemKind::MISSILEW, 'Toberin, the dwarwen shopkeeper')
void XLocation::BuildShop(int x, int y, int w, int h, int mask, const std::string& keeper_name)
{
    XRect shop_rect(x, y, x + w, y + h);
    current_location->CreateShop(mask, shop_rect, (char*)keeper_name.c_str(), SHOP_BUILD_IN);
}

//Furniture(x, y, xLIGHTRED, '~', 'a royal bad')
void* XLocation::Furniture(int x, int y, int color, const std::string& view, const std::string& descr)
{
    return new XFurniture(x, y, color, view[0], (char*)descr.c_str(), current_location);
}

//OuterObject(xLIGHTRED, '~', 'a royal bad', 'EventHandler')
void* XLocation::OuterObject(int color, const std::string& view, const std::string& descr, sol::optional<std::string> event)
{
    XPoint pt;
    current_location->GetFreeXY(&pt);
    return new XOuterObject(pt.x, pt.y, color, view[0], (char*)descr.c_str(), current_location, event ? event->c_str() : nullptr);
}

//OuterObject(x, y, xLIGHTRED, '~', 'a royal bad', 'EventHandler')
void* XLocation::OuterObjectAt(int x, int y, int color, const std::string& view, const std::string& descr, sol::optional<std::string> event)
{
    return new XOuterObject(x, y, color, view[0], (char*)descr.c_str(), current_location, event ? event->c_str() : nullptr);
}

//Altar(x, y, XDeity::LIFE)
void XLocation::Altar(int x, int y, int deity)
{
    new XAltar(x, y, (XDeity::Id)deity, current_location);
}

void XLocation::Treasure(int x, int y, int val)
{
    XMoney * money = new XMoney(vRand(val) + val);
    money->Drop(current_location, x, y);
}

void XLocation::Chest(int x, int y, sol::optional<int> cnt, sol::optional<int> flg, sol::optional<int> mnval, sol::optional<int> mxval)
{
    XChest * tchest = new XChest(cnt.value_or(5), (ItemKind)flg.value_or(static_cast<int>(ItemKind::ITEM)), mnval.value_or(100), mxval.value_or(25000));
    tchest->Drop(current_location, x, y);
}

void XLocation::Trap(int x, int y)
{
    new XTrap(x, y, current_location);
}

//EventPlace('MushroomCaveEvent')
void XLocation::EventPlace(const std::string& event)
{
    XRect area(0, 0, current_location->map->len, current_location->map->hgt);
    new XAnyPlace(area, current_location, (char*)event.c_str());
}

//EventPlace(x, y, 5, 2, 'SmallCaveEvent')
void XLocation::EventPlaceArea(int x, int y, int w, int h, const std::string& event)
{
    XRect area(x, y, x + w, y + h);
    new XAnyPlace(area, current_location, (char*)event.c_str());
}

int XLocation::GetSkill(void* cr, int skill)
{
    XSkill * sk = ((XCreature*)cr)->sk->GetSkill((XSkill::Skill)skill);
    return sk ? sk->GetLevel() : 0;
}

void XLocation::LearnSkill(void* cr, int skill, int val)
{
    XCreature * p = (XCreature*)cr;

    if (!p->sk->GetSkill((XSkill::Skill)skill)) {
        p->sk->Learn((XSkill::Skill)skill, val);
    }
}

int XLocation::MoneyOperation(void* cr, int val)
{
    return ((XCreature*)cr)->MoneyOp(val);
}

void XLocation::CreateTimerEvent(const std::string& event, int ttm)
{
    current_location->event = event;
    current_location->ttm = ttm;
    current_location->ttmb = current_location->ttm;
    Game.Scheduler.Add(current_location);
}

//InflictDamage (target, dmg, RESISTANCE_TYPE, creature ["msg"])
void XLocation::InflictDamage(void* target, int dmg, int resist, sol::optional<std::string> msg)
{
    XCreature * p = (XCreature*)target;
    dmg = p->onMagicDamage(dmg, (XResistance::Id)resist);
    p->_HP -= dmg;

    if (p->_HP < 0 && msg) {
        XFakeCreature * tcr = new XFakeCreature((char*)msg->c_str());
        p->Die(tcr);
        tcr->Invalidate();
    }
}

void XLocation::ChangeStats(void* cr, int st, int val)
{
    ((XCreature*)cr)->GainAttr((XStats::Id)st, val);
}

int XLocation::GetStats(void* cr, int st)
{
    return ((XCreature*)cr)->GetStats((XStats::Id)st);
}

int XLocation::Rand(int val)
{
    return vRand(val);
}

bool XLocation::isHero(void* cr)
{
    return ((XCreature*)cr)->isHero();
}

XCreature* XLocation::AsCreature(void* p)
{
    return (XCreature*)p;
}

XItem* XLocation::AsItem(void* p)
{
    return (XItem*)p;
}

void XLocation::SetCreatureAI(void* cr, const std::string& lua_class)
{
    XCreature* p = (XCreature*)cr;
    auto new_ai = std::make_unique<XLuaAI>(p, lua_class);
    p->xai->CopyBaseStateTo(*new_ai);
    p->xai = std::move(new_ai);
}

int XLocation::CreatureCountInLocation(int l_id, CreatureClass cc)
{
    return Game.locations[l_id]->GetCreatureCount(cc);
}

bool XLocation::IsWearingAvanorDefender(void* cr)
{
    XCreature* p = (XCreature*)cr;
    XItem* it1 = p->GetBodyPart(BP_HAND, 0)->Item();
    XItem* it2 = p->GetBodyPart(BP_HAND, 1)->Item();

    return (it1 && it1->guid() == XAvanorDefender::avanordefender_guid)
        || (it2 && it2->guid() == XAvanorDefender::avanordefender_guid);
}


bool XLocation::isEnemy(void* cr1, void* cr2)
{
    XCreature * p1 = (XCreature*)cr1;
    XCreature * p2 = (XCreature*)cr2;
    return p1 && p2 && p1->xai->isEnemy(p2);
}

void* XLocation::FindCreature(int l_id, const std::string& gid, sol::optional<int> x, sol::optional<int> y, sol::optional<int> w, sol::optional<int> h)
{
    XRect rect(0, 0, Game.locations[l_id]->map->len, Game.locations[l_id]->map->hgt);

    if (x) {
        rect.left = *x;
        rect.top = *y;
        rect.right = rect.left + *w;
        rect.bottom = rect.top + *h;
    }

    for (int i = rect.left; i < rect.right; i++)
        for (int j = rect.top; j < rect.bottom; j++) {
            XCreature* cr = Game.locations[l_id]->map->GetMonster(i, j);

            if (cr && cr->groupID() == gid) {
                return cr;
            }
        }

    return nullptr;
}

std::vector<void*> XLocation::FindCreatures(int l_id, const std::string& gid, sol::optional<int> x, sol::optional<int> y, sol::optional<int> w, sol::optional<int> h)
{
    XRect rect(0, 0, Game.locations[l_id]->map->len, Game.locations[l_id]->map->hgt);

    if (x) {
        rect.left = *x;
        rect.top = *y;
        rect.right = rect.left + *w;
        rect.bottom = rect.top + *h;
    }

    std::vector<void*> result;

    for (int i = rect.left; i < rect.right; i++)
        for (int j = rect.top; j < rect.bottom; j++) {
            XCreature* cr = Game.locations[l_id]->map->GetMonster(i, j);

            if (cr && cr->groupID() == gid) {
                result.push_back(cr);
            }
        }

    return result;
}

void XLocation::ExecuteCreatureScript(void* cr, sol::table script)
{
    std::vector<SCRIPT_CMD> cmds;

    for (auto& [key, value] : script) {
        sol::table row = value;
        SCRIPT_CMD cmd{};
        cmd.cmd = row.get_or("cmd", SCC_NONE);
        cmd.pt_x = row.get_or("pt_x", 0);
        cmd.pt_y = row.get_or("pt_y", 0);
        cmd.ln = row.get_or("ln", XLocation::UNKNOWN);
        cmd.kind = row.get_or("kind", ItemKind::UNKNOWN);
        cmds.push_back(cmd);
    }

    ((XCreature*)cr)->xai->ExecuteScript(cmds);
}

std::tuple<int, int> XLocation::GetWayXY(int l_id)
{
    XStairWay* way = (XStairWay*)*Game.locations[l_id]->ways_list.begin();
    return {way->x, way->y};
}

void XLocation::SetItEnemyFor(void* cr1, void* cr2)
{
    XCreature * p1 = (XCreature*)cr1;
    XCreature * p2 = (XCreature*)cr2;

    if (p1 && p2) {
        p2->xai->AddPersonalEnemy(p1);
        p2->xai->SetGroupEnemy(p1);
    }
}

void XLocation::SetEnemy(void* cr, int cr_class)
{
    ((XCreature*)cr)->xai->SetEnemyClass((CreatureClass)cr_class);
}

int XLocation::Gender(void* cr)
{
    return ((XCreature*)cr)->GetGender();
}

void XLocation::SetName(void* obj, const std::string& name)
{
    ((XMapObject*)obj)->SetName(name.c_str());
}

void XLocation::SetView(void* obj, const std::string& view, int color)
{
    ((XMapObject*)obj)->SetView(view[0], color);
}

std::string XLocation::GetView(void* obj)
{
    return std::string(1, ((XMapObject*)obj)->view);
}

void XLocation::AddMessage(const std::string& str)
{
    msgwin.Add(str);
}

//AskQuestion("Are you sure?", "yn", "Yes", "No")
struct ASK_QUESTION_REC {
    std::string val;
    int key;
};

std::string XLocation::AskQuestion(const std::string& msg, const std::string& key, sol::variadic_args va)
{
    msgwin.Add(msg);

    std::string out = "[";

    int offs = 0;
    char key_value[20];
    int index = 0;

    std::vector<ASK_QUESTION_REC> keys;

    while (sscanf(key.c_str() + offs, "%s10%n", key_value, &offs) > 0) {
        ASK_QUESTION_REC aqr;
        std::string variant;

        if (stricmp(key_value, "esc") == 0) {
            variant = MSG_CYAN "ESC" MSG_LIGHTGRAY;
            aqr.key = KEY_ESC;
            aqr.val = "esc";
        } else if (stricmp(key_value, "return") == 0) {
            variant = MSG_CYAN "Enter" MSG_LIGHTGRAY;
            aqr.key = KEY_ENTER;
            aqr.val = "enter";
        } else {
            variant = va[index].get<std::string>();
            index++;
            char substring[] = "x";
            substring[0] = key_value[0];
            char newstr[] = MSG_CYAN "x" MSG_LIGHTGRAY;
            newstr[2] = key_value[0];
            variant.replace(0, 1, newstr);
            aqr.key = key_value[0];
            aqr.val = substring;
        }

        keys.push_back(aqr);

        if (offs > 0) {
            out += ", ";
        }

        out += variant;
        offs += strlen(key_value);

        while (key[offs] == ' ') {
            offs++;
        }
    }

    out += "]";

    msgwin.Add(out);
    vRefresh();
    int ch = vGetch();
    msgwin.ClrMsg();

    for (auto it: keys) {
        if (ch == it.key) {
            return it.val;
        }
    }

    return keys.front().val;
}

void XLocation::SetEventHandler(void* cr, const std::string& event)
{
    ((XCreature*)cr)->SetEventHandler(event.c_str());
}

void XLocation::EnableMoveHandler(void* cr)
{
    ((XCreature*)cr)->EnableMoveHandler();
}

void XLocation::DisableMoveHandler(void* cr)
{
    ((XCreature*)cr)->DisableMoveHandler();
}

void XLocation::SetMainCreature(void* cr)
{
    if (XSettings::isDemo) {
        XCreature::main_creature = (XCreature*)cr;
    }
}

XGUID XLocation::GetObjectGUID(void* obj)
{
    return ((XObject*)obj)->guid();
}

std::tuple<int, int, int, int, int, std::string> XLocation::GetItemParam(void* item)
{
    XItem * p = (XItem*)item;
    return {static_cast<int>(p->kind), static_cast<int>(p->aet), p->wt, static_cast<int>(p->it), p->quantity, p->name};
}

void XLocation::SetItemBrand(void* item, int br)
{
    ((XItem*)item)->aet = (AttackEffectType)br;
}

int XLocation::MakeEffect(int effect, void* caller, void* location, int call_x, int call_y, void* target, int target_x, int target_y, int power)
{
    EFFECT_DATA ed;
    ed.effect = (XEffect::Id)effect;
    ed.caller = (XCreature*)caller;
    ed.l = (XLocation*)location;
    ed.call_x = call_x;
    ed.call_y = call_y;
    ed.target = (XCreature*)target;
    ed.target_x = target_x;
    ed.target_y = target_y;
    ed.power = power;

    return XEffect::Make(&ed);
}

void XLocation::DestroyObject(void* item)
{
    ((XItem*)item)->Invalidate();
}

void XLocation::SetCompanion(void* owner, void* slave, bool flag)
{
    XCreature * pOwner = (XCreature*)owner;
    XCreature * pSlave = (XCreature*)slave;

    if (flag) {
        pSlave->xai->SetCompanion(pOwner);
        pSlave->xai->companion_command = CC_FOLLOW;
    } else {
        pSlave->xai->SetCompanion(nullptr);
    }
}

void XLocation::GiveObjectToCreature(void* item, void* cr)
{
    ((XCreature*)cr)->ContainItem((XItem*)item);
}

bool XLocation::GiveAward(void* owner_ptr, XGUID aguid, void* target_ptr)
{
    const auto owner = (XCreature*)owner_ptr;
    const auto target = (XCreature*)target_ptr;
    auto item = dynamic_cast<XItem *>(GetObject(aguid));

    // Keep a live shared_ptr across the whole transfer below - owner's
    // contain can be item's only reference, and erasing it here (before
    // target's contain takes it over) would run Own()'s deleter on a
    // still-valid item and invalidate it outright instead of just handing
    // it to target.
    std::shared_ptr<XItem> item_sp;

    // Worn items are still resident in contain the whole time (see
    // XBodyPart::Wear()), so unwear first if needed - UnWear() no longer
    // needs a matching contain.insert(), it was never removed.
    for (const auto& bp: owner->components) {
        if (bp->Item() && bp->Item() == item) {
            bp->UnWear();
            break;
        }
    }

    const auto it = owner->contain.find(item);
    if (it != owner->contain.end()) {
        item_sp = *it;
        owner->contain.erase(it);
    }

    if (item) {
        owner->UnCarryItem(item);

        if (target->CarryItem(item)) {
            target->contain.insert(item_sp);
        } else {
            owner->DropItem(item);
        }

        return true;
    }

    return false;
}

void XLocation::Quest(int quest_id, int status, const std::string& know, const std::string& complete, const std::string& closed)
{
    auto qr = std::make_unique<XQuestRec>();
    qr->quest_id = quest_id;
    qr->status = (XQuest::Id)status;
    qr->know = know;
    qr->complete = complete;
    qr->closed = closed;
    XQuest::quest.quests.push_back(std::move(qr));
}

void XLocation::QuestModify(int id, int status)
{
    XQuestRec * qr = XQuest::quest.Find(id);

    if (qr) {
        qr->status = (XQuest::Id)status;
    }
}

int XLocation::QuestStatus(int id)
{
    XQuestRec * qr = XQuest::quest.Find(id);
    return qr ? qr->status : XQuest::UNKNOWN;
}

std::vector<int>* XLocation::lua_int_buffer = nullptr;
size_t XLocation::lua_int_index = 0;

int XLocation::StoreInt(lua_State * L)
{
    int tx = lua_tonumber(L, 1);
    lua_int_buffer->push_back(tx);

    return 0;
}

int XLocation::RestoreInt(lua_State * L)
{
    lua_pushnumber(L, (*lua_int_buffer)[lua_int_index++]);

    return 1;
}

bool XLocation::BinaryAND(int v1, int v2)
{
    return v1 & v2;
}

void XLocation::CreateMushroom(void* location)
{
    XLocation * p = (XLocation*)location;
    XPoint pt;
    p->GetFreeXY(&pt);
    new XMushSpawn(pt.x, pt.y, p);
}

void XLocation::RegisterLua(sol::state_view& lua)
{
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

lua_State* XLocation::L = nullptr;

void XLocation::CommonLuaInitialization()
{
    L = lua_open();
    sol::state_view lua(L);

    XLocation::RegisterLua(lua);
    RegisterLuaEventEnum(lua);
    CreatureTemplate::RegisterLua(lua);
    RegisterCrDefsEnums(lua);
    XItem::RegisterLua(lua);
    RegisterItemDefEnums(lua);
    XPotion::RegisterLua(lua);
    XCreature::RegisterLua(lua);
    XTileType::RegisterLua(lua);
    XStandardAI::RegisterLua(lua);
    XWarSkills::RegisterLua(lua);
    RegisterAttackEffectTypeLua(lua);
    XResistance::RegisterLua(lua);
    RegisterColorEnum(lua);
    RegisterBodyPartEnum(lua);
    XDeity::RegisterLua(lua);
    XReligion::RegisterLua(lua);
    XStats::RegisterLua(lua);
    XSkill::RegisterLua(lua);
    XQuest::RegisterLua(lua);
    XEffect::RegisterLua(lua);

    lua_register(L, "StoreInt", StoreInt);
    lua_register(L, "RestoreInt", RestoreInt);

    lua.open_libraries(sol::lib::base, sol::lib::string);

    // Sol2-bound Monster builder - registered before world scripts
    // load below, since world/creatures.lua calls it while loading.
    {
        lua.new_usertype<MonsterBuilder>("Monster",
            sol::constructors<MonsterBuilder(CREATURE_NAME), MonsterBuilder(CREATURE_NAME, CREATURE_NAME)>(),
            "View", &MonsterBuilder::View,
            "Basic", &MonsterBuilder::Basic,
            "Body", &MonsterBuilder::Body,
            "AI", &MonsterBuilder::AI,
            "Stats", &MonsterBuilder::Stats,
            "Resist", &MonsterBuilder::Resist,
            "Combat", &MonsterBuilder::Combat,
            "Main", &MonsterBuilder::Main,
            "Description", &MonsterBuilder::Description,
            "Melee", &MonsterBuilder::Melee,
            "MeleeExtra", &MonsterBuilder::MeleeExtra,
            "LearnSkill", &MonsterBuilder::LearnSkill,
            "LearnSpell", &MonsterBuilder::LearnSpell,
            "Equip", &MonsterBuilder::Equip,
            "EquipCount", &MonsterBuilder::EquipCount,
            "Corpse", &MonsterBuilder::Corpse,
            "CorpseEffect", &MonsterBuilder::CorpseEffect,
            "Unique", &MonsterBuilder::Unique,
            "Register", &MonsterBuilder::Register
        );
    }

    // Sol2-bound location/map-building functions
    // Registered before world scripts load below, since
    // locations.lua/valley.lua call these while loading.
    {
        lua.set_function("CreateLocation", &XLocation::CreateLocation);
        lua.set_function("Settle", &XLocation::Settle);
        lua.set_function("Creature", &XLocation::Creature);
        lua.set_function("Guardian", &XLocation::Guardian);
        lua.set_function("GuardianClass", &XLocation::GuardianClass);
        lua.set_function("Teleport", &XLocation::Teleport);
        lua.set_function("ScatterHerbBushes", &XLocation::ScatterHerbBushes);
        lua.set_function("Way", &XLocation::Way);
        lua.set_function("CreateObject", sol::overload(&XLocation::CreateObjectByName, &XLocation::CreateObjectByMask, &XLocation::CreateObjectByPotion));
        lua.set_function("DropItem", sol::overload(&XLocation::DropItem, &XLocation::DropItemAt));
        lua.set_function("SetPattern", &XLocation::SetPattern);
        lua.set_function("AddTranslation", &XLocation::AddTranslation);
        lua.set_function("DrawPattern", &XLocation::DrawPattern);
        lua.set_function("BuildShop", &XLocation::BuildShop);
        lua.set_function("Furniture", &XLocation::Furniture);
        lua.set_function("OuterObject", sol::overload(&XLocation::OuterObject, &XLocation::OuterObjectAt));
        lua.set_function("Altar", &XLocation::Altar);
        lua.set_function("Treasure", &XLocation::Treasure);
        lua.set_function("Chest", &XLocation::Chest);
        lua.set_function("Trap", &XLocation::Trap);
        lua.set_function("EventPlace", sol::overload(&XLocation::EventPlace, &XLocation::EventPlaceArea));
        lua.set_function("CreateMushroom", &XLocation::CreateMushroom);
    }

    {
        lua.set_function("isHero", &XLocation::isHero);
        lua.set_function("isEnemy", &XLocation::isEnemy);
        lua.set_function("SetCreatureAI", &XLocation::SetCreatureAI);
        lua.set_function("AsCreature", &XLocation::AsCreature);
        lua.set_function("AsItem", &XLocation::AsItem);
        lua.set_function("GetCreatureCount", &XLocation::CreatureCountInLocation);
        lua.set_function("IsWearingAvanorDefender", &XLocation::IsWearingAvanorDefender);
        lua.set_function("FindCreature", &XLocation::FindCreature);
        lua.set_function("FindCreatures", &XLocation::FindCreatures);
        lua.set_function("ExecuteCreatureScript", &XLocation::ExecuteCreatureScript);
        lua.set_function("GetWayXY", &XLocation::GetWayXY);

        lua.set_function("AddMessage", &XLocation::AddMessage);
        lua.set_function("AskQuestion", &XLocation::AskQuestion);

        lua.set_function("SetItEnemyFor", &XLocation::SetItEnemyFor);
        lua.set_function("SetEnemy", &XLocation::SetEnemy);
        lua.set_function("ChangeStats", &XLocation::ChangeStats);
        lua.set_function("GetStats", &XLocation::GetStats);
        lua.set_function("InflictDamage", &XLocation::InflictDamage);
        lua.set_function("Rand", &XLocation::Rand);
        lua.set_function("SetEventHandler", &XLocation::SetEventHandler);
        lua.set_function("EnableMoveHandler", &XLocation::EnableMoveHandler);
        lua.set_function("DisableMoveHandler", &XLocation::DisableMoveHandler);
        lua.set_function("SetMainCreature", &XLocation::SetMainCreature);
        lua.set_function("CreateTimerEvent", &XLocation::CreateTimerEvent);

        lua.set_function("GetSkill", &XLocation::GetSkill);
        lua.set_function("LearnSkill", &XLocation::LearnSkill);
        lua.set_function("MoneyOperation", &XLocation::MoneyOperation);

        lua.set_function("SetName", &XLocation::SetName);
        lua.set_function("SetView", &XLocation::SetView);
        lua.set_function("GetView", &XLocation::GetView);

        lua.set_function("GetObjectGUID", &XLocation::GetObjectGUID);
        lua.set_function("GetItemParam", &XLocation::GetItemParam);
        lua.set_function("SetItemBrand", &XLocation::SetItemBrand);
        lua.set_function("GiveObjectToCreature", &XLocation::GiveObjectToCreature);
        lua.set_function("GiveAward", &XLocation::GiveAward);

        lua.set_function("MakeEffect", &XLocation::MakeEffect);
        lua.set_function("DestroyObject", &XLocation::DestroyObject);
        lua.set_function("SetCompanion", &XLocation::SetCompanion);

        lua.set_function("Quest", &XLocation::Quest);
        lua.set_function("QuestModify", &XLocation::QuestModify);
        lua.set_function("QuestStatus", &XLocation::QuestStatus);
        lua.set_function("Gender", &XLocation::Gender);

        lua.set_function("BinaryAND", &XLocation::BinaryAND);

    }

    lua.script_file("./world/init.lua");

    // Catch Lua errors loading data
    assert(lua["LoadScripts"]().valid());
    XCreatureStorage::CreateQuickBase();
}

void XLocation::Restoration()
{
    CommonLuaInitialization();
}

void XLocation::CreateNewGame()
{
    CommonLuaInitialization();
    sol::state_view lua(L);
    assert(lua["InitWorld"]().valid());
}
