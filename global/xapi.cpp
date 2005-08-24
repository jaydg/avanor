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

#include "xapi.h"
#include "global.h"

int vMax(int value1, int value2)
{
   return ((value1 > value2) ? value1 : value2);
}

int vMin(int value1, int value2)
{
   return ((value1 < value2) ? value1 : value2);
}

int sgn(int n)
{
   return ((n > 0) - (n < 0));
}

int vRound(float f)
{
	if (f >= 0)
		return f - (int)f < 0.5f ? (int)f : (int)f + 1;
	else
		return f - (int)f > -0.5f ? (int)f : (int)f - 1;
}


unsigned int vGetRandomBit(unsigned int mask)
{
	unsigned int b_n = 1;
	int rn_count = 0;
	int i;
	for (i = 0; i < 32; i++)
	{
		if (mask & b_n)
			rn_count++;
		b_n = b_n * 2;
	}
	int rn_val = vRand() % rn_count;
	b_n = 1;
	for (i = 0; i < 32; i++)
	{
	 	if (mask & b_n)
		{
			rn_val--;
			if (rn_val < 0) break;
		}
		b_n = b_n * 2;
	}
	return b_n;
}

int vGetBitNumber(unsigned int mask)
{
	int res = 0;
	while (mask > 1)
	{
		mask /= 2;
		res++;
	}
	return res;
}

int vGetHighBitNum(unsigned int number)
{
	assert(number > 0);
	int res = 31;
	while ((number & 0x80000000) == 0)
	{
		res--;
		number <<= 1;
	}
	return res;
}

int vBitsCount(unsigned int number)
{
	int res = 0;
	while ( number > 0 )
	{
		if (number & 1)
			res++;

		number >>= 1;
	}
	return res;
}


int vCheckForCursorKey(int key, int * dx, int * dy)
{
	int res = 1;
	if (dx && dy)
	{
		switch (key)
		{
			case '8'		:
			case KEY_UP     : *dx = 0; *dy = -1; break;
			
			case '2'		:
			case KEY_DOWN   : *dx = 0; *dy = +1; break;
			
			case '4'		:
			case KEY_LEFT   : *dx = -1; *dy = 0; break;
			
			case '6'		:
			case KEY_RIGHT  : *dx = +1; *dy = 0; break;

			case '7'		:
			case KEY_HOME   : *dx = -1; *dy = -1; break;

			case '1'		:
			case KEY_END    : *dx = -1; *dy = +1; break;

			case '9'		:
			case KEY_PGUP   : *dx = +1; *dy = -1; break;
			
			case '3'		:
			case KEY_PGDOWN : *dx = +1; *dy = +1; break;
			
			case '5'		:
			case KEY_CENTER : *dx = 0; *dy = 0; break;

			default	: res = 0; break;
		}
	} else
	{
		switch (key)
		{
			case '8'		:
			case KEY_UP     :
			case '2'		:
			case KEY_DOWN   :
			case '4'		:
			case KEY_LEFT   :
			case '6'		:
			case KEY_RIGHT  :
			case '7'		:
			case KEY_HOME   :
			case '1'		:
			case KEY_END    :
			case '9'		:
			case KEY_PGUP   :
			case '3'		:
			case KEY_PGDOWN :
			case '5'		:
			case KEY_CENTER :
				break;
			default	: res = 0; break;
		}
	}
	return res;
}
