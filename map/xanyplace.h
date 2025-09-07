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

#ifndef __XANYPLACE_H
#define __XANYPLACE_H

#include "helpers/rect.h"
#include "global/global.h"
#include "engine/xobject.h"

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
        XAnyPlace(XRect& _area, XLocation * _loc);
        XAnyPlace(XRect& _area, XLocation * _loc, char* _onEventLua);
        ~XAnyPlace();
        virtual void Invalidate();

        virtual int onCreatureMove(XCreature * cr);
        virtual int onCreatureEnter(XCreature * cr);
        virtual int onCreatureLeave(XCreature * cr);
        virtual int onCreaturePickItem(XCreature * cr, XItem * item)
        {
            return 1;
        }

        virtual int onCreatureDropItem(XCreature * cr, XItem * item)
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

        virtual void Store(XFile * f);
        virtual void Restore(XFile * f);
        void Setup(XLocation * _map);
        XPtr<XLocation> location;
    protected:
        XRect area;
        XPtr<XCreature> owner;
        char* onEventLua;
};

#endif
