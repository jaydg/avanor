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

#ifndef __RECT_H
#define __RECT_H

#include "engine/xfile.h"
#include "helpers/point.h"

class XRect
{
    public:
        int left, top, right, bottom;

        XRect() {}

        XRect(int _left, int _top, int _right, int _bottom)
        {
            left = _left;
            top = _top;
            right = _right;
            bottom = _bottom;
        }

        XRect(XPoint * lt, XPoint * rb)
        {
            left = lt->x;
            top = lt->y;
            right = rb->x;
            bottom = rb->y;
        }

        XRect(XRect& r)
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

        void Setup(int _left, int _top, int _right, int _bottom)
        {
            left = _left;
            top = _top;
            right = _right;
            bottom = _bottom;
        }

        int Intersect(XRect * r);
        int PointIn(XPoint * pt);
        int PointIn(int x, int y);
        void Grow(int r);
        int Width();
        int Hight();
        void Store(XFile * f);
        void Restore(XFile * f);
};

#endif
