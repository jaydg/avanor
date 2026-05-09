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


#include "creature/creature.h"
#include "game/game.h"
#include "game/location.h"
#include "helpers/msgwin.h"
#include "item/itemf.h"
#include "map/map.h"
#include "map/map_objects.h"

#include <fmt/format.h>

extern "C"
{
#include "lauxlib.h"
}

REGISTER_CLASS(XTrap);

XTrap::XTrap(const int _x, const int _y, XLocation* _l, TRAP_LEVEL tl, TRAP_TYPE tt, XCreature* _owner, XItem* items)
{
    SetLocation(_l);
    im = IM_TRAP;
    x = _x;
    y = _y;
    owner = _owner;
    trap_item = items;

    if (tt == TT_RANDOM) {
        tt = static_cast<TRAP_TYPE>(vRand(TT_RANDOM));
    }

    trap_type = tt;

    if (tl == TL_RANDOM) {
        tl = static_cast<TRAP_LEVEL>(vRand(TL_RANDOM));
    }

    trap_level = tl;
    activation_count = vRand(20) + 5;
    isMagic = false;

    switch (tt) {
        case TT_MAGICARROW:
            color = xBROWN;
            isMagic = true;
            break;

        case TT_FIREBOLT:
            color = xRED;
            isMagic = true;
            break;

        case TT_ACIDBOLT:
            color = xGREEN;
            isMagic = true;
            break;

        case TT_ARROW:
            color = xBROWN;

            if (trap_item == nullptr) {
                trap_item = ICREATEB(IM_MISSILE, IT_ARROW, 0, 100000);
                trap_item->quantity = vRand(5) + 5;
            }

            break;

        case TT_TELEPORT:
            color = xLIGHTGREEN;
            isMagic = true;
            break;

        case TT_PIT:
            color = xDARKGRAY;
            isMagic = false;
            break;

        case TT_SPEAR_PIT:
            color = xDARKGRAY;
            isMagic = false;

            if (trap_item == nullptr) {
                trap_item = ICREATEB(IM_WEAPON, IT_SHORTSPEAR, 0, 100000);
                trap_item->quantity = vRand(3) + 2;
            }

            break;

        default:
            color = xBROWN;
            isMagic = true;
            break;
    }

    view = '^';

    if (owner && owner->isHero()) {
        isVisibleForHero = 1;
    } else {
        isVisibleForHero = 0;
    }

    assert(l->map->GetSpecial(x, y) == nullptr);
    l->map->SetSpecial(x, y, this);
    name = "trap";
}

int XTrap::MoveIn(XCreature* cr)
{
    assert(isValid());
    assert(cr->isValid());

    if (cr->isHero()) {
        if (!isVisibleForHero) {
            return Activate(cr);
        }
    } else {
        if (cr->xai->isKnowThisTrap(this)) {
            if (cr->isVisible()) {
                msgwin.Add(cr->GetNameEx(CRN_T1));
                msgwin.Add(cr->GetVerb("avoid"));
                msgwin.Add("a trap.");
            }
        } else {
            return Activate(cr);
        }
    }

    return 0;
}

int XTrap::MoveOut(XCreature* cr)
{
    if (last_activator == cr->guid() && (trap_type == TT_PIT || trap_type == TT_SPEAR_PIT)) {
        // to climb out pits you should be lucky!
        if (vRand(100) < 30 + cr->sk->GetLevel(XSkill::Skill::CLIMBING) * 5 + cr->GetStats(S_DEX) * 2) {
            cr->sk->UseSkill(XSkill::Skill::CLIMBING);

            if (cr->isVisible()) {
                msgwin.Add(cr->GetNameEx(CRN_T1));
                msgwin.Add("managed to climb out from the pit.");
            }

            last_activator = 0;
            return 1;
        }

        if (vRand(100) < 70) {
            cr->sk->UseSkill(XSkill::Skill::CLIMBING);

            if (cr->isVisible()) {
                msgwin.Add(cr->GetNameEx(CRN_T1));
                msgwin.Add("can not to climb out from the pit.");
            }
        } else {
            // no luck!
            Activate(cr);
        }

        return 0;
    }

    last_activator = 0;
    return 1;
}

