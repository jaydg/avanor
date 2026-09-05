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

#include <iostream>

#include <fmt/format.h>

#include <cereal/archives/json.hpp>
#include <cereal/types/polymorphic.hpp>
#include <sol/sol.hpp>

#include "creature/creature.h"
#include "engine/xlua.h"
#include "item/item.h"
#include "magic/brand.h"

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
    it = IT_NONE;
    kind = ItemKind::UNKNOWN;
    quantity = 1;
    wt = CS_NONE;
    bp = BP_OTHER;
    identified = true;
    dv = pv = to_hit = RNG = 0;
    dice.Setup("0d0");
    special_number.clear();

    // Never set here before, so every item that did not name one of its own
    // - all the food, for one - carried whatever was on the heap, and wrote
    // it into the save file. Nothing branches on it today, which is why it
    // went unnoticed.
    special_property = SPP_NONE;
    aet = BrandSet();
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
    const ItemTemplate* x_struct = nullptr;

    if (!it.empty()) {
        x_struct = pData->Find(it);

        // A name nothing defines - a typo in world/, now that item ids are
        // written there by hand. Say so, then carry on with any item of
        // the kind: a half-filled item has no stats and no resistances,
        // and the first thing to wear one dereferences them.
        //
        // It used to walk off the end of the table instead and fill the
        // item from whatever followed it in memory.
        if (!x_struct) {
            std::cerr << "world: nothing defines an item type '" << it
                      << "' - substituting another of its kind" << std::endl;
        }
    }

    if (!x_struct) {
        // Either nothing was asked for, or what was asked for is not
        // there: pick one, each row weighted by its :Chance().
        int r_val = vRand(pData->total_prob);

        for (int i = 0; i < pData->total_item; i++) {
            r_val -= pData->pFirstItem[i].probability;

            if (r_val < 0) {
                x_struct = &pData->pFirstItem[i];
                break;
            }
        }
    }

    if (!x_struct) {
        return 0;
    }

    // Whether this kind of item is meant to protect at all, read from
    // the template rather than from the roll - see PropFill().
    const XDice template_pv(x_struct->pv);
    const bool protective = template_pv.GetSides() > 0 || template_pv.GetBonus() > 0;

    MainFill(x_struct);
    PropFill(x_struct->iset, protective);

    if (vRand() % 20 > 18) {
        SpecialFill();
    }

    return 1;
}

void XItem::OnCreated(const XItemBasicStructure& pData)
{
    const ItemTemplate* row = pData.Find(it);

    if (!row || row->on_create.empty()) {
        return;
    }

    sol::state_view lua(XLua::State());
    sol::protected_function handler = lua[row->on_create];

    if (!handler.valid()) {
        std::cerr << "world: " << name << " wants finishing by '"
                  << row->on_create << "', which is not defined" << std::endl;

        return;
    }

    // this stays void*, like every other handler argument - see
    // XCreature::RegisterLua for why the dispatch never went typed.
    const auto result = handler((void*)this);

    if (!result.valid()) {
        const sol::error err = result;
        std::cerr << "world: " << row->on_create << ": " << err.what() << std::endl;
    }
}

void XItem::MainFill(const ItemTemplate *is)
{
    name = is->name;
    it = is->it;
    view = is->view;
    weight = is->valume;
    value = is->value;
    quality = is->iq;
    wt = is->wt;

    XDice d(is->dv);
    dv = d.NThrow();

    d.Setup(is->pv);
    pv = d.NThrow();

    d.Setup(is->hit);
    to_hit = d.NThrow();

    d.Setup(is->dice);
    int tx = d.GetCount();
    int ty = d.GetSides();

    d.Setup(is->z);
    dice.Setup(tx, ty, d.NThrow());

    d.Setup(is->r);
    RNG = d.NThrow();
}

