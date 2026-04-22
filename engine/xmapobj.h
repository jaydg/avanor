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

#include "engine/xobject.h"
#include "game/location.h"

class XCreature;
class XLocation;

class XMapObject : public XObject
{
    public:
        XPtr<XLocation> l;
        char name[50];

        DECLARE_CREATOR(XMapObject, XObject);
        XMapObject();
        explicit XMapObject(XMapObject * copy);
        virtual XObject* MakeCopy()
        {
            return new XMapObject(this);
        }

        void Invalidate() override;

        int x, y; //co-ordinates of any object
        int nx, ny; //used for new turn;

        char view; //char-represent view of object
        int color;


        int Compare(XObject * o) override;


        void Store(XFile * f) override;
        void Restore(XFile * f) override;
        virtual void toString(char* buf) {};

        bool SetLocation(XLocation * new_l);

        virtual bool isVisible()
        {
            return l->map->GetVisible(x, y);
        }

        virtual int onOuterUse(XCreature * user)
        {
            return 0;
        }

        void SetName(const char* _name)
        {
            strcpy(name, _name);
        }

        void SetView(char v, int c)
        {
            view = v;
            color = c;
        }

        virtual const char* GetName(XCreature * viewer)
        {
            return name;
        }

        int isInVisibleArea() const
        {
            return l->map->GetVisible(x, y); //check if object is in visible are to write Someone hits your etc.
        }

        int isVisibleArea(int x, int y) const
        {
            return l->map->GetVisible(x, y); //cheks, if this area is visible
        }

        virtual XObject* Pick(XCreature * picker)
        {
            return nullptr; //some objects (eg. Herbs) can be picked up.
        }

};

#endif
