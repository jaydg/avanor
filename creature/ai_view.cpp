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

#include "map/fov.h"
#include "creature/std_ai.h"

namespace {

// What the creature can make out around it: the same sweep the hero's
// view uses, reporting each lit cell to the AI instead of to the map.
class XAIView final : public XFieldOfView
{
    public:
        XAIView(XStandardAI* _ai, XMap* _map, const int _ox, const int _oy)
            : ai(_ai), map(_map), ox(_ox), oy(_oy) {}

    protected:
        [[nodiscard]] bool BlocksLight(const int x, const int y) const override
        {
            if (x < 0 || x >= map->len || y < 0 || y >= map->hgt) {
                return true;
            }

            return map->GetVisibility(x, y) == 0;
        }

        void MarkVisible(const int x, const int y) override
        {
            if (x >= 0 && x < map->len && y >= 0 && y < map->hgt) {
                // The AI weighs what it sees by how far away it is - the
                // nearest enemy, the nearest item, the nearest way out -
                // so every cell has to report its own distance, not the
                // radius of the sweep that found it. The creature's own
                // cell reports 0, which is what keeps it from reading
                // itself as something standing nearby.
                ai->AnalyzeGrid(x, y, Distance(x - ox, y - oy));
            }
        }

    private:
        XStandardAI* ai;
        XMap* map;
        int ox;
        int oy;
};

} // namespace

void XStandardAI::AnalyzeView(const int radius)
{
    XAIView view(this, ai_owner->l->map, ai_owner->x, ai_owner->y);

    view.Compute(ai_owner->x, ai_owner->y, radius);
}