void XItem::PropFill(ITEM_SET is, bool protective)
{
    int tval = 0;

    for (const auto& mat : item_prop) {
        if (mat.iflag & is) {
            tval += mat.probability;
        }
    }

    if (tval <= 0) {
        std::cerr << "world: nothing is made of that" << std::endl;
        return;
    }

    int trnd = vRand() % tval;
    const ItemMaterial* chosen = nullptr;

    for (const auto& mat : item_prop) {
        if (mat.iflag & is) {
            trnd -= mat.probability;
        }

        if (trnd < 0) {
            chosen = &mat;
            break;
        }
    }

    if (!chosen) {
        return;
    }

    const ItemMaterial& mat = *chosen;
    material_index = mat.id;
    name = fmt::format("{} {}", mat.propname, name);

    color = mat.color;
    weight *= mat.density;
    value =	mat.value * value / 10;

    XDice d;

    if (dv) {
        d.Setup(mat.dv.c_str());
        dv += d.NThrow();
    }

    // What the thing is made of adds to what it stops - for anything whose
    // template says it stops something at all.
    //
    // This used to test the rolled `pv` instead, which quietly made the
    // material irrelevant to most armour: NThrow() returns zero for three
    // rolls in four, so a robe or a cap or a pair of boots usually rolled
    // no protection, and once it had rolled none, being made of steel or
    // obsidian could no longer give it any. Testing the template instead
    // lets the material always have its say, while still keeping armour
    // off a sword - whose pv column is empty, so `protective` is false.
    if (protective) {
        d.Setup(mat.pv.c_str());
        pv += d.NThrow();
    }

    d.Setup(mat.hit.c_str());
    to_hit += d.NThrow();

    d.Setup(mat.dice.c_str());
    int tx = dice.GetCount() + d.GetCount();
    int ty = dice.GetSides() + d.GetSides();
    d.Setup(mat.z.c_str());
    dice.Setup(tx, ty, dice.GetBonus() + d.NThrow());

    quality = (ITEM_QUALITY)(quality + mat.iq);
    assert(resistances == nullptr);
    resistances = std::make_unique<XResistance>(mat.resistance.c_str());
    assert(stats == nullptr);
    stats = std::make_unique<XStats>();
}

// The format string a row names itself with, or a bare "{}" for an item
// whose enchantment content no longer defines.
static const std::string& EnchantmentName(const std::string& id)
{
    static const std::string plain = "{}";
    const ENHANCE_STRUCT* row = FindArmourEnchantment(id);
    return row ? row->name : plain;
}

void XItem::SpecialFill()
{
    const std::string id = RandomArmourEnchantment();
    const ENHANCE_STRUCT* row = FindArmourEnchantment(id);

    // Rarity is a threshold, not a weight: the higher it is, the fewer
    // rolls beat it. And an enchantment only lands on the sorts of item it
    // was written for.
    if (!row || !(row->rarity < vRand() % 101 && (kind & row->kind))) {
        return;
    }

    special_number = id;

    if (row->color) {
        color = row->color;
    }

    XResistance xres(row->r.c_str());
    XStats xst(row->s.c_str());
    resistances->Add(&xres);
    stats->Add(&xst);

    aet = row->brt;

    // Carried onto the item at last. Nothing reads it yet, so this changes
    // nothing today - but the row no longer declares something that is
    // dropped on the floor between the table and the item.
    special_property = row->spp;
}

