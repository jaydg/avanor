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

#include "helpers/msgwin.h"
#include "helpers/xgui.h"
#include <ctype.h>

XMsgWin msgwin;

XMsgWin::XMsgWin()
{
    history_list = new XGuiList();
    index_x = 0;
    index_y = 0;
    history_list->AddItem(new XGuiItem_Text("Welcome!"));
    strcpy(sent_buf, "");
}

XMsgWin::~XMsgWin()
{
    delete history_list;
}

void XMsgWin::AddLast(const char* str)
{
    static char buf[8192];
    sprintf(buf, "%s.", str);
    Add(buf);
};

void XMsgWin::Add(const char* tstr, int flag)
{
    char str[8192];
    strcpy(str, tstr);
    static bool end_sent = true;
    char xbuf[100];
    int word = 1;

    if (end_sent) {
        end_sent = false;
        str[0] = toupper(str[0]);
    }

    // addding part of sentence to a buffer
    strcat(sent_buf, str);
    strcat(sent_buf, " ");
    int tindex = 0;

    while (str[tindex]) {
        if (str[tindex] == '.' || str[tindex] == '!' || str[tindex] == '?') {
            end_sent = true;
            sent_buf[0] = toupper(sent_buf[0]);
            history_list->AddItem(new XGuiItem_Text(sent_buf), 1);
            strcpy(sent_buf, "");
            break;
        }

        tindex++;
    }

    while (GetWord(str, word, xbuf)) {
        int wlen = x_strlen(xbuf);

        if (index_y == 0) {
            if (index_x + wlen >= size_x) {
                index_y++;
                index_x = 0;
            }
        } else {
            if (index_x + wlen >= size_x - 8) {
                vSetAttr(xCYAN);
                vPutS("(more)");
                vRefresh();
                vGetch();
                ClrMsg();
                index_x = 0;
                index_y = 0;
            }
        }

        vGotoXY(index_x, index_y);
        vSetAttr(xLIGHTGRAY);
        vPutS(xbuf);
        index_x++;
        index_x += wlen;
        word++;
    }
}

void XMsgWin::ClrMsg(int flag)
{
    vGotoXY(0, 0);
    vClrEol();
    vGotoXY(0, 1);
    vClrEol();
    index_x = 0;
    index_y = 0;
}

int XMsgWin::GetWord(const char* str, int n, char* buf)
{
    int k = 0;
    int j = 0;

    while (n > 0) {
        while (str[k] && str[k] == ' ') {
            k++;
        }

        j = k;

        while (str[j] && str[j] != ' ') {
            j++;
        }

        if (j - k < 1) {
            return 0;
        }

        n--;

        if (n) {
            k = j;
        }
    }

    strncpy(buf, &str[k], j - k);
    buf[j - k] = 0;
    return 1;
}

void XMsgWin::ShowHistory()
{
    history_list->Run();
}
