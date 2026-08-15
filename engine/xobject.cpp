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

#include <algorithm>
#include <fstream>
#include <string>
#include <fmt/format.h>

#include "engine/global.h"
#include "engine/xobject.h"

long XObject::invalid_count = 0;
XObjectMap XObject::objects = XObjectMap();
std::vector<std::shared_ptr<XObject>> XObject::deferred_release;

XGUID guid = 1;

XClassInfo* XClassFactory::first_class = nullptr;
int XClassFactory::counter = 0;

XClassFactory::XClassFactory(const std::string& name, CLASS_CREATOR pClassNew)
{
    if (!first_class) {
        first_class = new XClassInfo(name, pClassNew);
    } else {
        XClassInfo * tmp = first_class;

        while (tmp->next) {
            tmp = tmp->next;
        }

        tmp->next = new XClassInfo(name, pClassNew);
    }

    counter++;
}

XClassFactory::~XClassFactory()
{
    if (--counter <= 0) {
        const XClassInfo* tmp = first_class;

        while (tmp) {
            const XClassInfo * del = tmp;
            tmp = tmp->next;
            delete del;
        }

        first_class = nullptr;
    }
}

XObject* XClassFactory::CreateNew(const std::string& name)
{
    const XClassInfo* tmp = first_class;

    while (tmp) {
        if (tmp->name == name) {
            return tmp->pClassNew();
        }

        tmp = tmp->next;
    }

    return nullptr;
}

void XObject::InvalidateAllObjects()
{
    // Erase-first, and always re-derive the next target from the live map:
    // Invalidate() may cascade into deleting other objects (a location
    // invalidating its places, say), so iterating `objects` directly - or
    // even a pre-captured snapshot of it - risks an invalidated loop
    // iterator or a dangling pointer to an object that is already gone.
    //
    // Taking the entry out up front is what guarantees this terminates.
    // The old form left removal entirely to Invalidate(), which erases by
    // identity (`objects[xguid] == this`) and so declines to erase when an
    // object's xguid no longer matches the key it is filed under - a
    // Cereal-restored object can end up in exactly that state. Such an
    // entry would never leave the map, while its already-invalid object
    // made every further Invalidate() a no-op: teardown spinning forever
    // on a full registry, the same shape of hang that used to happen in
    // XMapTile's item loop. Erasing the iterator we already hold also
    // sidesteps the identity question entirely - it is this object's own
    // entry by construction.
    while (!objects.empty()) {
        const auto it = objects.begin();
        XObject* obj = it->second;
        objects.erase(it);

        // Safe to keep using obj: `objects` is a non-owning registry of
        // raw pointers, so erasing the entry cannot destroy anything.
        obj->Invalidate();
    }

    // The sweep above parks self-evicting map objects (see
    // XMapObject::Invalidate()) in the graveyard - actually release them
    // now that nothing is mid-turn.
    DrainDeferred();
}

void XObject::DeferRelease(std::shared_ptr<XObject> p)
{
    if (p) {
        deferred_release.push_back(std::move(p));
    }
}

void XObject::DrainDeferred()
{
    // Swap-then-clear, repeated: destroying a parked object can itself
    // Invalidate()/evict further objects into the graveyard (a deleter
    // running Invalidate() on a never-shared legacy object, say) -
    // clearing the live vector in place would push into a vector that's
    // being iterated by its own clear().
    while (!deferred_release.empty()) {
        std::vector<std::shared_ptr<XObject>> batch;
        batch.swap(deferred_release);
        batch.clear();
    }
}
