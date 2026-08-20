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

#include "engine/xlua.h"
#include "creature/creature.h"
#include "game/game.h"
#include "game/location.h"
#include "helpers/msgwin.h"
#include "item/itemf.h"
#include "map/map.h"
#include "map/map_objects.h"

#include <fmt/format.h>
#include <cereal/archives/json.hpp>
#include <cereal/types/polymorphic.hpp>

#include <sol/sol.hpp>

REGISTER_CLASS(XTrap);
CEREAL_REGISTER_TYPE(XTrap);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XMapObject, XTrap);
// XTrap()'s only no-args constructor is an assert(0) guard - real
// instances always come from the parameterized constructor, so route
// Cereal's load-time construction through the DUMMY_STRUCT idiom
// instead of that assert.
CEREAL_LOAD_VIA_DUMMY_CONSTRUCT(XTrap, serialize);

XTrap::XTrap(const int _x, const int _y, XLocation* _l, XTrap::Level tl, XTrap::Type tt, XCreature* _owner, XItem* items)
{
    SetLocation(_l);
    x = _x;
    y = _y;
    owner = XCreature::ToWeakPtr(_owner);
    trap_item = XItem::Own(items);

    if (tt == XTrap::Type::RANDOM) {
        tt = static_cast<XTrap::Type>(vRand(static_cast<unsigned long>(XTrap::Type::RANDOM)));
    }

    trap_type = tt;

    if (tl == XTrap::Level::RANDOM) {
        tl = static_cast<XTrap::Level>(vRand(static_cast<unsigned long>(XTrap::Level::RANDOM)));
    }

    trap_level = tl;
    activation_count = vRand(20) + 5;
    isMagic = false;

    switch (tt) {
        case XTrap::Type::MAGICARROW:
            color = xBROWN;
            isMagic = true;
            break;

        case XTrap::Type::FIREBOLT:
            color = xRED;
            isMagic = true;
            break;

        case XTrap::Type::ACIDBOLT:
            color = xGREEN;
            isMagic = true;
            break;

        case XTrap::Type::ARROW:
            color = xBROWN;

            if (trap_item == nullptr) {
                trap_item = XItem::Own(ICREATEB(ItemKind::MISSILE, ItemType::ARROW, 0, 100000));
                trap_item->quantity = vRand(5) + 5;
            }

            break;

        case XTrap::Type::TELEPORT:
            color = xLIGHTGREEN;
            isMagic = true;
            break;

        case XTrap::Type::PIT:
            color = xDARKGRAY;
            isMagic = false;
            break;

        case XTrap::Type::SPEAR_PIT:
            color = xDARKGRAY;
            isMagic = false;

            if (trap_item == nullptr) {
                trap_item = XItem::Own(ICREATEB(ItemKind::WEAPON, ItemType::SHORTSPEAR, 0, 100000));
                trap_item->quantity = vRand(3) + 2;
            }

            break;

        default:
            color = xBROWN;
            isMagic = true;
            break;
    }

    view = '^';

    if (auto o = owner.lock(); o && o->isHero()) {
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
    if (last_activator == cr->guid() && (trap_type == XTrap::Type::PIT || trap_type == XTrap::Type::SPEAR_PIT)) {
        // to climb out pits you should be lucky!
        if (vRand(100) < 30 + cr->sk->GetLevel(XSkill::Skill::CLIMBING) * 5 + cr->GetStats(XStats::DEX) * 2) {
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

        if (trap_type == XTrap::Type::PIT || trap_type == XTrap::Type::SPEAR_PIT) {
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
        ed.caller	= owner.lock().get();
        ed.l	= l;
        ed.power	= 10 * (static_cast<int>(trap_level) + 1);
        ed.call_x	= x;
        ed.call_y	= y;
        ed.target_x	= x;
        ed.target_y	= y;
        ed.target	= cr;

        switch (trap_type) {
            case XTrap::Type::MAGICARROW:
                ed.effect = XEffect::MAGIC_ARROW;
                break;

            case XTrap::Type::FIREBOLT:
                ed.effect = XEffect::FIRE_BOLT;
                break;

            case XTrap::Type::ACIDBOLT:
                ed.effect = XEffect::ACID_BOLT;
                break;

            case XTrap::Type::TELEPORT:
                ed.effect = XEffect::TELEPORT;
                break;

            default:
                ed.effect = XEffect::MAGIC_ARROW;

        }

        XEffect::Make(&ed);
    } else {
        XItem* drop_item = nullptr;
        DAMAGE_DATA_EX dd{};

        switch (trap_type) {
            case XTrap::Type::ARROW:
                drop_item = trap_item->MakeCopy();
                drop_item->quantity = 1;

                if (trap_item->quantity-- <= 1) {
                    isTrapShouldDestroyed = true;
                }

                dd.damage	= drop_item->dice.Throw();
                dd.attacker	= owner.lock().get();

                // temporary solution, should be replaced in future with a
                // general solution which returns the name of an item with
                // or without 'a'
                switch (drop_item->it) {
                    case ItemType::ARROW:
                        dd.attack_name = "the arrow";
                        break;

                    case ItemType::QUARREL:
                        dd.attack_name = "the quarrel";
                        break;

                    case ItemType::SHORTSPEAR:
                    case ItemType::LONGSPEAR:
                        dd.attack_name = "the spear";
                        break;

                    default:
                        break;
                }

                dd.attack_HIT = 30;
                dd.attack_effect = drop_item->aet;
                dd.flags = DF_MAGIC_BOLT;
                cr->InflictDamage(&dd);
                break;

            case XTrap::Type::PIT:
                dd.damage	= vRand(30) + 2;
                dd.attacker	= owner.lock().get();
                dd.attack_name	= "the bottom of the pit";
                dd.attack_HIT	= 10000;
                dd.attack_effect	= AttackEffectType::NONE;
                dd.flags	= DF_MAGIC_BOLT;
                cr->InflictDamage(&dd);
                break;

            case XTrap::Type::SPEAR_PIT: {
                dd.damage	= 0;

                for (int i = 0; i < trap_item->quantity; i++) {
                    dd.damage += trap_item->dice.Throw();
                }

                dd.attacker	= owner.lock().get();
                dd.attack_name	= "the spears in the pit";
                dd.attack_HIT	= 10000;
                dd.attack_effect	= AttackEffectType::NONE;
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

        // Invalidate() handles the map-cell eviction itself now (see
        // XMapObject::Invalidate() + the deferred-release graveyard) -
        // no separate SetSpecial() call, no ordering to get wrong.
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
    int chance = cr->GetStats(XStats::PER) * 10;

    if (skill) {
        chance += skill->GetLevel() * 50;
    }

    chance -= static_cast<int>(trap_level) * 100;

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

    chance -= static_cast<int>(trap_level) * 100;
    int val = vRand() % 1000;

    if (val < chance) {
        msgwin.Add(cr->GetNameEx(CRN_T1));
        msgwin.Add(cr->GetVerb("disarm"));
        msgwin.Add("a trap successfully.");

        // Invalidate() handles the map-cell eviction itself (see
        // XMapObject::Invalidate()).
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

void XTrap::OnInvalidate()
{
    if (trap_item) {
        trap_item->Invalidate();
        trap_item = nullptr;
    }

    XMapObject::OnInvalidate();
}

REGISTER_CLASS(XStairWay);
CEREAL_REGISTER_TYPE(XStairWay);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XMapObject, XStairWay);

void XStairWay::RegisterLua(sol::state_view& lua)
{
    lua.new_enum("XStairWay",
        "UP", Type::UP,
        "DOWN", Type::DOWN
    );
}

XStairWay::XStairWay(const int _x, const int _y, XLocation* loc, const std::string& _ln, const Type type)
{
    // Unbound until Bind() finds the matching stairway on the other side.
    dest_x = -1;
    dest_y = -1;
    x = _x;
    y = _y;
    ln = _ln;
    ttm = 0;
    color = xLIGHTGRAY;
    l = loc;

    assert(loc->map->GetSpecial(x, y) == nullptr);

    switch (type) {
        case Type::UP:
            view = '<';
            loc->map->SetSpecial(x, y, this);
            name = "way up";
            break;

        case Type::DOWN:
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
    dest_x = way->x;
    dest_y = way->y;
    way->dest_x = x;
    way->dest_y = y;
}

REGISTER_CLASS(XTeleport);
CEREAL_REGISTER_TYPE(XTeleport);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XMapObject, XTeleport);

XTeleport::XTeleport(const int _x, const int _y, XLocation* loc, const std::string& _ln, const int _dest_x, const int _dest_y)
{
    dest_x = _dest_x;
    dest_y = _dest_y;
    x = _x;
    y = _y;
    ln = _ln;
    ttm = 0;
    color = xWHITE;
    l = loc;
    assert(loc->map->GetSpecial(x, y) == nullptr);

    view = '0';
    loc->map->SetSpecial(x, y, this);
    name = "magic circle";
}

int XTeleport::MoveIn(XCreature* cr)
{
    if (!cr->isHero()) {
        return 1; // Citizens shouldn't want to go visit the village...
    }

    if (Game.Location(ln)->map->XGetMovability(dest_x, dest_y) == 0) {
        cr->LastStep();
        cr->FirstStep(dest_x, dest_y, Game.Location(ln).get());
    }

    return 1;
}

REGISTER_CLASS(XDoor);
CEREAL_REGISTER_TYPE(XDoor);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XMapObject, XDoor);

XDoor::XDoor(const int _x, const int _y, const int flg, XLocation* _l)
{
    SetLocation(_l);
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

REGISTER_CLASS(XAltar);
CEREAL_REGISTER_TYPE(XAltar);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XMapObject, XAltar);

XAltar::XAltar(const int _x, const int _y, const XDeity::Id deity, XLocation* _l)
{
    const bool placed = PlaceAt(_l, _x, _y);
    assert(placed);

    if (deity != XDeity::LIFE) {
        color = xDARKGRAY;
    }
}

bool XAltar::PlaceAt(XLocation* location, const int _x, const int _y)
{
    if (!XMapObject::PlaceAt(location, _x, _y)) {
        return false;
    }

    view = '_';
    name = "altar";

    // White is the LIFE altar; XDeity::DEATH is the dark one. Script
    // chooses by overriding this with SetView().
    color = xWHITE;

    return true;
}

REGISTER_CLASS(XGrave);
CEREAL_REGISTER_TYPE(XGrave);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XMapObject, XGrave);

XGrave::XGrave(const int _x, const int _y, char* subscr, XLocation* _l)
{
    SetLocation(_l);
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
    hidden_items.insert(XItem::Own(item));
}

void XGrave::OnInvalidate()
{
    // Same idiom as XChest::Invalidate(): XItem::Invalidate() has no
    // idea this set exists (it only knows how to remove itself from
    // l->map's per-cell item_list), so there's no self-removal to race
    // against - just mark every item invalid and let hidden_items' own
    // destructor release the references afterward.
    for (auto& item : hidden_items) {
        item->Invalidate();
    }

    XMapObject::OnInvalidate();
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

    // Pre-existing bug, fixed in passing: erasing from hidden_items while
    // range-for iterating it invalidated the iterator on the very next
    // increment. Use the erase-returns-next-iterator form instead.
    for (auto it = hidden_items.begin(); it != hidden_items.end();) {
        (*it)->Drop(l, x, y);
        it = hidden_items.erase(it);
    }

    isOpened = 1;
    return 1;
}

REGISTER_CLASS(XFurniture);
CEREAL_REGISTER_TYPE(XFurniture);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XMapObject, XFurniture);

XFurniture::XFurniture(const int _x, const int _y, const int _c, const char _v, const char* subscr, XLocation* _l)
{
    SetLocation(_l);
    x = _x;
    y = _y;
    color = _c;
    view = _v;
    assert(l->map->GetSpecial(x, y) == nullptr);
    l->map->SetSpecial(x, y, this);
    name = subscr;
}

REGISTER_CLASS(XOuterObject);
CEREAL_REGISTER_TYPE(XOuterObject);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XMapObject, XOuterObject);
XOuterObject::XOuterObject(const int _x, const int _y, const int _c, const char _v, const char* subscr, XLocation* _l, const char* event)
{
    SetLocation(_l);
    x = _x;
    y = _y;

    SetName(subscr);
    SetView(_v, _c);
    assert(l->map->GetSpecial(x, y) == nullptr);
    l->map->SetSpecial(x, y, this);

    if (event) {
        onEventLua = event;
    }
}

int XOuterObject::onOuterUse(XCreature* cr)
{
    if (onEventLua.empty()) {
        return XMapObject::onOuterUse(cr);
    }

    sol::state_view lua(XLua::State());
    sol::protected_function_result result = lua[onEventLua](LuaEvent::OUTER_USE, (void*)cr, (void*)this);

    if (!result.valid()) {
        return 0;
    }

    return result.get<sol::optional<int>>().value_or(0);
}

