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

#ifndef __DEFS01_H
#define __DEFS01_H
/*
#ifndef _CRTDBG_MAP_ALLOC
#define _CRTDBG_MAP_ALLOC
#endif
#include <stdlib.h>
#include <crtdbg.h>
*/
//#define NDEBUG
#ifndef NDEBUG
	#define XDEBUG
#endif

/*
#define MA_HAND		20 // mele attack by hand
#define MA_FOOT		21 // by foot
#define MA_BIT		22 // Bit!
#define MA_TAIL		23 // strike by tail
*/

/*
#define INIT_RTTI( name , x ) RTTI name::rtti = x
#define TYPES_EQ( namea , nameb ) namea::rtti == nameb::rtti

enum RTTI 
{
	RTTI_XOBJECT	= 0,
	RTTI_XBASIS		= 1,
	RTTI_XBASIC		= 2,
	RTTI_XSTAIRWAY	= 3

};
*/
#endif
