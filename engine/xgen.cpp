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

#include <cereal/archives/json.hpp>
#include <cereal/types/polymorphic.hpp>

#include "creature/anycr.h"
#include "engine/xapi.h"
#include "engine/xgen.h"
#include "game/game.h"

// XGenerator is never itself a dynamic type - just a link in the
// polymorphic pointer-cast chain for its two concrete subclasses below.
CEREAL_REGISTER_POLYMORPHIC_RELATION(XMapObject, XGenerator);

REGISTER_CLASS(XUniversalGen);
CEREAL_REGISTER_TYPE(XUniversalGen);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XGenerator, XUniversalGen);

bool XUniversalGen::Run()
{
    unsigned int cr_count[32] = {0};

    for (const auto& [key, obj] : objects) {
        if ((obj->im & IM_CREATURE) && dynamic_cast<XCreature *>(obj)->l == l) {
            int n = vGetHighBitNum((dynamic_cast<XCreature *>(obj)->creature_class));
            cr_count[n]++;
        }
    }

    int cmask = 0;

    for (int i = 0; i < 32; i++) {
        cmask <<= 1;

        if (cr_count[31 - i] < max_creature) {
            cmask |= 0x01;
        }
    }

    if (auto n_mask = static_cast<CREATURE_CLASS>(cmask & crc)) {
        XCreature * cr = XCreatureStorage::CreateRnd(n_mask, crl);
        XPoint pt;
        l->GetFreeXY(&pt);
        Game.NewCreature(cr, pt.x, pt.y, l);
        cr->xai->SetAIFlag(AIF_ALLOW_MOVE_WAY_DOWN);
    }

    ttm = ttmb;

    return true;
};

REGISTER_CLASS(XMainLocationGen);
CEREAL_REGISTER_TYPE(XMainLocationGen);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XGenerator, XMainLocationGen);

bool XMainLocationGen::Run()
{
    ttm = ttmb;
    turns_count++;

    if (turns_count == 10000) {
        XRect small_town_area(20, 42, 28, 48);

        for (const auto& [key, obj] : XObject::objects) {
            if ((obj->im & IM_CREATURE) && reinterpret_cast<XCreature *>(obj)->creature_class & CR_ORC) {
                reinterpret_cast<XCreature *>(obj)->xai->SetArea(small_town_area, L_MAIN);
            }
        }
    }

    return false;
}

