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

#ifndef __XSCHEDULER_H__
#define __XSCHEDULER_H__

#include "engine/xlist.h"

constexpr int XSCHEDULER_TIME_SLICE = 100;
constexpr int XSCHEDULER_STEPS_AHEAD = 100;

class XMapObject;

class XScheduler
{
        long _time, head;
        XList<XObject*> data[XSCHEDULER_STEPS_AHEAD];
        void Place(XObject * p);
    public:
        XScheduler() : _time(0), head(0) { }

        ~XScheduler()
        {
            while (Get()) {
                Remove();
            }
        }

        void SetTime(long t)
        {
            _time = t;
        }

        [[nodiscard]] long GetTime() const
        {
            return _time;
        }

        void Add(XObject * p);
        XObject* Get();
        XObject* Remove();

        void Store(XFile * f);
        void Restore(XFile * f);
};

#endif
