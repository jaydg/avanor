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

#include "item/item_cereal.h"
#include "item/item_misc.h"
#include "item/itemf.h"

REGISTER_CLASS(XBatWing);
CEREAL_REGISTER_TYPE(XBatWing);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XItem, XBatWing);

REGISTER_CLASS(XRatTail);
CEREAL_REGISTER_TYPE(XRatTail);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XItem, XRatTail);

REGISTER_CLASS(XBone);
CEREAL_REGISTER_TYPE(XBone);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XItem, XBone);

REGISTER_CLASS(XAncientMachinePart);
CEREAL_REGISTER_TYPE(XAncientMachinePart);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XItem, XAncientMachinePart);

REGISTER_CLASS(XChest);
CEREAL_REGISTER_TYPE(XChest);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XItem, XChest);
// XChest()'s only no-args constructor is an assert(0) guard - real
// instances always come from XChest(int, ITEM_MASK, int, int), so
// route Cereal's load-time construction through the DUMMY_STRUCT idiom
// instead of that assert.
CEREAL_LOAD_VIA_DUMMY_CONSTRUCT(XChest, serialize);

XChest::XChest(int item_count, ITEM_MASK imask, int low_v, int high_v)
{
    color = xLIGHTGRAY;
    view = '~';
    it = IT_CHEST;
    name = "chest";
    value = 50;
    weight = 100;
    im = IM_CHEST;
    bp = BP_OTHER;

    for (int i = 0; i < item_count; i++) {
        XItem * it = ICREATE(imask, low_v, high_v);
        weight += it->weight;
        contain.insert(XItem::Own(it));
    }
}

std::string XChest::toString()
{
    auto str = name;

    if (contain.empty()) {
        str.append("{{empty}}");
    }

    return str;
}

void XChest::Invalidate()
{
    // Unlike XMapTile's ground-level item_list, XItem::Invalidate() has no
    // idea this set exists - it only knows how to remove itself from
    // l->map's per-cell item_list, not an arbitrary container - so
    // there's no self-removal to race against here. Same idiom as
    // XCreature::Invalidate() handling its own `contain`: just mark
    // every item invalid and let `contain`'s own destructor release the
    // (now-invalid, so ~XObject()'s assertion is satisfied) references
    // afterward.
    for (auto& item : contain) {
        item->Invalidate();
    }

    XItem::Invalidate();
}