int XTrap::Activate(XCreature* cr)
{
    if (cr->isVisible()) {
        msgwin.Add(cr->GetNameEx(CRN_T1));

        if (trap_type == TT_PIT || trap_type == TT_SPEAR_PIT) {
            msgwin.Add(cr->GetVerb("fall"));
            msgwin.Add("down to a pit.");
        } else {
            msgwin.Add(cr->GetVerb("activate"));
            msgwin.Add("a trap.");
        }

        isVisibleForHero = 1;
    }

    bool isTrapShouldDestroyed = false;
    last_activator = cr->guid();

    if (isMagic) {
        EFFECT_DATA ed{};
        ed.caller	= owner;
        ed.l	= l;
        ed.power	= 10 * (trap_level + 1);
        ed.call_x	= x;
        ed.call_y	= y;
        ed.target_x	= x;
        ed.target_y	= y;
        ed.target	= cr;

        switch (trap_type) {
            case TT_MAGICARROW:
                ed.effect = E_MAGIC_ARROW;
                break;

            case TT_FIREBOLT:
                ed.effect = E_FIRE_BOLT;
                break;

            case TT_ACIDBOLT:
                ed.effect = E_ACID_BOLT;
                break;

            case TT_TELEPORT:
                ed.effect = E_TELEPORT;
                break;

            default:
                ed.effect = E_MAGIC_ARROW;

        }

        XEffect::Make(&ed);
    } else {
        XItem* drop_item = nullptr;
        DAMAGE_DATA_EX dd{};

        switch (trap_type) {
            case TT_ARROW:
                drop_item = dynamic_cast<XItem*>(trap_item->MakeCopy());
                drop_item->quantity = 1;

                if (trap_item->quantity-- <= 1) {
                    isTrapShouldDestroyed = true;
                }

                dd.damage	= drop_item->dice.Throw();
                dd.attacker	= owner;

                // temporary solution, should be replaced in future with a
                // general solution which returns the name of an item with
                // or without 'a'
                switch (drop_item->it) {
                    case IT_ARROW:
                        dd.attack_name = "the arrow";
                        break;

                    case IT_QUARREL:
                        dd.attack_name = "the quarrel";
                        break;

                    case IT_SHORTSPEAR:
                    case IT_LONGSPEAR:
                        dd.attack_name = "the spear";
                        break;

                    default:
                        break;
                }

                dd.attack_HIT	= 30;
                dd.attack_brand	= drop_item->brt;
                dd.flags	= DF_MAGIC_BOLT;
                cr->InflictDamage(&dd);
                break;

            case TT_PIT:
                dd.damage	= vRand(30) + 2;
                dd.attacker	= owner;
                dd.attack_name	= "the bottom of the pit";
                dd.attack_HIT	= 10000;
                dd.attack_brand	= 0;
                dd.flags	= DF_MAGIC_BOLT;
                cr->InflictDamage(&dd);
                break;

            case TT_SPEAR_PIT: {
                dd.damage	= 0;

                for (int i = 0; i < trap_item->quantity; i++) {
                    dd.damage += trap_item->dice.Throw();
                }

                dd.attacker	= owner;
                dd.attack_name	= "the spears in the pit";
                dd.attack_HIT	= 10000;
                dd.attack_brand	= 0;
                dd.flags	= DF_MAGIC_BOLT;
                cr->InflictDamage(&dd);
            }
            break;

            default:
                break;
        }

        if (drop_item) {
            drop_item->Drop(l, x, y);
        }
    }

    if (isTrapShouldDestroyed || activation_count-- <= 0) {
        if (isInVisibleArea()) {
            msgwin.Add("The trap is broken.");
        }

        l->map->SetSpecial(x, y, nullptr);
        Invalidate();
    }

    return 1;
}

