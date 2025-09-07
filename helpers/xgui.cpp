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

#include "global/xstring.h"
#include "helpers/xgui.h"
#include "global/global.h"
#include "creature/xhero.h"

//////////////////////////////////////////////////////////////////////////////
// The XGuiItem_Text::WideBuffer(char * str, int newlen) function           //
// tries to widen input string str to size newlen. If it is impossible      //
// (size already is equal or greater then requested), the function simply   //
// exits and does nothing.                                                  //
//////////////////////////////////////////////////////////////////////////////

static char* after_next_space(char* str)
{
    char* p = x_strchr(str, ' ');

    if (p == 0) {
        return x_strchr(str, 0);
    }

    while (*p == ' ') {
        p++;
    }

    return p;
}

static char* insert_space(char* str)
{
    memmove(str + 1, str, x_strsize(str) + 1);
    *str++ = ' ';
    return str;
}

void XGuiItem_Text::WideBuffer(char* str, int newlen)
{
    // Count length of string and number of blank spaces (blank space may consist
    // of one or more ' ' characters)
    int len = 0, spaces = 0;
    char* p = str;

    while (*p != 0) {
        if (*p == 0x1F) {
            p += 2;
            continue;
        }

        if (*p == ' ') {
            spaces++;
            p++;
            len++;

            while (*p == ' ') {
                p++;
                len++;
            }

            if (*p == 0) {
                spaces--;
            }
        } else {
            p++;
            len++;
        }
    }

    while (p >= str + 2 && *(p - 1) == ' ' && *(p - 2) != 0x1F) {
        p--;
        len--;
        *p = 0;
    }

    if (len >= newlen || spaces == 0) {
        return;
    }

    int new_spaces = newlen - len;

    int count1 = spaces;
    int count2 = new_spaces * 2;

    int subtract = new_spaces < spaces ? new_spaces * 2 : spaces * 2;

    p = after_next_space(str);

    int counter = new_spaces;

    while (true) {
        count1 -= subtract;
        count2 -= subtract;

        if (count1 <= 0) {
            p = insert_space(p);
            count1 += spaces * 2;

            if (--counter <= 0) {
                break;
            }
        }

        if (count2 <= 0) {
            p = after_next_space(p);
            count2 += new_spaces * 2;
        }
    }
}

bool XGuiItem_Text::SetWidth(int width)
{
    int color = xLIGHTGRAY;
    char* str = text;

    while (lines_count > 0) {
        delete[] lines[--lines_count];
    }

    while (true) {
        bool is_last_line = false, is_line_break = false;

        if (x_strlen(str) == 0) {
            break;
        }

        int last_space = -1, size = 0, len = 0;

        while (len <= width) {
            if (str[size] == 0x1F) {
                size += 2;
                continue;
            }

            if (str[size] == 0) {
                is_last_line = true;
                last_space = size;
                break;
            }

            if (str[size] == '\n') {
                is_line_break = true;
                last_space = size;
                break;
            }

            if (str[size] == ' ') {
                last_space = size;
            }

            size++;
            len++;
        }

        if (last_space == -1) {
            return false;
        }

        char* tmp = new char[2 + width - len + size + 1];

        while (last_space >= 2 && str[last_space - 1] == ' ' && str[last_space - 2] != 0x1F) {
            last_space--;
        }

        if (str[0] != 0x1F) {
            tmp[0] = 0x1F;
            tmp[1] = color;
            memmove(tmp + 2, str, last_space);
            tmp[last_space + 2] = 0;
        } else {
            memmove(tmp, str, last_space);
            tmp[last_space] = 0;
        }

        char* p = tmp;

        while (*p != 0)
            if (*p++ == 0x1F) {
                color = *p++;
            }

        if (!is_last_line && !is_line_break) {
            WideBuffer(tmp, width);
        }

        lines[lines_count++] = tmp;
        str += last_space;

        if (is_line_break && *str == '\n') {
            str++;
        } else
            while (*str == ' ') {
                str++;
            }
    }

    this->width = width;
    return true;
}

void XGuiList::Put(FILE * f)
{
    vClrScr();

    if (caption) {
        int dx = size_x / 2 - x_strlen(caption) / 2;

        if (!f) {
            vGotoXY(dx, 0);
            vPutS(caption);
        } else {
            char buf[256];

            for (int i = 0; i < dx; i++) {
                buf[i] = ' ';
            }

            sprintf(buf + dx, "%s\n\n", caption);
            vFPutS(f, buf);
        }
    }

    XGuiItem * item = top_item;
    int item_first_line = top_item_first_line;
    int item_lines_count = top_item_lines_count;

    int count = list_height;

    if (f) {
        count = 100000;
    }

    int y_pos = 2;

    if (top_line + count >= lines_count) {
        count = lines_count - top_line;
    }

    int cur_line = top_line;
    int i = 0;
    selectable_items_count = 0;

    while (count > 0) {
        if (cur_line < item_first_line + item_lines_count) {

            if (cur_line == item_first_line) {
                vSetAttr(xLIGHTGRAY);
            }

            if (cur_line == item_first_line && item->isSelectable()) {
                if (f) {
                    vFPutS(f, " ");
                } else {
                    char buf[256];
                    sprintf(buf, MSG_BROWN "[" MSG_YELLOW "%c" MSG_BROWN "]", i++ + 65);
                    vGotoXY(0, y_pos);
                    vPutS(buf);
                    selectable_items_count++;
                }
            }

            if (f) {
                vFPutS(f, (*item)[cur_line++ - item_first_line]);
                vFPutS(f, "\n");
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

    if (!f) {
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

    if (footer && !f) {
        int dx = size_x / 2 - x_strlen(footer) / 2;
        vGotoXY(dx, size_y - 2);
        vPutS(footer);
    }

    vRefresh();
}

void XGuiList::LineUp(int count)
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

void XGuiList::LineDown(int count)
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

    while (item != 0) {
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

    while (1) {
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
                break;

            case '*' :
            case '+' :
            case KEY_DOWN :
                LineDown();
                continue;
                break;

            case '/' :
            case '-' :
            case KEY_UP :
                LineUp();
                continue;
                break;

            case KEY_PGUP :
                PageUp();
                continue;
                break;

            case KEY_PGDOWN :
                PageDown();
                continue;
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
