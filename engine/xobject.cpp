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
#include <sol/sol.hpp>

#include "engine/global.h"
#include "engine/xobject.h"

void RegisterItemMaskEnum(sol::state_view& lua)
{
    lua.new_enum("ITEM_MASK",
        "IM_HAT", IM_HAT,
        "IM_NECK", IM_NECK,
        "IM_BODY", IM_BODY,
        "IM_CLOAK", IM_CLOAK,
        "IM_WEAPON", IM_WEAPON,
        "IM_SHIELD", IM_SHIELD,
        "IM_HAND", IM_HAND,
        "IM_GLOVES", IM_GLOVES,
        "IM_RING", IM_RING,
        "IM_BOOTS", IM_BOOTS,
        "IM_MISSILEW", IM_MISSILEW,
        "IM_MISSILE", IM_MISSILE,
        "IM_POTION", IM_POTION,
        "IM_SCROLL", IM_SCROLL,
        "IM_BOOK", IM_BOOK,
        "IM_WAND", IM_WAND,
        "IM_FOOD", IM_FOOD,
        "IM_LIGHTSOURCE", IM_LIGHTSOURCE,
        "IM_TOOL", IM_TOOL,
        "IM_GEM", IM_GEM,
        "IM_MONEY", IM_MONEY,
        "IM_STACKABLE", IM_STACKABLE,
        "IM_CHEST", IM_CHEST,
        "IM_ARMOUR", IM_ARMOUR,
        "IM_ITEM", IM_ITEM
    );
}

long XObject::invalid_count = 0;
XObjectMap XObject::objects = XObjectMap();

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
    // Invalidate() removes its own entry from objects and may cascade into
    // deleting other objects, e.g. a location invalidating its places.
    // Iterating objects directly, or even a pre-captured snapshot of it,
    // risks either an invalidated loop iterator or a dangling pointer to
    // an object that already got deleted.
    //
    // Always re-derive the next target from the live map instead.
    while (!objects.empty()) {
        objects.begin()->second->Invalidate();
    }
}