int XTrap::Check(XCreature* cr)
{
    if (isVisibleForHero) {
        return 0;
    }

    XSkill * skill = cr->sk->GetSkill(XSkill::Skill::DETECTTRAP);
    int chance = cr->GetStats(S_PER) * 10;

    if (skill) {
        chance += skill->GetLevel() * 50;
    }

    chance -= trap_level * 100;

    if (vRand() % 1000 < chance) {
        if (cr->isVisible()) {
            msgwin.Add(cr->GetNameEx(CRN_T1));
            msgwin.Add(cr->GetVerb("found"));
            msgwin.Add("a trap.");
            isVisibleForHero = 1;

            if (skill) {
                skill->UseSkill();
            }
        }

        return 1;
    }

    return 0;
}

int XTrap::isVisible(XCreature * cr) const
{
    return isVisibleForHero;
}

int XTrap::Disarm(XCreature * cr)
{
    const XSkill* skill = cr->sk->GetSkill(XSkill::Skill::DISARMTRAP);
    int chance = 10;

    if (skill) {
        chance += skill->GetLevel() * 100;
    }

    chance -= trap_level * 100;
    int val = vRand() % 1000;

    if (val < chance) {
        msgwin.Add(cr->GetNameEx(CRN_T1));
        msgwin.Add(cr->GetVerb("disarm"));
        msgwin.Add("a trap successfully.");
        l->map->SetSpecial(x, y, nullptr);
        Invalidate();

        return 1;
    }

    if (val < static_cast<int>(chance * 1.3)) {
        msgwin.Add(cr->GetNameEx(CRN_T1));
        msgwin.Add("doesn't manage to disarm a trap.");
    } else {
        Activate(cr);
    }

    return 0;
}


void XTrap::Store(XFile * f)
{
    XMapObject::Store(f);
    f->Write(&trap_type, sizeof(TRAP_TYPE));
    f->Write(&isVisibleForHero, sizeof(int));
    f->Write(&trap_level, sizeof(TRAP_LEVEL));
    XObject::StorePointer(f, owner);
    XObject::StorePointer(f, trap_item);
    f->Write(&last_activator, sizeof(XGUID));
    f->Write(&activation_count, sizeof(int));
}

void XTrap::Restore(XFile * f)
{
    XMapObject::Restore(f);
    f->Read(&trap_type, sizeof(TRAP_TYPE));
    f->Read(&isVisibleForHero, sizeof(int));
    f->Read(&trap_level, sizeof(TRAP_LEVEL));
    owner = dynamic_cast<XCreature*>(XObject::RestorePointer(f, nullptr));
    trap_item = dynamic_cast<XItem*>(XObject::RestorePointer(f, nullptr));
    f->Read(&last_activator, sizeof(XGUID));
    f->Read(&activation_count, sizeof(int));
}

REGISTER_CLASS(XStairWay);

XStairWay::XStairWay(const int _x, const int _y, XLocation* loc, const LOCATION _ln, const STAIRWAY_TYPE type)
{
    nx = -1;
    ny = -1;
    x = _x;
    y = _y;
    ln = _ln;
    ttm = 0;
    color = xLIGHTGRAY;
    im = IM_WAY;
    l = loc;

    assert(loc->map->GetSpecial(x, y) == nullptr);

    switch (type) {
        case STW_UP:
            view = '<';
            loc->map->SetSpecial(x, y, this);
            name = "way up";
            break;

        case STW_DOWN:
            view = '>';
            loc->map->SetSpecial(x, y, this);
            name = "way down";
            break;

        default:
            assert(false);
            break;
    }
}

void XStairWay::Bind(XStairWay * way)
{
    nx = way->x;
    ny = way->y;
    way->nx = x;
    way->ny = y;
}

