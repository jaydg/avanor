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

#ifndef __MSGWIN_H
#define __MSGWIN_H

#include <string.h>
#include "global.h"


#define MSGWIN_X 0
#define MSGWIN_Y 0
#define MSGWIN_H 2
#define MSGWIN_L 80

class XGuiList;

class XMsgWin
{
    public:
        XMsgWin();
        ~XMsgWin();
        char sent_buf[1024];
        void Add(const char* str, int flag = 0); // flag == 1 if last space not needed
        void AddLast(const char* str); // and set dot.
        void ClrMsg(int flag = 0);
        void ShowHistory();
        int index_x;
        int index_y;
        int GetWord(const char* str, int n, char* buf); // return 0 if no such word;
        XGuiList* history_list;
};

extern XMsgWin msgwin;

#endif
