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

#include <stdio.h>
#include "xstring.h"
#include "global.h"
#include "xvector.h"

class XGuiItem
{
public:
   XGuiItem() : next(0), prev(0) {}
   virtual ~XGuiItem() {}
   XGuiItem * next;
   XGuiItem * prev;
   virtual int isSelectable() = 0;
   virtual int isTitle() = 0;

   virtual bool SetWidth(int new_width) { return false; }
   virtual int GetHeight() { return 0; }
   virtual const char * operator[](int index) { return 0; }
};

class  XGuiItem_SimpleSelect : public XGuiItem
{
	char buf[256];
public:
	XGuiItem_SimpleSelect(char * text) : XGuiItem() { 	strcpy(buf, text); }
	virtual int isSelectable() {return 1;}
	virtual int isTitle() {return 0;}
	virtual bool SetWidth(int new_width) { return true; }
	virtual int GetHeight() { return 1; }
	virtual const char * operator[](int index) { return buf; }
};


class XGuiItem_Text : public XGuiItem
{
   char                  * text;
   XSimpleVector<char *>   lines;
   int                     lines_count;
   int                     width;
   int                     select_flag;
   void WideBuffer(char * buf, int new_width);

public:  
   XGuiItem_Text() : XGuiItem(), select_flag(0), text(0)
   {
      lines_count = 0;
      SetText("");
   }

   XGuiItem_Text(const char * _text, int sf = 0) : XGuiItem(), select_flag(sf), text(0)
   {
      lines_count = 0;
      if(*_text == 0)
         SetText("\n");
      else
         SetText(_text);
   }

   void SetText(const char * _text)
   {
      while(lines_count > 0) delete [] lines[--lines_count];
      if(text != 0) delete [] text;
      int textsize = x_strsize(_text);
      text = new char[textsize + 1];
      x_strcpy(text, _text);
      SetWidth(x_strlen(text));
   }

   virtual ~XGuiItem_Text() 
   { 
      while(lines_count > 0) delete [] lines[--lines_count];
      if(text != 0) delete[] text;
   }
   
   virtual int isSelectable() { return select_flag; }
   virtual int isTitle() { return 0; }

   virtual bool SetWidth(int new_width);
   virtual int GetHeight() { return lines_count; }
   virtual const char * operator[](int index) { return lines[index]; }
};

#define XGUI_LIST_HEADER 3
#define XGUI_LIST_FOOTER 3

class XGuiList
{
	XGuiItem * head;
	XGuiItem * tail;

	XGuiItem * top_item;
	int        top_item_first_line;
	int        top_item_lines_count;
	int        top_line;
	int        top_item_index;
	int		   top_selectable_index;
	int		   selectable_items_count;

	int items_count;
	int lines_count;
	int width;

	int list_height;
	int list_width;

	void SetFirstVisible();
	int CalculateVisibleCount();

	const char * caption;
	const char * footer;

	int last_pressed_key;


public:
	XGuiList() : 
	  head(NULL), tail(NULL), items_count(0), lines_count(0), top_selectable_index(0), selectable_items_count(0),
		 caption(NULL), footer(NULL)
	{
		list_height = size_y - 5;
		list_width  = size_x - 6;
	}

	virtual ~XGuiList()
	{
		while (head != 0)
		{
  			XGuiItem * tmp = head;
  			head = head->next;
  			delete tmp;
		}
	}

	void AddItemHead(XGuiItem * item)
	{
		if(head != 0)
		{
			item->next = head;
			head->prev = item;
			head = item;
		} 
		else
		{
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
 		if(tail != 0)
 		{
 			tail->next = item;
 			item->prev = tail;
 			tail = item;
 		} 
 		else
 		{
 			head = item;
 			tail = item;
   			top_item = item;
   			top_item_first_line = 0;
			top_item_lines_count = item->GetHeight();
	   		top_line = 0;
		    top_item_index = 0;
 		}
	}

	void AddItem(XGuiItem * item, int is_first = 0) 
	{
		if(is_first)
			AddItemHead(item);
		else
			AddItemTail(item);

		items_count++;
		lines_count += item->GetHeight();
	}

	void AddHtmlText(char * text);

	void Put(FILE * f = NULL);
	int Run(int flag = 0, int flag2 = 0);

	void LineUp(int count = 1);
	void LineDown(int count = 1);
	void PageUp();
	void PageDown();
	void SetCaption(const char * _caption){	caption = _caption;	}
	void SetFooter(const char * _footer) { footer = _footer; }
	virtual int GetHeight() { return lines_count; }
	int GetTopItemIndex() { return top_item_index; }

	int GetLastKey() { return last_pressed_key; }
};

#endif
