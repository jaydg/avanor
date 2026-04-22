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

#ifndef XANYPLACE_H
#define XANYPLACE_H

#include "engine/xobject.h"
#include "helpers/rect.h"

class XMap;
class XCreature;
class XItem;
class XLocation;

class XAnyPlace : public XObject
{
    protected:
        XAnyPlace() {}

    public:
        DECLARE_CREATOR(XAnyPlace, XObject);
        XAnyPlace(const XRect& _area, XLocation* _loc);
        XAnyPlace(const XRect& _area, XLocation* _loc, const char* _onEventLua);
        ~XAnyPlace() override;
        void Invalidate() override;

        virtual int onCreatureMove(XCreature* cr);
        virtual int onCreatureEnter(XCreature* cr);
        virtual int onCreatureLeave(XCreature* cr);
        virtual int onCreaturePickItem(XCreature* cr, XItem* item)
        {
            return 1;
        }

        virtual int onCreatureDropItem(XCreature* cr, XItem* item)
        {
            return 1;
        }

        virtual void onShowItem(XItem * item, char* buf);

        XRect &GetArea()
        {
            return area;
        }

        virtual XPtr<XCreature> &GetOwner()
        {
            return owner;
        }

        void Store(XFile* f) override;
        void Restore(XFile* f) override;
        void Setup(XLocation* _map);
        XPtr<XLocation> location;

    protected:
        XRect area;
        XPtr<XCreature> owner;
        char* onEventLua{};
};

#endif
