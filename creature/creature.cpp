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

#include <algorithm>
#include <cmath>
#include <fmt/format.h>

#include <cereal/archives/json.hpp>
#include <cereal/types/polymorphic.hpp>

#include "creature/anycr.h"
#include "creature/creature.h"
#include "creature/los.h"
#include "creature/std_ai.h"
#include "creature/xhero.h"
#include "game/game.h"
#include "game/setting.h"
#include "helpers/msgwin.h"
#include "magic/modifier.h"
#include "map/map_objects.h"

#include <sol/sol.hpp>

// XCreature is never itself a dynamic type - every actual creature is
// a concrete subclass (XAnyCreature, XHero, the uniques), each with
// its own CEREAL_REGISTER_TYPE against XCreature - this just extends
// the polymorphic pointer-cast chain one more hop, up to XObject, for
// XScheduler::Entry's shared_ptr<XObject>.
CEREAL_REGISTER_POLYMORPHIC_RELATION(XBaseObject, XCreature);

// a creature which is currently being displayed
XCreature* XCreature::main_creature = nullptr;

XCreatureGroupMap XCreature::group_members = XCreatureGroupMap();

// Defined here rather than inline in creature.h: XHero derives from
// XCreature, so creature.h can never see a complete XHero to dynamic_cast
// against - same circular-include shape as XLocation::GroupId earlier.
bool XCreature::isHero() const
{
    return dynamic_cast<const XHero*>(this) != nullptr;
}

void XCreature::RegisterLua(sol::state_view& lua)
{
    lua.new_enum("Gender",
        "MALE", XCreature::MALE,
        "FEMALE", XCreature::FEMALE,
        "NEUTER", XCreature::NEUTER
    );

    lua.new_enum("PersonType",
        "IT", XCreature::IT,
        "HE", XCreature::HE,
        "SHE", XCreature::SHE,
        "NAMED_HE", XCreature::NAMED_HE,
        "NAMED_SHE", XCreature::NAMED_SHE,
        "NAMED_IT", XCreature::NAMED_IT
    );

    // Real C++ methods/properties, not one-off void*-taking free functions -
    // reachable from Lua via AsCreature(void*) on any existing void* handle
    // (event_handler dispatch, FindCreature, etc.), which still pass plain
    // void* under the hood (verified: a usertype-wrapped pointer can't be
    // read back correctly by a void*-parameter function, so the existing
    // void* dispatch plumbing had to stay as-is rather than switch to
    // passing real XCreature* to it).
    lua.new_usertype<XCreature>("XCreature",
        "MoneyOp", &XCreature::MoneyOp,
        "IsCreatureVisible", [](XCreature& cr, XCreature* target) { return cr.isCreatureVisible(target) != 0; },
        "isHero", &XCreature::isHero,
        "name", &XMapObject::name,
        "ContainItem", &XCreature::ContainItem,
        "IsMale", [](XCreature& cr) { return static_cast<bool>(cr.creature_person_type & XCreature::HE); },
        "xai", sol::property([](XCreature& cr) -> XStandardAI* { return cr.xai.get(); }),
        "religion", &XCreature::religion,
        "IsWearingItemType", [](XCreature& cr, int bodypart, int slot, ItemType it) {
            XBodyPart* bp = cr.GetBodyPart((BODY_PART)bodypart, slot);
            return bp && bp->Item() && bp->Item()->it == it;
        },
        // Wear() is a no-op (returns without swapping) if the slot is
        // already occupied - clear it first, same as XBandit's old
        // cloak-swap ctor did before this became a generic Lua primitive.
        //
        // Named PutOnBody, not the more obvious WearItem/EquipItem/
        // SetWornItem: empirically, each of those three specific strings,
        // bound here as this exact method (same signature, same or even
        // empty body), reproducibly corrupts something elsewhere in the
        // Lua state - AsCreature(x).xai:someMethod() on an unrelated
        // creature starts throwing "attempt to index field 'xai' (a
        // userdata value)" on every subsequent run. Confirmed it's the
        // string, not the logic: an identical-signature method with a
        // nonsense name ("FooBarBaz") and the exact same body is 100%
        // stable across repeated fresh runs, and swapping only the name
        // back to any of the three above reintroduces the corruption
        // deterministically. Root cause not identified (smells like a
        // hash collision in sol2 or LuaJIT's own string interning, since
        // it reproduces independent of parameter types/count/body content
        // and depends only on the bound name string) - if this resurfaces
        // when renaming/adding usertype methods elsewhere, suspect this
        // class of bug before assuming a logic error, and verify any
        // fix (or any new method name) with several repeated fresh runs,
        // not just one - this does not reproduce on every single run of
        // a build that has NOT changed, only across genuinely different
        // registered-string sets.
        "PutOnBody", [](XCreature& cr, int bodypart, int slot, void* item_ptr) {
            XItem* item = (XItem*)item_ptr;
            XBodyPart* bp = cr.GetBodyPart((BODY_PART)bodypart, slot);

            if (bp->Item()) {
                auto old_item = bp->UnWear();

                if (auto it = cr.contain.find(old_item); it != cr.contain.end()) {
                    cr.contain.erase(it);
                }

                old_item->Invalidate();
            }

            bp->Wear(item);
        }
    );
}

XCreature::XCreature()
{
    total_cr++;

    added_DV = 0;
    added_PV = 0;
    added_HIT = 0;
    added_DMG = 0;
    added_RNG = 0;
    added_HP = 0;
    added_PP = 0;
    carried_weight = 0;

    base_nutrio = 500;
    nutrio = 5000;
    nutrio_speed = 10;

    _EXP = 0;
    level = 1;
    RNG = 3;

    creature_size = CS_NORMAL;
    creature_person_type = XCreature::HE;

    xai = std::make_unique<XStandardAI>(this);
    md = new XModifier();
    m = new XMagic();
    sk = new XSkills();
    wsk = new XWarSkills();

    weight = 1000;

    creature_class = CreatureClass::NONE;

    tactics = TS_NORMAL;
    group_id = GID_NONE;
    food_feeling = FF_NORMAL;
    event_handler = nullptr;
}

void XCreature::Invalidate()
{
    components.clear();

    for (auto item: contain) {
        item->Invalidate();
    }

    if (action_data.item) {
        action_data.item->Invalidate();
        action_data.item = nullptr;
    }

    delete sk;
    sk = nullptr;

    if (md) {
        delete md;
        md = nullptr;
    }

    delete m;
    m = nullptr;

    delete wsk;
    wsk = nullptr;

    delete xai.release();

    if (event_handler) {
        delete[] event_handler;
        event_handler = nullptr;
    }

    // remove perished creature from the group members list
    if (group_id != GID_NONE) {
        auto group = group_members.equal_range(group_id);

        for (auto el = group.first; el != group.second;) {
            if (el->second == this) {
                el = group_members.erase(el);
            } else {
                ++el;
            }
        }
    }

    total_cr--;

    XBaseObject::Invalidate();
}

