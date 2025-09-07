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
#include "map/map.h"
#include "map/map_objects.h"

extern "C"
{
#include "lauxlib.h"
}


REGISTER_CLASS(XTrap);

XTrap::XTrap(int _x, int _y, XLocation * _l, TRAP_LEVEL tl, TRAP_TYPE tt, XCreature * _owner, XItem * items)
{
    SetLocation(_l);
    im = IM_TRAP;
    x = _x;
    y = _y;
    owner = _owner;
    trap_item = items;

    if (tt == TT_RANDOM) {
        tt = (TRAP_TYPE)vRand(TT_RANDOM);
    }

    trap_type = tt;

    if (tl == TL_RANDOM) {
        tl = (TRAP_LEVEL)vRand(TL_RANDOM);
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

            if (trap_item == NULL) {
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

            if (trap_item == NULL) {
                trap_item = ICREATEB(IM_WEAPON, IT_SHORTSPEAR, 0, 100000);
                trap_item->quantity = vRand(3) + 2;
            }

            break;

        default:
            color = xBROWN;
            isMagic = true;
            tt = TT_MAGICARROW;
            break;
    }

    view = '^';

    if (owner && owner->isHero()) {
        isVisibleForHero = 1;
    } else {
        isVisibleForHero = 0;
    }

    assert(l->map->GetSpecial(x, y) == NULL);
    l->map->SetSpecial(x, y, this);
    strcpy(name, "trap");
}

int XTrap::MoveIn(XCreature * cr)
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

int XTrap::MoveOut(XCreature * cr)
{
    if (last_activator == cr->xguid && (trap_type == TT_PIT || trap_type == TT_SPEAR_PIT)) {
        //to climb out from some pits you should be lucky!
        if (vRand(100) < 30 + cr->sk->GetLevel(SKT_CLIMBING) * 5 + cr->GetStats(S_DEX) * 2) {
            cr->sk->UseSkill(SKT_CLIMBING);

            if (cr->isVisible()) {
                msgwin.Add(cr->GetNameEx(CRN_T1));
                msgwin.Add("managed to climb out from the pit.");
            }

            last_activator = 0;
            return 1;
        } else {
            if (vRand(100) < 70) {
                cr->sk->UseSkill(SKT_CLIMBING);

                if (cr->isVisible()) {
                    msgwin.Add(cr->GetNameEx(CRN_T1));
                    msgwin.Add("can not to climb out from the pit.");
                }
            } else { //no luck!
                Activate(cr);
            }
        }

        return 0;
    }

    last_activator = 0;
    return 1;
}

int XTrap::Activate(XCreature * cr)
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
    last_activator = cr->xguid;

    if (isMagic) {
        EFFECT_DATA ed;
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
        int dmg = 0;
        XItem * drop_item = NULL;
        DAMAGE_DATA_EX dd;

        switch (trap_type) {
            case TT_ARROW:
                drop_item = (XItem*)trap_item->MakeCopy();
                drop_item->quantity = 1;

                if (trap_item->quantity-- <= 1) {
                    isTrapShouldDestroyed = true;
                }

                dd.damage	= drop_item->dice.Throw();
                dd.attacker	= owner;

                //temporary soulution, should be replaced in future on general solution
                //which returns name of item with or without 'a'
                switch (drop_item->it) {
                    case IT_ARROW:
                        dd.attack_name = "the arrow";
                        break;

                    case IT_QUARREL:
                        dd.attack_name = "the quarrel";
                        break;

                    case IT_SHORTSPEAR:
                        dd.attack_name = "the spear";
                        break;

                    case IT_LONGSPEAR:
                        dd.attack_name = "the spear";
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
        }

        if (drop_item) {
            drop_item->Drop(l, x, y);
        }
    }

    if (isTrapShouldDestroyed || activation_count-- <= 0) {
        if (isInVisibleArea()) {
            msgwin.Add("The trap is broken.");
        }

        l->map->SetSpecial(x, y, NULL);
        Invalidate();
    }

    return 1;
}

int XTrap::Check(XCreature * cr)
{
    if (isVisibleForHero) {
        return 0;
    }

    XSkill * skill = cr->sk->GetSkill(SKT_DETECTTRAP);
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
            skill->UseSkill();
        }

        return 1;
    }

    return 0;
}

int XTrap::isVisible(XCreature * cr)
{
    return isVisibleForHero;
}

int XTrap::Disarm(XCreature * cr)
{
    XSkill * skill = cr->sk->GetSkill(SKT_DISARMTRAP);
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
        l->map->SetSpecial(x, y, NULL);
        Invalidate();
        return 1;
    } else if (val < (int)(chance * 1.3)) {
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
    owner = (XCreature*)XObject::RestorePointer(f, NULL);
    trap_item = (XItem*)XObject::RestorePointer(f, NULL);
    f->Read(&last_activator, sizeof(XGUID));
    f->Read(&activation_count, sizeof(int));
}

REGISTER_CLASS(XStairWay);

XStairWay::XStairWay(int _x, int _y, XLocation * loc, LOCATION _ln, STAIRWAYTYPE type)
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

    assert(loc->map->GetSpecial(x, y) == NULL);

    switch (type) {
        case STW_UP:
            view = '<';
            loc->map->SetSpecial(x, y, this);
            strcpy(name, "way up");
            break;

        case STW_DOWN:
            view = '>';
            loc->map->SetSpecial(x, y, this);
            strcpy(name, "way down");
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

XTeleport::XTeleport(int _x, int _y, XLocation * loc, LOCATION _ln, int _nx, int _ny)
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
    assert(loc->map->GetSpecial(x, y) == NULL);

    view = '0';
    loc->map->SetSpecial(x, y, this);
    strcpy(name, "magic circle");
}

