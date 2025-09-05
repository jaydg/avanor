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

#ifndef __XSCROLL_H
#define __XSCROLL_H

#include "item.h"
#include "effect.h"

enum SCROLL_NAME
{
	SCROLL_BURNING_HANDS,
	SCROLL_ICE_TOUCH,
	SCROLL_HEROISM,
	SCROLL_HEALING,
	SCROLL_POWER,
	SCROLL_IDENTIFY,
	SCROLL_MAGIC_ARROW,
	SCROLL_FIRE_BOLT,
	SCROLL_ICE_BOLT,
	SCROLL_LIGHTNING_BOLT,
	SCROLL_ACID_BOLT,
	SCROLL_SUMMON_MONSTER,
	SCROLL_CREATE_ITEM,
	SCROLL_CURE_DISEASE,
	SCROLL_CURE_POISON,
	SCROLL_BLINK,
	SCROLL_SELF_KNOWLEDGE,
	SCROLL_SEE_INVISIBLE,
	SCROLL_RECIPIE,
	SCROLL_RANDOM
};

struct SCROLL_REC
{
	SCROLL_REC(const char* rn, EFFECT eff, SCROLL_NAME scrn, int val, int rar) //generate scroll name
	{
		real_name = rn;
		effect = eff;
		scroll_name = scrn;
		identify = 0;
		value = val;
		total_value += rar;
		rarity = rar;

		const char* tmp = "euioa";
		int words = vRand() % 2 + 1;
		strcpy(name, "");
		for (int i = 0; i < words; i++)
		{
			int word_len = vRand() % (5 - words) + 3;
			for (int j = 0; j < word_len; j++)
			{
				char let[2] = "X"; //default letter;
				if (j % 2 == 0)
				{
					let[0] = tmp[vRand() % 5];
				} else
				{
					let[0] = (char)(vRand() % 26 + 'a');
				}
				strcat(name, let);
			}
			if (i < words - 1)
			{
				if(vRand() % 4 == 1)
				{
					strcat(name, "-");
				} else
				{
					strcat(name, " ");
				}
			}
		}
		
	};

	EFFECT effect;
	SCROLL_NAME scroll_name;
	int identify;
	char name[20];
	const char* real_name;
	int value;
	int rarity;

	void Store(XFile * f);
	void Restore(XFile * f);
	static int total_value;
	static int GetRandomDescription(SCROLL_NAME scrn);
};


class XScroll : public XItem
{
public:
	DECLARE_CREATOR(XScroll, XItem);
	XScroll(SCROLL_NAME _scrn = SCROLL_RANDOM);
	XScroll(XScroll * copy):XItem((XItem*)copy) {descr = copy->descr; sc_name = copy->sc_name;}
	virtual XObject * MakeCopy() { return new XScroll(this); }
	virtual int isIdentifed();
	virtual void Identify(int level);
	virtual void toString(char * buf);
	virtual int Compare(XObject * o);
	virtual int onRead(XCreature * cr);
	virtual void Store(XFile * f);
	virtual void Restore(XFile * f);
	static void StoreTable(XFile * f);
	static void RestoreTable(XFile * f);
	SCROLL_NAME sc_name;
protected:
	int descr;
};


#endif