void XCreature::Regenerate()
{
    if (_HP < GetMaxHP()) {
        XSkill * xsk = sk->GetSkill(XSkill::Skill::HEALING);
        int val = 1;

        if (xsk) {
            val += xsk->GetLevel();
        }

        if (vRand(20) < val) {
            int rest = MAX_HP / 100 + 1;

            if (xsk) {
                xsk->UseSkill();
                rest *= vRand((int)xsk->GetMastery() + 1);
            }

            onHeal(rest);
        }
    }

    if (_PP < GetMaxPP()) {
        XSkill * xsk = sk->GetSkill(XSkill::Skill::CONCENTRATION);
        int val = 1;

        if (xsk) {
            val += xsk->GetLevel();
        }

        if (vRand() % 20 < val) {
            int rest = MAX_PP / 100 + 1;

            if (xsk) {
                xsk->UseSkill();
                rest *= (int)xsk->GetMastery();
            }

            onRestorePP(rest);
        }
    }
}

int XCreature::onHeal(int _hp)
{
    int last_HP = _HP;
    int max_HP = GetMaxHP();
    _HP += _hp;

    if (_HP > max_HP) {
        _HP = max_HP;
    }

    return last_HP >= max_HP ? 0 : 1;
}

int XCreature::onRestorePP(int _pp)
{
    int last_PP = _PP;
    int max_PP = GetMaxPP();
    _PP += _pp;

    if (_PP > max_PP) {
        _PP = max_PP;
    }

    return last_PP >= max_PP ? 0 : 1;
}

void XCreature::setGroupID(const GROUP_ID& gid)
{
    if (gid == GID_NONE) return;

    group_id = gid;
    group_members.insert(std::make_pair(gid, this));
}

std::vector<XCreature*> XCreature::getGroupMembers() const
{
    std::vector<XCreature*> result{};

    if (group_id != GID_NONE) {
        auto [begin, end] = group_members.equal_range(group_id);
        for (auto it = begin; it != end; ++it) {
            result.push_back(it->second);
        }
    }

    return result;
}

int XCreature::stopAction()
{
    if (action_data.action == A_USE_TOOL) {
        dynamic_cast<XTool *>(action_data.item.get())->onUse(XTool::FINISH, this);
    } else {
        if (action_data.item) {
            contain.insert(action_data.item);
        }
    }

    action_data.action = A_MOVE;
    action_data.item = nullptr;
    isDisturb = 0; //prevents hero to continue automove when attaked by ghosts...
    return 1;
}

int XCreature::continueEat()
{
    XAnyFood * food = (XAnyFood*)action_data.item.get();
    assert(food->kind & ItemKind::FOOD);
    int res = food->onEat(this);

    if (res != 2) {
        action_data.action = A_MOVE;
        action_data.item = nullptr;
    }

    return 1;
}

int XCreature::Eat(XAnyFood * food)
{
    int res = food->onEat(this);

    if (res) {
        if (res == 2) {
            action_data.action = A_EAT;
            action_data.item = XItem::Own(food);
            return 1;
        } else {
            return 1;
        }
    } else {
        return 0;
    }
}

int XCreature::DecNutrio()
{
    nutrio -= nutrio_speed;

    if (nutrio < 0) {
        Die(nullptr);
        return 0;
    }

    return 1;
}

bool XCreature::Run()
{
    if (action_data.action == A_EAT) {
        continueEat();
    } else if (action_data.action == A_READ) {
        continueRead();
    } else if (action_data.action == A_USE_TOOL) {
        continueUseItem();
    } else {
        NewMove();
    }

    // safety net: when killed by the actions above indicate the object
    // must be removed from the scheduler and deleted.
    if (!isValid())
        return false;

    if (md && md->Run(this)) {
        int athletics = sk->GetLevel(XSkill::Skill::ATHLETICS);

        if (GetCarryState() >= CSTATE_STRAINED) {
            if (vRand(3000 / (5 + athletics)) == 0) {
                GainAttr(XStats::STR, 1);
                sk->UseSkill(XSkill::Skill::ATHLETICS, 10);
            }
        } else if (vRand(6000 / (5 + athletics)) == 0) {
            GainAttr(XStats::DEX, 1);
            sk->UseSkill(XSkill::Skill::ATHLETICS, 10);
        }

        DoMove();
    }

    if (ttm <= 0) {
        ttm += GetSpeed();
    }

    return isValid();
}

void XCreature::DoMove()
{
    if (l->map->XGetMovability(nx, ny) == 2 && (nx != x || ny != y)) {
        Attack();
    } else if (TestMove() || (x == nx && y == ny)) {
        Move();
    }
}

void XCreature::Move()
{
    // Demo/attract mode ("-demo") has no real hero - whichever creature is
    // flagged main_creature (see SetMainCreature(), Lua-callable) stands in
    // for one, so the same view-refresh sequence XHero drives itself needs
    // to run here too. Was XBeelzvile-specific (creature/unique.cpp) before
    // being generalized to any main_creature, since it's demo-mode
    // plumbing, not creature-specific behavior.
    if (XSettings::isDemo && this == main_creature) {
        HideOldView();
        ShowNewView();
    }

    if (wants_move_hook && event_handler) {
        sol::state_view lua(XLocation::L);
        lua[event_handler](LuaEvent::PRE_MOVE, (void*)this);
    }

    XMapObject * tobj = l->map->GetSpecial(x, y);

    if (auto* ttrap = dynamic_cast<XTrap *>(tobj)) {
        //we can move easy from pit or web
        if ((nx != x || ny != y) && !ttrap->MoveOut(this)) {
            nx = x;
            ny = y;

            if (!isValid()) {
                return;
            }
        }
    }

    XAnyPlace * new_place = l->map->GetPlace(nx, ny);
    XAnyPlace * old_place = l->map->GetPlace(x, y);

    if (old_place && new_place) {
        new_place->onCreatureMove(this);
    } else if (new_place && !old_place) {
        new_place->onCreatureEnter(this);
    } else if (old_place && !new_place) {
        old_place->onCreatureLeave(this);
    }

    //check if die when moved.
    if (!isValid()) {
        return;
    }

    Regenerate();
    l->map->ResMonster(x, y);
    l->map->SetMonster(nx, ny, this);

    int flag = 1;

    if (x == nx && y == ny) {
        flag = 0;
    }

    x = nx;
    y = ny;

    if (flag) {
        XMapObject * obj = l->map->GetSpecial(x, y);

        if (auto* mtrap = dynamic_cast<XTrap *>(obj)) {
            mtrap->MoveIn(this);
        } else if (auto* mtel = dynamic_cast<XTeleport *>(obj)) {
            mtel->MoveIn(this);
        }
    }
}

//////////////////////////////////////////////////////////////////////////////

struct opaque_info {
    XCreature* mover;
    XMap* map;
};

static int is_grid_viewable(void* opaque, const int x, const int y)
{
    const auto info = static_cast<opaque_info *>(opaque);

    if (x < 0 || x >= info->map->len) return false;
    if (y < 0 || y >= info->map->hgt) return false;

    XCreature* tcr = info->mover->l->map->GetMonster(x, y);

    if (tcr
        && tcr != info->mover
        && info->mover->isCreatureVisible(tcr)
        && tcr->xai->isEnemy(info->mover))
    {
        info->mover->isDisturb = 0;
    }

    return (info->mover->l->map->GetVisibility(x, y) != 0);
}