void XTeleport::Store(XFile * f)
{
    XMapObject::Store(f);
    f->Write(&ln, sizeof(LOCATION));
}

void XTeleport::Restore(XFile * f)
{
    XMapObject::Restore(f);
    f->Read(&ln, sizeof(LOCATION));
}

int XTeleport::MoveIn(XCreature * cr)
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

XDoor::XDoor(int _x, int _y, int flg, XLocation * _l)
{
    SetLocation(_l);
    im = IM_DOOR;
    x = _x;
    y = _y;
    isOpened = !flg;
    Switch();
    color = xBROWN;

    assert(l->map->GetSpecial(x, y) == NULL);
    l->map->SetSpecial(x, y, this);
    strcpy(name, "door");
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

void XDoor::Store(XFile * f)
{
    XMapObject::Store(f);
    f->Write(&isOpened, sizeof(int));

}

void XDoor::Restore(XFile * f)
{
    XMapObject::Restore(f);
    f->Read(&isOpened, sizeof(int));
}

REGISTER_CLASS(XAltar);

XAltar::XAltar(int _x, int _y, DEITY deity, XLocation * _l)
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

    assert(l->map->GetSpecial(x, y) == NULL);
    l->map->SetSpecial(x, y, this);
    strcpy(name, "altar");
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

XGrave::XGrave(int _x, int _y, char* subscr, XLocation * _l)
{
    SetLocation(_l);
    im = IM_MISC;
    x = _x;
    y = _y;
    color = xLIGHTGRAY;
    view = '+';
    assert(l->map->GetSpecial(x, y) == NULL);
    l->map->SetSpecial(x, y, this);
    sprintf(name, "the grave signed '%s'", subscr);
    isOpened = 0;
}

void XGrave::HideItem(XItem * item)
{
    hiden_items.push_back(item);
}

int XGrave::onOuterUse(XCreature * cr)
{
    if (cr->isHero()) {
        if (isOpened) {
            msgwin.Add("This grave has already been looted.");
        } else {
            msgwin.Add("You move the headstone. Something glimmers in the dark.");
        }
    }

    while (!hiden_items.empty()) {
        hiden_items.begin()->Drop(l, x, y);
        hiden_items.pop_front();
    }

    isOpened = 1;
    return 1;
}

void XGrave::Store(XFile * f)
{
    XMapObject::Store(f);
    f->Write(&isOpened);
    hiden_items.StoreList(f);
}

void XGrave::Restore(XFile * f)
{
    XMapObject::Restore(f);
    f->Read(&isOpened);
    hiden_items.RestoreList(f);
}

REGISTER_CLASS(XFurniture);

XFurniture::XFurniture(int _x, int _y, int _c, char _v, char* subscr, XLocation * _l)
{
    SetLocation(_l);
    im = IM_MISC;
    x = _x;
    y = _y;
    color = _c;
    view = _v;
    assert(l->map->GetSpecial(x, y) == NULL);
    l->map->SetSpecial(x, y, this);
    strcpy(name, subscr);
}

REGISTER_CLASS(XOuterObject);
XOuterObject::XOuterObject(int _x, int _y, int _c, char _v, char* subscr, XLocation * _l, const char* event)
{
    SetLocation(_l);
    im = IM_MISC;
    x = _x;
    y = _y;

    SetName(subscr);
    SetView(_v, _c);
    assert(l->map->GetSpecial(x, y) == NULL);
    l->map->SetSpecial(x, y, this);

    if (event) {
        onEventLua = new char[strlen(event) + 1];
        strcpy(onEventLua, event);
    } else {
        onEventLua = NULL;
    }
}

XOuterObject::~XOuterObject()
{
    if (onEventLua) {
        delete[] onEventLua;
    }
}

int XOuterObject::onOuterUse(XCreature * cr)
{
    if (onEventLua) {
        lua_pushstring(XLocation::L, onEventLua);
        lua_gettable(XLocation::L, LUA_GLOBALSINDEX);
        lua_pushnumber(XLocation::L, LE_OUTER_USE);
        lua_pushlightuserdata(XLocation::L, cr);
        lua_pushlightuserdata(XLocation::L, this);
        lua_call(XLocation::L, 3, 1);
        int res = lua_tonumber(XLocation::L, 2);
        lua_pop(XLocation::L, 1);
        return res;
    } else {
        return XMapObject::onOuterUse(cr);
    }
}

void XOuterObject::Store(XFile * f)
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
        int res = lua_tonumber(XLocation::L, 2);
        lua_pop(XLocation::L, 1);
    }
}

void XOuterObject::Restore(XFile * f)
{
    XMapObject::Restore(f);

    int sz = 0;
    f->Read(&sz);

    if (sz > 0) {
        onEventLua = new char [sz];
        f->Read(onEventLua, sz);
    } else {
        onEventLua = NULL;
    }

    if (onEventLua) {
        lua_pushstring(XLocation::L, onEventLua);
        lua_gettable(XLocation::L, LUA_GLOBALSINDEX);
        lua_pushnumber(XLocation::L, LE_LOAD);
        lua_call(XLocation::L, 1, 1);
        int res = lua_tonumber(XLocation::L, 2);
        lua_pop(XLocation::L, 1);
    }
}
