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

#ifndef __MAP_H
#define __MAP_H

#include <stdlib.h>
#include <assert.h>

#include "global.h"
#include "point.h"
#include "rect.h"
#include "xanyplace.h"
#include "xlist.h"

#define MAP_MINX 10
#define MAP_MINY 10
#define MAP_MAXX 200
#define MAP_MAXY 200
#define SCR_HGT  (size_y - 5)
#define SCR_LEN  (size_x)
#define SCR_X    0
#define SCR_Y    2

enum MOVEABLE {MO_UNKNOWN = 0, MO_NORMAL = 1,
	MO_SHARD, MO_AHARD, MO_HARD, MO_VHARD,
	MO_WALL = 80,
	MO_WATER, MO_DEEPWATER, MO_MOUNTAIN};

enum VISIABLE {VI_UNKNOWN = 0, VI_NORMAL = 1,
VI_SHARD, VI_AHARD, VI_HARD, VI_VHARD,
VI_WALL = 80};

struct xMAP
{
	char view;
	char color;
	char * name;
	MOVEABLE moveable; //0 - normal, 1 -little hard, 80 - UNMOVEABLE
	VISIABLE visiable; //0 - noraml, 1 ... 80 - wall
};

extern xMAP stdmap[];
/*
enum MFLAG {
	MF_NONE = 0x0000,
	MF_DOOR = 0x0002,
	MF_WAYU = 0x0004,
	MF_WAYD = 0x0008,
	MF_WAY  = 0x000C
};

#define SC_NONE     0x0000
#define SC_ODOOR    0x0001
#define SC_CDOOR    0x0002
*/

class XMapObject;
class XItem;

struct MAP
{
	MAP();
	~MAP();
	void Store(XFile * f);
	void Restore(XFile * f);

	STDMAP n;
	XPtr<XCreature> pMonster; //if null then no monster here
	XSortedList<XItem *> item_list; //list of item in this cell of map. Automatic construct/destruct
	XPtr<XMapObject> pSpecialObject; //door, way, trap door.
//  unsigned char spec; //if something special (such as door or trap here)
//  MFLAG mflag; //this flag for a monster AI (
	bool visible; // visisble for HERO!!!
	char known; //for hero memory
	char color; //for hero memory
	XPtr<XAnyPlace> place; //pointer to an object which describe this place... it can be NULL, Shop, special room etc.
};

class XMap
{
public:
	int hgt, len;
	int wx, wy;
	MAP * map;
	XMap ();
	XMap (int l, int h);
	~XMap ();

	void Put(XCreature * cr);
	void Center(int x, int y);
	void PutChar(int x, int y, char c, int color);
	int GetMovability(int x, int y);
	int XGetMovability(int x, int y);
	int GetVisibility(int x, int y);
	void SetXY(int x, int y, STDMAP stdm);
	STDMAP GetXY(int x, int y);

	void SetVisible(int x, int y);
	void ResVisible(int x, int y);
	bool GetVisible(int x, int y);

	void PutItem(int x, int y, XItem * item);
	unsigned int GetItemCount(int x, int y);
	XSortedList<XItem *> * GetItemList(int x, int y);

	void SetMonster(int x, int y, XCreature * monst);
	void ResMonster(int x, int y);
	XCreature * GetMonster(int x, int y);



	void SetPlace(int x, int y, XAnyPlace * place);
	XAnyPlace * GetPlace(int x, int y);

	void SetKnown(int x, int y);
	void ResKnown(int x, int y);
	int GetKnown(int x, int y);

	char * GetDescription(int x, int y);

	void SetSpecial(int x, int y, XMapObject * spec);
	XMapObject * GetSpecial(int x, int y);

	void CreateRoom(int x, int y, int l, int h, STDMAP m1, STDMAP m2);
	void CreateRoom(int x, int y, int l, int h, int px, int py, STDMAP m1, STDMAP m2);

	void Store(XFile * f);
	void Restore(XFile * f);

	void Dump(FILE * f);
};

#endif
