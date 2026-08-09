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

#include <sstream>
#include <string>

#include "creature/bodypart.h"
#include "creature/creature.h"

const char* bp_names[] = {
    "",
    "head",
    "neck",
    "body",
    "cloak",
    "hand",
    "ring",
    "gloves",
    "boots",
    "light_source",
    "tool",
    "missile_weapon",
    "missile",
    "eof"
};

ItemKind bpim[] = {
    ItemKind::OTHER,
    ItemKind::HAT,
    ItemKind::NECK,
    ItemKind::BODY,
    ItemKind::CLOAK,
    ItemKind::HAND,
    ItemKind::RING,
    ItemKind::GLOVES,
    ItemKind::BOOTS,
    ItemKind::LIGHTSOURCE,
    ItemKind::TOOL,
    ItemKind::MISSILEW,
    ItemKind::MISSILE,
    ItemKind::ALL
};

XBodyPart::XBodyPart(XCreature* o, const BODY_PART bp)
{
    SetOwner(o);
    bp_uin = bp;
    item.reset();
}

void XBodyPart::SetOwner(XCreature* o)
{
    owner_raw = o;
    owner = XCreature::ToWeakPtr(o);
}

ItemKind XBodyPart::GetProperKind() const
{
    return bpim[bp_uin];
}

const char* XBodyPart::GetName() const
{
    return bp_names[bp_uin];
}

const char* XBodyPart::XGetName(const BODY_PART bp)
{
    return bp_names[bp];
}

int XBodyPart::Wear(XItem* new_item)
{
    assert(new_item);

    if (Fit(new_item->bp)) {
        if (!item.expired()) {
            return 1;
        }

        // Put it in the inventory before wearing it: this establishes
        // new_item's real shared_ptr ownership (via XCreature::contain,
        // through ContainItem() -> XItem::Own()) regardless of whether the
        // caller already had it in contain (picker-driven wear - a no-op
        // re-insert) or is equipping brand-new starting gear that never
        // touched contain at all (construction-time equip loops). Safe to
        // call even mid-construction, before the creature is
        // shared_from_this()-safe: owner_raw is a direct member access,
        // not gated on that (unlike owner.lock() below).
        owner_raw->ContainItem(new_item);
        item = XItem::ToWeakPtr(new_item);

        // owner can still be unresolved here if this is a creature
        // equipping its own starting gear from within its own constructor -
        // it isn't shared_from_this()-safe yet at that point. XCreature::
        // FirstStep() finishes this (onWear()) once it's safe.
        if (auto o = owner.lock()) {
            new_item->onWear(o.get());
        }

        return 0;
    }

    return 2;
}

std::shared_ptr<XItem> XBodyPart::UnWear()
{
    // Worn items stay resident in contain the whole time they're worn (see
    // Wear()), so this lock() should always succeed - contain, not this
    // weak_ptr, is what's actually keeping the item alive.
    auto locked = item.lock();
    assert(locked);

    // owner can be unresolved here too - see the matching comment in Wear().
    if (auto o = owner.lock()) {
        locked->onUnWear(o.get());
    }

    item.reset();

    return locked;
}

XItem* XBodyPart::Item() const
{
    return item.lock().get();
}

int XBodyPart::GetPartSize() const
{
    switch (bp_uin) {
        case BP_HEAD :
            return 10;

        case BP_BODY :
            return 100;

        case BP_GLOVES :
        case BP_BOOTS :
            return 20;

        default :
            return 0;
    }
}

void XBodyPart::Create(XCreature *cr, std::string &str)
{
    std::istringstream stream(str);
    std::string token;

    while (stream >> token)
    {
        int i = 0;

        for (; i < BP_EOF; i++)
        {
            if (token == XGetName(static_cast<BODY_PART>(i)))
            {
                cr->components.emplace_back(std::make_unique<XBodyPart>(cr, static_cast<BODY_PART>(i)));
                break;
            }
        }

        assert(i < BP_EOF);
    }
}
