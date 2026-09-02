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
#include "item/uniquei.h"
#include "magic/attack_effect_type.h"

REGISTER_CLASS(XGreatElementalRing);
CEREAL_REGISTER_TYPE(XGreatElementalRing);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XItem, XGreatElementalRing);
XGreatElementalRing::XGreatElementalRing() : XRing()
{
    resistances = std::make_unique<XResistance>("fire:1d1+79 cold:1d1+79 acid:1d1+79 see_invisible:0d0+20");
    stats = std::make_unique<XStats>("Wi:1d1+4");
    name = "Great Elemental Ring";
}