static int set_grid_visible(void* opaque, int x, int y, int radius, int see_center)
{
    const auto info = static_cast<opaque_info *>(opaque);

    if (x < 0 || x >= info->map->len) return false;
    if (y < 0 || y >= info->map->hgt) return false;

    if (!see_center && (info->map->GetVisibility(x, y) != 0)) {
        return is_grid_viewable(opaque, x, y);
    }

    info->map->SetVisible(x, y);

    return is_grid_viewable(opaque, x, y);
}

static int set_grid_invisible(void* opaque, int x, int y, int radius, int see_center)
{
    const auto info = static_cast<opaque_info *>(opaque);

    if (x < 0 || x >= info->map->len) return false;
    if (y < 0 || y >= info->map->hgt) return false;

    info->map->ResVisible(x, y);

    return is_grid_viewable(opaque, x, y);
}

void XCreature::HideOldView()
{
    opaque_info info = { this, l->map };
    LineOfSight(
        x,
        y,
        GetVisibleRadius(),
        &info,
        set_grid_invisible);
}

void XCreature::ShowNewView()
{
    opaque_info info = { this, l->map };
    LineOfSight(
        nx,
        ny,
        GetVisibleRadius(),
        &info,
        set_grid_visible);
}

void XCreature::PutStatus()
{
    vGotoXY(0, size_y - 3);
    vSetAttr(xLIGHTGRAY);
    vPutS(name);

    vGotoXY(0, size_y - 2);
    vPutS(fmt::format("DV/PV:{}/{}  ", GetDV(), GetPV()));

    vGotoXY(15, size_y - 3);
    vPutS(fmt::format(
        "St:{:<2} Dx:{:<2} To:{:<2} Le:{:<2} Wi:{:<2} Ma:{:<2} Pe:{:<2} Ch:{:<2} Sp:{:<3} L:{}",
        GetStats(XStats::STR),
        GetStats(XStats::DEX),
        GetStats(XStats::TOU),
        GetStats(XStats::LEN),
        GetStats(XStats::WIL),
        GetStats(XStats::MAN),
        GetStats(XStats::PER),
        GetStats(XStats::CHR),
        100000 / GetSpeed(),
        l->GetBriefName()));
    vClrEol();

    vGotoXY(14, size_y - 2);
    vPutS(fmt::format("HP:{}({})  PP:{}({})  ", _HP, GetMaxHP(), _PP, GetMaxPP()));

    vGotoXY(38, size_y - 2);
    vPutS(fmt::format("Exp({}){}", level, _EXP));

#ifdef _DEBUG
    vGotoXY(60, size_y - 2);
    vPutS(fmt::format("x:y[{}:{}]", x, y));
#endif

    vGotoXY(0, size_y - 1);

    if (nutrio > base_nutrio * 18) {
        vPutS(MSG_RED "overfed! " MSG_LIGHTGRAY);
    } else if (nutrio > base_nutrio * 14) {
        vPutS("bloated ");
    } else if (nutrio > base_nutrio * 10 && nutrio <= base_nutrio * 14) {
        vPutS("satiated ");
    } else if (nutrio > base_nutrio * 8 && nutrio <= base_nutrio * 10) {
        vPutS("");
    } else if (nutrio > base_nutrio * 6 && nutrio <= base_nutrio * 8) {
        vPutS("hungry ");
    } else if (nutrio > base_nutrio * 4 && nutrio <= base_nutrio * 6) {
        vPutS(MSG_YELLOW "very hungry " MSG_LIGHTGRAY);

        if (action_data.action != A_EAT) {
            stopAction();
        }
    } else if (nutrio > base_nutrio && nutrio <= base_nutrio * 4) {
        vPutS( MSG_RED "weak " MSG_LIGHTGRAY);

        if (action_data.action != A_EAT) {
            stopAction();
        }
    } else if (nutrio <= base_nutrio) {
        vPutS(MSG_RED "dying! " MSG_LIGHTGRAY);

        if (action_data.action != A_EAT) {
            stopAction();
        }
    }

    switch (action_data.action) {
        case A_READ	:
            vPutS("[reading] ");
            break;

        case A_EAT	:
            vPutS("[eating] ");
            break;

        case A_USE_TOOL	:
            vPutS("[using tool] ");
            break;
    }

    CARRY_STATE cstate = GetCarryState();

    switch (cstate) {
        case CSTATE_NORMAL:
            break;

        case CSTATE_BURDENED:
            vPutS("burdened ");
            break;

        case CSTATE_STRAINED:
            vPutS("strained ");
            break;

        case CSTATE_OVERBURDEN:
            vPutS("overburdened ");
            break;

        default :
            break;
    };

    vPutS(md->toString());
    vClrEol();
    vSetAttr(xLIGHTGRAY);
}

void XCreature::NewMove()
{
    // See the matching comment in Move() - same demo-mode main_creature
    // stand-in, same generalized-from-XBeelzvile reasoning.
    if (XSettings::isDemo && this == main_creature) {
        l->map->Center(x, y);
        l->map->Put(this);
        PutStatus();
        vRefresh();
        msgwin.ClrMsg();
    }

    if (wants_move_hook && event_handler) {
        sol::state_view lua(XLocation::L);
        lua[event_handler](LuaEvent::AI_TURN, (void*)this);
    }

    xai->Move();
}

int XCreature::GetSpeed()
{
    int speed = ttmb;

    if (nutrio < base_nutrio * 8 && nutrio > base_nutrio * 4) {
        speed = (int)(speed * 0.92);
    } else if (nutrio < base_nutrio * 2) {
        speed = (int)(speed * 1.2);
    } else if (nutrio > base_nutrio * 12) {
        speed = (int)(speed * 1.1);
    }

    int str = stats->Get(XStats::STR);

    if (carried_weight >= str * 120 && carried_weight < str * 200) {
        speed = (int)(speed * 1.1);
    } else if (carried_weight >= str * 200 && carried_weight < str * 280) {
        speed = (int)(speed * 1.3);
    } else if (carried_weight >= str * 280) {
        speed = (int)(speed * 2);
    }

    return speed;
}

int XCreature::TestMove()
{
    if (l->map->XGetMovability(nx, ny) == 0) {
        return 1;
    } else {
        return 0;
    }
}

int XCreature::GetHIT()
{
    int tht = _HIT + added_HIT;
    return tht + GetTacticsHITBonus();
}

int XCreature::GetDV(XCreature * attacker)
{
    int tdv = added_DV + _DV + GetTacticsDVBonus() + GetShieldDVBonus();
    return tdv < 1 ? 1 : tdv;
}

int XCreature::GetShieldDVBonus()
{
    for (auto& xbp: components)
    {
        XItem* i = xbp->Item();

        if (i && i->kind == ItemKind::SHIELD) {
            int shld_skl = wsk->GetDV(XWarSkills::SHIELD);
            int shield_dv = i->_DV;

            if (i->_DV < shld_skl) {
                return i->_DV + shield_dv;
            } else {
                return shld_skl + shield_dv;
            }
        }
    }

    return 0;
}

