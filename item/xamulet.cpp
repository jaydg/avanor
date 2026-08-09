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
#include "item/xamulet.h"

REGISTER_CLASS(XAmulet);
CEREAL_REGISTER_TYPE(XAmulet);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XItem, XAmulet);

int aidentify_db[20] = {};

void XAmulet::SaveTable(cereal::JSONOutputArchive& ar)
{
    for (int i = 0; i < 20; i++) {
        ar(aidentify_db[i]);
    }
}

void XAmulet::LoadTable(cereal::JSONInputArchive& ar)
{
    for (int i = 0; i < 20; i++) {
        ar(aidentify_db[i]);
    }
}

XAmulet::XAmulet(Type enh) : XEnhance(enh)
{
    kind = ItemKind::IM_NECK;
    bp = BP_NECK;
    name = "amulet";
    it = IT_AMULET;
    view = '\'';
}

int XAmulet::isIdentifed()
{
    return aidentify_db[descr];
}

void XAmulet::Identify(const int level)
{
    aidentify_db[descr] = level;
}
