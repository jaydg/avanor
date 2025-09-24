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

#include "engine/global.h"
#include "engine/xfile.h"
#include "helpers/dice.h"

void XDice::Setup(const char* str)
{
    if (str == nullptr || strlen(str) < 2) {
        X = Y = 0;
        Z = 0;
        Throw();
    } else {
        char buf[100];
        strcpy(buf, str);

        for (unsigned int i = 0; i < strlen(buf); i++) {
            if (buf[i] == 'd') {
                buf[i] = ' ';
            }

            if ((buf[i] == '+' || buf[i] == '-') && buf[i + 1] == ' ') {
                buf[i + 1] = buf[i];
                buf[i] = ' ';
            }
        }

        X = Y = 0;
        Z = 0;
        sscanf(buf, "%d %d %d", &X, &Y, &Z);
        assert(X >= 0 && Y >= 0);
        Throw();
    }
}

int XDice::Throw()
{
    S = Z;

    if (Y > 0)
        for (int i = 0; i < X; i++) {
            S += vRand() % Y + 1;
        }

    return S;

}

int XDice::NThrow() const
{
    return NDFunc(X * Y) + Z;
}

void XDice::Store(const XFile* f) const {
    size_t wc;

    wc = f->Write(&S); assert(wc == sizeof(S));
    wc = f->Write(&X); assert(wc == sizeof(X));
    wc = f->Write(&Y); assert(wc == sizeof(Y));
    wc = f->Write(&Z); assert(wc == sizeof(Z));
}

void XDice::Restore(const XFile* f)
{
    f->Read(&S);
    f->Read(&X);
    f->Read(&Y);
    f->Read(&Z);
}

int dfunc_data[22] =
{ 750, 800, 840, 870, 890, 902, 914, 926, 937, 947, 956, 964, 971, 977, 982, 986, 990, 993, 996, 998, 999, 100000};

int XDice::DFunc()
{
    int r = vRand(1000);
    int i = 0;

    while (dfunc_data[i] < r) {
        i++;
    }

    return i;
}

int XDice::NDFunc(const int maximum)
{
    return std::lround((DFunc() * maximum) / 20.0f);
}
