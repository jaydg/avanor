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

int XLocation::rand_location_count = L_RANDOM;

REGISTER_CLASS(XLocation);

XLocation::XLocation(LOCATION location)
{
    visited_by_hero = 0;
    map = nullptr;	//map will created by XBuilder...

    for (int i = 0; i < MAX_PLACES; i++) {
        places[i] = nullptr;
    }

    assert(Game.locations[location] == nullptr);
    ln = location;
    Game.locations[location] = this;

    ttmb = 1000000;
    ttm = ttmb;
    im = IM_OTHER;
}

void XLocation::Invalidate()
{
    for (int i = 0; i < MAX_PLACES; i++)
        if (places[i]) {
            places[i]->Invalidate();
            places[i] = nullptr;
        }

    delete map; // map must be the last!!!!!

    XObject::Invalidate();
}

int XLocation::Run()
{
    if (event.size()) {
        lua_pushstring(XLocation::L, event.c_str());
        lua_gettable(XLocation::L, LUA_GLOBALSINDEX);
        lua_pushlightuserdata(XLocation::L, this);
        lua_call(XLocation::L, 1, 1);
        int res = lua_tonumber(XLocation::L, 3);
        lua_pop(XLocation::L, 1);
        ttm = ttmb;
        return res;
    }

    return 1;
}

