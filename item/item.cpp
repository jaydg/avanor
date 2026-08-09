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

#include "creature/creature.h"
#include "item/item.h"

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
    it = IT_UNKNOWN;
    im = IM_OTHER;
    wt = XWarSkills::OTHER;
    bp = BP_OTHER;
    identify = 1;
    _DV = _PV = _HIT = RNG = 0;
    dice.Setup("0d0");
    special_number = -1;
    brt = BR_NONE;
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

void XItem::Invalidate()
{
    if (!isValid()) {
        return;
    }

    // If this item is currently lying on the ground, its map cell's
    // item_list holds the item's master shared_ptr (see XItem::Own()).
    // Capture that list now, before XBaseObject::Invalidate() clears our
    // location below, but don't actually erase from it until after is_valid
    // is cleared - if this is the item's last reference, erasing it runs
    // Own()'s deleter synchronously, and that deleter must see isValid() ==
    // false (delete outright) rather than re-entering this very Invalidate()
    // call while it's still on the stack. l->map may already be gone (its
    // location's teardown got there first and already reclaimed every item
    // on it), in which case there's nothing left to remove ourselves from.
    XItemList* ground_list = (l && l->map && x >= 0 && y >= 0) ? l->map->GetItemList(x, y) : nullptr;

    total_it--;
    XBaseObject::Invalidate();

    if (ground_list) {
        if (auto it = ground_list->find(this); it != ground_list->end()) {
            ground_list->erase(it);
        }
    }
}

int XItem::BasicFill(ITEM_TYPE it, XItemBasicStructure * pData)
{
    _MAIN_ITEM_STRUCT * x_struct = pData->pFirstItem;

    if (it == IT_RANDOM) {
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

void XItem::MainFill(_MAIN_ITEM_STRUCT *is)
{
    name = is->name;
    it = is->it;
    view = is->view;
    weight = is->valume;
    value = is->value;
    quality = is->iq;

    XDice * d;

    d = new XDice(is->dv);
    _DV = d->NThrow();

    d->Setup(is->pv);
    _PV = d->NThrow();

    d->Setup(is->hit);
    _HIT = d->NThrow();

    d->Setup(is->dice);
    int tx = d->GetCount();
    int ty = d->GetSides();

    d->Setup(is->z);
    dice.Setup(tx, ty, d->NThrow());

    d->Setup(is->r);
    RNG = d->NThrow();
    delete d;
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

    if (im & IM_WEAPON) {
        int uu = 0;
    }

    r_val = vRand() % ENH_DB_SZ;

    if (!(ienh_db[r_val].val < vRand() % 101 && (im & ienh_db[r_val].im))) {
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

    brt = ienh_db[r_val].brt;
}

XItem::XItem(XItem * copy) : XBaseObject((XBaseObject*)copy)
{
    total_it++;

    bp = copy->bp;
    durability = copy->durability;
    identify = copy->identify;
    is_selected = copy->is_selected;
    it = copy->it;
    material_index = copy->material_index;
    special_number = copy->special_number;
    special_property = copy->special_property;
    value = copy->value;
    wt = copy->wt;
    brt = copy->brt;
    owner = copy->owner;
}

int XItem::Compare(XObject * o)
{
    assert(dynamic_cast<XItem*>(o));
    XItem * tit = (XItem*)o;

    if (it == tit->it && tit->brt == brt && XBaseObject::Compare(o) == 0) {
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

    if (im & IM_VALUEDICE) {
        xdice = (dice.GetCount() * dice.GetSides() + dice.GetCount()) * 3;
    }

    if (im & IM_VALUEDVPV) {
        xdvpv = (_DV + 6 * _PV) * 4;
    }

    if (im & IM_SHIELD) {
        xdvpv = xdvpv + _DV * 5;
    }

    if (im & IM_VALUEHITDMG) {
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
        for (i = R_WHITE; i < R_EOF; i++) {
            int tr = resistances->GetResistance((RESISTANCE)i);

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

    if (brt & BR_FIRE) {
        brtval += 200;
    }

    if (brt & BR_COLD) {
        brtval += 150;
    }

    if (brt & BR_ORCSLAYER) {
        brtval += 300;
    }

    if (brt & BR_DEMONSLAYER) {
        brtval += 220;
    }

    int xval = brtval + value + xdice + xdvpv + xhitdmg + xstats * 150 + xresist + xrng;

    if (im & IM_MISSILE) {
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

            if (im & (IM_BOOTS | IM_GLOVES)) {
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
            if (im & (IM_BOOTS | IM_GLOVES)) {
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

        if (im & IM_WEAPON) {
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

    if (im != IM_SHIELD) {
        cr->added_DV	+= _DV;
    }

    cr->added_PV	+= _PV;

    if (im & IM_TOHIT) {
        cr->added_HIT	+= _HIT;
    }

    if (!(im & (IM_WEAPON | IM_MISSILE | IM_MISSILEW))) {
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

    if (im != IM_SHIELD) {
        cr->added_DV	-= _DV;
    }

    cr->added_PV	-= _PV;

    if (im & IM_TOHIT) {
        cr->added_HIT	-= _HIT;
    }

    if (!(im & IM_WEAPON | IM_MISSILE | IM_MISSILEW)) {
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
    if (ienh_db[special_number].brt & BR_FIRE) {
        //	user->MagicAttack(target, dice.Throw(), R_FIRE);
    }

    return 1;
}

_ITEMPROP* XItem::GetMaterial(int index)
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
