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

#ifndef __XTIME_H
#define __XTIME_H

class XFile;

class XTime
{
public:
	static int GetMonth();
	static char * GetMonthName();

	static int GetDay();
	static char * GetDayName();

	static int GetWeek();
	static char * GetWeekName();

	static int GetYear();
	
	static void RunTime();

	static int GetHour();
	static int GetSec();
	static int GetMin();

	static void Store(XFile * f);
	static void Restore(XFile * f);

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
