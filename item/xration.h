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

#ifndef XRATION_H
#define XRATION_H

#include "item/itemdef.h"
#include "item/xanyfood.h"

class XRation : public XAnyFood
{
    public:
        DECLARE_CREATOR(XRation, XAnyFood);

        explicit XRation(ITEM_TYPE it = IT_RANDOM);

        explicit XRation(XRation* copy) : XAnyFood(static_cast<XAnyFood*>(copy)) {}

        XObject* MakeCopy() override
        {
            return new XRation(this);
        }

        RESULT onEat(XCreature * eater) override;

        void Store(XFile * f) override
        {
            XAnyFood::Store(f);
        }

        void Restore(XFile * f) override
        {
            XAnyFood::Restore(f);
        }
};

#endif
