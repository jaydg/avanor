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

#ifndef __ITEM_FACTORY_H
#define __ITEM_FACTORY_H

#include "item/incl_i.h"

class XItemFactory
{
    public:
        static XItem* CreateAnyItem(ITEM_MASK im = IM_ITEM, ITEM_TYPE it = IT_RANDOM, int low_v = 0, int hi_v = 10000);

        static XItem* CreateItem(ITEM_MASK im = IM_ITEM, ITEM_TYPE it = IT_RANDOM);
};

#define ICREATE(x, lv, hv) XItemFactory::CreateAnyItem(x, IT_RANDOM, lv, hv)
#define ICREATEA(x) XItemFactory::CreateAnyItem(x, IT_RANDOM)
#define ICREATEB(x, it, lv, hv) XItemFactory::CreateAnyItem(x, it, lv, hv)

#endif