int XCreature::GetDMG()
{
    //this function don't include 'hand' damage
    //i.e. calculate additional damage. i.e. +dmg
    return added_DMG + GetTacticsDMGBonus();
}

int XCreature::GetTacticsDVBonus()
{
    switch (tactics) {
        case TS_COWARD	:
            return (3 * (GetStats(XStats::DEX) + sk->GetLevel(XSkill::Skill::TACTICS))) / 2;
            break;

        case TS_DEFENSIVE	:
            return GetStats(XStats::DEX) + sk->GetLevel(XSkill::Skill::TACTICS);
            break;

        case TS_NORMAL	:
            return (2 * GetStats(XStats::DEX)) / 3 + sk->GetLevel(XSkill::Skill::TACTICS);
            break;

        case TS_AGGRESSIVE	:
            return GetStats(XStats::DEX) / 3 + sk->GetLevel(XSkill::Skill::TACTICS);
            break;

        case TS_BERSERKER	:
            return GetStats(XStats::DEX) / 10 + sk->GetLevel(XSkill::Skill::TACTICS);
            break; //compensate DV given by Dx

        default	:
            assert(0);
    }

    return 0;
}

int XCreature::GetTacticsHITBonus()
{
    switch (tactics) {
        case TS_COWARD	:
            return GetStats(XStats::DEX) / 10 + sk->GetLevel(XSkill::Skill::TACTICS) - 5;
            break;

        case TS_DEFENSIVE	:
            return GetStats(XStats::DEX) / 7 + sk->GetLevel(XSkill::Skill::TACTICS) - 3;
            break;

        case TS_NORMAL	:
            return GetStats(XStats::DEX) / 4 + sk->GetLevel(XSkill::Skill::TACTICS);
            break;

        case TS_AGGRESSIVE	:
            return GetStats(XStats::DEX) / 3 + sk->GetLevel(XSkill::Skill::TACTICS);
            break;

        case TS_BERSERKER	:
            return GetStats(XStats::DEX) / 2 + sk->GetLevel(XSkill::Skill::TACTICS);
            break;

        default	:
            assert(0);
    }

    return 0;
}

int XCreature::GetTacticsDMGBonus()
{
    switch (tactics) {
        case TS_COWARD	:
            return GetStats(XStats::STR) / 20 + sk->GetLevel(XSkill::Skill::TACTICS) - 3;
            break;

        case TS_DEFENSIVE	:
            return GetStats(XStats::STR) / 10 + sk->GetLevel(XSkill::Skill::TACTICS) - 1;
            break;

        case TS_NORMAL	:
            return GetStats(XStats::STR) / 7 + sk->GetLevel(XSkill::Skill::TACTICS);
            break;

        case TS_AGGRESSIVE	:
            return GetStats(XStats::STR) / 5 + sk->GetLevel(XSkill::Skill::TACTICS);
            break;

        case TS_BERSERKER	:
            return GetStats(XStats::STR) / 2 + sk->GetLevel(XSkill::Skill::TACTICS);
            break;

        default	:
            assert(0);
    }

    return 0;
}

int XCreature::GetPV()
{
    return _PV + added_PV + GetStats(XStats::TOU) / 10;
}

int XCreature::GainAttr(XStats::Id st, int val)
{
    int cur = stats->Get(st);
    int max = max_stats.Get(st);

    if (val > 0) {
        if (cur < max) {
            if (cur + val > max) {
                val = max - cur;
            }

            stats->Modify(st, val);

            if (isHero()) {
                switch (st) {
                    case XStats::STR:
                        msgwin.Add("You feel stronger!");
                        break;

                    case XStats::DEX:
                        msgwin.Add("You are becoming more graceful!");
                        break;

                    case XStats::TOU:
                        msgwin.Add("Your health increases!");
                        break;

                    case XStats::MAN:
                        msgwin.Add("You feel power surging through your body!");
                        break;

                    case XStats::WIL:
                        msgwin.Add("You feel more powerfull!");
                        break;

                    case XStats::LEN:
                        msgwin.Add("You feel smarter!");
                        break;

                    case XStats::PER:
                        msgwin.Add("You feel more perceptive!");
                        break;

                    case XStats::CHR:
                        msgwin.Add("Your beauty improves!");
                        break;
                }
            }

            return 1;
        }
    } else {
        if (cur - val > 1) {
            stats->Modify(st, val);

            if (isHero()) {
                switch (st) {
                    case XStats::STR:
                        msgwin.Add("Your muscles weaken!");
                        break;

                    case XStats::DEX:
                        msgwin.Add("You feel clumsy!");
                        break;

                    case XStats::TOU:
                        msgwin.Add("You feel like you might be getting sick!");
                        break;

                    case XStats::MAN:
                        msgwin.Add("You feel power draining from your body!");
                        break;

                    case XStats::WIL:
                        msgwin.Add("You feel diminished!");
                        break;

                    case XStats::LEN:
                        msgwin.Add("Thinking becomes more difficult!");
                        break;

                    case XStats::PER:
                        msgwin.Add("Your senses dull!");
                        break;

                    case XStats::CHR:
                        msgwin.Add("Your features harden!");
                        break;
                }
            }

            return 1;
        }
    }

    return 0;
}

int XCreature::GainResist(XResistance::Id rs, int val)
{
    resistances->ChangeResistance(rs, val);

    if (val > 0) {
        switch (rs) {
            case XResistance::FIRE:
                msgwin.Add("Your blood cools down!");
                break;

            case XResistance::COLD:
                msgwin.Add("Your skin grows warm!");
                break;

            case XResistance::ACID:
                msgwin.Add("Your stomach settles!");
                break;

            case XResistance::POISON:
                msgwin.Add("Your flesh tingles!");
                break;

            case XResistance::PARALYSE:
                msgwin.Add("Your movements grow stronger!");
                break;
        }
    } else {
        switch (rs) {
            case XResistance::FIRE:
                msgwin.Add("Your blood warms up!");
                break;

            case XResistance::COLD:
                msgwin.Add("Your skin grows cold!");
                break;

            case XResistance::ACID:
                msgwin.Add("You feel a pain in your stomach!");
                break;

            case XResistance::POISON:
                msgwin.Add("You feel vulnerable!");
                break;

            case XResistance::PARALYSE:
                msgwin.Add("Your movements are unsure!");
                break;
        }
    }

    return 1;
}

int XCreature::GetStats(XStats::Id st)
{
    assert(st > XStats::UNKNOWN && st < XStats::COUNT);

    int res = stats->Get(st) + added_stats.Get(st);
    return res > 0 ? res : 1;
}

int XCreature::GetResistance(XResistance::Id tr)
{
    assert(resistances);
    return resistances->GetResistance(tr) + added_resists.GetResistance(tr);

}

