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

#ifndef MODIFIER_H
#define MODIFIER_H

#include "engine/xlist.h"
#include "engine/xobject.h"
#include "magic/modifiers.h"

class XCreature;

class XModifier
{
    public:
        XModifier();
        ~XModifier();
        XModifier(XModifier * m);
        int Add(XBasicModifier * mod, XCreature * owner);
        int Add(MODIFIER_TYPE mt, int _val, XCreature * owner, XCreature * _cr = NULL);
        int Remove(MODIFIER_TYPE mdt, XCreature * owner);
        int Get(MODIFIER_TYPE mt); //return val
        int Run(XCreature * cr);
        void toString(char* buf);

        void Store(XFile * f);
        void Restore(XFile * f, XCreature * owner);

    protected:
        XList<XBasicModifier*> ml;
};

#endif
