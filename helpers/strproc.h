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

#ifndef __STRPROC_H
#define __STRPROC_H

#include <string.h>
#include <assert.h>

//warning!!! this class don't do a copy of string content, so
//beware it cause an error!

class XStringProc
{
public:
	XStringProc(const char * _str) { str = _str; index = 0;}
	void Reset() {index = 0;}
	int GetData(char * buf, char delimiter = ' ');
	int GetParam(char * buf, char * param);

protected:
	const char * str;
	int index;

};

#endif