void XCreature::Die(XCreature* killer)
{
    assert(isValid());

    // LastStep() below drops the map cell's shared_ptr reference (pMonster),
    // which may be the only thing keeping this object alive. Keep ourselves
    // alive for the rest of this function regardless.
    auto self = shared_from_this();

    if (event_handler) {
        // this/killer stay void*, not XCreature* -  killer can legitimately be
        // nullptr (e.g. DecNutrio()'s Die(nullptr))
        sol::state_view lua(XLocation::L);
        lua[event_handler](LuaEvent::DIE, (void*)this, (void*)killer);
    }

    // Unwear everything first, firing onUnWear() side effects - the items
    // themselves stay in contain regardless (see XBodyPart::Wear()), so
    // the single drop loop below picks up worn and carried items alike.
    for (auto& bp: components) {
        if (bp->Item()) {
            bp->UnWear();
        }
    }

    for (auto item: contain) {
        // Never drop an already-invalid item as loot. XItem::Invalidate()
        // does not remove an item from a creature's contain (only from a
        // ground list), so an item invalidated while still carried stays
        // here - and putting one on the ground leaves a corpse-of-an-item
        // in a cell's item_list that can never clean itself up again.
        // ~XMapTile now tolerates that regardless, but there is no reason
        // to manufacture it: contain.clear() below releases such items.
        if (item->isValid()) {
            item->Drop(l, x, y);
        }
    }

    // Drop() moves each item onto the ground but leaves it in contain -
    // without clearing it here, Invalidate() below would walk contain again
    // and delete every item just dropped, instead of leaving it as loot.
    contain.clear();

    LastStep();

    if (killer && killer != this && !dynamic_cast<XFakeCreature*>(killer)) {
        xai->onDie(killer);
        killer->religion.KillCreature(killer, this);
        killer->AddExp(GetExp());
    }

    Invalidate();
}

int XCreature::DropItem(XItem * i)
{
    XAnyPlace * place = l->map->GetPlace(x, y);
    int flag = 1;

    if (place) {
        flag = place->onCreatureDropItem(this, i);
    }

    if (flag) {
        // Adjust weight
        UnCarryItem(i);

        // Drop() (via XMap::PutItem -> XItem::Own()) establishes the
        // ground's item_list as a second owner of i before we let go of
        // contain's reference below - if contain was i's only reference,
        // erasing it first (before the item is anywhere else) would run
        // Own()'s deleter on a still-valid item and invalidate it outright
        // instead of just moving it to the ground.
        i->Drop(l, x, y);

        if (auto it = contain.find(i); it != contain.end()) {
            contain.erase(it);
        }
    }

    return flag;
}

int XCreature::PickUpItem(XItem * i)
{
    XAnyPlace * place = l->map->GetPlace(x, y);
    int flag = 1;

    if (place) {
        flag = place->onCreaturePickItem(this, i);
    }

    if (flag) {
        if (CarryItem(i)) {
            i->x = -1;
            i->y = -1;

            // If picked item is a missile the creature is shooting with, add
            // it to quiver instead of backpack
            XBodyPart * xbp = GetBodyPart(BP_MISSILE);

            if (xbp && xbp->Item() && xbp->Item()->Compare(i) == 0) {
                xbp->Item()->Concat(i);
            } else {
                contain.insert(XItem::Own(i));
            }

            return 1;
        } else { //if we can't pick item, then drop it
            if (isHero()) {
                msgwin.ClrMsg();
                msgwin.Add(fmt::format("{} is to heavy for you!", i->toString()));
            }

            int tx = i->x;
            int ty = i->y;
            i->x = -1;
            i->y = -1;

            if (place) {
                place->onCreatureDropItem(this, i);
            }

            i->x = tx;
            i->y = ty;

            return 0;
        }
    } else {
        return 0;
    }
}

XCreature::Gender XCreature::GetGender()
{
    switch (creature_person_type) {
        case XCreature::HE:
        case XCreature::NAMED_HE:
        case XCreature::MALE_YOU:
            return XCreature::MALE;
            break;

        case XCreature::SHE:
        case XCreature::NAMED_SHE:
        case XCreature::FEMALE_YOU:
            return XCreature::FEMALE;
            break;

        default:
            break;
    }

    return XCreature::NEUTER;
}

int XCreature::GetMaxHP()
{
    return MAX_HP + (MAX_HP * GetStats(XStats::TOU)) / 20;
}

int XCreature::GetMaxPP()
{
    return MAX_PP + (MAX_PP * GetStats(XStats::MAN)) / 10;
}

int XCreature::GetExp() const
{
    return base_exp + _EXP / 10;
}

void XCreature::AddExp(unsigned long exp)
{
    _EXP += exp;

    while (ExpOfLevel(level) <= _EXP) {
        IncLevel();
    }
}

void XCreature::IncLevel()
{
    MAX_HP += vRand((GetStats(XStats::TOU) / 5) + 1) + 1;
    MAX_PP += vRand((GetStats(XStats::MAN) / 2) + 1) + 1;
    level++;
}

std::weak_ptr<XCreature> XCreature::ToWeakPtr(XCreature * cr)
{
    // A creature isn't shared_from_this()-safe until it's been placed on
    // the map for the first time (see XMap::SetMonster's birth path) -
    // notably, that's after its own constructor has already returned, so
    // this can be reached for a creature referencing itself mid-construction
    // (e.g. XAnyCreature equipping its own starting gear). Guard against
    // that rather than letting shared_from_this() throw std::bad_weak_ptr.
    if (cr && cr->isValid() && !cr->weak_from_this().expired()) {
        return std::static_pointer_cast<XCreature>(cr->shared_from_this());
    }

    return {};
}

unsigned long XCreature::ExpOfLevel(const int lev) const
{
    return static_cast<unsigned long>(2.0 * base_exp * std::pow(static_cast<float>(lev), 2.5f));
}

int XCreature::GetHITFHBonus(XItem* weapon)
{
    XItem * h1 = GetItem(BP_HAND, 0);
    XItem * h2 = GetItem(BP_HAND, 1);
    int mult = (h1 && h2) ? 2 : 1;
    float f = (float)(5.0 * log((300.0 * GetStats(XStats::STR)) / (10.0 * (weapon->weight) * mult)));
    return std::min((int)f, 0);
}

int XCreature::GetDMGFHBonus(XItem* weapon)
{
    return 0;
}

XBodyPart* XCreature::GetRNDBodyPart()
{
    int value = 0;
    for (auto& bp: components) {
        value += bp->GetPartSize();
    }

    int v = value > 0 ? vRand() % value : 0;

    for (auto& bp: components) {
        v -= bp->GetPartSize();

        if (v <= 0)
            return bp.get();
    }

    return nullptr;
}

XBodyPart* XCreature::GetRNDBodyPart(ItemKind kind, RBP_FLAG rbpf)
{
    if (rbpf == RBP_BLOCK && kind & ItemKind::SHIELD) {
        auto bpi = std::find_if(
            components.begin(),
            components.end(),
            [](const std::unique_ptr<XBodyPart>& xbp) { return xbp->Item() && xbp->Item()->kind & ItemKind::SHIELD; }
        );

        if (bpi != components.end() && (vRand() % 100 < 5 * wsk->GetLevel(XWarSkills::SHIELD) + 5)) {
            return bpi->get();
        }
    }

    int count = 0;
    for (auto& xbp: components) {
        if (xbp->GetProperKind() & kind) {
            count++;
        }
    }

    if (count == 0) {
        return nullptr;
    }

    int n = vRand() % count;

    count = 0;
    for (const auto& xbp: components) {
        if (xbp->GetProperKind() & kind) {
            if (n == count) {
                return xbp.get();
            }
            count++;
        }
    }

    assert(0);
    return nullptr;
}

