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
#include "creature/unique.h"
#include "engine/xgen.h"
#include "game/cbuilder.h"
#include "game/game.h"
#include "game/location.h"
#include "game/quest.h"
#include "game/shop.h"
#include "helpers/msgwin.h"
#include "item/itemf.h"
#include "item/item_misc.h"
#include "item/xherb.h"
#include "map/map_objects.h"

//Location Script Support
extern "C"
{
#include "lauxlib.h"
#include "lualib.h"
}

#include <sol/sol.hpp>

int XLocation::rand_location_count = L_RANDOM;

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
        if (p && (p->im & IM_WAY)) {
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

XLocation::XLocation(LOCATION location)
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
    im = IM_OTHER;
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
                map->SetXY(j, i, XTileType::GREEN_GRAS);
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
            XChest * ch1 = new XChest(vRand(6) + 1, IM_ITEM, 1, 5000);
            ch1->Drop(this, pt.x, pt.y);
        }
    }
}

XCreature* XLocation::NewCreature(CREATURE_NAME cn, int x, int y, GROUP_ID gid)
{
    XCreature * cr = XCreatureStorage::Create(cn);
    cr->setGroupID(gid);

    if (cr->xai->GetAIFlag() & AIF_PEACEFUL) {
        cr->xai->SetEnemyClass(CR_NONE); //by default all creatures in pease with others.
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

    if (ai_flags & AIF_GUARD_AREA) {
        cr->xai->SetArea(rect, ln);
        cr->xai->LearnTraps();
    }

    cr->xai->SetAIFlag((AI_FLAG)(ai_flags));

    return cr;
}

XCreature* XLocation::NewCreature(CREATURE_CLASS crc)
{
    XPoint pt;
    GetFreeXY(&pt, nullptr);
    XCreature * cr = XCreatureStorage::CreateRnd(crc);

    Game.NewCreature(cr, pt.x, pt.y, this);

    return cr;
}

XCreature* XLocation::NewCreature(CREATURE_CLASS crc, XRect& rect, GROUP_ID gid, unsigned int ai_flags)
{
    XPoint pt;
    GetFreeXY(&pt, &rect);
    XCreature * cr = XCreatureStorage::CreateRnd(crc);
    cr->setGroupID(gid);

    if (cr->xai->GetAIFlag() & AIF_PEACEFUL) {
        cr->xai->SetEnemyClass(CR_NONE); //by default all creatures in pease with others.
    }

    Game.NewCreature(cr, pt.x, pt.y, this);

    if (ai_flags & AIF_GUARD_AREA) {
        cr->xai->SetArea(rect, ln);
        cr->xai->LearnTraps();
    }

    cr->xai->SetAIFlag((AI_FLAG)(ai_flags));
    return cr;
}

XStairWay* XLocation::NewWay(LOCATION target_ln, STAIRWAY_TYPE s_type, XRect * area)
{
    XPoint pt;
    GetFreeXY(&pt, area);
    return NewWay(pt.x, pt.y, target_ln, s_type);
}

XStairWay* XLocation::NewWay(int x, int y, LOCATION target_ln, STAIRWAY_TYPE s_type)
{
    XStairWay * pWay = new XStairWay(x, y, this, target_ln, s_type);
    ways_list.push_back(pWay);
    return pWay;
}

void XLocation::CreateShop(unsigned int im, XRect& rect, char* sk_name, SHOP_DOOR sd)
{
    XShop * shop = new XShop(rect, (ITEM_MASK)im, this, sd);
    AddPlace(shop);
    XCreature * cr = NewCreature(CN_SHOPKEEPER, rect);
    ((XShopkeeper*)cr)->SetShop(sk_name, shop);
}

int XLocation::GetCreatureCount(unsigned int creature_class)
{
    int count = 0;

    for (const auto& [key, obj] : objects) {
        if ((obj->im & IM_CREATURE) && (((XCreature*)obj)->l->guid() == this->guid())
                && (((XCreature*)obj)->creature_class) & creature_class) {
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
    int start_cr_lvl = vRand(CRL_AVG);

    XRect tr(115, 60, 180, 80);
    Game.locations[L_MAIN]->NewWay((LOCATION)rand_location_count, STW_DOWN, &tr);
    new XRandomLocation(1, view, L_MAIN, rand_location_count + 1, start_cr_lvl);
    int i = 1;

    for (; i < deep - 1; i++) {
        new XRandomLocation(i + 1, view, rand_location_count - 1, rand_location_count + 1, start_cr_lvl + (1 << i));
    }

    new XRandomLocation(i + 1, view, rand_location_count - 1, 0, start_cr_lvl + (1 << i));
}

XRandomLocation::XRandomLocation(int deep, int view, int way_up, int way_down, int cr_lvl) : XLocation((LOCATION)(XLocation::rand_location_count))
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
        NewWay((LOCATION)way_up, STW_UP, nullptr);
    }

    if (way_down) {
        NewWay((LOCATION)way_down, STW_DOWN, nullptr);
    }

    Game.Scheduler.Add(new XUniversalGen(this, (CREATURE_CLASS)(CR_UNDEAD | CR_BLOB | CR_INSECT | CR_REPTILE | CR_RAT | CR_ALL_IMPL), (CREATURE_LEVEL)cr_lvl, 4, 50000));
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
    current_location = new XLocation((LOCATION)loc_id);
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

//Settle(CR_RAT + CR_FELINE + CR_INSECT, CRL_VERY_LOW)
void XLocation::Settle(int crc, int crl)
{
    Game.Scheduler.Add(new XUniversalGen(current_location, (CREATURE_CLASS)(crc), (CREATURE_LEVEL)(crl), 5, 25000));
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
void* XLocation::Guardian(const std::string& crn, int gid, int x, int y, sol::optional<int> w, sol::optional<int> h, sol::optional<int> flags)
{
    XRect rect = w ? XRect(x, y, x + *w, y + *h) : XRect(x, y, x + 1, y + 1);
    int flag = AIF_GUARD_AREA;

    if (flags) {
        flag |= *flags;
    }

    XCreature * cr = current_location->NewCreature(crn, rect, (GROUP_ID)gid, flag);
    cr->xai->SetEnemyClass((CREATURE_CLASS)(CR_ALL ^ (CR_HUMAN | CR_HUMANOID)));
    return cr;
}

//Way(DOWN, L_SMALL_CAVE2)
//Way(DOWN, L_SMALL_CAVE2, x, y)
void XLocation::Way(int type, int loc_id, sol::optional<int> x, sol::optional<int> y)
{
    if (x) {
        current_location->NewWay(*x, *y, (LOCATION)loc_id, (STAIRWAY_TYPE)type);
    } else {
        current_location->NewWay((LOCATION)loc_id, (STAIRWAY_TYPE)type);
    }
}

//CreateObject("XCookingSet")
void* XLocation::CreateObjectByName(const std::string& name)
{
    return XClassFactory::CreateNew((char*)name.c_str());
}

//CreateObject(IM_ITEM - IM_FOOD, 20, 500)
void* XLocation::CreateObjectByMask(int flag, int min_val, int max_val)
{
    return ICREATE((ITEM_MASK)(flag), min_val, max_val);
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

//BuildShop(x, y, 9, 3, IM_ARMOUR + IM_WEAPON + IM_POTION + IM_BOOK + IM_SCROLL + IM_NECK + IM_MISSILE + IM_MISSILEW, 'Toberin, the dwarwen shopkeeper')
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

//Altar(x, y, D_LIFE)
void XLocation::Altar(int x, int y, int deity)
{
    new XAltar(x, y, (DEITY)deity, current_location);
}

void XLocation::Treasure(int x, int y, int val)
{
    XMoney * money = new XMoney(vRand(val) + val);
    money->Drop(current_location, x, y);
}

void XLocation::Chest(int x, int y, sol::optional<int> cnt, sol::optional<int> flg, sol::optional<int> mnval, sol::optional<int> mxval)
{
    XChest * tchest = new XChest(cnt.value_or(5), (ITEM_MASK)flg.value_or(IM_ITEM), mnval.value_or(100), mxval.value_or(25000));
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
    dmg = p->onMagicDamage(dmg, (RESISTANCE)resist);
    p->_HP -= dmg;

    if (p->_HP < 0 && msg) {
        XFakeCreature * tcr = new XFakeCreature((char*)msg->c_str());
        p->Die(tcr);
        tcr->Invalidate();
    }
}

void XLocation::ChangeStats(void* cr, int st, int val)
{
    ((XCreature*)cr)->GainAttr((STATS)st, val);
}

int XLocation::GetStats(void* cr, int st)
{
    return ((XCreature*)cr)->GetStats((STATS)st);
}

int XLocation::Rand(int val)
{
    return vRand(val);
}

bool XLocation::isHero(void* cr)
{
    return ((XCreature*)cr)->isHero();
}

bool XLocation::isEnemy(void* cr1, void* cr2)
{
    XCreature * p1 = (XCreature*)cr1;
    XCreature * p2 = (XCreature*)cr2;
    return p1 && p2 && p1->xai->isEnemy(p2);
}

void* XLocation::FindCreature(int l_id, int gid, sol::optional<int> x, sol::optional<int> y, sol::optional<int> w, sol::optional<int> h)
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
    ((XCreature*)cr)->xai->SetEnemyClass((CREATURE_CLASS)cr_class);
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

XGUID XLocation::GetObjectGUID(void* obj)
{
    return ((XObject*)obj)->guid();
}

std::tuple<int, int, int, int, int, std::string> XLocation::GetItemParam(void* item)
{
    XItem * p = (XItem*)item;
    return {p->im, p->brt, p->wt, p->it, p->quantity, p->name};
}

void XLocation::SetItemBrand(void* item, int br)
{
    ((XItem*)item)->brt = (BRAND_TYPE)br;
}

int XLocation::MakeEffect(int effect, void* caller, void* location, int call_x, int call_y, void* target, int target_x, int target_y, int power)
{
    EFFECT_DATA ed;
    ed.effect = (EFFECT)effect;
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
    qr->status = (QUEST)status;
    qr->know = know;
    qr->complete = complete;
    qr->closed = closed;
    XQuest::quest.quests.push_back(std::move(qr));
}

void XLocation::QuestModify(int id, int status)
{
    XQuestRec * qr = XQuest::quest.Find(id);

    if (qr) {
        qr->status = (QUEST)status;
    }
}

int XLocation::QuestStatus(int id)
{
    XQuestRec * qr = XQuest::quest.Find(id);
    return qr ? qr->status : Q_UNKNOWN;
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

void XLocation::ExecuteAIScript()
{
    std::vector<SCRIPT_CMD> script;
    SCRIPT_CMD cmd;

    XPoint pt;

    cmd.cmd = SCC_MOVE_POINT;

    cmd.pt_x = ((XStairWay*)(*Game.locations[L_MUSHROOMS_CAVE5]->ways_list.begin()))->x;
    cmd.pt_y = ((XStairWay*)(*Game.locations[L_MUSHROOMS_CAVE5]->ways_list.begin()))->y;
    cmd.ln = L_MUSHROOMS_CAVE5;
    script.push_back(cmd);

    cmd.cmd = SCC_COLLECT_MUSHROOM;
    script.push_back(cmd);

    cmd.cmd = SCC_MOVE_POINT;
    cmd.pt_x = 13;
    cmd.pt_y = 8;
    cmd.ln = L_MAIN;
    script.push_back(cmd);

    cmd.cmd = SCC_DROP_ITEM;
    cmd.im = IM_FOOD;
    script.push_back(cmd);

    for (const auto& [key, obj] : objects) {
        if (!(obj->im & IM_CREATURE))
            continue;

        auto creature = dynamic_cast<XCreature *>(obj);

        if (creature->groupID() == GID_SMALL_VILLAGE_FARMER) {
            creature->xai->ExecuteScript(script);
        }
    }
}

void XLocation::CreateMushroom(void* location)
{
    XLocation * p = (XLocation*)location;
    XPoint pt;
    p->GetFreeXY(&pt);
    new XMushSpawn(pt.x, pt.y, p);
}

#define LUA_REG(x) { char buf[256]; sprintf(buf, #x "=%d", x); luaL_dostring(L, buf); }
#define LUA_REG_ALTNAME(name, value) { char buf[256]; sprintf(buf, #name "=%d", value); luaL_dostring(L, buf); }

lua_State* XLocation::L = nullptr;

void XLocation::CommonLuaInitialization()
{
    L = lua_open();

    LUA_REG(L_MAIN);

    LUA_REG(L_MUSHROOMS_CAVE1);
    LUA_REG(L_MUSHROOMS_CAVE2);
    LUA_REG(L_MUSHROOMS_CAVE3);
    LUA_REG(L_MUSHROOMS_CAVE4);
    LUA_REG(L_MUSHROOMS_CAVE5);

    LUA_REG(L_DWARFCITYCAVE1);
    LUA_REG(L_DWARFCITYCAVE2);
    LUA_REG(L_DWARFCITYCAVE3);
    LUA_REG(L_DWARFCITYCAVE4);
    LUA_REG(L_DWARFCITYCAVE5);
    LUA_REG(L_DWARFCITYCAVE6);
    LUA_REG(L_DWARFCITY);
    LUA_REG(L_DWARFTREASURE);
    LUA_REG(L_GASMINE1);
    LUA_REG(L_GASMINE2);
    LUA_REG(L_GASMINE3);
    LUA_REG(L_RATCELLAR);
    LUA_REG(L_EXTINCT_VOLCANO);

    LUA_REG(L_WIZTOWER_TOP);
    LUA_REG(L_KINGS_TREASURE);

    LUA_REG(L_WIZARD_DUNGEON1);
    LUA_REG(L_WIZARD_DUNGEON2);
    LUA_REG(L_WIZARD_DUNGEON3);
    LUA_REG(L_WIZARD_DUNGEON4);
    LUA_REG(L_WIZARD_DUNGEON5);
    LUA_REG(L_AHKULAN_CASTLE);

    LUA_REG(GFS_SUPRESS_INVIS);
    LUA_REG(GFS_SEE_INVIS);

    LUA_REG(CR_RAT);
    LUA_REG(CR_FELINE);
    LUA_REG(CR_CANINE);
    LUA_REG(CR_REPTILE);
    LUA_REG(CR_INSECT);
    LUA_REG(CR_HUMAN);
    LUA_REG(CR_ORC);
    LUA_REG(CR_GIANT);
    LUA_REG(CR_KOBOLD);
    LUA_REG(CR_UNDEAD);
    LUA_REG(CR_GOBLIN);
    LUA_REG(CR_DEMON);
    LUA_REG(CR_HUMANOID);
    LUA_REG(CR_BLOB);

    LUA_REG(CRL_VERY_LOW);
    LUA_REG(CRL_LOW);
    LUA_REG(CRL_ABOVE_LOW);
    LUA_REG(CRL_AVG);
    LUA_REG(CRL_ABOVE_AVG);
    LUA_REG(CRL_HI);
    LUA_REG(CRL_ABOVE_HI);
    LUA_REG(CRL_VERY_HI);
    LUA_REG(CRL_EXTREM_HI);
    LUA_REG(CRL_UNIQUE);
    LUA_REG(CRL_ANY);
    LUA_REG(CRL_VL);
    LUA_REG(CRL_LA);
    LUA_REG(CRL_AH);
    LUA_REG(CRL_HVH);

    LUA_REG_ALTNAME(GREEN_GRASS, XTileType::GREEN_GRAS);
    LUA_REG_ALTNAME(TREE, XTileType::TREE);
    LUA_REG_ALTNAME(SAND, XTileType::SAND);
    LUA_REG_ALTNAME(WINDOW, XTileType::WINDOW);
    LUA_REG_ALTNAME(MAGMA, XTileType::MAGMA);
    LUA_REG_ALTNAME(QUARTZ, XTileType::QUARTZ);
    LUA_REG_ALTNAME(CAVE_FLOOR, XTileType::CAVE_FLOOR);
    LUA_REG_ALTNAME(STONE_FLOOR, XTileType::STONE_FLOOR);
    LUA_REG_ALTNAME(PATH, XTileType::PATH);
    LUA_REG_ALTNAME(WOOD_WALL, XTileType::WOOD_WALL);
    LUA_REG_ALTNAME(STONE_WALL, XTileType::STONE_WALL);
    LUA_REG_ALTNAME(WATER, XTileType::WATER);
    LUA_REG_ALTNAME(DEEP_WATER, XTileType::DEEP_WATER);
    LUA_REG_ALTNAME(LAVA, XTileType::LAVA);
    LUA_REG_ALTNAME(HILL, XTileType::HILL);
    LUA_REG_ALTNAME(LOW_MOUNTAIN, XTileType::LOW_MOUNTAIN);
    LUA_REG_ALTNAME(MOUNTAIN, XTileType::MOUNTAIN);
    LUA_REG_ALTNAME(HIGH_MOUNTAIN, XTileType::HIGH_MOUNTAIN);
    LUA_REG_ALTNAME(BRIDGE, XTileType::BRIDGE);
    LUA_REG_ALTNAME(ROAD, XTileType::ROAD);
    LUA_REG_ALTNAME(OBSIDIAN_FLOOR, XTileType::OBSIDIAN_FLOOR);
    LUA_REG_ALTNAME(FENCE, XTileType::FENCE);
    LUA_REG_ALTNAME(GOLDEN_FLOOR, XTileType::GOLDEN_FLOOR);
    LUA_REG_ALTNAME(MARBLE_WALL, XTileType::MARBLE_WALL);
    LUA_REG_ALTNAME(BLACK_MARBLE_WALL, XTileType::BLACK_MARBLE_WALL);
    LUA_REG_ALTNAME(GOLDEN_FENCE, XTileType::GOLDEN_FENCE);
    LUA_REG_ALTNAME(TELEPORT_WHITE, XTileType::TELEPORT_WHITE);

    LUA_REG(GID_ORCS_WAR_PARTY);
    LUA_REG(GID_FOREST_BROTHER);
    LUA_REG(GID_GUARDIAN);
    LUA_REG(GID_SMALL_VILLAGE_FARMER);
    LUA_REG(GID_TOWNEE_1);
    LUA_REG(GID_DWARVEN_GUARDIAN);
    LUA_REG(GID_AHKULAN_GUARDIAN);
    LUA_REG(GID_RODERICK_GUARDIAN);

    LUA_REG(AIF_ALLOW_PICK_UP);
    LUA_REG(AIF_ALLOW_MOVE_WAY_UP);
    LUA_REG(AIF_ALLOW_MOVE_WAY_DOWN);
    LUA_REG(AIF_FREE_WAY);
    LUA_REG(AIF_ALLOW_MOVE_OUT);
    LUA_REG(AIF_FREE_MOVE);
    LUA_REG(AIF_FIND_WAY);
    LUA_REG(AIF_PEACEFUL);
    LUA_REG(AIF_COWARD);
    LUA_REG(AIF_ALLOW_PACK);
    LUA_REG(AIF_ALLOW_WEAR_ITEM);
    LUA_REG(AIF_GUARD_AREA);
    LUA_REG(AIF_PROTECT_AREA);
    LUA_REG(AIF_RANDOM_MOVE);

    LUA_REG(AIF_EXPLORER_MOVE);
    LUA_REG(AIF_EXECUTE_SCRIPT);
    LUA_REG(AIF_NO_SWAP);
    LUA_REG(AIF_INSECT);
    LUA_REG(AIF_LO_ANIMAL);
    LUA_REG(AIF_HI_ANIMAL);
    LUA_REG(AIF_CREATURE);
    LUA_REG(AIF_HUMAN);
    LUA_REG(AIF_GHOST);

    LUA_REG(IM_HAT);
    LUA_REG(IM_NECK);
    LUA_REG(IM_BODY);
    LUA_REG(IM_CLOAK);
    LUA_REG(IM_WEAPON);
    LUA_REG(IM_SHIELD);
    LUA_REG(IM_HAND);
    LUA_REG(IM_GLOVES);
    LUA_REG(IM_RING);
    LUA_REG(IM_BOOTS);
    LUA_REG(IM_MISSILEW);
    LUA_REG(IM_MISSILE);
    LUA_REG(IM_POTION);
    LUA_REG(IM_SCROLL);
    LUA_REG(IM_BOOK);
    LUA_REG(IM_WAND);
    LUA_REG(IM_FOOD);
    LUA_REG(IM_HERB);
    LUA_REG(IM_LIGHTSOURCE);
    LUA_REG(IM_TOOL);
    LUA_REG(IM_GEM);
    LUA_REG(IM_MONEY);
    LUA_REG(IM_STACKABLE);
    LUA_REG(IM_CHEST);
    LUA_REG(IM_ARMOUR);
    LUA_REG(IM_ITEM);

    // WSK_SWORD is the only weapon skill used
    LUA_REG_ALTNAME(WSK_SWORD, XWarSkills::SWORD);

    LUA_REG(BR_NONE);
    LUA_REG(BR_FIRE);
    LUA_REG(BR_HELLFIRE);
    LUA_REG(BR_COLD);
    LUA_REG(BR_ULTIMATECOLD);
    LUA_REG(BR_LIGHTNING);
    LUA_REG(BR_EARTH);
    LUA_REG(BR_ELEMENTAL_MASK);

    LUA_REG(BR_ACID);
    LUA_REG(BR_POISON);
    LUA_REG(BR_DEATH);
    LUA_REG(BR_DISEASE);
    LUA_REG(BR_PARALYSE);
    LUA_REG(BR_STUN);
    LUA_REG(BR_CONFUSE);
    LUA_REG(BR_DRAIN_LIFE);
    LUA_REG(BR_BLACK_MASK);

    LUA_REG(BR_UNDEADSLAYER);
    LUA_REG(BR_HUMANOIDSLAYER);
    LUA_REG(BR_ANIMALSLAYER);
    LUA_REG(BR_DRAGONSLAYER);
    LUA_REG(BR_GIANTSLAYER);
    LUA_REG(BR_ORCSLAYER);
    LUA_REG(BR_TROLLSLAYER);
    LUA_REG(BR_DEMONSLAYER);
    LUA_REG(BR_SLAYER_MASK);

    LUA_REG(BR_HOLYSLAYER);
    LUA_REG(BR_EVILSLAYER);
    LUA_REG(BR_ANY);

    LUA_REG(R_WHITE);
    LUA_REG(R_BLACK);
    LUA_REG(R_FIRE);
    LUA_REG(R_WATER);
    LUA_REG(R_AIR);
    LUA_REG(R_EARTH);
    LUA_REG(R_ACID);
    LUA_REG(R_COLD);
    LUA_REG(R_POISON);
    LUA_REG(R_DISEASE);
    LUA_REG(R_PARALYSE);
    LUA_REG(R_STUN);
    LUA_REG(R_CONFUSE);
    LUA_REG(R_BLIND);
    LUA_REG(R_LIGHT);
    LUA_REG(R_DARKNESS);
    LUA_REG(R_INVISIBLE);
    LUA_REG(R_SEE_INVISIBLE);

    LUA_REG(xBLACK);
    LUA_REG(xBLUE);
    LUA_REG(xGREEN);
    LUA_REG(xCYAN);
    LUA_REG(xRED);
    LUA_REG(xMAGENTA);
    LUA_REG(xBROWN);
    LUA_REG(xLIGHTGRAY);
    LUA_REG(xDARKGRAY);
    LUA_REG(xLIGHTBLUE);
    LUA_REG(xLIGHTGREEN);
    LUA_REG(xLIGHTCYAN);
    LUA_REG(xLIGHTRED);
    LUA_REG(xLIGHTMAGENTA);
    LUA_REG(xYELLOW);
    LUA_REG(xWHITE);

    LUA_REG(D_LIFE);
    LUA_REG(D_DEATH);

    LUA_REG(S_STR);
    LUA_REG(S_DEX);
    LUA_REG(S_TOU);
    LUA_REG(S_LEN);
    LUA_REG(S_WIL);
    LUA_REG(S_MAN);
    LUA_REG(S_PER);
    LUA_REG(S_CHR);
    LUA_REG(S_EOF);

    LUA_REG_ALTNAME(SKT_ARCHERY, XSkill::Skill::ARCHERY);
    LUA_REG_ALTNAME(SKT_FINDWEAKNESS, XSkill::Skill::FINDWEAKNESS);
    LUA_REG_ALTNAME(SKT_HEALING, XSkill::Skill::HEALING);
    LUA_REG_ALTNAME(SKT_CONCENTRATION, XSkill::Skill::CONCENTRATION);
    LUA_REG_ALTNAME(SKT_DODGE, XSkill::Skill::DODGE);
    LUA_REG_ALTNAME(SKT_TRADING, XSkill::Skill::TRADING);
    LUA_REG_ALTNAME(SKT_STEALING, XSkill::Skill::STEALING);
    LUA_REG_ALTNAME(SKT_LITERACY, XSkill::Skill::LITERACY);
    LUA_REG_ALTNAME(SKT_DETECTTRAP, XSkill::Skill::DETECTTRAP);
    LUA_REG_ALTNAME(SKT_DISARMTRAP, XSkill::Skill::DISARMTRAP);
    LUA_REG_ALTNAME(SKT_COOKING, XSkill::Skill::COOKING);
    LUA_REG_ALTNAME(SKT_MINING, XSkill::Skill::MINING);
    LUA_REG_ALTNAME(SKT_HERBALISM, XSkill::Skill::HERBALISM);
    LUA_REG_ALTNAME(SKT_RELIGION, XSkill::Skill::RELIGION);
    LUA_REG_ALTNAME(SKT_BACKSTABBING, XSkill::Skill::BACKSTABBING);
    LUA_REG_ALTNAME(SKT_FIRST_AID, XSkill::Skill::FIRST_AID);
    LUA_REG_ALTNAME(SKT_TACTICS, XSkill::Skill::TACTICS);
    LUA_REG_ALTNAME(SKT_ALCHEMY, XSkill::Skill::ALCHEMY);
    LUA_REG_ALTNAME(SKT_WOODCRAFT, XSkill::Skill::WOODCRAFT);
    LUA_REG_ALTNAME(SKT_CREATETRAP, XSkill::Skill::CREATETRAP);
    LUA_REG_ALTNAME(SKT_NECROMANCY, XSkill::Skill::NECROMANCY);
    LUA_REG_ALTNAME(SKT_ATHLETICS, XSkill::Skill::ATHLETICS);
    LUA_REG_ALTNAME(SKT_CLIMBING, XSkill::Skill::CLIMBING);

    LUA_REG(LE_MOVE);
    LUA_REG(LE_MOVE_IN);
    LUA_REG(LE_MOVE_OUT);
    LUA_REG(LE_OUTER_USE);
    LUA_REG(LE_CHAT);
    LUA_REG(LE_GIVE_ITEM);
    LUA_REG(LE_DIE);
    LUA_REG(LE_EVENT_SET);
    LUA_REG(LE_SAVE);
    LUA_REG(LE_LOAD);

    LUA_REG(IT_UNKNOWN);
    LUA_REG(IT_HAT);
    LUA_REG(IT_CAP);
    LUA_REG(IT_HELMET);
    LUA_REG(IT_AMULET);
    LUA_REG(IT_NECKLACE);
    LUA_REG(IT_RING);
    LUA_REG(IT_CLUB);
    LUA_REG(IT_WARHAMMER);
    LUA_REG(IT_DAGGER);
    LUA_REG(IT_KNIFE);
    LUA_REG(IT_ORCISHDAGGER);
    LUA_REG(IT_LONGDAGGER);
    LUA_REG(IT_SHORTSWORD);
    LUA_REG(IT_LONGSWORD);
    LUA_REG(IT_BROADSWORD);
    LUA_REG(IT_RAPIER);
    LUA_REG(IT_SCIMITAR);
    LUA_REG(IT_KATANA);
    LUA_REG(IT_WAKIZASHI);
    LUA_REG(IT_SMALLAXE);
    LUA_REG(IT_WARAXE);
    LUA_REG(IT_BATTLEAXE);
    LUA_REG(IT_GREATAXE);
    LUA_REG(IT_ORCISHAXE);
    LUA_REG(IT_MACE);
    LUA_REG(IT_FLAIL);
    LUA_REG(IT_SHORTSPEAR);
    LUA_REG(IT_LONGSPEAR);
    LUA_REG(IT_PITCHFORK);
    LUA_REG(IT_PIKE);
    LUA_REG(IT_HALBERD);
    LUA_REG(IT_STAFF);
    LUA_REG(IT_SHORTBOW);
    LUA_REG(IT_LONGBOW);
    LUA_REG(IT_LIGHTCROSSBOW);
    LUA_REG(IT_CROSSBOW);
    LUA_REG(IT_HEAVYCROSSBOW);
    LUA_REG(IT_SLING);
    LUA_REG(IT_GLOVES);
    LUA_REG(IT_GAUNTLETS);
    LUA_REG(IT_KNUCKLES);
    LUA_REG(IT_SMALLSHIELD);
    LUA_REG(IT_MEDIUMSHIELD);
    LUA_REG(IT_LARGESHIELD);
    LUA_REG(IT_TOWERSHIELD);
    LUA_REG(IT_SANDALS);
    LUA_REG(IT_LIGHTBOOTS);
    LUA_REG(IT_SOFTBOOTS);
    LUA_REG(IT_HARDBOOTS);
    LUA_REG(IT_CLOTHES);
    LUA_REG(IT_DRESS);
    LUA_REG(IT_ROBE);
    LUA_REG(IT_LIGHTMAIL);
    LUA_REG(IT_SCALEMAIL);
    LUA_REG(IT_PLATEMAIL);
    LUA_REG(IT_CHAINMAIL);
    LUA_REG(IT_RINGMAIL);
    LUA_REG(IT_CLOAK);
    LUA_REG(IT_SHADOWCLOAK);
    LUA_REG(IT_CAPE);
    LUA_REG(IT_LIGHTCLOAK);
    LUA_REG(IT_FORESTBROTHERCLOAK);
    LUA_REG(IT_TORCH);
    LUA_REG(IT_SCROLL);
    LUA_REG(IT_BOOK);
    LUA_REG(IT_POTION);
    LUA_REG(IT_HERB);
    LUA_REG(IT_LARGERATION);
    LUA_REG(IT_RATION);
    LUA_REG(IT_SMALLRATION);
    LUA_REG(IT_ELVISHWAYBREAD);
    LUA_REG(IT_CORPSE);
    LUA_REG(IT_BONE);
    LUA_REG(IT_RATTAIL);
    LUA_REG(IT_BATWING);
    LUA_REG(IT_ARROW);
    LUA_REG(IT_QUARREL);
    LUA_REG(IT_SLINGBULLET);
    LUA_REG(IT_ROCK);
    LUA_REG(IT_SHURIKEN);
    LUA_REG(IT_COOKINGSET);
    LUA_REG(IT_PICKAXE);
    LUA_REG(IT_ANCIENTMACHINEPART);
    LUA_REG(IT_EYEOFRAA);
    LUA_REG(IT_ALCHEMYSET);
    LUA_REG(IT_CHEST);
    LUA_REG(IT_MONEY);
    LUA_REG(IT_GEM);
    LUA_REG(IT_RANDOM);
    LUA_REG(IT_EOF);

    LUA_REG(Q_UNKNOWN);
    LUA_REG(Q_KNOWN);
    LUA_REG(Q_COMPLETE);
    LUA_REG(Q_CLOSED);
    LUA_REG(Q_FAIL);

    LUA_REG(GEN_MALE);
    LUA_REG(GEN_FEMALE);
    LUA_REG(GEN_NEUTER);

    LUA_REG(E_NONE);
    LUA_REG(E_CURE_LIGHT_WOUNDS);
    LUA_REG(E_CURE_SERIOUS_WOUNDS);
    LUA_REG(E_CURE_CRITICAL_WOUNDS);
    LUA_REG(E_CURE_MORTAL_WOUNDS);
    LUA_REG(E_HEAL);
    LUA_REG(E_ULTRAHEAL);
    LUA_REG(E_POWER);
    LUA_REG(E_ULTRAPOWER);
    LUA_REG(E_RESTORATION);
    LUA_REG(E_CURE_POISON);
    LUA_REG(E_CURE_DISEASE);

    LUA_REG(E_BURNING_HANDS);
    LUA_REG(E_ICE_TOUCH);
    LUA_REG(E_DRAIN_LIFE);

    LUA_REG(E_MAGIC_ARROW);
    LUA_REG(E_FIRE_BOLT);
    LUA_REG(E_ICE_BOLT);
    LUA_REG(E_LIGHTNING_BOLT);
    LUA_REG(E_ACID_BOLT);

    LUA_REG(E_HEROISM);
    LUA_REG(E_IDENTIFY);
    LUA_REG(E_GREAT_IDENTIFY);
    LUA_REG(E_SUMMON_MONSTER);
    LUA_REG(E_CREATE_ITEM);
    LUA_REG(E_BLINK);
    LUA_REG(E_TELEPORT);
    LUA_REG(E_SELF_KNOWLEDGE);
    LUA_REG(E_SEE_INVISIBLE);
    LUA_REG(E_ACID_RESISTANCE);
    LUA_REG(E_FIRE_RESISTANCE);
    LUA_REG(E_COLD_RESISTANCE);
    LUA_REG(E_POISON_RESISTANCE);

    lua_register(L, "StoreInt", StoreInt);
    lua_register(L, "RestoreInt", RestoreInt);

    luaopen_base(L);
    luaopen_string(L);

    // Sol2-bound Monster builder - registered before world scripts
    // load below, since world/creatures.lua calls it while loading.
    {
        sol::state_view lua(L);
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
        sol::state_view lua(L);
        lua.set_function("CreateLocation", &XLocation::CreateLocation);
        lua.set_function("Settle", &XLocation::Settle);
        lua.set_function("Creature", &XLocation::Creature);
        lua.set_function("Guardian", &XLocation::Guardian);
        lua.set_function("Way", &XLocation::Way);
        lua.set_function("CreateObject", sol::overload(&XLocation::CreateObjectByName, &XLocation::CreateObjectByMask));
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
        sol::state_view lua(L);
        lua.set_function("isHero", &XLocation::isHero);
        lua.set_function("isEnemy", &XLocation::isEnemy);
        lua.set_function("FindCreature", &XLocation::FindCreature);

        lua.set_function("AddMessage", &XLocation::AddMessage);
        lua.set_function("AskQuestion", &XLocation::AskQuestion);

        lua.set_function("SetItEnemyFor", &XLocation::SetItEnemyFor);
        lua.set_function("SetEnemy", &XLocation::SetEnemy);
        lua.set_function("ChangeStats", &XLocation::ChangeStats);
        lua.set_function("GetStats", &XLocation::GetStats);
        lua.set_function("InflictDamage", &XLocation::InflictDamage);
        lua.set_function("Rand", &XLocation::Rand);
        lua.set_function("SetEventHandler", &XLocation::SetEventHandler);
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

        lua.set_function("ExecuteAIScript", &XLocation::ExecuteAIScript);
    }

    luaL_dofile(L, "./world/init.lua");

    luaL_dostring(L, "LoadScripts()");
    XCreatureStorage::CreateQuickBase();

    // Additive only: nothing above depends on this, and nothing
    // below reads Sol2Ping.
    {
        sol::state_view lua(L);
        lua.set_function("Sol2Ping", [](int x) { return x + 1; });
        assert(lua["Sol2Ping"](41).get<int>() == 42);
        assert(lua.script("return Sol2Ping(99)").get<int>() == 100);
    }
}

void XLocation::Restoration()
{
    CommonLuaInitialization();
}

void XLocation::CreateNewGame()
{
    CommonLuaInitialization();
    luaL_dostring(L, "MakeAvanorValley()");
    luaL_dostring(L, "MakeSmallCave()");
    luaL_dostring(L, "MakeMushroomCave()");
    luaL_dostring(L, "MakeDwarvenCity()");
    luaL_dostring(L, "MakeRatCellar()");
    luaL_dostring(L, "MakeVulcano()");
    luaL_dostring(L, "MakeWizardDungeon()");
    luaL_dostring(L, "CreateAllQuests()");
}
