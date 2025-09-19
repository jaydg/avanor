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

#include <cstdlib>

#include "helpers/rect.h"

int XRect::Intersect(const XRect* r) const
{
    if (PointIn(r->left, r->top) ||
        PointIn(r->right, r->top) ||
        PointIn(r->right, r->bottom) ||
        PointIn(r->left, r->bottom)) {
        return 1;
    }

    // check for situation as shown on picture
    //
    // +--+
    // |  |
    // +-+--+-+
    // + +--+-+
    // |  |
    // +--+

    const int min_x = right - left + r->right - r->left;
    const int min_y = bottom - top + r->bottom - r->top;
    const int px = abs((right + left) / 2 - (r->right + r->left) / 2);

    if (const int py = abs((top + bottom) / 2 - (r->top + r->bottom) / 2); px < min_x && py < min_y) {
        return 1;
    } else {
        return 0;
    }
}

int XRect::PointIn(const XPoint* pt) const {
    if (pt->x >= left && pt->x < right && pt->y >= top && pt->y < bottom) {
        return 1;
    } else {
        return 0;
    }
}

int XRect::PointIn(const int x, const int y) const
{
    if (x >= left && x < right && y >= top && y < bottom) {
        return 1;
    } else {
        return 0;
    }
}

void XRect::Grow(const int r)
{
    left -= r;
    right += r;
    top -= r;
    bottom += r;
}

int XRect::Width() const
{
    return right - left;
}

int XRect::Height() const
{
    return bottom - top;
}

void XRect::Store(const XFile* f) const
{
    f->Write(&left, sizeof(int));
    f->Write(&right, sizeof(int));
    f->Write(&top, sizeof(int));
    f->Write(&bottom, sizeof(int));
}

void XRect::Restore(const XFile* f)
{
    f->Read(&left, sizeof(int));
    f->Read(&right, sizeof(int));
    f->Read(&top, sizeof(int));
    f->Read(&bottom, sizeof(int));
}
