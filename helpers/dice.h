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

class XFile;

// this class is represented XdY : throw Y - side cube X turns
class XDice
{
    public:
        XDice() = default;

        XDice(const int _x, const int _y, const int _z = 0)
        {
            Setup(_x, _y, _z);
        }

        explicit XDice(const XDice* d)
        {
            Setup(*d);
        }

        explicit XDice(const char* str)
        {
            Setup(str); // represent "XdY +Z" for example "2d6 - 5" or "4d12 + 30"
        }

        void Setup(const int _x, const int _y, const int _z = 0)
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

        void Add(const XDice* d)
        {
            X += d->X;
            Y = (Y + d->Y) / 2;
            Z += d->Z;
        }

        void Add(const int toX, const int toY, const int toZ)
        {
            X += toX;
            Y += toY;
            Z += toZ;
        }

        int Throw();
        bool isEqual(const XDice* d) const
        {
            return X == d->X && Y == d->Y && Z == d->Z;
        }

        int S{}; // generated result by throw
        int X{};
        int Y{};
        int Z{};

        void Store(const XFile* f) const;
        void Restore(const XFile* f);

        // the function of very good (for Avanor's purpose) number distribution
        // current version can generate value from 0 to 20 with next probability
        // 0 - 75%
        // 1 - 5%
        // 20 - 0.1%
        static int DFunc();
        // normalization DFunc to (-maximum, +maximum)
        static int NDFunc(int maximum);

        // throw normalized by DFunc
        [[nodiscard]] int NThrow() const;
};

#endif
