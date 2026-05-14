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

#include "creature/xhero.h"
#include "helpers/xgui.h"
#include "helpers/xstring.h"

#include <algorithm>

// The XGuiItem_Text::WideBuffer() function tries to widen the input string
// str to the given width. If it is impossible (size already is equal or
// greater than requested), the function simply exits and does nothing.
void XGuiItem_Text::WideBuffer(std::string& str, const std::string::size_type new_width) {
    // Determine the length of the string, excluding color escape sequences
    std::string::difference_type num_escapes = std::count(str.begin(), str.end(), 0x1F);
    std::string::size_type len = str.size() - (num_escapes * 2);

    // Find spaces (these may consist of one or more ' ' characters)
    std::vector<std::string::size_type> spaces;

    for(std::string::size_type i = 0; i < str.size(); ++i) {
        if (str[i] == ' ') {
            spaces.push_back(i);

            while (str[i]== ' ') {
                ++i;
            }

            if (i == str.size()) {
               break;
            }
        }
    }

    if (len >= new_width || spaces.empty()) {
        return;
    }

    std::string::size_type missing_spaces = new_width - len;
    std::string::size_type spaces_per_space = std::max(static_cast<std::string::size_type>(1), missing_spaces / spaces.size());

    while (!spaces.empty() && missing_spaces > 0) {
        const auto position = spaces.back();
        str.insert(position, spaces_per_space, ' ');

        spaces.pop_back();
        missing_spaces -= spaces_per_space;
    }
}

bool XGuiItem_Text::SetWidth(const std::string::size_type new_width) {
    char color = xLIGHTGRAY;
    std::size_t start = 0;

    Clear();

    while (true) {
        bool is_last_line = false, is_line_break = false;

        if (start == text.size()) {
            break;
        }

        std::size_t last_space = 0;
        std::size_t size = 0; // count of characters, including color codes
        std::size_t len = 0;  // count of visible characters

        while (len <= new_width) {
            if (text[size + start] == 0x1F) {
                size += 2;
                continue;
            }

            if ((size + start) == text.size()) {
                is_last_line = true;
                last_space = size + start;
                break;
            }

            if (text[size + start] == '\n') {
                is_line_break = true;
                last_space = size + start;
                break;
            }

            if (text[size + start] == ' ') {
                last_space = size + start;
            }

            size++;
            len++;
        }

        if (last_space == 0) {
            return false;
        }

        // handle trailing spaces
        while (last_space >= 2 && text[last_space - 1] == ' ') {
            last_space--;
        }

        std::string tmp;
        tmp.reserve(new_width + 2);

        if (text[start] != 0x1F) {
            tmp = {0x1F, color};
        }

        tmp += text.substr(start, last_space - start);

        // memorize active color
        for(std::string::size_type i = 0; i < tmp.size(); ++i) {
            if (tmp[i] == 0x1F) {
                color = tmp[i + 1];
            }
        }

        // justify line
        if (!is_last_line && !is_line_break) {
            WideBuffer(tmp, new_width);
        }

        lines.emplace_back(tmp);

        // move forward string reference
        start = last_space;

        // step over whitespace
        if (is_line_break && text[start] == '\n') {
            start++;
        } else while (std::isspace(text[start])) {
            start++;
        }
    }

    this->width = static_cast<int>(new_width);

    return true;
}

