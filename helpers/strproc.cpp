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

#include "strproc.h"
#include "stats.h"
#include "resist.h"
#include "xstr.h"

int XStringProc::GetData(char * buf, char delimiter)
{
	while (str[index] && str[index] != delimiter) index++;
	if (!str[index]) return 0;
	index++;
	int k = index;
	if (delimiter == '(') delimiter = ')';
	if (delimiter == '[') delimiter = ']';
	if (delimiter == '{') delimiter = '}';
	while (str[index] && str[index] != delimiter) index++;
	assert(index - k > 0);
	strncpy(buf, &str[k], index - k);
	buf[index - k] = 0;
	index++;
	return 1;

}

int XStringProc::GetParam(char * buf, const char* param)
{
	assert(param);
	int i = 0;
	int xlen = strlen(param);
	while (str[i])
	{
		if (str[i] == param[0] && strncmp(&str[i], param, xlen) == 0 && (i == 0 || str[i - 1] == ' '))
		{
			int j = i + xlen + 1;
			i = j;
			while (str[j] > 32) j++;
			strncpy(buf, &str[i], j - i);
			buf[j - i] = 0;
			return 1;
		}
		i++;
	}
	strcpy(buf, "");
	return 0;
}

struct KEYWORD
{
	const char * key;
	int value;
};


KEYWORD keywords[] = {
	{"St", S_STR},
	{"Dx", S_DEX},
	{"To", S_TOU},
	{"Le", S_LEN},
	{"Wi", S_WIL},
	{"Ma", S_MAN},
	{"Pe", S_PER},
	{"Ch", S_CHR},

	{"white",		R_WHITE},
	{"black",		R_BLACK},
	{"fire",		R_FIRE},
	{"water",		R_WATER},
	{"air",			R_AIR},
	{"earth",		R_EARTH},
	{"acid",		R_ACID},
	{"cold",		R_COLD},
	{"poison",		R_POISON},
	{"decease",		R_DISEASE},
	{"paralyse",	R_PARALYSE},
	{"stun",		R_STUN},
	{"confuse",		R_CONFUSE},
	{"blind",		R_BLIND},
	{"light",		R_LIGHT},
	{"darkness",	R_DARKNESS},
	{"invisible",	R_INVISIBLE},
	{"see_invisible",R_SEEINVISIBLE},
	{NULL, -1}
};

XStringProcEx::XStringProcEx(const char* str)
{
	int i = 0;

	do 
	{
		while (str[i] == ' ') i++;

		int keyword_begin = i;
		while (str[i] > ' ' && str[i] != ':') i++;

		int kwd = 0;
		while (keywords[kwd].key && strnicmp(keywords[kwd].key, str + keyword_begin, i - keyword_begin) != 0)
			kwd++;
		assert(keywords[kwd].key);
		
		KEYWORD_DICE_PAIR key_pair;
		key_pair.keyword_index = keywords[kwd].value;

		while (str[i] == ' ' || str[i] == ':') i++;
		keyword_begin = i;
		while (str[i] > ' ' && str[i] != ':') i++;
		XStr dice_str(str + keyword_begin, i - keyword_begin);
		key_pair.dice.Setup(dice_str.c_str());
		pairs.push_back(key_pair);
	} while (str[i] > 0);
}