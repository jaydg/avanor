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

#ifndef XMAPOBJECT_H
#define XMAPOBJECT_H

#include <cereal/types/base_class.hpp>
#include <cereal/types/string.hpp>

#include "engine/xobject.h"

class XCreature;
class XLocation;

class XMapObject : public XObject
{
    public:
        XLocation* l;
        std::string name;

        DECLARE_CREATOR(XMapObject, XObject);

        XMapObject() : l(nullptr), name{}, x(-1), y(-1), view(0), color(0) {}

        explicit XMapObject(XMapObject * copy);

        // Mirrors XCreature::ToWeakPtr/XItem::ToWeakPtr: a map object is
        // not shared_from_this()-safe until something owns it through a
        // shared_ptr (XMapTile::pSpecialObject), so guard against that
        // rather than letting shared_from_this() throw std::bad_weak_ptr.
        // Lets an AI hold on to a door, stairway or trap without keeping
        // a raw pointer to something the map may destroy under it - and
        // lets that reference be saved.
        static std::weak_ptr<XMapObject> ToWeakPtr(XMapObject* o);

    protected:
        // teardown hook, called by XObject::Invalidate()
        void OnInvalidate() override;
    public:

        // Coordinates of any object
        int x, y;

        // character representation view of object
        char view;

        int color;

        // Compare() is an item-stacking predicate rooted at XItem, and this
        // is only ever reached as a qualified call from XItem::Compare().
        bool Compare(XObject* o);

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XObject>(this));
            // l, the owning XLocation, is deliberately not persisted - it is
            // re-established structurally via SetLocation() when this object
            // is restored into its owning location's map grid
            ar(x, y, view, color, name);
        }

        // Put this object on a map at (x, y), returning false if the cell
        // is already taken. Separate from the constructors so an object can
        // be created by class name (XClassFactory::CreateNew, reachable
        // from Lua as CreateObject) and placed afterwards.
        //
        // Subclasses that need more than a map cell - picking their
        // species, registering with the scheduler - override this; see
        // XLuaObject::PlaceAt().
        virtual bool PlaceAt(XLocation* location, int _x, int _y);

        bool SetLocation(XLocation* new_l)
        {
            l = new_l;
            return true;
        }

        virtual bool isVisible();

        virtual bool onOuterUse(XCreature * /*user*/)
        {
            return false;
        }

        void SetName(std::string_view _name)
        {
            name = _name;
        }

        void SetView(char v, int c)
        {
            view = v;
            color = c;
        }

        virtual const std::string GetName(XCreature * /*viewer*/)
        {
            return name;
        }

        // Check if object is in visible area to write "Someone hits your" etc.
        int isInVisibleArea() const;

        // Checks if this area is visible
        int isVisibleArea(int px, int py) const;

        // Calculate the distance to another XMapObjet
        int Distance(const XMapObject* other) const;

        // Some objects (e.g. herbs) can be picked up.
        virtual XObject* Pick(XCreature * /*picker*/)
        {
            return nullptr;
        }

};

#endif