const char* XCreature::GetWoundMsg(int flag)
{
    float rel = (float)(GetMaxHP()) / ((float)_HP);

    if (rel <= 1.0) {
        if (flag) {
            return "";
        } else {
            return "not wounded";
        }
    } else if (rel < 1.3) {
        if (flag) {
            return "slightly wounds";
        } else {
            return "slightly wounded";
        }
    } else if (rel < 2.0) {
        if (flag) {
            return "wounds";
        } else {
            return "wounded";
        }
    } else if (rel < 3.0) {
        if (flag) {
            return "seriously wounds";
        } else {
            return "seriously wounded";
        }
    } else {
        if (flag) {
            return "critically wounds";
        } else {
            return "critically wounded";
        }
    }
}

void XCreature::MoveStairWay()
{
    XCreature * tc = this;
    XLocation * xl = l;

    XMapObject * spec = xl->map->GetSpecial(tc->x, tc->y);
    XStairWay * way = dynamic_cast<XStairWay *>(spec);

    if (way) {
        XLocation * tgtloc = Game.locations[way->ln].get();
        int tgt_x = spec->nx;
        int tgt_y = spec->ny;
        int n_x = tgt_x;
        int n_y = tgt_y;

        if (tgtloc->map->XGetMovability(tgt_x, tgt_y) != 0) {
            for (int i = -1; i < 2; i++)
                for (int j = -1; j < 2; j++)
                    if (tgtloc->map->XGetMovability(tgt_x + i, tgt_y + j) == 0) {
                        n_x = i + tgt_x;
                        n_y = j + tgt_y;
                    }
        }

        if (!tgtloc->map->GetMonster(n_x, n_y)) {
            // tc is ordinarily the creature whose own turn is executing
            // (protected by the scheduler's own strong ref for the
            // duration of Run()), same as every other LastStep()-then-
            // FirstStep() call site - but unlike those, this one is also
            // reachable from XStandardAI::MoveTo()'s cross-location
            // branch for a *companion* crossing a stairway to follow its
            // leader.
            auto tc_keepalive = std::static_pointer_cast<XCreature>(tc->shared_from_this());
            tc->LastStep();
            tc->FirstStep(n_x, n_y, tgtloc);
            tc->l = tgtloc;
            tc->action_data.action = A_MOVE;

            if (tc->isHero()) {
                tgtloc->visited_by_hero = 1;
                tgtloc->map->Put(tc);
                vRefresh();
            }

            return;
        } else if (tc->isHero()) {
            msgwin.Add("The way is blocked.");
        }
    }

    return;
}

void XCreature::GetRangeAttackInfo(int* range, int* hit, XDice * dmg)
{
    XItem * missile = GetItem(BP_MISSILE);
    XItem * bow = GetItem(BP_MISSILE_WEAPON);

    if (!missile || !XMissile::isProperWeapon(missile, bow)) {
        *range = 0;
        *hit = 0;
        dmg->Setup(0, 0, 0);
        return;
    }

    XSkill * skill = sk->GetSkill(XSkill::Skill::ARCHERY);

    int str = stats->Get(XStats::STR);
    int dex = stats->Get(XStats::DEX);

    *range = missile->RNG;
    *hit = dex / 2 + missile->_HIT;
    dmg->Setup(missile->dice);

    if (bow) {
        *range += bow->RNG;
        dmg->Add(&(bow->dice));
        *range += wsk->GetDV(bow->wt);
        dmg->ModifyBonus(wsk->GetDMG(bow->wt));
        *hit += wsk->GetHIT(bow->wt);
    } else {
        *range += RNG + str / 25;
        dmg->ModifyBonus(str / 10);
        *range += wsk->GetDV(XWarSkills::THROW);
        dmg->ModifyBonus(wsk->GetDMG(XWarSkills::THROW));
        *hit += wsk->GetHIT(XWarSkills::THROW);
    }

    if (skill) {
        int lvl = skill->GetLevel();
        dmg->ModifyBonus(lvl / 2);
        *range += lvl / 5;
        *hit += lvl * 3;
    }
}

int XCreature::Shoot(int tx, int ty)
{
    if (tx == x && ty == y) {
        return 0; //can't do suicide!
    }

    XItem * bow = GetItem(BP_MISSILE_WEAPON);
    XItem * missile = GetItem(BP_MISSILE);
    XSkill * skill = sk->GetSkill(XSkill::Skill::ARCHERY);

    if (!missile) {
        return 0; //there are no missile to shoot!
    }

    int hit = 0;
    int range = 0;
    XDice dmg;
    GetRangeAttackInfo(&range, &hit, &dmg);
    int vis1 = isVisibleArea(x, y);
    int vis2 = isVisibleArea(tx, ty);

    if (vis1 || vis2) {
        msgwin.Add(GetNameEx(CRN_T1));

        if (bow) {
            msgwin.Add(fmt::format("{} from {}.",
                GetVerb("shoot"), bow->name));
        } else {
            msgwin.Add(fmt::format("{} {}.",
                GetVerb("throw"), missile->name));
        }
    }

    // split missile
    XItem * msl = (XItem*)missile->MakeCopy();
    msl->quantity = 1;

    if (--missile->quantity <= 0) {
        XBodyPart * xbp = GetBodyPart(BP_MISSILE);
        auto used_up = xbp->UnWear();

        // UnWear() doesn't remove it from contain anymore (worn items stay
        // resident there - see XBodyPart::Wear()), so this must, or
        // Invalidate() below leaves a zombie entry behind: still in
        // contain, but invalid.
        if (auto it = contain.find(used_up); it != contain.end()) {
            contain.erase(it);
        }

        used_up->Invalidate();
    }

    // fly away
    MF_DATA mfd;
    mfd.arrow_type = MFT_ARROW;
    mfd.arrow_color = xBROWN;
    mfd.l = l;
    mfd.sx = x;
    mfd.sy = y;
    mfd.ex = tx;
    mfd.ey = ty;
    mfd.to_hit = hit;
    mfd.max_range = range;
    MF_RESULT res = MissileFlight(&mfd);

    if (res == MF_HIT) {
        XCreature * target = l->map->GetMonster(mfd.pt.x, mfd.pt.y);
        DAMAGE_DATA_EX dd;
        dd.damage	= dmg.Throw();
        dd.attacker	= this;

        //temporary soulution, should be replaced in future on general solution
        //which returns name of item with or without 'a'
        switch (msl->it) {
            case ItemType::ARROW:
                dd.attack_name = "the arrow";
                break;

            case ItemType::QUARREL:
                dd.attack_name = "the quarell";
                break;

            case ItemType::SLINGBULLET:
                dd.attack_name = "the sling bullet";
                break;

            case ItemType::ROCK:
                dd.attack_name = "the rock";
                break;

            default:
                dd.attack_name = missile->name.c_str();
                break;
        }

        dd.attack_HIT = hit;
        dd.attack_effect = msl->aet;
        dd.flags = DF_MAGIC_BOLT;
        target->InflictDamage(&dd);

        //if successfull increase bow level
        if (bow) {
            wsk->UseSkill(bow->wt);
        } else {
            wsk->UseSkill(XWarSkills::THROW);
        }

    } else {
        XCreature * tgt = l->map->GetMonster(tx, ty);

        if (tgt && tgt->isVisible()) {
            msgwin.Add(tgt->GetNameEx(CRN_T1));
            msgwin.Add(tgt->GetVerb("avoid"));
            msgwin.Add("missile.");
        }
    }

    msl->Drop(l, mfd.pt.x, mfd.pt.y);
    return 1;
}

