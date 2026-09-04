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
#include "item/xcap.h"

REGISTER_CLASS(XCap);
CEREAL_REGISTER_TYPE(XCap);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XItem, XCap);

// Filled from world/items/ as those scripts load.
XItemBasicStructure gi_cap;

XCap::XCap(ItemType _it)
{
    // kind before BasicFill, not after: BasicFill() may enchant the item,
    // and an enchantment only lands on the sorts of item it was written
    // for. Set afterwards, the test was "does UNKNOWN count as armour",
    // which is no - so a cap could never be found enchanted at all.
    bp = BP_HEAD;
    kind = ItemKind::HAT;
    BasicFill(_it, &gi_cap);
    to_hit = 0;
    dice.SetBonus(0);
    RNG = 0;
}
