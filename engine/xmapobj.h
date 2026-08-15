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

#ifndef XMAPOBJECT_H
#define XMAPOBJECT_H

#include <cereal/types/base_class.hpp>
#include <cereal/types/string.hpp>

#include "engine/xobject.h"
#include "game/location.h"

class XCreature;
class XLocation;

class XMapObject : public XObject
{
    public:
        XLocation* l;
        std::string name;

        DECLARE_CREATOR(XMapObject, XObject);

        XMapObject() : l(nullptr), name{}, x(-1), y(-1), nx(-1), ny(-1), view(0), color(0) {}

        explicit XMapObject(XMapObject * copy);

    protected:
        // teardown hook, called by XObject::Invalidate()
        void OnInvalidate() override;
    public:

        // Coordinates of any object
        int x, y;

        // Used for the next turn
        int nx, ny;

        // character representation view of object
        char view;

        int color;

        // Compare() is an item-stacking predicate rooted at XItem, and this
        // is only ever reached as a qualified call from XItem::Compare().
        int Compare(XObject * o);

        template<class Archive>
        void serialize(Archive& ar)
        {
            ar(cereal::base_class<XObject>(this));
            // l, the owning XLocation, is deliberately not persisted - it is
            // re-established structurally via SetLocation() when this object
            // is restored into its owning location's map grid
            ar(x, y, nx, ny, view, color, name);
        }

        virtual std::string toString()
        {
            return "";
        }

        bool SetLocation(XLocation* new_l)
        {
            l = new_l;
            return true;
        }

        virtual bool isVisible()
        {
            return l->map->GetVisible(x, y);
        }

        virtual int onOuterUse(XCreature * user)
        {
            return 0;
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

        virtual const std::string GetName(XCreature *viewer)
        {
            return name;
        }

        // Check if object is in visible area to write "Someone hits your" etc.
        int isInVisibleArea() const
        {
            return l->map->GetVisible(x, y);
        }

        // Checks if this area is visible
        int isVisibleArea(int px, int py) const
        {
            return l->map->GetVisible(px, py);
        }

        // Calculate the distance to another XMapObjet
        int Distance(const XMapObject* other) const;

        // Some objects (e.g. herbs) can be picked up.
        virtual XObject* Pick(XCreature * picker)
        {
            return nullptr;
        }

};

#endif
