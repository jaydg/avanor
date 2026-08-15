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

#include <fmt/format.h>

#include <cereal/archives/json.hpp>
#include <cereal/types/polymorphic.hpp>
#include <sol/sol.hpp>

#include "creature/creature.h"
#include "item/item.h"
#include "magic/attack_effect_type.h"

void XItem::RegisterLua(sol::state_view& lua)
{
    lua.new_enum("ItemKind",
        "HAT", ItemKind::HAT,
        "NECK", ItemKind::NECK,
        "BODY", ItemKind::BODY,
        "CLOAK", ItemKind::CLOAK,
        "WEAPON", ItemKind::WEAPON,
        "SHIELD", ItemKind::SHIELD,
        "HAND", ItemKind::HAND,
        "GLOVES", ItemKind::GLOVES,
        "RING", ItemKind::RING,
        "BOOTS", ItemKind::BOOTS,
        "MISSILEW", ItemKind::MISSILEW,
        "MISSILE", ItemKind::MISSILE,
        "POTION", ItemKind::POTION,
        "SCROLL", ItemKind::SCROLL,
        "BOOK", ItemKind::BOOK,
        "WAND", ItemKind::WAND,
        "FOOD", ItemKind::FOOD,
        "LIGHTSOURCE", ItemKind::LIGHTSOURCE,
        "TOOL", ItemKind::TOOL,
        "GEM", ItemKind::GEM,
        "MONEY", ItemKind::MONEY,
        "STACKABLE", ItemKind::STACKABLE,
        "CHEST", ItemKind::CHEST,
        "ARMOUR", ItemKind::ARMOUR,
        "ITEM", ItemKind::ITEM
    );

    // Real C++ properties/methods, reachable from Lua via AsItem(void*) on
    // any existing void* item handle (see XCreature::RegisterLua for why
    // void* dispatch itself couldn't just switch to passing typed
    // pointers).
    lua.new_usertype<XItem>("XItem",
        "it", &XItem::it,
        "kind", &XItem::kind,
        "quantity", &XItem::quantity,
        "GetValue", &XItem::GetValue,
        "guid", &XObject::guid
    );
}

// XItem is never itself a dynamic type either (every actual item is a
// concrete subclass, each with its own CEREAL_REGISTER_TYPE against
// XItem) - this just extends the polymorphic pointer-cast chain one
// more hop, up to XObject, for XScheduler::Entry's shared_ptr<XObject>.
CEREAL_REGISTER_POLYMORPHIC_RELATION(XBaseObject, XItem);

XItem::XItem()
{
    total_it++;

    ttm = 0;
    ttmb = 0;
    durability = 10;

    is_selected = 0;

    view = '*';
    color = xBLUE;
    it = ItemType::UNKNOWN;
    kind = ItemKind::UNKNOWN;
    quantity = 1;
    wt = XWarSkills::OTHER;
    bp = BP_OTHER;
    identify = 1;
    _DV = _PV = _HIT = RNG = 0;
    dice.Setup("0d0");
    special_number = -1;
    aet = AttackEffectType::NONE;
    owner.reset();
}

std::weak_ptr<XItem> XItem::ToWeakPtr(XItem * it)
{
    // Mirrors XCreature::ToWeakPtr: an item isn't shared_from_this()-safe
    // until something has actually wrapped it via Own() - guard against
    // that rather than letting shared_from_this() throw std::bad_weak_ptr.
    if (it && it->isValid() && !it->weak_from_this().expired()) {
        return std::static_pointer_cast<XItem>(it->shared_from_this());
    }

    return {};
}

