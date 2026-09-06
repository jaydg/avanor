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

#ifndef MODIFIER_H
#define MODIFIER_H

#include <memory>
#include <vector>

#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>

#include "magic/modifiers.h"

class XCreature;

// Whether anything knows a modifier by this id. The builders ask before
// writing one into a row, so that a typo in world/ is caught while that
// file loads, naming itself, rather than silently doing nothing to
// somebody three hours into a game.
[[nodiscard]] bool IsKnownModifier(const MODIFIER& mt);

class XModifier
{
    public:
        XModifier() = default;

        void Add(std::unique_ptr<XBasicModifier> mod, XCreature* owner);

        // A delay of 0 lays the modifier on at once; anything higher lays it
        // on that many turns from now, which is what eating something bad
        // does to you.
        int Add(const MODIFIER& mt, int val, XCreature* owner, XCreature* cr = nullptr,
            int delay = 0);

        void Remove(const MODIFIER& mdt, XCreature* owner);

        // return val
        [[nodiscard]] int Get(const MODIFIER& mt) const;

        int Run(XCreature* cr);

        std::string toString() const;

        // Unlike Restore() above, no owner param is needed here - each
        // XBasicModifier's own `setter` (a weak_ptr<XCreature>) already
        // carries its owner reference and resolves directly against the
        // Cereal graph, same as any other weak_ptr in this migration.
        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(ml);
        }

    protected:
        std::vector<std::unique_ptr<XBasicModifier>> ml;
};

#endif
