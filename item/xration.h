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

#ifndef __XRATION_H
#define __XRATION_H

#include "xanyfood.h"
#include "itemdef.h"

class XRation : public XAnyFood
{
public:
	DECLARE_CREATOR(XRation, XAnyFood);
	XRation(ITEM_TYPE it = IT_RANDOM);
	XRation(XRation * copy) : XAnyFood((XAnyFood *)copy){}
	virtual XObject * MakeCopy() { return new XRation(this); }
	virtual RESULT onEat(XCreature * eater);
	
	virtual void Store(XFile * f) {XAnyFood::Store(f);}
	virtual void Restore(XFile * f) {XAnyFood::Restore(f);}

};

#endif