std::shared_ptr<XItem> XItem::Own(XItem * raw)
{
    if (!raw) {
        return {};
    }

    // First time this item is handed to something that will hold it long
    // term (contain/item_list/chest-contain insertion, a trap's ammo
    // template, ...): nothing owns it yet, so this is the one place its
    // master shared_ptr gets constructed - mirrors XMap::SetMonster's birth
    // path for XCreature. Every later hand-off just transfers a fresh
    // shared_ptr from the same control block via shared_from_this(), never
    // a second, independent one.
    if (raw->weak_from_this().expired()) {
        // No XPtr<XItem> cross-reference exists anywhere anymore (all
        // migrated to owning shared_ptr: XBodyPart::item, ACTION_DATA::item,
        // XTool::cooked_item, XTrap::trap_item), so nothing can still be
        // holding a legacy reference by the time this deleter runs - safe
        // to delete unconditionally once Invalidate() has run (or already
        // had, on a previous pass). Mirrors XMap::SetMonster's deleter.
        return std::shared_ptr<XItem>(raw, [](XItem * p) {
            if (p->isValid()) {
                p->Invalidate();
            } else {
                delete p;
            }
        });
    }

    return std::static_pointer_cast<XItem>(raw->shared_from_this());
}

void XItem::OnInvalidate()
{
    // If this item is currently lying on the ground, its map cell's
    // item_list holds the item's master shared_ptr (see XItem::Own()).
    // Capture that list now, before XBaseObject::OnInvalidate() below
    // clears our location. l->map may already be gone (its location's
    // teardown got there first and already reclaimed every item on it),
    // in which case there's nothing left to remove ourselves from.
    //
    // Erasing below can drop the item's last reference, but that is safe
    // from here: XObject::Invalidate() has already cleared is_valid (so
    // Own()'s deleter plain-deletes rather than re-entering Invalidate())
    // and holds its own strong reference across this call (so the actual
    // destruction happens after we return, not under our feet).
    XItemList* ground_list = (l && l->map && x >= 0 && y >= 0) ? l->map->GetItemList(x, y) : nullptr;

    // The mirror image of the ground case, for an item being carried.
    // A carried item has no `l`, so the lookup above finds nothing, and
    // for a long time that meant nothing detached it from its carrier
    // either: invalidating a carried item left a dead entry sitting in
    // contain, which XCreature::Die() would then Drop() onto the floor -
    // and a ground list can never clean such an item up again (it is the
    // state that used to wedge ~XMapTile()'s teardown loop).
    //
    // Detach through the `owner` back-reference instead. Deliberately
    // not a new "which list am I in" pointer on XItem: `owner` is
    // already maintained by SetOwner()/CarryItem() and already survives
    // a save/load, whereas a new back-pointer would need re-deriving
    // after every load - the exact class of missing-back-reference bug
    // this is meant to end. UnCarryItem() also repairs carried_weight
    // and clears `owner`, so this is a no-op for the many callers that
    // correctly uncarried the item before invalidating it.
    const auto owner_sp = owner.lock();

    total_it--;
    XBaseObject::OnInvalidate();

    if (ground_list) {
        if (auto it = ground_list->find(this); it != ground_list->end()) {
            ground_list->erase(it);
        }
    }

    if (owner_sp) {
        owner_sp->UnCarryItem(this);

        if (auto it = owner_sp->contain.find(this); it != owner_sp->contain.end()) {
            owner_sp->contain.erase(it);
        }
    }
}

int XItem::BasicFill(ItemType it, XItemBasicStructure * pData)
{
    ItemTemplate* x_struct = pData->pFirstItem;

    if (it == ItemType::RANDOM) {
        int r_val = vRand(pData->total_prob);

        while (1) {
            r_val -= x_struct->probability;

            if (r_val < 0) {
                break;
            }

            x_struct++;
        }
    } else {
        for (int i = 0; i < pData->total_item; i++, x_struct++)
            if (x_struct->it == it) {
                break;
            }
    }

    if (x_struct) {
        MainFill(x_struct);
        PropFill(x_struct->iset);

        if (vRand() % 20 > 18) {
            SpecialFill();
        }

        return 1;
    } else {
        return 0;
    }
}

