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

#include <cmath>

#include "map/fov.h"

namespace {

// Where a cell of octant `octant` at (x, y) lies on the map, given the
// origin. The eight octants are the eight ways of mirroring and
// transposing the first, which is what lets one sweep serve all of them.
void OctantToMap(const unsigned x, const unsigned y, const int octant, const int ox, const int oy,
                 int& mx, int& my)
{
    const int dx = static_cast<int>(x);
    const int dy = static_cast<int>(y);

    switch (octant) {
        case 0: mx = ox + dx; my = oy - dy; break;
        case 1: mx = ox + dy; my = oy - dx; break;
        case 2: mx = ox - dy; my = oy - dx; break;
        case 3: mx = ox - dx; my = oy - dy; break;
        case 4: mx = ox - dx; my = oy + dy; break;
        case 5: mx = ox - dy; my = oy + dx; break;
        case 6: mx = ox + dy; my = oy + dx; break;
        default: mx = ox + dx; my = oy + dy; break;
    }
}

} // namespace

int XFieldOfView::Distance(const int x, const int y) const
{
    return static_cast<int>(std::sqrt(static_cast<double>(x) * x + static_cast<double>(y) * y));
}

bool XFieldOfView::BlocksLightInOctant(const unsigned x, const unsigned y, const int octant,
                                       const int ox, const int oy) const
{
    int mx = 0;
    int my = 0;

    OctantToMap(x, y, octant, ox, oy, mx, my);

    return BlocksLight(mx, my);
}

void XFieldOfView::MarkVisibleInOctant(const unsigned x, const unsigned y, const int octant,
                                       const int ox, const int oy)
{
    int mx = 0;
    int my = 0;

    OctantToMap(x, y, octant, ox, oy, mx, my);

    MarkVisible(mx, my);
}

void XFieldOfView::Compute(const int ox, const int oy, const int range)
{
    MarkVisible(ox, oy);

    for (int octant = 0; octant < 8; octant++) {
        ComputeOctant(octant, ox, oy, range, 1, {1, 1}, {0, 1});
    }
}

void XFieldOfView::ComputeOctant(const int octant, const int ox, const int oy, const int range,
                                 unsigned x, Slope top, Slope bottom)
{
    for (; x <= static_cast<unsigned>(range); x++) {
        // The topmost cell of this column that the wedge still covers.
        unsigned top_y;

        if (top.x == 1) {
            top_y = x;
        } else {
            top_y = ((x * 2 - 1) * top.y + top.x) / (top.x * 2);

            if (BlocksLightInOctant(x, top_y, octant, ox, oy)) {
                // A wall: the cell above it is lit too if the wedge
                // reaches its near corner and it is not itself a wall.
                if (top.GreaterOrEqual(top_y * 2 + 1, x * 2)
                    && !BlocksLightInOctant(x, top_y + 1, octant, ox, oy)) {
                    top_y++;
                }
            } else {
                unsigned ax = x * 2;

                if (BlocksLightInOctant(x + 1, top_y + 1, octant, ox, oy)) {
                    ax++;
                }

                if (top.Greater(top_y * 2 + 1, ax)) {
                    top_y++;
                }
            }
        }

        // ...and the bottommost.
        unsigned bottom_y;

        if (bottom.y == 0) {
            bottom_y = 0;
        } else {
            bottom_y = ((x * 2 - 1) * bottom.y + bottom.x) / (bottom.x * 2);

            if (bottom.GreaterOrEqual(bottom_y * 2 + 1, x * 2)
                && BlocksLightInOctant(x, bottom_y, octant, ox, oy)
                && !BlocksLightInOctant(x, bottom_y + 1, octant, ox, oy)) {
                bottom_y++;
            }
        }

        // -1 until the first cell of the column tells us which it was.
        int was_opaque = -1;

        for (unsigned y = top_y; static_cast<int>(y) >= static_cast<int>(bottom_y); y--) {
            if (range >= 0 && Distance(static_cast<int>(x), static_cast<int>(y)) > range) {
                continue;
            }

            const bool is_opaque = BlocksLightInOctant(x, y, octant, ox, oy);

            // A wall is lit whenever the wedge touches it at all; open
            // ground is lit if the wedge reaches its near corner.
            //
            // This is the symmetric form Milazzo gives as an alternative:
            // it costs a little extra visibility at the edges of a wedge
            // and buys the property a roguelike wants - if you can see a
            // creature, it can see you. The centre-of-cell test he
            // defaults to would read:
            //
            //   (y != top_y || top.Greater(y * 4 - 1, x * 4 + 1))
            //       && (y != bottom_y || bottom.Less(y * 4 + 1, x * 4 - 1))
            const bool is_visible = is_opaque
                || ((y != top_y || top.GreaterOrEqual(y, x))
                    && (y != bottom_y || bottom.LessOrEqual(y, x)));

            if (is_visible) {
                MarkVisibleInOctant(x, y, octant, ox, oy);
            }

            if (x == static_cast<unsigned>(range)) {
                continue;
            }

            if (is_opaque) {
                if (was_opaque == 0) {
                    // Light ends here: everything below is in shadow, so
                    // the wedge is split and its lower half swept on.
                    unsigned nx = x * 2;
                    const unsigned ny = y * 2 + 1;

                    if (BlocksLightInOctant(x, y + 1, octant, ox, oy)) {
                        nx--;
                    }

                    if (top.Greater(ny, nx)) {
                        if (y == bottom_y) {
                            bottom = {ny, nx};
                            break;
                        }

                        ComputeOctant(octant, ox, oy, range, x + 1, top, {ny, nx});
                    } else if (y == bottom_y) {
                        return;
                    }
                }

                was_opaque = 1;
            } else {
                if (was_opaque > 0) {
                    // Light begins again: the top of the wedge moves down
                    // to the edge of the wall we just cleared.
                    unsigned nx = x * 2;
                    const unsigned ny = y * 2 + 1;

                    if (BlocksLightInOctant(x + 1, y + 1, octant, ox, oy)) {
                        nx++;
                    }

                    if (bottom.GreaterOrEqual(ny, nx)) {
                        return;
                    }

                    top = {ny, nx};
                }

                was_opaque = 0;
            }
        }

        // The whole column was wall: nothing beyond it can be seen.
        if (was_opaque != 0) {
            break;
        }
    }
}
