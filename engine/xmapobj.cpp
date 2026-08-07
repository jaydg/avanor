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

#include <cereal/archives/json.hpp>
#include <cereal/types/polymorphic.hpp>

#include "engine/xmapobj.h"

// XMapObject is never itself the dynamic type of a serialized object
// (only ever a base class in the middle of a longer chain), so it
// doesn't need its own CEREAL_REGISTER_TYPE - this relation just
// connects the polymorphic pointer-cast graph so shared_ptr<XObject>
// (e.g. XScheduler::Entry) can resolve any XMapObject-derived leaf.
CEREAL_REGISTER_POLYMORPHIC_RELATION(XObject, XMapObject);

XMapObject::XMapObject(XMapObject* copy) :
    XObject(static_cast<XObject *>(copy)),
    x(copy->x),
    y(copy->y),
    nx(copy->nx),
    ny(copy->ny),
    view(copy->view),
    color(copy->color)
{
    l = copy->l;
    name = copy->name;
}

void XMapObject::Invalidate()
{
    SetLocation(nullptr);
    XObject::Invalidate();
}

int XMapObject::Compare(XObject * o)
{
    assert(dynamic_cast<XMapObject*>(o));

    if (!isValid() || !o->isValid()) {
        return 1;
    }

    auto tit = dynamic_cast<XMapObject *>(o);

    if (tit->x == x && tit->y == y && tit->view == view) {
        return 0;
    } else {
        return 1;
    }
}

int XMapObject::Distance(const XMapObject* other) const
{
    int d = (other->x - this->x) * (other->x - this->x) + (other->y - this->y) * (other->y - this->y);

    return static_cast<int>(sqrt(static_cast<double>(d)));
}
