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

#include <unordered_map>

#include <cereal/archives/json.hpp>
#include <cereal/types/polymorphic.hpp>

#include "creature/anycr.h"
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
    // How many of each sort are already here. The ceiling is per class,
    // so a generator naming eight of them settles up to eight times as
    // many creatures as one naming a single class.
    std::unordered_map<CREATURE_CLASS, unsigned int> cr_count;

    for (const auto& [key, obj] : objects) {
        auto* cr = dynamic_cast<XCreature *>(obj);

        if (cr && !cr->isHero() && cr->l == l) {
            cr_count[cr->creature_class]++;
        }
    }

    CreatureClassSet room;

    for (const CREATURE_CLASS& id : crc) {
        if (cr_count[id] < max_creature) {
            room.Add(id);
        }
    }

    // No room right now is not an error - the generator simply produces
    // nothing this turn and tries again on the next.
    if (!room.Empty()) {
        if (const auto pt = l->GetFreeXY()) {
            if (XCreature* cr = XCreatureStorage::CreateRnd(room, crl)) {
                Game.NewCreature(cr, pt->x, pt->y, l);
                cr->xai->SetAIFlag(XStandardAI::ALLOW_MOVE_WAY_DOWN);
            }
        }
    }

    ttm = ttmb;

    return true;
};

