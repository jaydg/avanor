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

#ifndef CAVE_BUILDER_H
#define CAVE_BUILDER_H

class XLocation;

// A natural cave: circular blobs stamped out of solid rock until the
// floor they leave behind is one connected whole.
class XCaveBuilder
{
    public:
        explicit XCaveBuilder(XLocation* _location) : location(_location)
        {
        }

        void Build();

    private:
        XLocation* location;
};

#endif
