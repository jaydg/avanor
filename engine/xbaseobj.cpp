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

#include "engine/global.h"
#include "engine/xbaseobj.h"

#include "xfile.h"
#include "xobject.h"

XBaseObject::XBaseObject() :
    RNG(0), weight(0), resistances(nullptr), stats(nullptr)
{
}

void XBaseObject::Invalidate()
{
    XMapObject::Invalidate();
}

XBaseObject::XBaseObject(XBaseObject * copy) :
    XMapObject(static_cast<XMapObject *>(copy)),
    _DV(copy->_DV),
    _PV(copy->_PV),
    _HIT(copy->_HIT),
    RNG(copy->RNG),
    _HP(copy->_HP),
    _PP(copy->_PP),
    MAX_HP(copy->MAX_HP),
    MAX_PP(copy->MAX_PP),
    weight(copy->weight),
    dice(copy->dice)
{
    if (copy->resistances) {
        resistances = std::make_unique<XResistance>(copy->resistances.get());
    } else {
        resistances = nullptr;
    }

    if (copy->stats) {
        stats = std::make_unique<XStats>(copy->stats.get());
    } else {
        stats = nullptr;
    }
}

int XBaseObject::Compare(XObject * o)
{
    assert(dynamic_cast<XBaseObject*>(o));

    auto* tit = dynamic_cast<XBaseObject *>(o);

    if (XMapObject::Compare(o) == 0
        && _DV == tit->_DV && _PV == tit->_PV && RNG == tit->RNG
        && _HIT == tit->_HIT && dice.X == tit->dice.X
        && dice.Y == tit->dice.Y && dice.Z == tit->dice.Z
        && resistances->isEqual(tit->resistances.get()) && stats->isEqual(tit->stats.get())) {
        return 0;
    } else {
        return 1;
    }
}

void XBaseObject::Store(XFile* f)
{
    XMapObject::Store(f);

    f->Write(&_DV);
    f->Write(&_PV);
    f->Write(&_HIT);
    f->Write(&RNG);
    f->Write(&_HP);
    f->Write(&_PP);
    f->Write(&MAX_HP);
    f->Write(&MAX_PP);
    f->Write(&weight);

    dice.Store(f);

    int flag = 0;

    if (resistances) {
        flag = 1;
    }

    f->Write(&flag, sizeof(int));

    if (flag) {
        resistances->Store(f);
    }

    flag = 0;

    if (stats) {
        flag = 1;
    }

    f->Write(&flag, sizeof(int));

    if (flag) {
        stats->Store(f);
    }
}


void XBaseObject::Restore(XFile * f)
{
    XMapObject::Restore(f);

    f->Read(&_DV);
    f->Read(&_PV);
    f->Read(&_HIT);
    f->Read(&RNG);
    f->Read(&_HP);
    f->Read(&_PP);
    f->Read(&MAX_HP);
    f->Read(&MAX_PP);
    f->Read(&weight);

    dice.Restore(f);

    int flag = 0;
    f->Read(&flag, sizeof(int));

    if (flag) {
        resistances = std::make_unique<XResistance>();
        resistances->Restore(f);
    } else {
        resistances = nullptr;
    }

    flag = 0;
    f->Read(&flag, sizeof(int));

    if (flag) {
        stats = std::make_unique<XStats>();
        stats->Restore(f);
    } else {
        stats = nullptr;
    }
}
