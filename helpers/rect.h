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

#ifndef RECT_H
#define RECT_H

#include "engine/xfile.h"
#include "helpers/point.h"

class XRect
{
    public:
        int left{}, top{}, right{}, bottom{};

        XRect() = default;

        XRect(const int _left, const int _top, const int _right, const int _bottom)
        {
            left = _left;
            top = _top;
            right = _right;
            bottom = _bottom;
        }

        XRect(const XPoint* lt, const XPoint* rb)
        {
            left = lt->x;
            top = lt->y;
            right = rb->x;
            bottom = rb->y;
        }

        XRect(const XRect& r)
        {
            left = r.left;
            right = r.right;
            top = r.top;
            bottom = r.bottom;
        }

        void Setup(XRect& r)
        {
            left = r.left;
            right = r.right;
            top = r.top;
            bottom = r.bottom;
        }

        void Setup(const int _left, const int _top, const int _right, const int _bottom)
        {
            left = _left;
            top = _top;
            right = _right;
            bottom = _bottom;
        }

        int Intersect(const XRect * r) const;
        int PointIn(const XPoint * pt) const;
        [[nodiscard]] int PointIn(int x, int y) const;
        void Grow(int r);
        [[nodiscard]] int Width() const;
        [[nodiscard]] int Height() const;
        void Store(const XFile* f) const;
        void Restore(const XFile* f);
};

#endif
