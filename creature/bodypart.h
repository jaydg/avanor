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

#ifndef __BODYPART_H
#define __BODYPART_H

#include "xobject.h"
#include "xmapobj.h"
#include "item.h"

class XItem;
class XCreature;

class XBodyPart : public XObject
{
	XPtr<XItem>     item; 	//main item for this body part;
   XPtr<XCreature> owner;
	XBodyPart() {}
public:
	DECLARE_CREATOR(XBodyPart, XObject);
	XBodyPart(XCreature * _owner, BODYPART bp);
	virtual void Invalidate();
	const char* GetName();
	static const char* XGetName(BODYPART bp);
	static void Create(XCreature * cr, const char * str);
	int Compare(XObject * o) { return 1; }

	int Fit(BODYPART bp) { return (bp == bp_uin); }
	BODYPART bp_uin;

	XItem * Item();
   int     Wear(XItem * item);
   XItem * UnWear();

	int GetPartSize();
	ITEM_MASK GetProperIM();

	virtual void Store(XFile * f);
	virtual void Restore(XFile * f);
};

#endif
