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

#include "creature/creature.h"
#include "creature/los.h"
#include "game/game.h"
#include "helpers/msgwin.h"
#include "magic/modifier.h"
#include "map/map_objects.h"

extern "C"
{
#include "lauxlib.h"
}

// a creature which is currently being displayed
XCreature* XCreature::main_creature = NULL;

void ACTION_DATA::Store(XFile * f)
{
    f->Write(&action, sizeof(ACTION));
    item.Store(f);
}

void ACTION_DATA::Restore(XFile * f)
{
    f->Read(&action, sizeof(ACTION));
    item.Restore(f);
}

XCreatureGroupMap XCreature::group_members = XCreatureGroupMap();

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
    creature_person_type = CPT_HE;

    im = IM_CREATURE;
    xai = std::make_unique<XStandardAI>(this);
    md = new XModifier();
    m = new XMagic();
    sk = new XSkills();
    wsk = new XWarSkills();

    weight = 1000;

    creature_class = CR_NONE;

    tactics = TS_NORMAL;
    group_id = GID_NONE;
    food_feeling = FF_NORMAL;
    event_handler = NULL;
}

void XCreature::Invalidate()
{
    for (auto it = components.begin(); it != components.end(); it++) {
        delete *it;
        components.erase(it);
    }

    for (auto item: contain) {
        item->Invalidate();
    }

    delete sk;
    sk = NULL;

    if (md) {
        delete md;
        md = NULL;
    }

    delete m;
    m = NULL;

    delete wsk;
    wsk = NULL;

    delete xai.release();

    if (event_handler) {
        delete[] event_handler;
    }

    // remove perished creature from the group members list
    if (group_id != GID_NONE) {
        auto group = group_members.equal_range(group_id);
        for (auto el = group.first; el != group.second; el++) {
            if (el->second == this) {
                group_members.erase(el);
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

void XCreature::setGroupID(GROUP_ID gid)
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
        action_data.item->onUse(UIS_STOP, this);
    } else {
        if (action_data.item) {
            contain.insert(action_data.item.get());
        }
    }

    action_data.action = A_MOVE;
    action_data.item = NULL;
    isDisturb = 0; //prevents hero to continue automove when attaked by ghosts...
    return 1;
}

int XCreature::continueEat()
{
    XAnyFood * food = (XAnyFood*)action_data.item.get();
    assert(food->im & IM_FOOD);
    int res = food->onEat(this);

    if (res != 2) {
        action_data.action = A_MOVE;
        action_data.item = NULL;
    }

    return 1;
}


int XCreature::Eat(XAnyFood * food)
{
    int res = food->onEat(this);

    if (res) {
        if (res == 2) {
            action_data.action = A_EAT;
            action_data.item = food;
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
        Die(NULL);
        return 0;
    }

    return 1;
}

int XCreature::Run()
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

    if (md && md->Run(this)) {
        int atletics = sk->GetLevel(XSkill::Skill::ATHLETICS);

        if (GetCarryState() >= CSTATE_STRAINED) {
            if (vRand(3000 / (5 + atletics)) == 0) {
                GainAttr(S_STR, 1);
                sk->UseSkill(XSkill::Skill::ATHLETICS, 10);
            }
        } else if (vRand(6000 / (5 + atletics)) == 0) {
            GainAttr(S_DEX, 1);
            sk->UseSkill(XSkill::Skill::ATHLETICS, 10);
        }

        DoMove();
    }

    if (ttm <= 0 && isValid()) {
        ttm += GetSpeed();
    }

    return 1;
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

    XMapObject * tobj = l->map->GetSpecial(x, y);

    if (tobj) {
        //we can move easy from pit or web
        if (tobj->im == IM_TRAP && (nx != x || ny != y) && !((XTrap*)tobj)->MoveOut(this)) {
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

        if (obj) {
            if (obj->im == IM_TRAP) {
                ((XTrap*)obj)->MoveIn(this);
            }

            if (obj->im == IM_TELEPORT) {
                ((XTeleport*)obj)->MoveIn(this);
            }
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
    char buf[250];

    vGotoXY(0, size_y - 3);
    vSetAttr(xLIGHTGRAY);
    vPutS(name);

    if (XGame::isAvgPV) {
        sprintf(buf, "DV/PV:%d/%d  ", GetDV(), GetPV());
    } else {
        sprintf(buf, "DV/PV:%d/%d  ", GetDV(), GetPV());
    }

    vGotoXY(0, size_y - 2);
    vPutS(buf);

    sprintf(buf, "St:%-2d Dx:%-2d To:%-2d Le:%-2d Wi:%-2d Ma:%-2d Pe:%-2d Ch:%-2d Sp:%-3d L:%s", GetStats(S_STR), GetStats(S_DEX), GetStats(S_TOU),
        GetStats(S_LEN), GetStats(S_WIL), GetStats(S_MAN), GetStats(S_PER), GetStats(S_CHR), 100000 / GetSpeed(), l->GetBriefName());
    vGotoXY(15, size_y - 3);
    vPutS(buf);
    vClrEol();

    sprintf(buf, "HP:%d(%d)  PP:%d(%d)  ", _HP, GetMaxHP(), _PP, GetMaxPP());
    vGotoXY(14, size_y - 2);
    vPutS(buf);

    sprintf(buf, "Exp(%d)%lu", level, _EXP);
    vGotoXY(38, size_y - 2);
    vPutS(buf);

#ifdef _DEBUG
    sprintf(buf, "x:y[%d:%d]", x, y);
    vGotoXY(60, size_y - 2);
    vPutS(buf);
#endif

    if (nutrio > base_nutrio * 18) {
        sprintf(buf, MSG_RED "overfed! ");
    } else if (nutrio > base_nutrio * 14) {
        sprintf(buf, "bloated ");
    } else if (nutrio > base_nutrio * 10 && nutrio <= base_nutrio * 14) {
        sprintf(buf, "satiated ");
    } else if (nutrio > base_nutrio * 8 && nutrio <= base_nutrio * 10) {
        sprintf(buf, "");
    } else if (nutrio > base_nutrio * 6 && nutrio <= base_nutrio * 8) {
        sprintf(buf, "hungry ");
    } else if (nutrio > base_nutrio * 4 && nutrio <= base_nutrio * 6) {
        sprintf(buf, MSG_YELLOW "very hungry ");

        if (action_data.action != A_EAT) {
            stopAction();
        }
    } else if (nutrio > base_nutrio && nutrio <= base_nutrio * 4) {
        sprintf(buf, MSG_RED "weak ");

        if (action_data.action != A_EAT) {
            stopAction();
        }
    } else if (nutrio <= base_nutrio) {
        sprintf(buf, MSG_RED "dying! ");

        if (action_data.action != A_EAT) {
            stopAction();
        }
    }

    strcat(buf, MSG_LIGHTGRAY);
    vGotoXY(0, size_y - 1);
    vPutS(buf);

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

    md->toString(buf);

    vPutS(buf);

    vClrEol();

    vSetAttr(xLIGHTGRAY);
}

void XCreature::NewMove()
{
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

    int str = s->Get(S_STR);

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
    for (auto xbp: components)
    {
        XItem* i = xbp->Item();

        if (i && i->im == IM_SHIELD) {
            int shld_skl = wsk->GetDV(WSK_SHIELD);
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
            return (3 * (GetStats(S_DEX) + sk->GetLevel(XSkill::Skill::TACTICS))) / 2;
            break;

        case TS_DEFENSIVE	:
            return GetStats(S_DEX) + sk->GetLevel(XSkill::Skill::TACTICS);
            break;

        case TS_NORMAL	:
            return (2 * GetStats(S_DEX)) / 3 + sk->GetLevel(XSkill::Skill::TACTICS);
            break;

        case TS_AGGRESSIVE	:
            return GetStats(S_DEX) / 3 + sk->GetLevel(XSkill::Skill::TACTICS);
            break;

        case TS_BERSERKER	:
            return GetStats(S_DEX) / 10 + sk->GetLevel(XSkill::Skill::TACTICS);
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
            return GetStats(S_DEX) / 10 + sk->GetLevel(XSkill::Skill::TACTICS) - 5;
            break;

        case TS_DEFENSIVE	:
            return GetStats(S_DEX) / 7 + sk->GetLevel(XSkill::Skill::TACTICS) - 3;
            break;

        case TS_NORMAL	:
            return GetStats(S_DEX) / 4 + sk->GetLevel(XSkill::Skill::TACTICS);
            break;

        case TS_AGGRESSIVE	:
            return GetStats(S_DEX) / 3 + sk->GetLevel(XSkill::Skill::TACTICS);
            break;

        case TS_BERSERKER	:
            return GetStats(S_DEX) / 2 + sk->GetLevel(XSkill::Skill::TACTICS);
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
            return GetStats(S_STR) / 20 + sk->GetLevel(XSkill::Skill::TACTICS) - 3;
            break;

        case TS_DEFENSIVE	:
            return GetStats(S_STR) / 10 + sk->GetLevel(XSkill::Skill::TACTICS) - 1;
            break;

        case TS_NORMAL	:
            return GetStats(S_STR) / 7 + sk->GetLevel(XSkill::Skill::TACTICS);
            break;

        case TS_AGGRESSIVE	:
            return GetStats(S_STR) / 5 + sk->GetLevel(XSkill::Skill::TACTICS);
            break;

        case TS_BERSERKER	:
            return GetStats(S_STR) / 2 + sk->GetLevel(XSkill::Skill::TACTICS);
            break;

        default	:
            assert(0);
    }

    return 0;
}

int XCreature::GetPV()
{
    return _PV + added_PV + GetStats(S_TOU) / 10;
}

int XCreature::GainAttr(STATS st, int val)
{
    int cur = s->Get(st);
    int max = max_stats.Get(st);

    if (val > 0) {
        if (cur < max) {
            if (cur + val > max) {
                val = max - cur;
            }

            s->Modify(st, val);

            if (isHero()) {
                switch (st) {
                    case S_STR:
                        msgwin.Add("You feel stronger!");
                        break;

                    case S_DEX:
                        msgwin.Add("You are becoming more graceful!");
                        break;

                    case S_TOU:
                        msgwin.Add("Your health increases!");
                        break;

                    case S_MAN:
                        msgwin.Add("You feel power surging through your body!");
                        break;

                    case S_WIL:
                        msgwin.Add("You feel more powerfull!");
                        break;

                    case S_LEN:
                        msgwin.Add("You feel smarter!");
                        break;

                    case S_PER:
                        msgwin.Add("You feel more perceptive!");
                        break;

                    case S_CHR:
                        msgwin.Add("Your beauty improves!");
                        break;
                }
            }

            return 1;
        }
    } else {
        if (cur - val > 1) {
            s->Modify(st, val);

            if (isHero()) {
                switch (st) {
                    case S_STR:
                        msgwin.Add("Your muscles weaken!");
                        break;

                    case S_DEX:
                        msgwin.Add("You feel clumsy!");
                        break;

                    case S_TOU:
                        msgwin.Add("You feel like you might be getting sick!");
                        break;

                    case S_MAN:
                        msgwin.Add("You feel power draining from your body!");
                        break;

                    case S_WIL:
                        msgwin.Add("You feel diminished!");
                        break;

                    case S_LEN:
                        msgwin.Add("Thinking becomes more difficult!");
                        break;

                    case S_PER:
                        msgwin.Add("Your senses dull!");
                        break;

                    case S_CHR:
                        msgwin.Add("Your features harden!");
                        break;
                }
            }

            return 1;
        }
    }

    return 0;
}

int XCreature::GainResist(RESISTANCE rs, int val)
{
    r->ChangeResistance(rs, val);

    if (val > 0) {
        switch (rs) {
            case R_FIRE:
                msgwin.Add("Your blood cools down!");
                break;

            case R_COLD:
                msgwin.Add("Your skin grows warm!");
                break;

            case R_ACID:
                msgwin.Add("Your stomach settles!");
                break;

            case R_POISON:
                msgwin.Add("Your flesh tingles!");
                break;

            case R_PARALYSE:
                msgwin.Add("Your movements grow stronger!");
                break;
        }
    } else {
        switch (rs) {
            case R_FIRE:
                msgwin.Add("Your blood warms up!");
                break;

            case R_COLD:
                msgwin.Add("Your skin grows cold!");
                break;

            case R_ACID:
                msgwin.Add("You feel a pain in your stomach!");
                break;

            case R_POISON:
                msgwin.Add("You feel vulnerable!");
                break;

            case R_PARALYSE:
                msgwin.Add("Your movements are unsure!");
                break;
        }
    }

    return 1;
}

int XCreature::GetStats(STATS st)
{
    assert(st > S_UNKNOWN && st < S_EOF);

    int res = s->Get(st) + added_stats.Get(st);
    return res > 0 ? res : 1;
}

int XCreature::GetResistance(RESISTANCE tr)
{
    assert(r);
    return r->GetResistance(tr) + added_resists.GetResistance(tr);

}

void XCreature::Die(XCreature * killer)
{
    assert(isValid());

    if (event_handler) {
        lua_pushstring(XLocation::L, event_handler);
        lua_gettable(XLocation::L, LUA_GLOBALSINDEX);
        lua_pushnumber(XLocation::L, LE_DIE);
        lua_pushlightuserdata(XLocation::L, this);
        lua_pushlightuserdata(XLocation::L, killer);
        lua_call(XLocation::L, 3, 1);
        lua_pop(XLocation::L, 1);
    }

    // Drop inventory to the ground
    for (auto bp: components) {
        if (bp->Item()) {
            bp->UnWear()->Drop(l, x, y);
        }
    }

    for (auto item: contain) {
        item->Drop(l, x, y);
    }

    LastStep();

    if (killer && killer != this) {
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
        UnCarryItem(i);
        i->Drop(l, x, y);
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

            if (xbp && xbp->Item() && xbp->Item()->Compare(i) == 0 && i->GetRef() == 0) {
                xbp->Item()->Concat(i);
            } else {
                contain.insert(i);
            }

            return 1;
        } else { //if we can't pick item, then drop it
            if (im & IM_HERO) {
                char bufx[256];
                char buf[256];
                i->toString(buf);
                sprintf(bufx, "%s is to heavy for you!", buf);
                msgwin.ClrMsg();
                msgwin.Add(bufx);
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

CR_GENDER XCreature::GetGender()
{
    switch (creature_person_type) {
        case CPT_HE:
        case CPT_NAMED_HE:
        case CPT_MALE_YOU:
            return GEN_MALE;
            break;

        case CPT_SHE:
        case CPT_NAMED_SHE:
        case CPT_FEMALE_YOU:
            return GEN_FEMALE;
            break;

        default:
            break;
    }

    return GEN_NEUTER;
}

int XCreature::GetMaxHP()
{
    return MAX_HP + (MAX_HP * GetStats(S_TOU)) / 20;
}

int XCreature::GetMaxPP()
{
    return MAX_PP + (MAX_PP * GetStats(S_MAN)) / 10;
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
    MAX_HP += vRand((GetStats(S_TOU) / 5) + 1) + 1;
    MAX_PP += vRand((GetStats(S_MAN) / 2) + 1) + 1;
    level++;

    if (level > XGame::best_cr_level) {
        XGame::best_cr_level = level;
        XGame::best_creature = this;
    }
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
    float f = (float)(5.0 * log((300.0 * GetStats(S_STR)) / (10.0 * (weapon->weight) * mult)));
    return std::min((int)f, 0);
}

int XCreature::GetDMGFHBonus(XItem* weapon)
{
    return 0;
}

XBodyPart* XCreature::GetRNDBodyPart()
{
    int value = 0;
    for (auto bp: components) {
        value += bp->GetPartSize();
    }

    int v = value > 0 ? vRand() % value : 0;

    for (auto bp: components) {
        v -= bp->GetPartSize();

        if (v <= 0)
            return bp;
    }

    return nullptr;
}

XBodyPart* XCreature::GetRNDBodyPart(ITEM_MASK xim, RBP_FLAG rbpf)
{
    if (rbpf == RBP_BLOCK && xim & IM_SHIELD) {
        auto bpi = std::find_if(
            components.begin(),
            components.end(),
            [](XBodyPart* xbp) { return xbp->Item() && xbp->Item()->im & IM_SHIELD; }
        );

        if (bpi != components.end() && (vRand() % 100 < 5 * wsk->GetLevel(WSK_SHIELD) + 5)) {
            return bpi[0];
        }
    }

    int count = 0;
    for (auto xbp: components) {
        if (xbp->GetProperIM() & xim) {
            count++;
        }
    }

    if (count == 0) {
        return nullptr;
    }

    int n = vRand() % count;

    count = 0;
    for (const auto xbp: components) {
        if (xbp->GetProperIM() & xim) {
            if (n == count) {
                return xbp;
            }

            count++;
        }
    }

    assert(0);
    return NULL;
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

    if (spec && spec->im & IM_WAY) {
        XLocation * tgtloc = Game.locations[((XStairWay*)spec)->ln].get();
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
            tc->LastStep();
            tc->FirstStep(n_x, n_y, tgtloc);
            tc->l = tgtloc;
            tc->action_data.action = A_MOVE;

            if (tc->im & IM_HERO) {
                tgtloc->visited_by_hero = 1;
                tgtloc->map->Put(tc);
                vRefresh();
            }

            return;
        } else if (tc->im & IM_HERO) {
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

    int str = s->Get(S_STR);
    int dex = s->Get(S_DEX);

    *range = missile->RNG;
    *hit = dex / 2 + missile->_HIT;
    dmg->Setup(missile->dice);

    if (bow) {
        *range += bow->RNG;
        dmg->Add(&(bow->dice));
        *range += wsk->GetDV(bow->wt);
        dmg->Z += wsk->GetDMG(bow->wt);
        *hit += wsk->GetHIT(bow->wt);
    } else {
        *range += RNG + str / 25;
        dmg->Z += str / 10;
        *range += wsk->GetDV(WSK_THROW);
        dmg->Z += wsk->GetDMG(WSK_THROW);
        *hit += wsk->GetHIT(WSK_THROW);
    }

    if (skill) {
        int lvl = skill->GetLevel();
        dmg->Z += lvl / 2;
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
            msgwin.Add(GetVerb("shoot"));
            msgwin.Add("from");
            msgwin.AddLast(bow->name);
        } else {
            msgwin.Add(GetVerb("throw"));
            msgwin.AddLast(missile->name);
        }
    }

    // split missile
    XItem * msl = (XItem*)missile->MakeCopy();
    msl->quantity = 1;

    if (--missile->quantity <= 0) {
        XBodyPart * xbp = GetBodyPart(BP_MISSILE);
        xbp->UnWear()->Invalidate();
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
            case IT_ARROW:
                dd.attack_name = "the arrow";
                break;

            case IT_QUARREL:
                dd.attack_name = "the quarell";
                break;

            case IT_SLINGBULLET:
                dd.attack_name = "the sling bullet";
                break;

            case IT_ROCK:
                dd.attack_name = "the rock";
                break;

            default:
                dd.attack_name = missile->name;
                break;
        }

        dd.attack_HIT	= hit;
        dd.attack_brand	= msl->brt;
        dd.flags	= DF_MAGIC_BOLT;
        target->InflictDamage(&dd);

        //if successfull increase bow level
        if (bow) {
            wsk->UseSkill(bow->wt);
        } else {
            wsk->UseSkill(WSK_THROW);
        }

    } else {
        XCreature * tgt = l->map->GetMonster(tx, ty);

        if (tgt && tgt->isVisible()) {
            msgwin.Add(tgt->GetNameEx(CRN_T1));
            msgwin.Add(tgt->GetVerb("avoid"));
            msgwin.Add("missile.");
        }
    }

    msl->Drop(l.get(), mfd.pt.x, mfd.pt.y);
    return 1;
}

XBodyPart* XCreature::GetBodyPart(BODY_PART bp, int count)
{
    for (auto xbp: components) {
        if (xbp->bp_uin == bp && count-- == 0) {
            return xbp;
        }
    }

    return nullptr;
}

bool XCreature::CanWear(const XItem* item)
{
    return std::any_of(
        components.begin(),
        components.end(),
        [item](XBodyPart* bp){ return bp->Fit(item->bp) && !bp->Item(); }
    );
}

bool XCreature::Wear(XItem* item) const {
    for (const auto bp: components) {
        if (bp->Fit(item->bp) && !bp->Item()) {
            bp->Wear(item);
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
        return NULL;
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
    l->map->SetMonster(_x, _y, this);
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
        action_data.item = NULL;

        if (vRand(5) == 0) {
            GainAttr(S_LEN, 1);
        }
    }

    return 1;
}

int XCreature::Read(XItem * item)
{
    XSkill * skill = sk->GetSkill(XSkill::Skill::LITERACY);

    if (!skill) {
        if (im & IM_HERO) {
            msgwin.Add("You are illiterate!");
        }

        return 0;
    }

    if (item->im & IM_SCROLL) {
        skill->UseSkill();
        ((XScroll*)item)->onRead(this);
        item->UnCarry();
        item->Invalidate();

        if (vRand(10) == 0) {
            GainAttr(S_LEN, 1);
        }

        return 1;
    } else if (item->im & IM_BOOK) {
        ((XBook*)item)->onRead(this);

        if (((XBook*)item)->left_to_read <= 0) {
            item->UnCarry();
            item->Invalidate();
        } else {
            action_data.action = A_READ;
            action_data.item = item;
        }

        return 1;
    }

    return 0;
}

void XCreature::Store(XFile * f)
{
    XBaseObject::Store(f);

    f->Write(&_EXP, sizeof(unsigned long));
    f->Write(&added_DMG);
    f->Write(&added_DV);
    f->Write(&added_HIT);
    f->Write(&added_HP);
    f->Write(&added_PP);
    f->Write(&added_PV);

    f->Write(&attack_energy);
    f->Write(&move_energy);
    f->Write(&base_speed);
    f->Write(&added_speed);

    added_resists.Store(f);
    f->Write(&added_RNG);
    added_stats.Store(f);
    f->Write(&base_exp);
    f->Write(&base_nutrio);
    f->Write(&carried_weight);

    // FIXME: Implement when porting saving/restoring to Cereal
    // components.StoreList(f);

    f->Write(&creature_class, sizeof(CREATURE_CLASS));
    f->Write(&creature_size, sizeof(CREATURE_SIZE));
    f->Write(&food_feeling, sizeof(FOOD_FEELING));
    f->Write(&group_id, sizeof(GROUP_ID));

    f->Write(&level);

    m->Store(f);
    md->Store(f);
    f->Write(&nutrio);
    f->Write(&nutrio_speed);

    // FIXME: implement when saving with Cereal
    // sk->Store(f);

    f->Write(&tactics, sizeof(TACTICS_STATE));
    wsk->Store(f);

    // FIXME: Implement when porting saving/restoring to Cereal
    // XObject::StorePointer(f, xai);

    action_data.Store(f);

    // FIXME: Implement when porting saving/restoring to Cereal
    // contain.StoreList(f);

    religion.Store(f);
    max_stats.Store(f);
    f->Write(&creature_person_type, sizeof(creature_person_type));
    f->Write(&creature_name, sizeof(creature_name));

    int sz = 0;

    if (event_handler) {
        sz = strlen(event_handler) + 1;
    }

    f->Write(&sz);

    if (sz > 0) {
        f->Write(event_handler, sz);
    }

    if (event_handler && strlen(event_handler)) {
        lua_pushstring(XLocation::L, event_handler);
        lua_gettable(XLocation::L, LUA_GLOBALSINDEX);
        lua_pushnumber(XLocation::L, LE_SAVE);
        lua_call(XLocation::L, 1, 1);
        int res = lua_tonumber(XLocation::L, 2);
        lua_pop(XLocation::L, 1);
    }

}

void XCreature::Restore(XFile * f)
{
    XBaseObject::Restore(f);

    f->Read(&_EXP, sizeof(unsigned long));
    f->Read(&added_DMG);
    f->Read(&added_DV);
    f->Read(&added_HIT);
    f->Read(&added_HP);
    f->Read(&added_PP);
    f->Read(&added_PV);

    f->Read(&attack_energy);
    f->Read(&move_energy);
    f->Read(&base_speed);
    f->Read(&added_speed);


    added_resists.Restore(f);
    f->Read(&added_RNG);
    added_stats.Restore(f);
    f->Read(&base_exp);
    f->Read(&base_nutrio);
    f->Read(&carried_weight);

    assert(components.empty());
    // FIXME: Implement when porting saving/restoring to Cereal
    // components.RestoreList(f);

    f->Read(&creature_class, sizeof(CREATURE_CLASS));
    f->Read(&creature_size, sizeof(CREATURE_SIZE));
    f->Read(&food_feeling, sizeof(FOOD_FEELING));
    f->Read(&group_id, sizeof(GROUP_ID));

    f->Read(&level);

    m = new XMagic();
    m->Restore(f);
    md = new XModifier();
    md->Restore(f, this);
    f->Read(&nutrio);
    f->Read(&nutrio_speed);
    sk = new XSkills();

    // FIXME: Implement when saving with Cereal
    // sk->Restore(f);

    f->Read(&tactics, sizeof(TACTICS_STATE));
    wsk = new XWarSkills();
    wsk->Restore(f);

    // FIXME: Implement when porting saving/restoring to Cereal
    // xai = (XStandardAI*)XObject::RestorePointer(f, this);

    action_data.Restore(f);

    assert(contain.empty());
    // FIXME: Implement when porting saving/restoring to Cereal
    // contain.RestoreList(f);

    religion.Restore(f);
    max_stats.Restore(f);
    f->Read(&creature_person_type, sizeof(creature_person_type));
    f->Read(&creature_name, sizeof(creature_name));

    if (!isHero()) { //skip restoing of descriptions and other for hero
        XCreatureStorage::RestoreCreatureInfo(this);
    }

    int sz = 0;
    f->Read(&sz);

    if (sz > 0) {
        event_handler = new char [sz];
        f->Read(event_handler, sz);
    } else {
        event_handler = NULL;
    }

    if (event_handler && strlen(event_handler)) {
        lua_pushstring(XLocation::L, event_handler);
        lua_gettable(XLocation::L, LUA_GLOBALSINDEX);
        lua_pushnumber(XLocation::L, LE_LOAD);
        lua_call(XLocation::L, 1, 1);
        int res = lua_tonumber(XLocation::L, 2);
        lua_pop(XLocation::L, 1);
    }
}

void XCreature::SetEventHandler(const char* handler)
{
    event_handler = new char [strlen(handler) + 1];
    strcpy(event_handler, handler);
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
    int tdmg = (dice.X * dice.Y + dice.Z + GetDMG());

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
    if (event_handler) {
        lua_pushstring(XLocation::L, event_handler);
        lua_gettable(XLocation::L, LUA_GLOBALSINDEX);
        lua_pushnumber(XLocation::L, LE_CHAT);
        lua_pushlightuserdata(XLocation::L, this);
        lua_pushlightuserdata(XLocation::L, chatter);
        lua_pushlightuserdata(XLocation::L, const_cast<char*>(msg));
        lua_call(XLocation::L, 4, 1);
        int res = lua_tonumber(XLocation::L, 3);
        lua_pop(XLocation::L, 1);
        return res;
    }

    return 0;
}

bool XCreature::ContainItem(XItem * item)
{
    if (CarryItem(item)) {
        contain.insert(item);
        return true;
    } else {
        return false;
    }
}

bool XCreature::CarryItem(XItem * item)
{
    if (item->GetOwner() && item->GetOwner().get() == this) {
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
    if (item->GetOwner()) {
        assert(item->GetOwner().get() == this);
        carried_weight -= item->weight * item->quantity;
    }

    item->SetOwner(NULL);
}

int XCreature::CarryValue(CARRY_STATE cs)
{
    int str = s->Get(S_STR) + added_stats.Get(S_STR);

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
    int perception = s->Get(S_PER);

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

int XCreature::onGiveItem(XCreature * giver, XItem * item)
{
    if (event_handler) {
        lua_pushstring(XLocation::L, event_handler);
        lua_gettable(XLocation::L, LUA_GLOBALSINDEX);
        lua_pushnumber(XLocation::L, LE_GIVE_ITEM);
        lua_pushlightuserdata(XLocation::L, this);
        lua_pushlightuserdata(XLocation::L, giver);
        lua_pushlightuserdata(XLocation::L, item);
        lua_call(XLocation::L, 4, 1);
        int res = lua_tonumber(XLocation::L, 3);
        lua_pop(XLocation::L, 1);
        return res;
    }

    return 0;
}

int XCreature::MoneyOp(int money_count)
{
    XItem* money = nullptr;

    for (auto it: contain) {
        if (it->im & IM_MONEY) {
            money = it;
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
            money->Invalidate();
            contain.erase(money);

            return 0;
        }

        return money->quantity + money_count;
    }

    if (money_count > 0) {
        carried_weight += money_count / 10;
        contain.insert(new XMoney(money_count));

        return money_count;
    }

    return money_count;
}

const char* XCreature::GetGenderStr()
{
    CR_GENDER g = GetGender();

    if (g == GEN_MALE) {
        return "male";
    } else if (g == GEN_FEMALE) {
        return "female";
    } else {
        return "neuter";
    }
}

const char* XCreature::GetNameEx(CR_NAME_TYPE crn)
{
    static char cool_buf[1024]; //hope the name will never larger than

    if (isVisible()) {
        switch (creature_person_type) {
            case CPT_YOU:
            case CPT_MALE_YOU:
            case CPT_FEMALE_YOU:
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

            case CPT_NAMED_HE:
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

            case CPT_NAMED_SHE:
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

            case CPT_NAMED_IT:
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


            case CPT_HE:
                switch (crn) {
                    case CRN_T1:
                        sprintf(cool_buf, "the %s", name);
                        return cool_buf;

                    case CRN_T2:
                        return "he";

                    case CRN_T3:
                        return "him";

                    case CRN_T4:
                        return "his";
                }

            case CPT_SHE:
                switch (crn) {
                    case CRN_T1:
                        sprintf(cool_buf, "the female %s", name);
                        return cool_buf;

                    case CRN_T2:
                        return "she";

                    case CRN_T3:
                        return "her";

                    case CRN_T4:
                        return "hers";
                }

            case CPT_IT:
                switch (crn) {
                    case CRN_T1:
                        sprintf(cool_buf, "the %s", name);
                        return cool_buf;

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

const char* XCreature::GetVerb(const char* verb)
{
    static char cool_buf[1024]; //hope the name will never larger than

    if (creature_person_type & CPT_YOU) {
        sprintf(cool_buf, "%s", verb);
    } else {
        char lch = verb[strlen(verb) - 1];

        if (lch == 's' || lch == 'h') {
            sprintf(cool_buf, "%ses", verb);
        } else {
            sprintf(cool_buf, "%ss", verb);
        }
    }

    return cool_buf;
}