XBodyPart* XCreature::GetBodyPart(BODY_PART bp, int count)
{
    for (auto& xbp: components) {
        if (xbp->bp_uin == bp && count-- == 0) {
            return xbp.get();
        }
    }

    return nullptr;
}

bool XCreature::CanWear(const XItem* item)
{
    return std::any_of(
        components.begin(),
        components.end(),
        [item](const std::unique_ptr<XBodyPart>& bp){ return bp->Fit(item->bp) && !bp->Item(); }
    );
}

bool XCreature::Wear(XItem* item) const {
    for (const auto& bp: components) {
        if (bp->Fit(item->bp) && !bp->Item()) {
            bp->Wear(item);
            return true;
        }
    }

    return false;
}

bool XCreature::IsWorn(const XItem* item) const {
    for (const auto& bp: components) {
        if (bp->Item() == item) {
            return true;
        }
    }

    return false;
}

XItem* XCreature::GetItem(BODY_PART bp, int count)
{
    XBodyPart * xbp = GetBodyPart(bp, count);

    if (xbp) {
        return xbp->Item();
    } else {
        return nullptr;
    }
}

void XCreature::FirstStep(int _x, int _y, XLocation * _l)
{
    x = _x;
    y = _y;
    nx = _x;
    ny = _y;
    SetLocation(_l);

    assert(!l->map->GetMonster(_x, _y));

    // The very first placement is also the first point at which this
    // creature becomes shared_from_this()-safe (see XMap::SetMonster's
    // birth path). Starting gear worn/carried during this creature's own
    // constructor (XAnyCreature's equip loop) couldn't establish item/
    // bodypart owner backlinks - or, for worn items, run their onWear()
    // side effect - since shared_from_this() wasn't usable yet.
    // XBodyPart::Wear() already puts worn items in contain via
    // owner_raw->ContainItem() regardless (that doesn't need
    // shared_from_this()), so weight accounting is already correct; only
    // the owner backlinks and the deferred onWear() callback are left to
    // finish here.
    bool first_placement = weak_from_this().expired();

    l->map->SetMonster(_x, _y, this);

    if (first_placement) {
        for (auto& bp: components) {
            if (XItem* worn = bp->Item()) {
                bp->SetOwner(this);
                worn->onWear(this);
            }
        }

        for (auto item: contain) {
            item->SetOwner(this);
        }
    }
}

void XCreature::LastStep()
{
    l->map->ResMonster(x, y);
}

int XCreature::continueRead()
{
    XBook * book = (XBook*)action_data.item.get();
    book->onRead(this);

    if (book->left_to_read <= 0) {
        book->UnCarry();
        book->Invalidate();
        action_data.action = A_MOVE;
        action_data.item = nullptr;

        if (vRand(5) == 0) {
            GainAttr(XStats::LEN, 1);
        }
    }

    return 1;
}

int XCreature::Read(XItem * item)
{
    XSkill * skill = sk->GetSkill(XSkill::Skill::LITERACY);

    if (!skill) {
        if (isHero()) {
            msgwin.Add("You are illiterate!");
        }

        return 0;
    }

    if (item->kind & ItemKind::SCROLL) {
        skill->UseSkill();
        ((XScroll*)item)->onRead(this);
        item->UnCarry();
        item->Invalidate();

        if (vRand(10) == 0) {
            GainAttr(XStats::LEN, 1);
        }

        return 1;
    } else if (item->kind & ItemKind::BOOK) {
        ((XBook*)item)->onRead(this);

        if (((XBook*)item)->left_to_read <= 0) {
            item->UnCarry();
            item->Invalidate();
        } else {
            action_data.action = A_READ;
            action_data.item = XItem::Own(item);
        }

        return 1;
    }

    return 0;
}

void XCreature::SetEventHandler(const char* handler)
{
    event_handler = new char [strlen(handler) + 1];
    strcpy(event_handler, handler);
}

void XCreature::EnableMoveHandler()
{
    wants_move_hook = true;
}

void XCreature::DisableMoveHandler()
{
    wants_move_hook = false;
}

void XCreature::SaveModifier(cereal::JSONOutputArchive& ar) const
{
    ar(*md);
}

void XCreature::LoadModifier(cereal::JSONInputArchive& ar)
{
    md = new XModifier();
    ar(*md);
}

void XCreature::FixupCreatureInfo()
{
    if (!isHero()) { //skip restoing of descriptions and other for hero
        XCreatureStorage::RestoreCreatureInfo(this);
    }
}

void XCreature::FixupXaiOwner()
{
    if (xai) {
        xai->SetOwner(this);
    }
}

void XCreature::NotifyLuaEventHandler(LuaEvent event)
{
    XLocation::lua_int_buffer = &lua_ints;
    XLocation::lua_int_index = 0;

    if (event_handler && strlen(event_handler)) {
        sol::state_view lua(XLocation::L);
        lua[event_handler](event);
    }
}

int XCreature::GetCreatureStrength()
{
    int tdv = GetDV();
    int tpv = GetPV();

    if (tdv <= 0) {
        tdv = 1;
    }

    if (tpv <= 0) {
        tpv = 1;
    }

    int dv_pv_bonus = ((tdv * tpv * tpv) / 10 + (_DV * _PV * _PV));
    int thit = GetHIT() / 10;
    int tdmg = (dice.GetCount() * dice.GetSides() + dice.GetBonus() + GetDMG());

    if (thit <= 0) {
        thit = 1;
    }

    if (tdmg <= 0) {
        tdmg = 1;
    }

    int hit_dmg_bonus = thit * tdmg * GetMaxHP();

    return 20 + hit_dmg_bonus + dv_pv_bonus;
}

int XCreature::GetTarget(TARGET_REASON tr, XPoint * pt, int max_range, XObject** back)
{
    switch (tr) {
        case TR_ATTACK_TARGET:
            return xai->GetTargetPos(pt);
            break;
    }

    return 0;
}

int XCreature::Chat(XCreature * chatter, const char* msg)
{
    if (!event_handler) {
        return 0;
    }

    sol::state_view lua(XLocation::L);
    sol::protected_function_result result = lua[event_handler](LuaEvent::CHAT, (void*)this, (void*)chatter, std::string(msg));

    if (!result.valid()) {
        return 0;
    }

    return result.get<sol::optional<int>>().value_or(0);
}

