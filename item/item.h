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

#ifndef ITEM_H
#define ITEM_H

#include <set>

#include <cereal/types/base_class.hpp>
#include <cereal/types/memory.hpp>
#include <sol/forward.hpp>

#include "creature/bodypart.h"
#include "engine/xbaseobj.h"
#include "item/itemdb.h"
#include "item/itemdef.h"
#include "item/itemkind.h"
#include "item/itemlist.h"
#include "magic/attack_effect_type.h"
#include "magic/wskills.h"

#define DUR_INFINITE 65535

enum RESULT {
    FAIL,
    SUCCESS,
    CONTINUE,
    ABORT,
};

class XCreature;

class XItem : public XBaseObject
{
    protected:
        std::weak_ptr<XCreature> owner;
    public:
        DECLARE_CREATOR(XItem, XBaseObject);

        std::string StatsToString();
        void MainFill(ItemTemplate* is);
        XItem();
        XItem(XItem * copy);

        virtual XItem* MakeCopy()
        {
            assert(0);
            return nullptr;
        }

    protected:
        // teardown hook, called by XObject::Invalidate()
        void OnInvalidate() override;
    public:

        int BasicFill(ItemType it, XItemBasicStructure * pData);
        void PropFill(ITEM_SET is, int val = 1000);
        void SpecialFill();

        // Render an object as text
        virtual std::string toString();

        // items can be stacked to a bundle: "bundle of 23 arrow (1d4)"
        int quantity;

        // merges it into this item - adds it's quantity onto this one's
        // and invalidates it.
        void Concat(XItem *it);

        BODY_PART bp;    // fit to what bp???
        ItemType it;   // main type of item such ItemType::POTION
        XWarSkills::Type wt;  // weapon skill of item
        ITEM_QUALITY quality; // quality of item. Need for generation and may be basic identification...
        ItemKind kind;

        int durability; // DUR_INFINITE - infinite, Other - finite 1Dur == 1000 turn
        int ModifyDur(int val);

        int identify;    // 0 - not identifed, 1 - identifed
        int is_selected; // it is need to user interface....

        // Answer "can these two items stack?"
        virtual int Compare(XObject * o);
        virtual int isIdentifed()
        {
            return identify; // object can be indetifed or not
        }

        virtual void Identify(int level)
        {
            identify = level; //set object indetifed
        }

        int value;
        virtual int GetValue(); // Get Value of Object (for shop, monster AI etc.)

        SPECIAL_PROPERTY special_property; // slow digestion and other propertyes
        int special_number; // number of a struct with extension name such a "of Strength"
        AttackEffectType aet;
        int material_index; // reference to material;
        ItemMaterial* GetMaterial(int index);

        void Drop(XLocation * location, int _x, int _y);
        void Pickup(XCreature * picker);

        virtual int onWear(XCreature * cr);
        virtual int onUnWear(XCreature * cr);

        virtual int onPickUp(XCreature * cr);
        virtual int onPutOn(XCreature * cr);

        virtual int onHit(XCreature * user, XCreature * target);

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XBaseObject>(this));
            ar(
                owner, bp, it, wt, quality, durability, identify,
                is_selected, value, special_property, special_number,
                aet, material_index, quantity, kind
            );
        }

        bool SetOwner(XCreature * new_owner);
        std::weak_ptr<XCreature> &GetOwner()
        {
            return owner;
        }

        void UnCarry();

        static std::weak_ptr<XItem> ToWeakPtr(XItem * it);
        static std::shared_ptr<XItem> Own(XItem * raw);

        static void RegisterLua(sol::state_view& lua);

    protected:
        [[nodiscard]] std::string GetFullName();

        [[nodiscard]] std::string GetArtifactName(std::string real_name);
};

inline bool compare::operator()(const XItem* lhs, const XItem* rhs) const {
    if (lhs->kind != rhs->kind) {
        return lhs->kind < rhs->kind;
    }

    // `kind` is a broad category (every scroll shares ItemKind::SCROLL, every
    // ring shares ItemKind::RING, ...), not a unique key - without this
    // tiebreak, std::set treats any two items sharing a category as
    // "the same element" (its uniqueness test is exactly "neither
    // compares less than the other"), silently rejecting the second
    // one on insert. `kind` still determines the primary sort order
    // (grouping same-category items together for iteration/display);
    // this just keeps every distinct item its own key within that
    // group instead of colliding with the first.
    return lhs < rhs;
}

inline bool compare::operator()(const std::shared_ptr<XItem>& lhs, const std::shared_ptr<XItem>& rhs) const {
    return (*this)(lhs.get(), rhs.get());
}

inline bool compare::operator()(const std::shared_ptr<XItem>& lhs, const XItem* rhs) const {
    return (*this)(lhs.get(), rhs);
}

inline bool compare::operator()(const XItem* lhs, const std::shared_ptr<XItem>& rhs) const {
    return (*this)(lhs, rhs.get());
}

// Find a same-category item this new one is interchangeable with
// and Concat() into it instead of adding a second entry.
//
// The old XSortedList::insert(iterator, T) took a hint iterator too but
// ignored it for merge purposes - it always rescanned for a match
// regardless. Matched here: the hint-taking overload runs the same
// TryMerge() and only falls back to the hint for the no-match case.
inline XItemList::iterator XItemList::TryMerge(const std::shared_ptr<XItem>& item)
{
    for (auto it = begin(); it != end(); ++it) {
        if (it->get() == item.get()) {
            // Already this exact element - e.g. XCreature::stopAction()
            // reinserting action_data.item after an interrupted action,
            // where the item never left contain in the first place
            // (XItem::Own() just handed out a second shared_ptr to the
            // same live object via shared_from_this()).
            return it;
        }

        if ((*it)->kind == item->kind && (*it)->Compare(item.get()) == 0) {
            (*it)->Concat(item.get());
            return it;
        }
    }

    return end();
}

inline std::pair<XItemList::iterator, bool> XItemList::insert(std::shared_ptr<XItem> item)
{
    if (auto it = TryMerge(item); it != end()) {
        return {it, false};
    }

    return Base::insert(std::move(item));
}

inline XItemList::iterator XItemList::insert(iterator hint, std::shared_ptr<XItem> item)
{
    if (auto it = TryMerge(item); it != end()) {
        return it;
    }

    return Base::insert(hint, std::move(item));
}

inline void XItemList::InvalidateAll()
{
    // begin() is re-derived every pass rather than cached: invalidating
    // one item can erase others from this same list (a container item
    // tearing down its contents, say). The local strong reference keeps
    // the item alive across Invalidate() even when this list held its
    // last reference; it is released at the end of each iteration.
    // Invalidate() is itself idempotent, so an already-invalid item
    // needs no special case here.
    while (!empty()) {
        const std::shared_ptr<XItem> item = *begin();
        erase(begin());
        item->Invalidate();
    }
}

#endif
