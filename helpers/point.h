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

#ifndef POINT_H
#define POINT_H

struct XPoint
{
    public:
        XPoint(): x(0), y(0) {}

        XPoint(const int _x, const int _y) : x(_x), y(_y) {}

        XPoint(const XPoint& pt) = default;

        XPoint &operator=(const XPoint& pt) = default;

        void Set(const int _x, const int _y)
        {
            x = _x;
            y = _y;
        }

        [[nodiscard]] int GetX() const
        {
            return x;
        }

        [[nodiscard]] int GetY() const
        {
            return y;
        }

        int x, y;
};

#endif
