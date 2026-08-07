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

#ifndef XTIME_H
#define XTIME_H

#include <cereal/cereal.hpp>

class XFile;

class XTime
{
    public:
        static int GetMonth();
        static const char* GetMonthName();

        static int GetDay();
        static const char* GetDayName();

        static int GetWeek();
        static const char* GetWeekName();

        static int GetYear();

        static void RunTime();

        static int GetHour();
        static int GetSec();
        static int GetMin();

        static void Store(XFile * f);
        static void Restore(XFile * f);

        // XTime has no instances (every field is static), so this can't
        // be a normal per-instance cereal serialize() picked up by ADL -
        // call it explicitly (XTime::serialize(ar)) from the top-level
        // archive code instead.
        template<class Archive>
        static void serialize(Archive& ar)
        {
            ar(sec, min, hour, day, month, year, tic);
        }

        static unsigned int tic;
    protected:

        static unsigned int sec;
        static unsigned int min;
        static unsigned int hour;
        static unsigned int day;
        static unsigned int month;
        static unsigned int year;
};

#endif
