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

#include "item/xclothe.h"

REGISTER_CLASS(XClothe);

XClothe::XClothe()
{

}

void XClothe::toString(char* buf)
{
    GetFullName(buf);

    if (isIdentifed()) {
        char tbuf[256];

        if (RNG != 0) {
            sprintf(tbuf, "<%+d>", RNG);
            strcat(buf, tbuf);
        }

        if (dice.Z != 0) {
            sprintf(tbuf, "(%+d, %+d)", _HIT, dice.Z);
            strcat(buf, tbuf);
        } else if (_HIT != 0) {
            sprintf(tbuf, "(%+d)", _HIT, dice.Z);
            strcat(buf, tbuf);
        }

        sprintf(tbuf, "[%+d, %+d]", _DV, _PV);
        strcat(buf, tbuf);
        StatsToString(tbuf);
        strcat(buf, tbuf);
    }
}
