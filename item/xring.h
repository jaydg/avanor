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

#ifndef XRING_H
#define XRING_H

#include "item/xenhance.h"

class XRing : public XEnhance
{
    public:
        DECLARE_CREATOR(XRing, XEnhance);
        XRing(ENHANCE enh = ENH_EOF);
        XRing(XRing * copy) : XEnhance((XEnhance*)copy) {}

        int isIdentifed() override;
        void Identify(int level) override;
        XObject* MakeCopy() override
        {
            return new XRing(this);
        }

        static void StoreTable(XFile * f);
        static void RestoreTable(XFile * f);
};

#endif
