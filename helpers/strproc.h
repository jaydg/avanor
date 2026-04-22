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

#ifndef STRPROC_H
#define STRPROC_H

#include "engine/xlist.h"
#include "helpers/dice.h"

// warning!!! this class don't do a copy of string content, so
// beware it cause an error!

class XStringProc
{
    public:
        explicit XStringProc(const char* _str)
        {
            str = _str;
            index = 0;
        }

        void Reset()
        {
            index = 0;
        }

        int GetData(char* buf, char delimiter = ' ');
        int GetParam(char* buf, const char* param) const;

    protected:
        const char* str;
        int index;

};

// Format
// "keyword:dice keyword:dice keyword dice keyword dice"
// "St 1d1+2 St 1d1" is the same to "St 2d1+2"
// "St 2d2+1 St 3d3-1" is the same to "St 5d3"
struct KEYWORD_DICE_PAIR {
    int keyword_index{};
    XDice dice;
};

class XStringProcEx
{
        XQList<KEYWORD_DICE_PAIR> pairs;
    public:
        explicit XStringProcEx(const char* str);
        XQList<KEYWORD_DICE_PAIR>* GetPairsList()
        {
            return &pairs;
        }
};

#endif
