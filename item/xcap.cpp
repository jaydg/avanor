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

#include "xcap.h"

REGISTER_CLASS(XCap);

_MAIN_ITEM_STRUCT CAP_STRUCT[] = 
{
{IT_HAT,		"hat", 			'[',		"1d2-1",	"1d1-1",	"",		"1d1",	"",	"",		ISET_SOFT, 		1,		1,	0,	""},
{IT_CAP,		"cap",			'[',		"1d2-2",	"1d2-1",	"",		"1d1",  "", "",		ISET_SOFT, 		2,		2,	0,	""},
{IT_HELMET,		"helmet",		'[',		"1d2-3",	"1d3-1",	"",		"1d3",  "",	"",		ISET_HARDMETAL,	3,		3,	0,	""},
{IT_CROWN,		"crown",		'[',		"1d2-3",	"1d1-2",	"",		"1d3",  "",	"",		ISET_METAL, 	8,		2,	0,	""}
};
const int r_size = 4;

XCap::XCap(ITEM_TYPE _it)
{
	BasicFill(_it, CAP_STRUCT, r_size);
	bp = BP_HEAD;
	im = IM_HAT;
	_HIT = 0;
	dice.Z = 0;
	RNG = 0;
}

XCap::XCap(XCap * copy) : XClothe((XClothe *)copy)
{
}

