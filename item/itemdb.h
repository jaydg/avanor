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

#ifndef __ITEMDB_H
#define __ITEMDB_H

#include "defs.h"
#include "itemdef.h"

#define DB_PROP_SZ	15 //number of materials!
#define ENH_DB_SZ	20 //number of special powers ("of Strength")


struct _MAIN_ITEM_STRUCT
{
	ITEM_TYPE it;
	const char* name;
	char view;
	const char* dv;
	const char* pv;
	const char* hit;
	const char* dice;
	const char* z; //random z to dice;
	const char* r;
	ITEM_SET iset;
	int value;
	int valume;
	int probability;
	ITEM_QUALITY iq;
	const char* breserved;
};

class XItemBasicStructure
{
public:
	XItemBasicStructure(_MAIN_ITEM_STRUCT * pIt, int count)
	{
		total_prob = 0;
		pFirstItem = pIt;
		total_item = count;
		for (int i = 0; i < count; i++)
		{
			total_prob += pIt->probability;
			pIt++;
		}
	}
	int total_prob;
	int total_item;
	_MAIN_ITEM_STRUCT * pFirstItem;
};


struct ENHANCE_STRUCT
{
	const char* name;
	int color; //if 0 don't change a color
	const char* dv;
	const char* pv;
	const char* hit;
	const char* dice;
	const char* z; //random z to dice;
	const char* rng;
	int val; // value
	ITEM_MASK im;
	BRAND_TYPE brt;
	CAN_FLAG res_flag;
	SPECIAL_PROPERTY spp;
	const char* s; //stats
	const char* r; //resists
};

typedef _MAIN_ITEM_STRUCT XITEM_STRUCT[];
typedef XITEM_STRUCT * ITEM_STRUCT;

extern _ITEMPROP item_prop[DB_PROP_SZ];
extern ENHANCE_STRUCT ienh_db[ENH_DB_SZ];


#endif
