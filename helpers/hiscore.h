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

#ifndef __HISCORE_H
#define __HISCORE_H

#include "xgui.h"

#define GAME_VERSION "0.5.6-dev"

#define HISCORE_FILE_NAME	"avanor.hsc"
#define HISCORE_TOP_REC		10

class XFile;

class XHiScoreItem : public XGuiItem_Text
{
public:
	char buf[256];
	XHiScoreItem(int place, unsigned int score, char * _name, char * _msg, int flg, int last_record = 0);
	XHiScoreItem() : isLastRecord(0) {}
	XHiScoreItem(XHiScoreItem * copy);
//	virtual void Put();
	virtual int isSelectable() {return 0;}
	virtual int isTitle() {return 0;}
//	virtual int GetHeight() {return 3;}
	void Store(XFile * f);
	void Restore(XFile * f);
	unsigned int score;
	int place;
	void SetText(int place, unsigned int score, char * name, int day, int month, int year, char * msg);
//   virtual const char * operator[](int index);
protected:
	int year;
	int day;
	int month;
	char name[80];
	char msg[160];
	int isLastRecord;
	int flag; //win or death
	int reserved[10];
};

class XHiScore
{
public:
	XHiScore();
	~XHiScore();
	void AddRecord(XHiScoreItem * item);
	void Show();
	XHiScoreItem * items[HISCORE_TOP_REC];
};

#endif
