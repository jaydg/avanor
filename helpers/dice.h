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

#ifndef __DICE_H
#define __DICE_H

#include <cassert>
#include <cstdlib>
#include <cstdio>
#include <cstring>

class XFile;

// this class is represent XdY : throw Y - side cube X turns
class XDice
{
    public:
        XDice() {}

        XDice(int _x, int _y, int _z = 0)
        {
            Setup(_x, _y, _z);
        }

        XDice(XDice * d)
        {
            Setup(d);
        }

        XDice(const char* str)
        {
            Setup(str); // represent "XdY +Z" for example "2d6 - 5" or "4d12 + 30"
        }

        void Setup(int _x, int _y, int _z = 0)
        {
            X = _x;
            Y = _y;
            Z = _z;
            Throw();
        }

        void Setup(const char* str);
        void Setup(const XDice & d)
        {
            Setup(d.X, d.Y, d.Z);
        }

        void Add(XDice * d)
        {
            X += d->X;
            Y = (Y + d->Y) / 2;
            Z += d->Z;
        }

        void Add(int _X, int _Y, int _Z)
        {
            X += _X;
            Y += _Y;
            Z += _Z;
        }

        int Throw();
        bool isEqual(XDice * d)
        {
            return X == d->X && Y == d->Y && Z == d->Z;
        }

        int S; // generated result by throw
        int X;
        int Y;
        int Z;

        void Store(XFile * f);
        void Restore(XFile * f);

        // the function of very good (for Avanor's purpose) number destribution
        // current version can generate value from 0 to 20 with next probability
        // 0 - 75%
        // 1 - 5%
        // 20 - 0.1%
        static int DFunc();
        // normalization DFunc to (-maximum, +maximum)
        static int NDFunc(int maximum);

        // throw normalized by DFunc
        int NThrow();
};

#endif
