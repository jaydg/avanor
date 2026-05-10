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

#include <cctype>
#include <string>
#include <string_view>

#include <fmt/core.h>
#include <fmt/format.h>

#include "helpers/msgwin.h"
#include "helpers/xgui.h"
#include "helpers/xstring.h"

XMsgWin msgwin;

XMsgWin::XMsgWin()
{
    history_list = new XGuiList();
    index_x = 0;
    index_y = 0;
    history_list->AddItem(new XGuiItem_Text("Welcome!"));
    sent_buf.clear();
}

XMsgWin::~XMsgWin()
{
    delete history_list;
}

void XMsgWin::Add(std::string_view tstr, int flag)
{
    // Local copy as std::string, as we need to write str[0]
    std::string str(tstr);
    static bool end_sent = true;

    if (end_sent) {
        end_sent = false;
        str[0] = static_cast<char>(toupper(static_cast<unsigned char>(str[0])));
    }

    // Fill sentence buffer
    fmt::format_to(std::back_inserter(sent_buf), "{} ", str);

    // Find the end of the sentence
    const auto sentence_end = str.find_first_of(".!?");

    if (sentence_end != std::string::npos) {
        end_sent = true;
        sent_buf[0] = static_cast<char>(toupper(static_cast<unsigned char>(sent_buf[0])));
        history_list->AddItem(new XGuiItem_Text(sent_buf.c_str()), 1);
        sent_buf.clear();
    }

    std::string_view view(str);
    while (!view.empty()) {
        // skip leading spaces
        const auto word_start = view.find_first_not_of(' ');
        if (word_start == std::string_view::npos)
            break;
        view.remove_prefix(word_start);

        // find end of word
        const auto word_end = view.find(' ');
        const auto word = view.substr(0, word_end);
        const int wlen = static_cast<int>(word.size());

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

        // vPutS expects a null terminated string
        // convert once per word
        const std::string word_str(word);
        vPutS(word_str);
        index_x++;
        index_x += wlen;

        if (word_end == std::string_view::npos)
            break;

        view.remove_prefix(word_end);
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

void XMsgWin::ShowHistory() const
{
    history_list->Run();
}
