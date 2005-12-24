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

#ifndef __RESIST_H
#define __RESIST_H

#include "strproc.h"
#include "dice.h"

enum RESISTANCE {R_NONE = -1,
R_WHITE, R_BLACK, R_FIRE, R_WATER, R_AIR, R_EARTH,
R_ACID, R_COLD, R_POISON, R_DISEASE, R_PARALYSE,
R_STUN, R_CONFUSE, R_BLIND,
R_LIGHT, R_DARKNESS, R_INVISIBLE, R_SEEINVISIBLE,
R_EOF};

enum FLUENCE {FLU_NONE = 0, FLU_CREATURE = 1, FLU_ITEM = 2, FLU_ALL = 3};

struct RESIST_REC
{
	char * name; //life, fire, death etc.
	FLUENCE flag;
};

class XResistance
{
public:
	XResistance(XResistance * xr);
    XResistance();
	XResistance(const char * str1); //format fire:3d6+N water:2d2+3
//	~XResistance() {}
	int GetResistance(RESISTANCE r){return resistances[r];}
	void SetResistance(RESISTANCE r, int val){resistances[r] = val;}
	void ChangeResistance(RESISTANCE r, int val){resistances[r] += val;}
	void Add(XResistance * r);
	void Sub(XResistance * r);
	void Set(XResistance * r);
	char * GetResistanceName(RESISTANCE r);
	char * GetResistanceLevel(RESISTANCE r);
	bool isEqual(XResistance * xr);

	void Store(XFile * f);
	void Restore(XFile * f);
protected:
	int resistances[R_EOF];
};


class XResistGenerator
{
	XDice resist[R_EOF];
public:
	XResistGenerator();
	void Init(const char * str);
	XResistance * Generate();
};

#endif
