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

#ifndef PLAIN_BUILDER_H
#define PLAIN_BUILDER_H

class XLocation;

// Open country: grass and trees inside a ring of mountains that erodes
// down to hills, so the map has an edge the player cannot walk off.
class XPlainBuilder
{
    public:
        XPlainBuilder(XLocation* _location, int _w, int _h)
            : location(_location), w(_w), h(_h)
        {
        }

        void Build();

    private:
        XLocation* location;
        int w, h;
};

#endif
