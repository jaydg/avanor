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

#include "stats.h"
#include "xfile.h"

char * stats_str[] = {"St", "Dx", "To", "Le", "Wi", "Ma", "Pe", "Ch", "EOF"};
char * stats_full_str[] = {
"Strength", "Dexterity", "Toughness", "Learning", "Willpower", 
"Mana", "Perception", "Charisma", "EOF"
};

XStats::XStats()
{
	for(int i = S_STR; i < S_EOF; i++)
		stats[i] = 0;
}

XStats::XStats(XStats * xs)
{
	if (xs)
		for(int i = S_STR; i < S_EOF; i++)
			stats[i] = xs->Get((STATS)i);
}


XStats::XStats(const char * str)
{
	Set(str);
}

void XStats::Set(const char * str)
{
	XStringProc xsp(str);
	char buf[256];
	XDice d;
	for(int i = S_STR; i < S_EOF; i++)
	{
		if (xsp.GetParam(buf, stats_str[i]))
		{
			d.Setup(buf);
			stats[i] = d.Throw() * 100;
		} else
			stats[i] = 0;
	}
}

char * XStats::GetFullName(STATS s)
{
	return stats_full_str[s];
}

void XStats::Add(XStats * s)
{
	for(int i = S_STR; i < S_EOF; i++)
		stats[i] += s->stats[i];
}

void XStats::Sub(XStats * s)
{
	for(int i = S_STR; i < S_EOF; i++)
		stats[i] -= s->stats[i];
}

void XStats::Set(XStats * s)
{
	for(int i = S_STR; i < S_EOF; i++)
		stats[i] = s->stats[i];
}


void XStats::Store(XFile * f)
{
	f->Write(&stats[S_STR], sizeof(int), S_EOF);
}

void XStats::Restore(XFile * f)
{
	f->Read(&stats[S_STR], sizeof(int), S_EOF);
}