std::shared_ptr<XItem> XCreature::ContainItem(XItem * item)
{
    if (CarryItem(item)) {
        // contain.insert() (XItemList::insert, item/item.h) returns an
        // iterator to whichever XItem this ended up as: `item` itself on
        // a fresh insert, or a pre-existing, stackable-equal item it got
        // Concat()-ed into instead (see the declaration comment on
        // ContainItem in creature.h for why that distinction matters).
        return *contain.insert(XItem::Own(item)).first;
    }

    return nullptr;
}

bool XCreature::CarryItem(XItem * item)
{
    if (item->GetOwner().lock().get() == this) {
        return 1;
    }

    carried_weight += item->weight * item->quantity;

    if (GetCarryState() == CSTATE_DIE) {
        carried_weight -= item->weight * item->quantity;
        return false;
    } else {
        item->SetOwner(this);
        return true;
    }
}

void XCreature::UnCarryItem(XItem * item)
{
    if (auto o = item->GetOwner().lock()) {
        assert(o.get() == this);
        carried_weight -= item->weight * item->quantity;
    }

    item->SetOwner(nullptr);
}

int XCreature::CarryValue(CARRY_STATE cs)
{
    int str = stats->Get(XStats::STR) + added_stats.Get(XStats::STR);

    switch (cs) {
        case CSTATE_NORMAL :
            return str * 120;
            break;

        case CSTATE_BURDENED :
            return str * 200;
            break;

        case CSTATE_STRAINED :
            return str * 280;
            break;

        case CSTATE_OVERBURDEN :
            return str * 360;
            break;

        case CSTATE_DIE :
            return str * 600;
            break;

        default :
            assert(0);
            break;
    };

    return 0;
}

CARRY_STATE XCreature::GetCarryState()
{
    if (carried_weight < CarryValue(CSTATE_NORMAL)) {
        return CSTATE_NORMAL;
    } else if (carried_weight < CarryValue(CSTATE_BURDENED)) {
        return CSTATE_BURDENED;
    } else if (carried_weight < CarryValue(CSTATE_STRAINED)) {
        return CSTATE_STRAINED;
    } else if (carried_weight < CarryValue(CSTATE_OVERBURDEN)) {
        return CSTATE_OVERBURDEN;
    } else {
        return CSTATE_DIE;
    }
}

int XCreature::GetVisibleRadius()
{
    int perception = stats->Get(XStats::PER);

    if (perception < 5) {
        return 3;
    } else if (perception < 10) {
        return 4;
    } else if (perception < 20) {
        return 5;
    } else if (perception < 50) {
        return 6;
    } else if (perception < 90) {
        return 7;
    } else {
        return 8;
    }
}

int XCreature::onGiveItem(XCreature* giver, XItem* item)
{
    if (!event_handler) {
        return 0;
    }

    sol::state_view lua(XLocation::L);
    sol::protected_function_result result = lua[event_handler](LuaEvent::GIVE_ITEM, (void*)this, (void*)giver, (void*)item);

    if (!result.valid()) {
        return 0;
    }

    return result.get<sol::optional<int>>().value_or(0);
}

int XCreature::MoneyOp(int money_count)
{
    XItem* money = nullptr;

    for (auto it: contain) {
        if (it->kind & ItemKind::MONEY) {
            money = it.get();
            break;
        }
    }

    if (money) {
        if (money_count >= 0) {
            carried_weight -= money->quantity / 10;
            money->quantity += money_count;
            carried_weight += money->quantity / 10;

            return money->quantity;
        }

        if (money->quantity + money_count > 0) {
            carried_weight -= money->quantity / 10;
            money->quantity += money_count;
            carried_weight += money->quantity / 10;

            return money->quantity;
        }

        if (money->quantity + money_count == 0) {
            carried_weight -= money->quantity / 10;

            // Invalidate() must run before erase() now that contain holds
            // money's real shared_ptr ownership (opposite of the old
            // raw-pointer ordering): contain can be money's only reference,
            // and erasing it while money is still valid would run Own()'s
            // deleter on a still-valid item, re-entering Invalidate() while
            // we're still about to call it ourselves below. Invalidate()
            // itself doesn't free money - contain still holds it alive at
            // that point - so money->kind stays safe to read for the erase's
            // tree walk immediately after.
            money->Invalidate();

            if (auto it = contain.find(money); it != contain.end()) {
                contain.erase(it);
            }

            return 0;
        }

        return money->quantity + money_count;
    }

    if (money_count > 0) {
        carried_weight += money_count / 10;
        contain.insert(XItem::Own(new XMoney(money_count)));

        return money_count;
    }

    return money_count;
}

const char* XCreature::GetGenderStr()
{
    XCreature::Gender g = GetGender();

    if (g == XCreature::MALE) {
        return "male";
    } else if (g == XCreature::FEMALE) {
        return "female";
    } else {
        return "neuter";
    }
}

const std::string XCreature::GetNameEx(CR_NAME_TYPE crn)
{
    if (isVisible()) {
        switch (creature_person_type) {
            case XCreature::YOU:
            case XCreature::MALE_YOU:
            case XCreature::FEMALE_YOU:
                switch (crn) {
                    case CRN_T1:
                        return "you";

                    case CRN_T2:
                        return "you";

                    case CRN_T3:
                        return "you";

                    case CRN_T4:
                        return "your";
                }

            case XCreature::NAMED_HE:
                switch (crn) {
                    case CRN_T1:
                        return name;

                    case CRN_T2:
                        return "he";

                    case CRN_T3:
                        return "him";

                    case CRN_T4:
                        return "his";
                }

            case XCreature::NAMED_SHE:
                switch (crn) {
                    case CRN_T1:
                        return name;

                    case CRN_T2:
                        return "she";

                    case CRN_T3:
                        return "her";

                    case CRN_T4:
                        return "hers";
                }

            case XCreature::NAMED_IT:
                switch (crn) {
                    case CRN_T1:
                        return name;

                    case CRN_T2:
                        return "it";

                    case CRN_T3:
                        return "it";

                    case CRN_T4:
                        return "its";
                }

            case XCreature::HE:
                switch (crn) {
                    case CRN_T1:
                        return fmt::format("the {}", name);

                    case CRN_T2:
                        return "he";

                    case CRN_T3:
                        return "him";

                    case CRN_T4:
                        return "his";
                }

            case XCreature::SHE:
                switch (crn) {
                    case CRN_T1:
                        return fmt::format("the female {}", name);

                    case CRN_T2:
                        return "she";

                    case CRN_T3:
                        return "her";

                    case CRN_T4:
                        return "hers";
                }

            case XCreature::IT:
                switch (crn) {
                    case CRN_T1:
                        return fmt::format("the {}", name);

                    case CRN_T2:
                        return "it";

                    case CRN_T3:
                        return "it";

                    case CRN_T4:
                        return "its";
                }
        }
    } else {
        switch (crn) {
            case CRN_T1:
                return "someone";

            case CRN_T2:
                return "it";

            case CRN_T3:
                return "it";

            case CRN_T4:
                return "its";
        }
    }

    assert(0);
    return "";
}

std::string XCreature::GetVerb(std::string verb) const
{
    if (creature_person_type & XCreature::YOU) {
        return verb;
    }

    char last_char = verb.back();

    if (last_char == 's' || last_char == 'h') {
        return verb.append("es");
    }

    return verb.append("s");
}