void XItem::MainFill(ItemTemplate *is)
{
    name = is->name;
    it = is->it;
    view = is->view;
    weight = is->valume;
    value = is->value;
    quality = is->iq;

    XDice d(is->dv);
    _DV = d.NThrow();

    d.Setup(is->pv);
    _PV = d.NThrow();

    d.Setup(is->hit);
    _HIT = d.NThrow();

    d.Setup(is->dice);
    int tx = d.GetCount();
    int ty = d.GetSides();

    d.Setup(is->z);
    dice.Setup(tx, ty, d.NThrow());

    d.Setup(is->r);
    RNG = d.NThrow();
}

void XItem::PropFill(ITEM_SET is, int val)
{
    int tval = 0;

    for (int i = 0; i < DB_PROP_SZ; i++) {
        if (item_prop[i].iflag & is) {
            tval += item_prop[i].probability;
        }
    }

    assert(tval > 0);

    int trnd = vRand() % tval;
    int r_val = 0;

    while (1) {
        if (item_prop[r_val].iflag & is) {
            trnd -= item_prop[r_val].probability;
        }

        if (trnd < 0) {
            break;
        }

        r_val++;
    }

    material_index = r_val;
    name = fmt::format("{} {}", item_prop[r_val].propname, name);

    color = item_prop[r_val].color;
    weight *= item_prop[r_val].density;
    value =	item_prop[r_val].value * value / 10;

    XDice d;

    if (_DV) {
        d.Setup(item_prop[r_val].dv);
        _DV += d.NThrow();
    }

    if (_PV) {
        d.Setup(item_prop[r_val].pv);
        _PV += d.NThrow();
    }

    d.Setup(item_prop[r_val].hit);
    _HIT += d.NThrow();

    d.Setup(item_prop[r_val].dice);
    int tx = dice.GetCount() + d.GetCount();
    int ty = dice.GetSides() + d.GetSides();
    d.Setup(item_prop[r_val].z);
    dice.Setup(tx, ty, dice.GetBonus() + d.NThrow());

    quality = (ITEM_QUALITY)(quality + item_prop[r_val].iq);
    assert(resistances == nullptr);
    resistances = std::make_unique<XResistance>(item_prop[r_val].resistance);
    assert(stats == nullptr);
    stats = std::make_unique<XStats>();
}

void XItem::SpecialFill()
{
    int r_val;

    if (kind & ItemKind::WEAPON) {
        int uu = 0;
    }

    r_val = vRand() % ENH_DB_SZ;

    if (!(ienh_db[r_val].val < vRand() % 101 && (kind & ienh_db[r_val].kind))) {
        return;
    }

    special_number = r_val;

    if (ienh_db[r_val].color) {
        color = ienh_db[r_val].color;
    }

    XDice * d;

    d = new XDice(ienh_db[r_val].dv);
    _DV += d->Throw();

    d->Setup(ienh_db[r_val].pv);
    _PV += d->Throw();

    d->Setup(ienh_db[r_val].hit);
    _HIT += d->Throw();

    d->Setup(ienh_db[r_val].dice);
    int tx = dice.GetCount() + d->GetCount();
    int ty = dice.GetSides() + d->GetSides();
    d->Setup(ienh_db[r_val].z);
    dice.Setup(tx, ty, dice.GetBonus() + d->Throw());
    delete d;

    XResistance xres(ienh_db[r_val].r);
    XStats xst(ienh_db[r_val].s);
    resistances->Add(&xres);
    stats->Add(&xst);

    aet = ienh_db[r_val].brt;
}

XItem::XItem(XItem * copy) : XBaseObject((XBaseObject*)copy)
{
    total_it++;

    bp = copy->bp;
    durability = copy->durability;
    identify = copy->identify;
    is_selected = copy->is_selected;
    it = copy->it;
    kind = copy->kind;
    material_index = copy->material_index;
    special_number = copy->special_number;
    special_property = copy->special_property;
    value = copy->value;
    wt = copy->wt;
    aet = copy->aet;
    owner = copy->owner;
    quantity = copy->quantity;
}

