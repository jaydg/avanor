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

#ifndef __GAME_H
#define __GAME_H

#include <time.h>

#include "creature/xhero.h"
#include "game/location.h"
#include "engine/xarchive.h"
#include "engine/xscheduler.h"
#include "game/shop.h"
#include "item/incl_i.h"
#include "item/itemf.h"
#include "other/other_misc.h"
#include "other/xgen.h"

extern char static_buffer[];

class XGame
{
        void CreateLocations();
        void CreateHero();
    public:
        XGame();
        ~XGame();
        void Run();
        void RunWithoutHero();
        void RunDemo();
        void CreateRatCellar();
        void Create(char type_of_start);
        int MoveToStairway(XLocation * xl, XCreature * tc);
        XCreature* NewCreature(XCreature * cr, int x, int y, XLocation * loc);
        XPtr<XLocation> locations[L_EOF];
        XScheduler Scheduler;
        static int current_location;
        static XGUID hero_guid;

        static int best_cr_level;
        static XPtr<XCreature> best_creature;

        static bool isAvgPV;
        static bool isGodMode;
};

extern XGame Game;

#endif
