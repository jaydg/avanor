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

#include "engine/xmapobj.h"
#include "engine/xobject.h"
#include "item/item.h"

class XItem;
class XCreature;

class XBodyPart final : public XObject
{
        XPtr<XItem> item;	//main item for this body part;
        XPtr<XCreature> owner;
        XBodyPart() : bp_uin() {
        }

    public:
        DECLARE_CREATOR(XBodyPart, XObject);
        XBodyPart(XCreature * _owner, BODYPART bp);
        void Invalidate() override;
        const char* GetName() const;
        static const char* XGetName(BODYPART bp);
        static void Create(XCreature* cr, const char* str);
        int Compare(XObject* o) override
        {
            return 1;
        }

        int Fit(const BODYPART bp) const
        {
            return (bp == bp_uin);
        }

        BODYPART bp_uin;

        XItem* Item() const;
        int Wear(XItem* new_item);
        XItem* UnWear();

        int GetPartSize() const;
        ITEM_MASK GetProperIM() const;

        void Store(XFile* f) override;
        void Restore(XFile* f) override;
};

#endif