void XItem::Concat(XItem* it)
{
    quantity += it->quantity;
    it->Invalidate();
}

int XItem::Compare(XObject * o)
{
    assert(dynamic_cast<XItem*>(o));
    XItem * tit = (XItem*)o;

    if (it == tit->it && tit->aet == aet && XBaseObject::Compare(o) == 0) {
        return 0;
    } else {
        return 1;
    }
}

std::string XItem::toString()
{
    assert(0);
}

int XItem::ModifyDur(int val)
{
    if (durability == DUR_INFINITE) {
        return 0;
    }

    durability -= val;

    if (durability <= 0) {
        return 0;
    } else {
        return 1;
    }
}

int XItem::GetValue()
{
    int xdice = 0;
    int xdvpv = 0;
    int xhitdmg = 0;

    if (kind & ItemKind::VALUEDICE) {
        xdice = (dice.GetCount() * dice.GetSides() + dice.GetCount()) * 3;
    }

    if (kind & ItemKind::VALUEDVPV) {
        xdvpv = (_DV + 6 * _PV) * 4;
    }

    if (kind & ItemKind::SHIELD) {
        xdvpv = xdvpv + _DV * 5;
    }

    if (kind & ItemKind::VALUEHITDMG) {
        xhitdmg = (_HIT + dice.GetBonus() * 3) * 3;
    }

    int xrng = RNG * (abs(RNG) + 5);

    int xstats = 0;
    int i;

    if (stats)
        for (i = XStats::STR; i < XStats::COUNT; i++) {
            xstats += stats->Get((XStats::Id)i);
        }

    int xresist = 0;

    if (resistances)
        for (i = XResistance::WHITE; i < XResistance::COUNT; i++) {
            int tr = resistances->GetResistance((XResistance::Id)i);

            if (tr < 10) {
                xresist += tr * 2;
            } else if (tr < 30) {
                xresist += tr * 4;
            } else if (tr < 50) {
                xresist += tr * 8;
            } else if (tr < 75) {
                xresist += tr * 10;
            } else {
                xresist += tr * 20;
            }
        }

    int brtval = 0;

    if ((aet & AttackEffectType::FIRE) != AttackEffectType::NONE) {
        brtval += 200;
    }

    if ((aet & AttackEffectType::COLD) != AttackEffectType::NONE) {
        brtval += 150;
    }

    if ((aet & AttackEffectType::ORCSLAYER) != AttackEffectType::NONE) {
        brtval += 300;
    }

    if ((aet & AttackEffectType::DEMONSLAYER) != AttackEffectType::NONE) {
        brtval += 220;
    }

    int xval = brtval + value + xdice + xdvpv + xhitdmg + xstats * 150 + xresist + xrng;

    if (kind & ItemKind::MISSILE) {
        xval /= 7;
    }

    if (xval <= 0) {
        return 1;
    } else {
        return xval;
    }
}

std::string XItem::StatsToString()
{
    std::string str = "{";
    int flag = 0;

    if (stats) {
        for (int stat = XStats::STR; stat < XStats::COUNT; stat++) {
            if (stats->Get(static_cast<XStats::Id>(stat)) != 0) {
                if (flag) {
                    str.append(" ");
                }

                str.append(fmt::format("{}: {}",
                    stats->GetName(static_cast<XStats::Id>(stat)),
                    stats->Get(static_cast<XStats::Id>(stat))));

                flag++;
            }
        }

        str.append("}");
    }

    return flag ? str : "";
}