void XStairWay::Store(XFile * f)
{
    XMapObject::Store(f);
    f->Write(&ln, sizeof(LOCATION));
}

void XStairWay::Restore(XFile * f)
{
    XMapObject::Restore(f);
    f->Read(&ln, sizeof(LOCATION));
}

REGISTER_CLASS(XTeleport);

XTeleport::XTeleport(const int _x, const int _y, XLocation* loc, const LOCATION _ln, const int _nx, const int _ny)
{
    nx = _nx;
    ny = _ny;
    x = _x;
    y = _y;
    ln = _ln;
    ttm = 0;
    color = xWHITE;
    im = IM_TELEPORT;
    l = loc;
    assert(loc->map->GetSpecial(x, y) == nullptr);

    view = '0';
    loc->map->SetSpecial(x, y, this);
    name = "magic circle";
}

void XTeleport::Store(XFile* f)
{
    XMapObject::Store(f);
    f->Write(&ln, sizeof(LOCATION));
}

void XTeleport::Restore(XFile* f)
{
    XMapObject::Restore(f);
    f->Read(&ln, sizeof(LOCATION));
}

int XTeleport::MoveIn(XCreature* cr)
{
    if (!cr->isHero()) {
        return 1; // Citizens shouldn't want to go visit the village...
    }

    if (Game.locations[ln]->map->XGetMovability(nx, ny) == 0) {
        cr->LastStep();
        cr->FirstStep(nx, ny, Game.locations[ln]);
    }

    return 1;
}

REGISTER_CLASS(XDoor);

XDoor::XDoor(const int _x, const int _y, const int flg, XLocation* _l)
{
    SetLocation(_l);
    im = IM_DOOR;
    x = _x;
    y = _y;
    isOpened = !flg;
    Switch();
    color = xBROWN;

    assert(l->map->GetSpecial(x, y) == nullptr);
    l->map->SetSpecial(x, y, this);
    name = "door";
}

void XDoor::Switch()
{
    if (isOpened) {
        isOpened = 0;
        view = '+';
    } else {
        isOpened = 1;
        view = '/';
    }
}

void XDoor::Store(XFile* f)
{
    XMapObject::Store(f);
    f->Write(&isOpened, sizeof(int));

}

void XDoor::Restore(XFile* f)
{
    XMapObject::Restore(f);
    f->Read(&isOpened, sizeof(int));
}

REGISTER_CLASS(XAltar);

XAltar::XAltar(const int _x, const int _y, const DEITY deity, XLocation* _l)
{
    SetLocation(_l);
    im = IM_ALTAR;
    x = _x;
    y = _y;

    if (deity == D_LIFE) {
        color = xWHITE;
    } else {
        color = xDARKGRAY;
    }

    view = '_';

    assert(l->map->GetSpecial(x, y) == nullptr);
    l->map->SetSpecial(x, y, this);
    name = "altar";
}

void XAltar::Store(XFile * f)
{
    XMapObject::Store(f);
}

void XAltar::Restore(XFile * f)
{
    XMapObject::Restore(f);
}

REGISTER_CLASS(XGrave);

XGrave::XGrave(const int _x, const int _y, char* subscr, XLocation* _l)
{
    SetLocation(_l);
    im = IM_MISC;
    x = _x;
    y = _y;
    color = xLIGHTGRAY;
    view = '+';
    assert(l->map->GetSpecial(x, y) == nullptr);
    l->map->SetSpecial(x, y, this);
    name = fmt::format("the grave signed '{}'", subscr);
    isOpened = 0;
}

void XGrave::HideItem(XItem* item)
{
    hidden_items.insert(item);
}

