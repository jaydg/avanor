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

int XStringProc::GetParam(char * buf, char * param)
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

