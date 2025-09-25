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

#include "creature/bodypart.h"
#include "creature/creature.h"

REGISTER_CLASS(XBodyPart);

const char* bp_names[] = {"",
        "head", "neck", "body", "cloak",
        "hand", "ring", "gloves",
        "boots", "light_source", "tool", "missile_weapon", "missile", "eof"
    };

ITEM_MASK bpim[] = {IM_OTHER,
        IM_HAT, IM_NECK, IM_BODY, IM_CLOAK,
        IM_HAND, IM_RING, IM_GLOVES,
        IM_BOOTS, IM_LIGHTSOURCE, IM_TOOL, IM_MISSILEW, IM_MISSILE, IM_ALL
    };

XBodyPart::XBodyPart(XCreature* _owner, const BODY_PART bp) : owner(_owner)
{
    owner = _owner;
    bp_uin = bp;
    item = nullptr;
    //	im     = bpim[bp];
    im	= IM_OTHER;
}

ITEM_MASK XBodyPart::GetProperIM() const
{
    return bpim[bp_uin];
}

void XBodyPart::Invalidate()
{
    item = nullptr;
    owner = nullptr;

    XObject::Invalidate();
}

const char* XBodyPart::GetName() const
{
    return bp_names[bp_uin];
}

const char* XBodyPart::XGetName(const BODY_PART bp)
{
    return bp_names[bp];
}

int XBodyPart::Wear(XItem* new_item)
{
    assert(new_item);

    if (Fit(new_item->bp)) {
        if (item) {
            return 1;
        }

        item = new_item;
        owner->CarryItem(item.get());
        item->onWear(owner.get());
        return 0;
    } else {
        return 2;
    }
}

XItem* XBodyPart::UnWear()
{
    assert(owner && item);
    item->onUnWear(owner.get());
    XItem * tmp = item.get();
    item = nullptr;
    return tmp;
}

XItem* XBodyPart::Item() const
{
    if (item) {
        return item.get();
    }

    return nullptr;
}

int XBodyPart::GetPartSize() const
{
    switch (bp_uin) {
        case BP_HEAD :
            return 10;
            break;

        case BP_BODY :
            return 100;
            break;

        case BP_GLOVES :
        case BP_BOOTS :
            return 20;
            break;

        default :
            return 0;
            break;
    }

    return 0;
}

void XBodyPart::Store(XFile* f)
{
    XObject::Store(f);
    f->Write(&bp_uin);
    item.Store(f);
    owner.Store(f);
}

void XBodyPart::Restore(XFile* f)
{
    XObject::Restore(f);
    f->Read(&bp_uin);
    item.Restore(f);
    owner.Restore(f);
}

void XBodyPart::Create(XCreature* cr, const char* str)
{
    int l = 0;
    int k = 0;
    char buf[100];

    while (str[k]) {
        while (str[k] == ' ') {
            k++;
        }

        l = k;

        while (str[k] != ' ' && str[k]) {
            k++;
        }

        strncpy(buf, &str[l], k - l);
        buf[k - l] = 0;
        int i;

        for (i = 0; i < BP_EOF; i++)
            if (strcmp(buf, XGetName(static_cast<BODY_PART>(i))) == 0) {
                auto* bp = new XBodyPart(cr, static_cast<BODY_PART>(i));
                cr->components.Add(bp);
                break;
            }

        assert(i < BP_EOF);
    }
}
