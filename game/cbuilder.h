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

#ifndef __CBUILDER_H
#define __CBUILDER_H

#include "cave.h"

class XMap;
class XLocation;

class XCaveBuilder
{
	bool isCreateDoorTrapChest;
public:
	XMap * m;
	XLocation * location;
	XCaveBuilder(XLocation * _l, int create_door_trap_chest = 1);
	void Build();
	bool Link(XPoint * p1, XPoint * p2);
	void CreateDoors();
};

#endif
