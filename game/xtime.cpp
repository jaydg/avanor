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

#include "engine/xfile.h"
#include "game/xtime.h"

unsigned int XTime::month = 4;
unsigned int XTime::day = 0;
unsigned int XTime::year = 243;
unsigned int XTime::sec = 0;
unsigned int XTime::min = 0;
unsigned int XTime::hour = 0;

unsigned int XTime::tic = 0;

const char* month_name[] = {
    "month_1",
    "month_2",
    "month_3",
    "month_4",
    "month_5",
    "month_6",
    "month_7",
    "month_8",
    "month_9",
    "month_10",
    "month_11",
    "month_12",
    "month_13",
};

const char* day_name[] = {
    "day_1",
    "day_2",
    "day_3",
    "day_4",
    "day_5",
    "day_6",
    "day_7",
};

const char* week_name[] = {
    "week_1",
    "week_2",
    "week_3",
    "week_4",
};

void XTime::Store(XFile * f)
{
    f->Write(&sec, sizeof(unsigned int));
    f->Write(&min, sizeof(unsigned int));
    f->Write(&hour, sizeof(unsigned int));
    f->Write(&day, sizeof(unsigned int));
    f->Write(&month, sizeof(unsigned int));
    f->Write(&year, sizeof(unsigned int));

    f->Write(&tic, sizeof(unsigned int));
}

void XTime::Restore(XFile * f)
{
    f->Read(&sec, sizeof(unsigned int));
    f->Read(&min, sizeof(unsigned int));
    f->Read(&hour, sizeof(unsigned int));
    f->Read(&day, sizeof(unsigned int));
    f->Read(&month, sizeof(unsigned int));
    f->Read(&year, sizeof(unsigned int));

    f->Read(&tic, sizeof(unsigned int));
}

int XTime::GetMonth()
{
    return month;
}

const char* XTime::GetMonthName()
{
    return month_name[month];
}

int XTime::GetDay()
{
    return day;
}

const char* XTime::GetDayName()
{
    return day_name[day % 7];
};

int XTime::GetWeek()
{
    return day / 7;
};

const char* XTime::GetWeekName()
{
    return week_name[day / 7];
};

int XTime::GetYear()
{
    return year;
}

int XTime::GetHour()
{
    return hour;
}

int XTime::GetMin()
{
    return min;
}

int XTime::GetSec()
{
    return sec;
}

void XTime::RunTime()
{
    sec += 17;

    if (sec > 60) {
        sec -= 60;
        min++;

        if (min > 59) {
            min = 0;
            hour++;

            if (hour > 23) {
                hour = 0;
                day++;

                if (day > 27) {
                    day = 0;
                    month++;

                    if (month > 12) {
                        month = 0;
                        year++;
                    }
                }
            }
        }
    }
}