XItem::XItem(XItem * copy) : XBaseObject((XBaseObject*)copy)
{
    total_it++;

    bp = copy->bp;
    durability = copy->durability;
    identified = copy->identified;
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
        xdvpv = (dv + 6 * pv) * 4;
    }

    if (kind & ItemKind::SHIELD) {
        xdvpv = xdvpv + dv * 5;
    }

    if (kind & ItemKind::VALUEHITDMG) {
        xhitdmg = (to_hit + dice.GetBonus() * 3) * 3;
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
        // Only what this item actually carries, rather than every
        // resistance that could exist.
        for (const auto& [id, tr] : resistances->All()) {
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

    // What the brands add. Content prices them: a row with no :Value()
    // adds nothing, which is what the six unimplemented slayers and the
    // effect-less brands did before.
    int brtval = 0;

    for (const BRAND& id : aet) {
        if (const BrandStats* row = FindBrand(id)) {
            brtval += row->value;
        }
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

void XItem::OnOutfit(XCreature* who)
{
    const XItemBasicStructure* pool = PoolFor(kind);

    if (!pool) {
        return;
    }

    const ItemTemplate* row = pool->Find(it);

    if (!row || row->on_outfit.empty()) {
        return;
    }

    sol::state_view lua(XLua::State());
    sol::protected_function handler = lua[row->on_outfit];

    if (!handler.valid()) {
        std::cerr << "world: " << name << " wants outfitting through '"
                  << row->on_outfit << "', which is not defined" << std::endl;

        return;
    }

    const auto result = handler((void*)this, (void*)who);

    if (!result.valid()) {
        const sol::error err = result;
        std::cerr << "world: " << row->on_outfit << ": " << err.what() << std::endl;
    }
}

std::string XItem::GetNameEx(const Article article)
{
    if (const std::string proper = GetProperName(); !proper.empty()) {
        return proper;
    }

    if (article == Article::NONE) {
        return name;
    }

    if (article == Article::DEFINITE) {
        return fmt::format("the {}", name);
    }

    const char first = name.empty() ? '\0' : name.front();
    const bool vowel = first == 'a' || first == 'e' || first == 'i'
        || first == 'o' || first == 'u';

    return fmt::format("{} {}", vowel ? "an" : "a", name);
}

std::string XItem::GetFullName()
{
    std::string fullname;

    if (!special_number.empty()) {
        if (quantity == 1) {
            fullname = fmt::format(
                fmt::runtime(EnchantmentName(special_number)), name);
        } else {
            fullname = fmt::format("heap of ({})", quantity);

            if (kind & (ItemKind::BOOTS | ItemKind::GLOVES)) {
                fullname.append(fmt::format(
                    fmt::runtime(EnchantmentName(special_number)), name));
            } else {
                fullname.append(fmt::format(
                    fmt::runtime(EnchantmentName(special_number)),
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

    if (isIdentified()) {
        str = real_name;

        if (RNG != 0) {
            str.append(fmt::format(" <{:+}>", RNG));
        }

        if (dv != 0 || pv != 0) {
            str.append(fmt::format(" [{:+}, {:+}]", dv, pv));
        }

        if (kind & ItemKind::WEAPON) {
            str.append(fmt::format(
                " ({:+}, {}d{}{:+})",
                to_hit, dice.GetCount(), dice.GetSides(), dice.GetBonus()));
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
        cr->added_DV	+= dv;
    }

    cr->added_PV	+= pv;

    if (kind & ItemKind::TOHIT) {
        cr->added_HIT	+= to_hit;
    }

    if (!(kind & (ItemKind::WEAPON | ItemKind::MISSILE | ItemKind::MISSILEW))) {
        cr->added_DMG	+= dice.GetBonus();
    }

    cr->added_range	+= RNG;

    if (special_property & SPP_SLOWDIGESTION) {
        cr->added_slow_digestion++;
    }

    return 1;
}

int XItem::onUnWear(XCreature * cr)
{
    cr->added_stats.Sub(stats.get()); //modify stats;
    cr->added_resists.Sub(resistances.get()); //modify resist;

    if (kind != ItemKind::SHIELD) {
        cr->added_DV	-= dv;
    }

    cr->added_PV	-= pv;

    if (kind & ItemKind::TOHIT) {
        cr->added_HIT	-= to_hit;
    }

    if (!(kind & (ItemKind::WEAPON | ItemKind::MISSILE | ItemKind::MISSILEW))) {
        cr->added_DMG	-= dice.GetBonus();
    }

    cr->added_range	-= RNG;

    if (special_property & SPP_SLOWDIGESTION) {
        cr->added_slow_digestion--;
    }

    return 1;
}

int XItem::onPickUp(XCreature * cr)
{
    SetOwner(cr);
    return 1;
}

int XItem::onPutOn(XCreature * /*cr*/)
{
    return 1;
}

int XItem::onHit(XCreature * /*user*/, XCreature * /*target*/)
{
    const ENHANCE_STRUCT* enh = FindArmourEnchantment(special_number);

    if (enh && enh->brt.Has("fire")) {
        //	user->MagicAttack(target, dice.Throw(), "fire");
    }

    return 1;
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

void XItem::Pickup(XCreature * /*picker*/)
{
    assert(0);
}

void XItem::UnCarry()
{
    if (auto o = owner.lock()) {
        o->UnCarryItem(this);
    }
}