std::string XItem::GetFullName()
{
    std::string fullname;

    if (special_number >= 0) {
        if (quantity == 1) {
            fullname = fmt::format(ienh_db[special_number].name, name);
        } else {
            fullname = fmt::format("heap of ({})", quantity);

            if (kind & (ItemKind::BOOTS | ItemKind::GLOVES)) {
                fullname.append(fmt::format(ienh_db[special_number].name, name));
            } else {
                fullname.append(fmt::format(ienh_db[special_number].name,
                    name.append("s")));
            }
        }

        fullname.append(" ");
    } else {
        if (quantity == 1) {
            fullname = name;
        } else {
            if (kind & (ItemKind::BOOTS | ItemKind::GLOVES)) {
                fullname = fmt::format("heap of ({}) {} ", quantity, name);
            } else {
                fullname = fmt::format("heap of ({}) {}s ", quantity, name);
            }
        }
    }

    return fullname;
}

std::string XItem::GetArtifactName(std::string real_name)
{
    std::string str;

    if (isIdentifed()) {
        str = real_name;

        if (RNG != 0) {
            str.append(fmt::format(" <{:+}>", RNG));
        }

        if (_DV != 0 || _PV != 0) {
            str.append(fmt::format(" [{:+}, {:+}]", _DV, _PV));
        }

        if (kind & ItemKind::WEAPON) {
            str.append(fmt::format(
                " ({:+}, {}d{}{:+})",
                _HIT, dice.GetCount(), dice.GetSides(), dice.GetBonus()));
        }

        str.append(StatsToString());
    } else {
        str = name;
    }

    return str;
}

int XItem::onWear(XCreature * cr)
{
    cr->added_stats.Add(stats.get()); // modify stats
    cr->added_resists.Add(resistances.get()); // modify resist

    if (kind != ItemKind::SHIELD) {
        cr->added_DV	+= _DV;
    }

    cr->added_PV	+= _PV;

    if (kind & ItemKind::TOHIT) {
        cr->added_HIT	+= _HIT;
    }

    if (!(kind & (ItemKind::WEAPON | ItemKind::MISSILE | ItemKind::MISSILEW))) {
        cr->added_DMG	+= dice.GetBonus();
    }

    cr->added_RNG	+= RNG;

    cr->added_HP	+= _HP;
    cr->added_PP	+= _PP;

    return 1;
}

int XItem::onUnWear(XCreature * cr)
{
    cr->added_stats.Sub(stats.get()); //modify stats;
    cr->added_resists.Sub(resistances.get()); //modify resist;

    if (kind != ItemKind::SHIELD) {
        cr->added_DV	-= _DV;
    }

    cr->added_PV	-= _PV;

    if (kind & ItemKind::TOHIT) {
        cr->added_HIT	-= _HIT;
    }

    if (!(kind & (ItemKind::WEAPON | ItemKind::MISSILE | ItemKind::MISSILEW))) {
        cr->added_DMG	-= dice.GetBonus();
    }

    cr->added_RNG	-= RNG;

    cr->added_HP	-= _HP;
    cr->added_PP	-= _PP;
    return 1;
}

int XItem::onPickUp(XCreature * cr)
{
    SetOwner(cr);
    return 1;
}

int XItem::onPutOn(XCreature * cr)
{
    return 1;
}

int XItem::onHit(XCreature * user, XCreature * target)
{
    if ((ienh_db[special_number].brt & AttackEffectType::FIRE) != AttackEffectType::NONE) {
        //	user->MagicAttack(target, dice.Throw(), XResistance::FIRE);
    }

    return 1;
}

ItemMaterial* XItem::GetMaterial(int index)
{
    return &item_prop[index];
}

void XItem::Drop(XLocation * location, int _x, int _y)
{
    SetOwner(nullptr);
    SetLocation(location);
    l->map->PutItem(_x, _y, this);
}

bool XItem::SetOwner(XCreature * new_owner)
{
    owner = XCreature::ToWeakPtr(new_owner);

    if (new_owner) {
        SetLocation(nullptr);
    }

    return true;
}

void XItem::Pickup(XCreature * picker)
{
    assert(0);
}

void XItem::UnCarry()
{
    if (auto o = owner.lock()) {
        o->UnCarryItem(this);
    }
}
