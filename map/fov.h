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

#ifndef FOV_H
#define FOV_H

// Adam Milazzo's symmetric shadowcasting, as described in
// http://www.adammil.net/blog/v125_Roguelike_Vision_Algorithms.html
//
// What it gives over the table-driven line-of-sight it replaces:
// symmetry (if you can see a cell, someone standing there can see you),
// no blind spots behind corners, and walls that are lit when any part of
// them can be seen. The engine knows nothing about what blocks light or
// what being seen means - a caller says so by implementing the two
// methods below.
class XFieldOfView
{
    public:
        virtual ~XFieldOfView() = default;

        // Everything within `range` of (ox, oy) that can be seen from it
        // is handed to MarkVisible(), the origin included.
        void Compute(int ox, int oy, int range);

    protected:
        // Whether light stops at this cell. Cells outside the map should
        // say yes, so the sweep does not run off the edge.
        [[nodiscard]] virtual bool BlocksLight(int x, int y) const = 0;

        // This cell can be seen from the origin.
        virtual void MarkVisible(int x, int y) = 0;

        // How far a cell is from the origin, in the shape the range
        // describes - round by default.
        [[nodiscard]] virtual int Distance(int x, int y) const;

    private:
        // A slope as the ratio y/x, kept exact so that the comparisons
        // the algorithm makes never drift.
        struct Slope {
            unsigned y;
            unsigned x;

            [[nodiscard]] bool Greater(const unsigned _y, const unsigned _x) const
            {
                return y * _x > x * _y;
            }

            [[nodiscard]] bool GreaterOrEqual(const unsigned _y, const unsigned _x) const
            {
                return y * _x >= x * _y;
            }

            [[nodiscard]] bool Less(const unsigned _y, const unsigned _x) const
            {
                return y * _x < x * _y;
            }

            [[nodiscard]] bool LessOrEqual(const unsigned _y, const unsigned _x) const
            {
                return y * _x <= x * _y;
            }
        };

        void ComputeOctant(int octant, int ox, int oy, int range, unsigned x, Slope top, Slope bottom);

        [[nodiscard]] bool BlocksLightInOctant(unsigned x, unsigned y, int octant, int ox, int oy) const;
        void MarkVisibleInOctant(unsigned x, unsigned y, int octant, int ox, int oy);
};

#endif
