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

#ifndef __XGUI_H
#define __XGUI_H

#include <cstdio>

#include "engine/global.h"
#include "helpers/xstring.h"
#include "helpers/xvector.h"

class XGuiItem
{
    public:
        XGuiItem() : next(nullptr), prev(nullptr) {}

        virtual ~XGuiItem() = default;

        XGuiItem* next;
        XGuiItem* prev;
        virtual int isSelectable() = 0;
        virtual int isTitle() = 0;

        virtual bool SetWidth(int new_width)
        {
            return false;
        }

        virtual int GetHeight()
        {
            return 0;
        }

        virtual const char* operator[](int index)
        {
            return nullptr;
        }
};

class XGuiItem_SimpleSelect final : public XGuiItem
{
        char buf[256]{};
    public:
        explicit XGuiItem_SimpleSelect(const char* text) : XGuiItem()
        {
            strcpy(buf, text);
        }

        int isSelectable() override
        {
            return 1;
        }

        int isTitle() override
        {
            return 0;
        }

        bool SetWidth(int new_width) override
        {
            return true;
        }

        int GetHeight() override
        {
            return 1;
        }

        const char * operator[](int index) override
        {
            return buf;
        }
};


class XGuiItem_Text final : public XGuiItem
{
        char* text;
        XSimpleVector<char*> lines;
        int lines_count;
        int width{};
        int select_flag;

        static void WideBuffer(char* buf, int new_width);

    public:
        XGuiItem_Text() : XGuiItem(), text(nullptr), select_flag(0)
        {
            lines_count = 0;
            SetText("");
        }

        explicit XGuiItem_Text(const char* _text, const int sf = 0) : XGuiItem(), text(nullptr), select_flag(sf)
        {
            lines_count = 0;

            if (*_text == 0) {
                SetText("\n");
            } else {
                SetText(_text);
            }
        }

        void SetText(const char* _text)
        {
            while (lines_count > 0) {
                delete[] lines[--lines_count];
            }

            delete[] text;

            int textsize = x_strsize(_text);
            text = new char[textsize + 1];
            x_strcpy(text, _text);
            SetWidth(x_strlen(text));
        }

        ~XGuiItem_Text() override
        {
            while (lines_count > 0) {
                delete[] lines[--lines_count];
            }

            delete[] text;
        }

        int isSelectable() override
        {
            return select_flag;
        }

        int isTitle() override
        {
            return 0;
        }

        bool SetWidth(int new_width) override;
        int GetHeight() override
        {
            return lines_count;
        }

        const char* operator[](const int index) override
        {
            return lines[index];
        }
};

#define XGUI_LIST_HEADER 3
#define XGUI_LIST_FOOTER 3

class XGuiList final
{
        XGuiItem* head;
        XGuiItem* tail;

        XGuiItem* top_item{};
        int top_item_first_line{};
        int top_item_lines_count{};
        int top_line{};
        int top_item_index{};
        int top_selectable_index;
        int selectable_items_count;

        int items_count;
        int lines_count;
        int width{};

        int list_height;
        int list_width;

        void SetFirstVisible();
        int CalculateVisibleCount();

        const char* caption;
        const char* footer;

        int last_pressed_key{};


    public:
        XGuiList() :
            head(nullptr), tail(nullptr), top_selectable_index(0), selectable_items_count(0), items_count(0), lines_count(0),
            caption(nullptr), footer(nullptr)
        {
            list_height = size_y - 5;
            list_width = size_x - 6;
        }

        virtual ~XGuiList()
        {
            while (head != nullptr) {
                const XGuiItem* tmp = head;
                head = head->next;
                delete tmp;
            }
        }

        void AddItemHead(XGuiItem * item)
        {
            if (head != nullptr) {
                item->next = head;
                head->prev = item;
                head = item;
            } else {
                head = item;
                tail = item;
            }

            top_item = item;
            top_item_first_line = 0;
            top_item_lines_count = item->GetHeight();
            top_line = 0;
            top_item_index = 0;
        }

        void AddItemTail(XGuiItem * item)
        {
            if (tail != nullptr) {
                tail->next = item;
                item->prev = tail;
                tail = item;
            } else {
                head = item;
                tail = item;
                top_item = item;
                top_item_first_line = 0;
                top_item_lines_count = item->GetHeight();
                top_line = 0;
                top_item_index = 0;
            }
        }

        void AddItem(XGuiItem * item, const int is_first = 0)
        {
            if (is_first) {
                AddItemHead(item);
            } else {
                AddItemTail(item);
            }

            items_count++;
            lines_count += item->GetHeight();
        }

        void AddHtmlText(char* text);

        void Put(FILE* f = nullptr);
        int Run(int flag = 0, int flag2 = 0);

        void LineUp(int count = 1);
        void LineDown(int count = 1);
        void PageUp();
        void PageDown();
        void SetCaption(const char* _caption)
        {
            caption = _caption;
        }

        void SetFooter(const char* _footer)
        {
            footer = _footer;
        }

        virtual int GetHeight()
        {
            return lines_count;
        }

        [[nodiscard]] int GetTopItemIndex() const
        {
            return top_item_index;
        }

        [[nodiscard]] int GetLastKey() const
        {
            return last_pressed_key;
        }
};

#endif
