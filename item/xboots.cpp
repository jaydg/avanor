/*
This file is part of "Avanor, the Land of Mystery" roguelike game

Copyright (C) 2000-2006 Vadim Gaidukevich
Copyright (C) 2025,2026 Joachim de Groot

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
#include "item/xboots.h"

REGISTER_CLASS(XBoots);
CEREAL_REGISTER_TYPE(XBoots);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XItem, XBoots);

// Filled from world/items/ as those scripts load.
XItemBasicStructure gi_boots;

XBoots::XBoots(const ItemType it)
{
    kind = ItemKind::BOOTS;
    BasicFill(it, &gi_boots);
    bp = BP_BOOTS;
    dice.SetBonus(0);
    RNG = 0;
    to_hit = -(weight / 500);
}
