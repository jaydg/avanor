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

#include "item/xamulet.h"

REGISTER_CLASS(XAmulet);

int aidentify_db[20] = {
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0,
    0, 0, 0, 0, 0
};

void XAmulet::StoreTable(XFile * f)
{
    f->Write(aidentify_db, sizeof(int), 20);
}

void XAmulet::RestoreTable(XFile * f)
{
    f->Read(aidentify_db, sizeof(int), 20);
}

XAmulet::XAmulet(ENHANCE enh) : XEnhance(enh)
{
    im = IM_NECK;
    bp = BP_NECK;
    name = "amulet";
    it = IT_AMULET;
    view = '\'';
}

int XAmulet::isIdentifed()
{
    return aidentify_db[descr];
}

void XAmulet::Identify(int level)
{
    aidentify_db[descr] = level;
}