void XGuiList::Put(const std::optional<std::reference_wrapper<std::ofstream>> file)
{
    vClrScr();

    if (!caption.empty()) {
        const int dx = size_x / 2 - x_strlen(caption.c_str()) / 2;

        if (!file) {
            vGotoXY(dx, 0);
            vPutS(caption);
        } else {
            auto line = fmt::format("{}{}\n\n", std::string(dx, ' '), caption);
            vFPutS(file.value(), line);
        }
    }

    XGuiItem* item = top_item;
    std::size_t item_first_line = top_item_first_line;
    std::size_t item_lines_count = top_item_lines_count;
    std::size_t count = list_height;

    if (file) {
        count = 100000;
    }

    int y_pos = 2;

    if (top_line + count >= lines_count) {
        count = lines_count - top_line;
    }

    std::size_t cur_line = top_line;
    int i = 0;
    selectable_items_count = 0;

    while (count > 0) {
        if (cur_line < item_first_line + item_lines_count) {

            if (cur_line == item_first_line) {
                vSetAttr(xLIGHTGRAY);
            }

            if (cur_line == item_first_line && item->isSelectable()) {
                if (file) {
                    vFPutS(file.value(), " ");
                } else {
                    char buf[256];
                    sprintf(buf, MSG_BROWN "[" MSG_YELLOW "%c" MSG_BROWN "]", i++ + 65);
                    vGotoXY(0, y_pos);
                    vPutS(buf);
                    selectable_items_count++;
                }
            }

            if (file) {
                vFPutS(file.value(), (*item)[cur_line++ - item_first_line]);
                vFPutS(file.value(), "\n");
            } else {
                vGotoXY(4, y_pos);
                vPutS((*item)[cur_line++ - item_first_line]);
            }

            y_pos++;
            count--;
        } else {
            item_first_line += item_lines_count;
            item = item->next;
            item_lines_count = item->GetHeight();
        }
    }

    if (!file) {
        const char* tprompt = MSG_LIGHTGRAY "Use " MSG_BROWN "[" MSG_YELLOW "/*-+" MSG_BROWN "]" MSG_LIGHTGRAY "to scroll up/down, " MSG_BROWN "[" MSG_YELLOW "ESC" MSG_LIGHTGRAY "," MSG_YELLOW "Z" MSG_BROWN "]" MSG_LIGHTGRAY " to exit.";
        vGotoXY(size_x / 2 - x_strlen(tprompt) / 2, size_y - 1);
        vPutS(tprompt);

        if (top_line > 0) {
            vGotoXY(size_x - 6, 1);
            vPutS(MSG_LIGHTGRAY "(" MSG_CYAN "more" MSG_LIGHTGRAY ")");
        }

        if (top_line + list_height < lines_count) {
            vGotoXY(size_x - 6, size_y - 3);
            vPutS(MSG_LIGHTGRAY "(" MSG_CYAN "more" MSG_LIGHTGRAY ")");
        }
    }

    if (!footer.empty() && !file) {
        int dx = size_x / 2 - x_strlen(footer.c_str()) / 2;
        vGotoXY(dx, size_y - 2);
        vPutS(footer);
    }

    vRefresh();
}

void XGuiList::LineUp(size_t count)
{
    while (count > 0) {
        if (top_line <= 0) {
            return;
        }

        if (top_item_first_line < top_line) {
            top_line--;
            count--;
            continue;
        }

        top_item = top_item->prev;

        if (top_item->isSelectable()) {
            top_selectable_index--;
        }

        top_item_lines_count = top_item->GetHeight();
        top_item_first_line -= top_item_lines_count;
        top_item_index--;
    }
}

void XGuiList::LineDown(size_t count)
{
    while (count > 0) {
        if (top_line + list_height >= lines_count) {
            return;
        }

        if (top_line + 1 < top_item_first_line + top_item_lines_count) {
            top_line++;
            count--;
            continue;
        }

        if (top_item->isSelectable()) {
            top_selectable_index++;
        }

        top_item = top_item->next;

        top_item_first_line += top_item_lines_count;

        top_item_lines_count = top_item->GetHeight();
        top_item_index++;
    }
}

void XGuiList::PageUp()
{
    LineUp(list_height);
}

void XGuiList::PageDown()
{
    LineDown(list_height);
}

int XGuiList::Run(int flag, int flag2)
{
    V_BUFFER xyzbuf;
    vStore(&xyzbuf);
    vClrScr();
    vHideCursor();

    XGuiItem * item = head;
    lines_count = 0;

    while (item != nullptr) {
        item->SetWidth(size_x - 6);
        lines_count += item->GetHeight();
        item = item->next;
    }

    top_item = head;
    top_item_first_line = 0;
    top_item_lines_count = top_item->GetHeight();
    top_line = 0;
    top_item_index = 0;
    top_selectable_index = 0;

    if (flag2) {
        LineDown(flag2);
    }

    while (true) {
        Put();
        int ch = vGetch();
        last_pressed_key = ch;

        switch (ch) {
            case KEY_ESC :
            case 'Z' :
            case 'z' :
            case ' ' :
                vRestore(&xyzbuf);
                vRefresh();
                return -1;

            case '*' :
            case '+' :
            case KEY_DOWN :
                LineDown();
                continue;

            case '/' :
            case '-' :
            case KEY_UP :
                LineUp();
                continue;

            case KEY_PGUP :
                PageUp();
                continue;

            case KEY_PGDOWN :
                PageDown();
                continue;

            default:
                break;
        }

        if ((ch >= 'a' && ch < 'a' + selectable_items_count) || (ch >= 'A' && ch < 'A' + selectable_items_count)) {
            vRestore(&xyzbuf);

            if (!flag) {
                vRefresh();
            }

            if (ch - 'a' >= 0) {
                return top_selectable_index + (ch - 'a');
            } else {
                return top_selectable_index + (ch - 'A');
            }
        }

        // if flag then we need to return
        if (flag) {
            vRestore(&xyzbuf);
            return -1;
        }

    }
}