void XLocation::AddPlace(XAnyPlace * pl)
{
    for (int i = 0; i < MAX_PLACES; i++)
        if (places[i] == nullptr) {
            places[i] = pl;
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

void XLocation::Store(XFile * f)
{
    XObject::Store(f);
    map->Store(f);

    for (int i = 0; i < MAX_PLACES; i++) {
        places[i].Store(f);
    }

    f->Write(brief_name, 10);
    f->Write(full_name, 80);
    f->Write(&visited_by_hero);
    f->Write(&ln, sizeof(LOCATION));
    f->WriteStr(event);
}

void XLocation::Restore(XFile * f)
{
    XObject::Restore(f);
    map = new XMap();
    map->Restore(f);

    for (int i = 0; i < MAX_PLACES; i++) {
        places[i].Restore(f);

        if (places[i] && places[i]->im & IM_WAY) {
            ways_list.push_back(places[i]);
        }
    }

    f->Read(brief_name, 10);
    f->Read(full_name, 80);
    f->Read(&visited_by_hero);
    f->Read(&ln, sizeof(LOCATION));
    f->ReadStr(event);
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
    sprintf(brief_name, "Rnd%d", deep);
    sprintf(full_name, "Random Place Level %d", deep);

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

LOCATION_PATTERN XLocation::current_pattern = {nullptr, 0, 0};
std::vector<PALETTE_MAP> XLocation::pattern_translation;

//CreateLocation(L_SMALL_CAVE1, "SmCv:1", "Small Cave Level 1", CAVE)
int XLocation::CreateLocation(lua_State * L)
{
    int loc_id = lua_tonumber(L, 1);
    const char* lbrief = lua_tostring(L, 2);
    const char* lfull = lua_tostring(L, 3);
    int type = lua_tonumber(L, 4);

    current_location = new XLocation((LOCATION)loc_id);
    strcpy(current_location->brief_name, lbrief);
    strcpy(current_location->full_name, lfull);

    if (type == 0) {
        current_location->BuildCave();
    } else if (type == 1) {
        current_location->BuildLabirint();
    } else {
        current_location->BuildPlain(200, 90);
    }

    return 0;
}

//Settle(CR_RAT + CR_FELINE + CR_INSECT, CRL_VERY_LOW)
int XLocation::Settle(lua_State * L)
{
    int crc = lua_tonumber(L, 1);
    int crl = lua_tonumber(L, 2);
    Game.Scheduler.Add(new XUniversalGen(current_location, (CREATURE_CLASS)(crc), (CREATURE_LEVEL)(crl), 5, 25000));
    return 0;
}

//cr = Creature(CN_ROTMOTH)
//cr = Creature(CN_RAT, [x, y, [w, h]])
int XLocation::Creature(lua_State * L)
{
    int crn = lua_tonumber(L, 1);
    int n = lua_gettop(L);
    XCreature * cr = nullptr;

    if (n == 1) {
        cr = current_location->NewCreature((CREATURE_NAME)crn);
    } else {
        XRect rect;
        int tx = lua_tonumber(L, 2);
        int ty = lua_tonumber(L, 3);

        if (n == 3) {
            rect = XRect(tx, ty, tx + 1, ty + 1);
        } else {
            int tw = lua_tonumber(L, 4);
            int th = lua_tonumber(L, 5);
            rect = XRect(tx, ty, tx + tw, ty + th);
        }

        cr = current_location->NewCreature((CREATURE_NAME)crn, rect);
    }

    lua_pushlightuserdata(L, cr);
    return 1;
}

//cr = Guardian(CN_DWARF_GUARD, GID_DWARVEN_GUARDIAN, x, y, [len,  hgt], [flags])
int XLocation::Guardian(lua_State * L)
{
    CREATURE_NAME crn = (CREATURE_NAME)lua_tonumber(L, 1);
    GROUP_ID gid = (GROUP_ID)lua_tonumber(L, 2);
    XRect rect;
    int tx = lua_tonumber(L, 3);
    int ty = lua_tonumber(L, 4);
    int n = lua_gettop(L);

    if (n == 4) {
        rect = XRect(tx, ty, tx + 1, ty + 1);
    } else {
        int tw = lua_tonumber(L, 5);
        int th = lua_tonumber(L, 6);
        rect = XRect(tx, ty, tx + tw, ty + th);
    }

    int flag = AIF_GUARD_AREA;

    if (n == 7) {
        flag |= static_cast<int>(lua_tonumber(L, 7));
    }

    XCreature * cr = current_location->NewCreature(crn, rect, gid, flag);
    cr->xai->SetEnemyClass((CREATURE_CLASS)(CR_ALL ^ (CR_HUMAN | CR_HUMANOID)));
    lua_pushlightuserdata(L, cr);
    return 1;
}

//Way(DOWN, L_SMALL_CAVE2)
//Way(DOWN, L_SMALL_CAVE2, x, y)
int XLocation::Way(lua_State * L)
{
    int type = lua_tonumber(L, 1);
    int loc_id = lua_tonumber(L, 2);
    int n = lua_gettop(L);

    if (n == 4) {
        int tx = lua_tonumber(L, 3);
        int ty = lua_tonumber(L, 4);
        current_location->NewWay(tx, ty, (LOCATION)loc_id, (STAIRWAY_TYPE)type);
    } else {
        current_location->NewWay((LOCATION)loc_id, (STAIRWAY_TYPE)type);
    }

    return 0;
}

//CreateObject("XCookingSet")
int XLocation::CreateObject(lua_State * L)
{
    int n = lua_gettop(L);
    XObject * p;

    if (n == 3) {
        int flag = lua_tonumber(L, 1);
        int min_val = lua_tonumber(L, 2);
        int max_val = lua_tonumber(L, 3);
        p = ICREATE((ITEM_MASK)(flag), min_val, max_val);
    } else {
        const char* name = lua_tostring(L, 1);
        p = XClassFactory::CreateNew((char*)name);
    }

    lua_pushlightuserdata(L, p);
    return 1;
}

//DropItem(item, 0, 0)
//DropItem(item)
//DropItem(item, object)
int XLocation::DropItem(lua_State * L)
{
    XItem * pItem = (XItem*)lua_topointer(L, 1);
    int n = lua_gettop(L);

    int tx;
    int ty;

    if (n == 3) {
        tx = lua_tonumber(L, 2);
        ty = lua_tonumber(L, 3);
    } else if (n == 1) {
        XPoint pt;
        current_location->GetFreeXY(&pt);
        tx = pt.x;
        ty = pt.y;
    } else if (n == 2) {
        XMapObject * pMO = (XMapObject*)lua_topointer(L, 2);
        tx = pMO->x;
        ty = pMO->y;
        pItem->Drop(pMO->l, tx, ty);
        return 0;
    }

    if (pItem) {
        pItem->Drop(current_location, tx, ty);
    }

    return 0;
}

//SetPattern(width, height,
// "###" ..
// "#.#" ..
// "###")
int XLocation::SetPattern(lua_State * L)
{
    current_pattern.w = lua_tonumber(L, 1);
    current_pattern.h = lua_tonumber(L, 2);
    const char* txt = lua_tostring(L, 3);

    if (current_pattern.pattern) {
        delete[] current_pattern.pattern;
    }

    current_pattern.pattern = new char[strlen(txt) + 1];
    strcpy(current_pattern.pattern, txt);
    //	current_pattern.pattern = lua_tostring(L, 3);
    pattern_translation.clear();
    return 0;
}

//AddTranslation("1", GOLDEN_FLOOR)
int XLocation::AddTranslation(lua_State * L)
{
    PALETTE_MAP pm;
    pm.this_view = (lua_tostring(L, 1))[0];

    if (lua_isnumber(L, 2)) {
        pm.real_view = (XTileType::Type)lua_tonumber(L, 2);
        pm.lua_str[0] = 0;
    } else {
        strcpy(pm.lua_str, lua_tostring(L, 2));
        pm.real_view = XTileType::UNKNOWN;
    }

    pattern_translation.push_back(pm);
    return 0;
}

//DrawPattern(x, y)
int XLocation::DrawPattern(lua_State * L)
{
    int tx = lua_tonumber(L, 1);
    int ty = lua_tonumber(L, 2);
    pat_offs_x = tx;
    pat_offs_y = ty;
    current_location->PutPalette(tx, ty);
    return 0;
}

//BuildShop(x, y, 9, 3, IM_ARMOUR + IM_WEAPON + IM_POTION + IM_BOOK + IM_SCROLL + IM_NECK + IM_MISSILE + IM_MISSILEW, 'Toberin, the dwarwen shopkeeper')
int XLocation::BuildShop(lua_State * L)
{
    int tx = lua_tonumber(L, 1);
    int ty = lua_tonumber(L, 2);
    int tw = lua_tonumber(L, 3);
    int th = lua_tonumber(L, 4);
    XRect shop_rect(tx, ty, tx + tw, ty + th);
    int mask = lua_tonumber(L, 5);
    const char* keeper_name = lua_tostring(L, 6);
    current_location->CreateShop(mask, shop_rect, (char*)keeper_name, SHOP_BUILD_IN);
    return 0;
}

//Furniture(x, y, xLIGHTRED, '~', 'a royal bad')
int XLocation::Furniture(lua_State * L)
{
    int tx = lua_tonumber(L, 1);
    int ty = lua_tonumber(L, 2);
    int tc = lua_tonumber(L, 3);
    const char* tv = lua_tostring(L, 4);
    const char* subscr = lua_tostring(L, 5);
    XFurniture * p = new XFurniture(tx, ty, tc, tv[0], (char*)subscr, current_location);
    lua_pushlightuserdata(L, p);
    return 1;
}

//Furniture(x, y, xLIGHTRED, '~', 'a royal bad', 'EventHandler')
int XLocation::OuterObject(lua_State * L)
{
    int n = lua_gettop(L);

    int tx;
    int ty;
    int tc;
    const char* tv;
    const char* subscr;
    const char* event = nullptr;

    if (n < 5) {
        XPoint pt;
        current_location->GetFreeXY(&pt);
        tx = pt.x;
        ty = pt.y;
        tc = lua_tonumber(L, 1);
        tv = lua_tostring(L, 2);
        subscr = lua_tostring(L, 3);

        if (n == 4) {
            event = lua_tostring(L, 4);
        }
    } else {
        tx = lua_tonumber(L, 1);
        ty = lua_tonumber(L, 2);
        tc = lua_tonumber(L, 3);
        tv = lua_tostring(L, 4);
        subscr = lua_tostring(L, 5);

        if (n > 5) {
            event = lua_tostring(L, 6);
        }
    }

    XOuterObject * p = new XOuterObject(tx, ty, tc, tv[0], (char*)subscr, current_location, event);
    lua_pushlightuserdata(L, p);
    return 1;
}

//Altar(x, y, D_LIFE)
int XLocation::Altar(lua_State * L)
{
    int tx = lua_tonumber(L, 1);
    int ty = lua_tonumber(L, 2);
    int deity = lua_tonumber(L, 3);
    new XAltar(tx, ty, (DEITY)deity, current_location);
    return 0;
}

int XLocation::Treasure(lua_State * L)
{
    int tx = lua_tonumber(L, 1);
    int ty = lua_tonumber(L, 2);
    int val = lua_tonumber(L, 3);
    XMoney * money = new XMoney(vRand(val) + val);
    money->Drop(current_location, tx, ty);
    return 0;
}

int XLocation::Chest(lua_State * L)
{
    int tx = lua_tonumber(L, 1);
    int ty = lua_tonumber(L, 2);

    int cnt = 5;
    int flg = IM_ITEM;
    int mnval = 100;
    int mxval = 25000;

    int n = lua_gettop(L);

    if (n > 2) {
        cnt = lua_tonumber(L, 3);
    }

    if (n > 3) {
        flg = lua_tonumber(L, 4);
    }

    if (n > 4) {
        mnval = lua_tonumber(L, 5);
        mxval = lua_tonumber(L, 6);
    }

    XChest * tchest = new XChest(cnt, (ITEM_MASK)flg, mnval, mxval);
    tchest->Drop(current_location, tx, ty);
    return 0;
}

int XLocation::Trap(lua_State * L)
{
    int tx = lua_tonumber(L, 1);
    int ty = lua_tonumber(L, 2);
    new XTrap(tx, ty, current_location);
    return 0;
}

//
int XLocation::EventPlace(lua_State * L)
{
    int n = lua_gettop(L);
    XRect area(0, 0, current_location->map->len, current_location->map->hgt);
    const char* event = nullptr;

    if (n > 1) {
        area.left = lua_tonumber(L, 1);
        area.top = lua_tonumber(L, 2);
        area.right = area.left + lua_tonumber(L, 3);
        area.bottom = area.top + lua_tonumber(L, 4);
        event = lua_tostring(L, 5);
    } else {
        event = lua_tostring(L, 1);
    }

    XAnyPlace * place = new XAnyPlace(area, current_location, (char*)event);
    return 0;
}

int XLocation::GetSkill(lua_State * L)
{
    XCreature * p = (XCreature*)lua_topointer(L, 1);
    int skill = lua_tonumber(L, 2);
    XSkill * sk = p->sk->GetSkill((XSkill::Skill)skill);

    if (sk) {
        lua_pushnumber(L, sk->GetLevel());
    } else {
        lua_pushnumber(L, 0);
    }

    return 1;
}

int XLocation::LearnSkill(lua_State * L)
{
    XCreature * p = (XCreature*)lua_topointer(L, 1);
    int skill = lua_tonumber(L, 2);
    int val = lua_tonumber(L, 3);

    if (!p->sk->GetSkill((XSkill::Skill)skill)) {
        p->sk->Learn((XSkill::Skill)skill, val);
    }

    return 0;
}

int XLocation::MoneyOperation(lua_State * L)
{
    XCreature * p = (XCreature*)lua_topointer(L, 1);
    int val = lua_tonumber(L, 2);
    lua_pushnumber(L, p->MoneyOp(val));
    return 1;
}

int XLocation::CreateTimerEvent(lua_State * L)
{
    current_location->event = lua_tostring(L, 1);
    current_location->ttm = lua_tonumber(L, 2);
    current_location->ttmb = current_location->ttm;
    Game.Scheduler.Add(current_location);
    return 0;
}

//InflictDamage (target, dmg, RESISTANCE_TYPE, creature ["msg"])
int XLocation::InflictDamage(lua_State * L)
{
    XCreature * p = (XCreature*)lua_topointer(L, 1);
    int dmg = lua_tonumber(L, 2);
    int resist = lua_tonumber(L, 3);
    dmg = p->onMagicDamage(dmg, (RESISTANCE)resist);
    p->_HP -= dmg;

    if (p->_HP < 0) {
        if (lua_isstring(L, 4)) {
            const char* str = lua_tostring(L, 4);
            XFakeCreature * tcr = new XFakeCreature((char*)str);
            p->Die(tcr);
            tcr->Invalidate();
        } else {
        }
    }

    return 0;
}

int XLocation::ChangeStats(lua_State * L)
{
    XCreature * p = (XCreature*)lua_topointer(L, 1);
    int st = lua_tonumber(L, 2);
    int val = lua_tonumber(L, 3);
    p->GainAttr((STATS)st, val);
    return 0;
}

int XLocation::GetStats(lua_State * L)
{
    XCreature * p = (XCreature*)lua_topointer(L, 1);
    int st = lua_tonumber(L, 2);
    int val = p->GetStats((STATS)st);
    lua_pushnumber(L, val);
    return 1;
}

int XLocation::Rand(lua_State * L)
{
    int val = lua_tonumber(L, 1);
    lua_pushnumber(L, vRand(val));
    return 1;
}

int XLocation::isHero(lua_State * L)
{
    XCreature * p = (XCreature*)lua_topointer(L, 1);
    lua_pushboolean(L, p->isHero());
    return 1;
}

int XLocation::isEnemy(lua_State * L)
{
    XCreature * p1 = (XCreature*)lua_topointer(L, 1);
    XCreature * p2 = (XCreature*)lua_topointer(L, 2);

    if (p1 && p2) {
        lua_pushboolean(L, p1->xai->isEnemy(p2));
    } else {
        lua_pushboolean(L, false);
    }

    return 1;
}

int XLocation::FindCreature(lua_State * L)
{
    int l_id = lua_tonumber(L, 1);
    int gid = lua_tonumber(L, 2);
    int n = lua_gettop(L);

    XRect rect(0, 0, Game.locations[l_id]->map->len, Game.locations[l_id]->map->hgt);

    if (n == 6) {
        rect.left = lua_tonumber(L, 3);
        rect.top = lua_tonumber(L, 4);
        rect.right = rect.left + lua_tonumber(L, 5);
        rect.bottom = rect.top + lua_tonumber(L, 6);
    }

    for (int i = rect.left; i < rect.right; i++)
        for (int j = rect.top; j < rect.bottom; j++) {
            XCreature* cr = Game.locations[l_id]->map->GetMonster(i, j);

            if (cr && cr->groupID() == gid) {
                lua_pushlightuserdata(L, cr);
                return 1;
            }
        }

    lua_pushlightuserdata(L, nullptr);
    return 1;
}

int XLocation::SetItEnemyFor(lua_State * L)
{
    XCreature * p1 = (XCreature*)lua_topointer(L, 1);

    if (lua_islightuserdata(L, 2)) {
        XCreature * p2 = (XCreature*)lua_topointer(L, 2);

        if (p2 && p1) {
            p2->xai->AddPersonalEnemy(p1);
            p2->xai->SetGroupEnemy(p1);
        }
    }

    return 0;
}

int XLocation::SetEnemy(lua_State * L)
{
    XCreature * p = (XCreature*)lua_topointer(L, 1);
    int cr_class = lua_tonumber(L, 2);
    p->xai->SetEnemyClass((CREATURE_CLASS)cr_class);
    return 0;
}

int XLocation::Gender(lua_State * L)
{
    XCreature * p = (XCreature*)lua_topointer(L, 1);
    int gender = p->GetGender();
    lua_pushnumber(L, gender);
    return 1;
}

int XLocation::SetName(lua_State * L)
{
    XMapObject * p = (XMapObject*)lua_topointer(L, 1);
    const char* str = lua_tostring(L, 2);
    p->SetName(str);
    return 0;
}

int XLocation::SetView(lua_State * L)
{
    XMapObject * p = (XMapObject*)lua_topointer(L, 1);
    const char* view = lua_tostring(L, 2);
    int color = lua_tonumber(L, 3);
    p->SetView(view[0], color);
    return 0;
}

int XLocation::GetView(lua_State * L)
{
    XMapObject * p = (XMapObject*)lua_topointer(L, 1);
    char buf[2] = "A";
    buf[0] = p->view;
    lua_pushstring(L, buf);
    return 1;
}

int XLocation::AddMessage(lua_State * L)
{
    const char* str = lua_tostring(L, 1);
    msgwin.Add(str);
    return 0;
}

//AskQuestion("Are you sure?", "yn", "Yes", "No")
struct ASK_QUESTION_REC {
    std::string val;
    int key;
};

int XLocation::AskQuestion(lua_State * L)
{
    const char* msg = lua_tostring(L, 1);
    const char* key = lua_tostring(L, 2);
    msgwin.Add(msg);

    std::string out = "[";

    int offs = 0;
    char key_value[20];
    int index = 0;

    std::vector<ASK_QUESTION_REC> keys;

    while (sscanf(key + offs, "%s10%n", key_value, &offs) > 0) {
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
            variant = lua_tostring(L, 3 + index);
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
            lua_pushstring(L, it.val.c_str());
            return 1;
        }
    }

    lua_pushstring(L, keys.front().val.c_str());
    return 1;
}

int XLocation::SetEventHandler(lua_State * L)
{
    XCreature * p = (XCreature*)lua_topointer(L, 1);
    const char* event = lua_tostring(L, 2);
    p->SetEventHandler(event);
    return 1;
}


int XLocation::GetObjectGUID(lua_State * L)
{
    XObject * p = (XObject*)lua_topointer(L, 1);
    lua_pushnumber(L, p->guid());
    return 1;
}

int XLocation::GetItemParam(lua_State * L)
{
    XItem * p = (XItem*)lua_topointer(L, 1);

    lua_pushnumber(L, p->im);
    lua_pushnumber(L, p->brt);
    lua_pushnumber(L, p->wt);
    lua_pushnumber(L, p->it);
    lua_pushnumber(L, p->quantity);
    lua_pushstring(L, p->name.c_str());

    return 6;
}

int XLocation::SetItemBrand(lua_State * L)
{
    XItem * p = (XItem*)lua_topointer(L, 1);
    int br = lua_tonumber(L, 2);
    p->brt = (BRAND_TYPE)br;
    return 0;
}

int XLocation::MakeEffect(lua_State * L)
{
    EFFECT_DATA ed;
    ed.effect = (EFFECT)lua_tonumber(L, 1);
    ed.caller = (XCreature*)lua_topointer(L, 2);
    ed.l = (XLocation*)lua_topointer(L, 3);
    ed.call_x = lua_tonumber(L, 4);
    ed.call_y = lua_tonumber(L, 5);
    ed.target = (XCreature*)lua_topointer(L, 6);
    ed.target_x = lua_tonumber(L, 7);
    ed.target_y = lua_tonumber(L, 8);
    ed.power = lua_tonumber(L, 9);

    lua_pushnumber(L, XEffect::Make(&ed));

    return 1;
}

int XLocation::DestroyObject(lua_State * L)
{
    XItem * it = (XItem*)lua_topointer(L, 1);
    it->Invalidate();

    return 0;
}

int XLocation::SetCompanion(lua_State * L)
{
    XCreature * owner = (XCreature*)lua_topointer(L, 1);
    XCreature * slave = (XCreature*)lua_topointer(L, 2);
    bool flag = lua_toboolean(L, 3);

    if (flag) {
        slave->xai->companion = owner;
        slave->xai->companion_command = CC_FOLLOW;
    } else {
        slave->xai->companion = nullptr;
    }

    return 0;
}


int XLocation::GiveObjectToCreature(lua_State * L)
{
    XItem * p = (XItem*)lua_topointer(L, 1);
    XCreature * cr = (XCreature*)lua_topointer(L, 2);
    cr->ContainItem(p);

    return 0;
}

int XLocation::GiveAward(lua_State * L)
{
    const auto owner = (XCreature*)lua_topointer(L, 1);
    const XGUID aguid = lua_tonumber(L, 2);
    const auto target = (XCreature*)lua_topointer(L, 3);
    auto item = dynamic_cast<XItem *>(GetObject(aguid));

    const auto it = owner->contain.find(item);
    if (it != owner->contain.end()) {
        owner->contain.erase(it);
    } else {
        for (const auto bp: owner->components) {
            if (bp->Item() && bp->Item() == item) {
                item = bp->UnWear();
                break;
            }
        }
    }

    if (item) {
        owner->UnCarryItem(item);

        if (target->CarryItem(item)) {
            target->contain.insert(item);
        } else {
            owner->DropItem(item);
        }

        lua_pushboolean(L, true);
    } else {
        lua_pushboolean(L, false);
    }

    return 1;
}

int XLocation::Quest(lua_State * L)
{
    QUEST_REC * qr = new QUEST_REC;
    qr->quest_id = lua_tonumber(L, 1);
    qr->status = (QUEST)lua_tonumber(L, 2);
    qr->know = lua_tostring(L, 3);
    qr->complete = lua_tostring(L, 4);
    qr->closed = lua_tostring(L, 5);
    XQuest::quest.quests.push_back(qr);

    return 0;
}

int XLocation::QuestModify(lua_State * L)
{
    int id = lua_tonumber(L, 1);
    QUEST_REC * qr = XQuest::quest.Find(id);

    if (qr) {
        qr->status = (QUEST)lua_tonumber(L, 2);
    }

    return 0;
}


int XLocation::QuestStatus(lua_State * L)
{
    int id = lua_tonumber(L, 1);
    QUEST_REC * qr = XQuest::quest.Find(id);

    if (qr) {
        lua_pushnumber(L, qr->status);
    } else {
        lua_pushnumber(L, Q_UNKNOWN);
    }

    return 1;
}


XFile* XLocation::svg_file = nullptr;
int XLocation::StoreInt(lua_State * L)
{
    int tx = lua_tonumber(L, 1);
    svg_file->Write(&tx);

    return 0;
}

int XLocation::RestoreInt(lua_State * L)
{
    int tx = 0;
    svg_file->Read(&tx);
    lua_pushnumber(L, tx);

    return 1;
}

int XLocation::StoreObject(lua_State * L)
{
    XObject * p = (XObject*)lua_topointer(L, 1);
    StorePointer(svg_file, p);

    return 0;

}

int XLocation::RestoreObject(lua_State * L)
{
    XObject * p = RestorePointer(svg_file, nullptr);
    lua_pushlightuserdata(L, p);

    return 1;
}

int XLocation::BinaryAND(lua_State * L)
{
    int v1 = lua_tonumber(L, 1);
    int v2 = lua_tonumber(L, 2);
    lua_pushboolean(L, (v1 & v2));

    return 1;
}

int XLocation::ExecuteAIScript(lua_State * L)
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

    return 0;
}

int XLocation::CreateMushroom(lua_State * L)
{
    XLocation * p = (XLocation*)lua_topointer(L, 1);
    XPoint pt;
    p->GetFreeXY(&pt);
    new XMushSpawn(pt.x, pt.y, p);

    return 0;
}

////// SCRIPT PORT ///////

int XLocation::CRVW(lua_State * L)
{
    CREATURE_NAME cn = (CREATURE_NAME)lua_tonumber(L, 1);
    const char* name = lua_tostring(L, 2);
    const char* view = lua_tostring(L, 3);
    int color = lua_tonumber(L, 4);
    CR_PERSON_TYPE crpt = (CR_PERSON_TYPE)lua_tonumber(L, 5);
    CREATURE_LEVEL crl = (CREATURE_LEVEL)lua_tonumber(L, 6);
    CREATURE_CLASS crc = (CREATURE_CLASS)lua_tonumber(L, 7);
    CREATURE_NAME cn_instance = CN_NONE;

    if (lua_gettop(L) == 8) {
        cn_instance = (CREATURE_NAME)lua_tonumber(L, 8);
    }

    XCreatureStorage::View(cn, name, view[0], color, crpt, crl, crc, cn_instance);
    return 0;
}

int XLocation::CRBA(lua_State * L)
{
    const char* speed = lua_tostring(L, 1);
    const char* energy = lua_tostring(L, 2);
    const char* combat_energy = lua_tostring(L, 3);
    CREATURE_SIZE crs = (CREATURE_SIZE)lua_tonumber(L, 4);
    const char* weight = lua_tostring(L, 5);
    XCreatureStorage::Basic(speed, energy, combat_energy, crs, weight);
    return 0;
}

int XLocation::CRBO(lua_State * L)
{
    const char* body = lua_tostring(L, 1);

    int prob = 0;
    int flg = 0;
    int n = lua_gettop(L);

    if (n > 1) {
        prob = lua_tonumber(L, 2);

        if (n > 2) {
            flg = lua_tonumber(L, 3);
        }
    }

    XCreatureStorage::Body(body, prob, flg);
    return 0;
}

int XLocation::CRA(lua_State * L)
{
    int flg = lua_tonumber(L, 1);
    XCreatureStorage::SetAI(flg);
    return 0;
}

int XLocation::CRS(lua_State * L)
{
    const char* s = lua_tostring(L, 1);
    XCreatureStorage::S(s);
    return 0;
}

int XLocation::CRR(lua_State * L)
{
    const char* r = lua_tostring(L, 1);
    XCreatureStorage::R(r);
    return 0;
}

int XLocation::CRM(lua_State * L)
{
    const char* dv = lua_tostring(L, 1);
    const char* pv = lua_tostring(L, 2);
    const char* hp = lua_tostring(L, 3);
    const char* pp = lua_tostring(L, 4);
    XCreatureStorage::Main(dv, pv, hp, pp);
    return 0;
}

int XLocation::CRD(lua_State * L)
{
    const char* descr = lua_tostring(L, 1);
    XCreatureStorage::D(descr);
    return 0;
}

int XLocation::CRC(lua_State * L)
{
    const char* hit = lua_tostring(L, 1);
    const char* dice = lua_tostring(L, 2);
    XCreatureStorage::Combat(hit, dice);
    return 0;
}

int XLocation::CRAT(lua_State * L)
{
    BRAND_TYPE brt = (BRAND_TYPE)lua_tonumber(L, 1);
    int prob = lua_tonumber(L, 2);
    XCreatureStorage::Melee(brt, prob);
    return 0;
}

int XLocation::CRAT2(lua_State * L)
{
    EXTENDED_ATTACK ea = (EXTENDED_ATTACK)lua_tonumber(L, 1);
    int prob = lua_tonumber(L, 2);
    XCreatureStorage::Melee(ea, prob);
    return 0;
}

int XLocation::CRL(lua_State * L)
{
    SPELL_NAME spn = (SPELL_NAME)lua_tonumber(L, 1);
    XCreatureStorage::Learn(spn);
    return 0;
}

int XLocation::CREQ(lua_State * L)
{
    int mask = lua_tonumber(L, 1);
    ITEM_TYPE it = (ITEM_TYPE)lua_tonumber(L, 2);
    int prob = lua_tonumber(L, 3);
    XCreatureStorage::Equip(mask, it, prob);
    return 0;
}

int XLocation::CREQ2(lua_State * L)
{
    int mask = lua_tonumber(L, 1);
    int count = lua_tonumber(L, 2);
    int prob = lua_tonumber(L, 3);
    XCreatureStorage::Equip(mask, count, prob);
    return 0;
}

int XLocation::CRCOE(lua_State * L)
{
    CORPSE_EFFECT_TYPE cet = (CORPSE_EFFECT_TYPE)lua_tonumber(L, 1);
    int val = lua_tonumber(L, 2);
    XCreatureStorage::CorpseEffects(cet, val);
    return 0;
}

int XLocation::CRCOD(lua_State * L)
{
    int roat_time = lua_tonumber(L, 1);
    FOOD_TYPE ft = (FOOD_TYPE)lua_tonumber(L, 2);
    XCreatureStorage::Corpse(roat_time, ft);
    return 0;
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

    lua_register(L, "CreateLocation", CreateLocation);
    lua_register(L, "Way", Way);
    lua_register(L, "Creature", Creature);
    lua_register(L, "Guardian", Guardian);
    lua_register(L, "Settle", Settle);
    lua_register(L, "CreateObject", CreateObject);
    lua_register(L, "DropItem", DropItem);
    lua_register(L, "SetPattern", SetPattern);
    lua_register(L, "AddTranslation", AddTranslation);
    lua_register(L, "DrawPattern", DrawPattern);
    lua_register(L, "BuildShop", BuildShop);
    lua_register(L, "Furniture", Furniture);
    lua_register(L, "OuterObject", OuterObject);
    lua_register(L, "Altar", Altar);
    lua_register(L, "Trap", Trap);
    lua_register(L, "Chest", Chest);
    lua_register(L, "Treasure", Treasure);
    lua_register(L, "EventPlace", EventPlace);
    lua_register(L, "isHero", isHero);
    lua_register(L, "isEnemy", isEnemy);
    lua_register(L, "FindCreature", FindCreature);

    lua_register(L, "AddMessage", AddMessage);
    lua_register(L, "AskQuestion", AskQuestion);

    lua_register(L, "SetItEnemyFor", SetItEnemyFor);
    lua_register(L, "SetEnemy", SetEnemy);
    lua_register(L, "ChangeStats", ChangeStats);
    lua_register(L, "GetStats", GetStats);
    lua_register(L, "InflictDamage", InflictDamage);
    lua_register(L, "Rand", Rand);
    lua_register(L, "SetEventHandler", SetEventHandler);
    lua_register(L, "CreateTimerEvent", CreateTimerEvent);

    lua_register(L, "GetSkill", GetSkill);
    lua_register(L, "LearnSkill", LearnSkill);
    lua_register(L, "MoneyOperation", MoneyOperation);

    lua_register(L, "SetName", SetName);
    lua_register(L, "SetView", SetView);
    lua_register(L, "GetView", GetView);

    lua_register(L, "GetObjectGUID", GetObjectGUID);
    lua_register(L, "GetItemParam", GetItemParam);
    lua_register(L, "SetItemBrand", SetItemBrand);
    lua_register(L, "GiveObjectToCreature", GiveObjectToCreature);
    lua_register(L, "GiveAward", GiveAward);

    lua_register(L, "MakeEffect", MakeEffect);
    lua_register(L, "DestroyObject", DestroyObject);
    lua_register(L, "SetCompanion", SetCompanion);

    lua_register(L, "Quest", Quest);
    lua_register(L, "QuestModify", QuestModify);
    lua_register(L, "QuestStatus", QuestStatus);
    lua_register(L, "Gender", Gender);

    lua_register(L, "StoreInt", StoreInt);
    lua_register(L, "RestoreInt", RestoreInt);
    lua_register(L, "StoreObject", StoreObject);
    lua_register(L, "RestoreObject", RestoreObject);
    lua_register(L, "BinaryAND", BinaryAND);

    lua_register(L, "ExecuteAIScript", ExecuteAIScript);
    lua_register(L, "CreateMushroom", CreateMushroom);

    lua_register(L, "CRVW", CRVW);
    lua_register(L, "CRBA", CRBA);
    lua_register(L, "CRBO", CRBO);
    lua_register(L, "CRA", CRA);
    lua_register(L, "CRS", CRS);
    lua_register(L, "CRR", CRR);
    lua_register(L, "CRM", CRM);
    lua_register(L, "CRD", CRD);
    lua_register(L, "CRC", CRC);
    lua_register(L, "CRAT", CRAT);
    lua_register(L, "CRAT2", CRAT2);
    lua_register(L, "CRL", CRL);
    lua_register(L, "CREQ", CREQ);
    lua_register(L, "CREQ2", CREQ2);
    lua_register(L, "CRCOE", CRCOE);
    lua_register(L, "CRCOD", CRCOD);

    luaopen_base(L);
    luaopen_string(L);

    luaL_dofile(L, "./world/init.lua");

    luaL_dostring(L, "LoadScripts()");
    XCreatureStorage::CreateQuickBase();
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
