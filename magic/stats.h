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

#ifndef __STATS_H
#define __STATS_H

#include <stdio.h>
#include "dice.h"
#include "strproc.h"
#include "global.h"

enum STATS {S_UNKNOWN = -1, S_STR, S_DEX, S_TOU, S_LEN, S_WIL, S_MAN, S_PER, S_CHR, S_EOF};

extern char * stats_str[];

class XStats
{
public:
	XStats(const char * str); // must be the same! "St:1d2 Dx:1d4 To:2d5"
	XStats(XStats * xs);
	XStats(); //all stats == 0 by default
	int Get(STATS s) { return stats[s] / 100; }
	char * GetName(STATS s) { return stats_str[s]; }
	char * GetFullName(STATS s);
	void SetStat(STATS s, int val) { stats[s] = val * 100; }
	void Modify(STATS s, int val) { stats[s] += val * 100; }
	void AddFract(STATS s, int val) {stats[s] += val;}
	void Set(XStats * s);
	void Set(const char * str);
	void Add(XStats * s);
	void Sub(XStats * s);

	void Store(XFile * f);
	void Restore(XFile * f);

	static STATS Random() { return (STATS)(vRand(S_EOF)); }

protected:
	int stats[S_EOF];
};

#endif