int XGrave::onOuterUse(XCreature* cr)
{
    if (cr->isHero()) {
        if (isOpened) {
            msgwin.Add("This grave has already been looted.");
        } else {
            msgwin.Add("You move the headstone. Something glimmers in the dark.");
        }
    }

    for (const auto item: hidden_items) {
        item->Drop(l, x, y);
        hidden_items.erase(item);
    }

    isOpened = 1;
    return 1;
}

void XGrave::Store(XFile* f)
{
    XMapObject::Store(f);
    f->Write(&isOpened);

    // FIXME: Implement when porting saving/restoring to Cereal
    // hidden_items.StoreList(f);
}

void XGrave::Restore(XFile* f)
{
    XMapObject::Restore(f);
    f->Read(&isOpened);

    // FIXME: Implement when porting saving/restoring to Cereal
    // hidden_items.RestoreList(f);
}

REGISTER_CLASS(XFurniture);

XFurniture::XFurniture(const int _x, const int _y, const int _c, const char _v, const char* subscr, XLocation* _l)
{
    SetLocation(_l);
    im = IM_MISC;
    x = _x;
    y = _y;
    color = _c;
    view = _v;
    assert(l->map->GetSpecial(x, y) == nullptr);
    l->map->SetSpecial(x, y, this);
    name = subscr;
}

REGISTER_CLASS(XOuterObject);
XOuterObject::XOuterObject(const int _x, const int _y, const int _c, const char _v, const char* subscr, XLocation* _l, const char* event)
{
    SetLocation(_l);
    im = IM_MISC;
    x = _x;
    y = _y;

    SetName(subscr);
    SetView(_v, _c);
    assert(l->map->GetSpecial(x, y) == nullptr);
    l->map->SetSpecial(x, y, this);

    if (event) {
        onEventLua = new char[strlen(event) + 1];
        strcpy(onEventLua, event);
    } else {
        onEventLua = nullptr;
    }
}

XOuterObject::~XOuterObject()
{
    delete[] onEventLua;
}

int XOuterObject::onOuterUse(XCreature* cr)
{
    if (onEventLua) {
        lua_pushstring(XLocation::L, onEventLua);
        lua_gettable(XLocation::L, LUA_GLOBALSINDEX);
        lua_pushnumber(XLocation::L, LE_OUTER_USE);
        lua_pushlightuserdata(XLocation::L, cr);
        lua_pushlightuserdata(XLocation::L, this);
        lua_call(XLocation::L, 3, 1);
        const int res = static_cast<int>(lua_tonumber(XLocation::L, 2));
        lua_pop(XLocation::L, 1);

        return res;
    }

    return XMapObject::onOuterUse(cr);
}

void XOuterObject::Store(XFile* f)
{
    XMapObject::Store(f);
    int sz = 0;

    if (onEventLua) {
        sz = strlen(onEventLua) + 1;
    }

    f->Write(&sz);

    if (sz > 0) {
        f->Write(onEventLua, sz);
    }

    if (onEventLua) {
        lua_pushstring(XLocation::L, onEventLua);
        lua_gettable(XLocation::L, LUA_GLOBALSINDEX);
        lua_pushnumber(XLocation::L, LE_SAVE);
        lua_call(XLocation::L, 1, 1);
        lua_tonumber(XLocation::L, 2);
        lua_pop(XLocation::L, 1);
    }
}

void XOuterObject::Restore(XFile* f)
{
    XMapObject::Restore(f);

    int sz = 0;
    f->Read(&sz);

    if (sz > 0) {
        onEventLua = new char [sz];
        f->Read(onEventLua, sz);
    } else {
        onEventLua = nullptr;
    }

    if (onEventLua) {
        lua_pushstring(XLocation::L, onEventLua);
        lua_gettable(XLocation::L, LUA_GLOBALSINDEX);
        lua_pushnumber(XLocation::L, LE_LOAD);
        lua_call(XLocation::L, 1, 1);
        lua_tonumber(XLocation::L, 2);
        lua_pop(XLocation::L, 1);
    }
}
