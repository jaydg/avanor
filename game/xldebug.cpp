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

#include "creature/skeep_ai.h"
#include "creature/unique.h"
#include "engine/xgen.h"
#include "game/game.h"
#include "game/shop.h"
#include "game/xldebug.h"
#include "item/incl_i.h"
#include "item/itemf.h"
#include "item/uniquei.h"
#include "map/map_objects.h"

XLDebug::XLDebug(LOCATION tl) : XLocation(tl)
{
    map = new XMap(80, 20);
    BuildLabirint();
    strcpy(brief_name, "DBGL:1");
    strcpy(full_name, "Debug Level 1");

    map->CreateRoom(0, 0, 80, 20, 0, 0, M_GREENGRAS, M_MAGMA);
}

void XLDebug::Init()
{
}
