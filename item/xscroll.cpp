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

#include "xscroll.h"
#include "xhero.h"

REGISTER_CLASS(XScroll);

int SCROLL_REC::total_value = 0;

SCROLL_REC scroll_descr[] = {
	SCROLL_REC("healing",		E_HEAL,					SCROLL_HEALING,				200,	10),
	SCROLL_REC("burning hands",	E_BURNING_HANDS,		SCROLL_BURNING_HANDS,		20,		100),
	SCROLL_REC("ice touch",		E_ICE_TOUCH,			SCROLL_ICE_TOUCH,			20,		100),
	SCROLL_REC("heroism",		E_HEROISM,				SCROLL_HEROISM,				25,		100),
	SCROLL_REC("power",			E_POWER,				SCROLL_POWER,				15,		100),
	SCROLL_REC("identify",		E_IDENTIFY,				SCROLL_IDENTIFY,			100,	30),
	SCROLL_REC("magic arrow",	E_MAGIC_ARROW,			SCROLL_MAGIC_ARROW,			15,		200),
	SCROLL_REC("fire bolt",		E_FIRE_BOLT,			SCROLL_FIRE_BOLT,			50,		60),
	SCROLL_REC("ice bolt",		E_ICE_BOLT,				SCROLL_ICE_BOLT,			50,		60),
	SCROLL_REC("lightning bolt", E_LIGHTNING_BOLT,		SCROLL_LIGHTNING_BOLT,		100,	20),
	SCROLL_REC("acid bolt",		E_ACID_BOLT,			SCROLL_ACID_BOLT,			150,	15),
	SCROLL_REC("summon monsters", E_SUMMON_MONSTER,		SCROLL_SUMMON_MONSTER,		10,		100),
	SCROLL_REC("create item",	E_CREATE_ITEM,			SCROLL_CREATE_ITEM,			200,	25),
	SCROLL_REC("cure disease",	E_CURE_DISEASE,			SCROLL_CURE_DISEASE,		40,		100),
	SCROLL_REC("cure poison",	E_CURE_POISON,			SCROLL_CURE_POISON,			40,		100),
	SCROLL_REC("blink",			E_BLINK,				SCROLL_BLINK,				70,		30),
	SCROLL_REC("self knowledge", E_SELF_KNOWLEDGE,		SCROLL_SELF_KNOWLEDGE,		150,	10),
	SCROLL_REC("see invisible", E_SEE_INVISIBLE,		SCROLL_SEE_INVISIBLE,		40,		50),
	SCROLL_REC("recipe",		E_NONE,					SCROLL_RECIPIE,				30,		25),
};


void SCROLL_REC::Store(XFile * f)
{
	f->Write(&effect, sizeof(EFFECT));
	f->Write(&scroll_name, sizeof(SCROLL_NAME));
	f->Write(&identify, sizeof(int));
	f->Write(name, sizeof(char), 20);
}

void SCROLL_REC::Restore(XFile * f)
{
	f->Read(&effect, sizeof(EFFECT));
	f->Read(&scroll_name, sizeof(SCROLL_NAME));
	f->Read(&identify, sizeof(int));
	f->Read(name, sizeof(char), 20);
}

int SCROLL_REC::GetRandomDescription(SCROLL_NAME scrn)
{
	if (scrn == SCROLL_RANDOM)
	{
		int val = vRand(total_value);
		int pos = -1;
		do
		{
			pos++;
			val -= scroll_descr[pos].rarity;
		} while (val >= 0);
		return pos;
	}
	else
	{
		for (int i = 0; i < SCROLL_RANDOM; i++)
			if (scroll_descr[i].scroll_name == scrn)
				return i;
	}
	return -1;
}

XScroll::XScroll(SCROLL_NAME scrn)
{
	descr = SCROLL_REC::GetRandomDescription(scrn);
	assert(descr > -1 && descr < SCROLL_RANDOM);
	sc_name = scroll_descr[descr].scroll_name;
	strcpy(name, scroll_descr[descr].real_name);
	value = scroll_descr[descr].value;
	im = IM_SCROLL;
	bp = BP_OTHER;
	it = IT_SCROLL;
	view = '?';
	color =	xLIGHTGRAY;
	weight = 2;
	dice.Setup("1d1");
}

int XScroll::isIdentifed()
{
	return scroll_descr[descr].identify;
}

void XScroll::Identify(int level)
{
	scroll_descr[descr].identify = level;
}

int XScroll::Compare(XObject * o)
{
	assert(dynamic_cast<XScroll *>(o));
	XScroll * tit = (XScroll *)o;

	if (descr == tit->descr && x == tit->x && y == tit->y)
		return 0;
	else
	{
		if (quantity > tit->quantity)
			return -1;
		else
			return 1;
	}
}

void XScroll::toString(char * buf)
{
	if (!isIdentifed())
	{
		if (quantity == 1)
			sprintf(buf, "scroll labeled \"%s\"", scroll_descr[descr].name);
		else
			sprintf(buf, "heap of (%d) scrolls labeled \"%s\"", quantity, scroll_descr[descr].name);
	} else
	{
		if (quantity == 1)
			sprintf(buf, "scroll of %s", name);
		else
			sprintf(buf, "heap of (%d) scrolls of %s", quantity, name);
	}
}

int XScroll::onRead(XCreature * cr)
{
	assert(cr->isValid());

	char buf[256];
	int flag = 0;
	if (scroll_descr[descr].effect != E_NONE)
	{
		if (cr->isHero())
		{
			msgwin.Add("You read the");
			toString(buf);
			strcat(buf, ".");
			msgwin.Add(buf);
		}
		else if (cr->isVisible())
		{
			msgwin.Add(cr->name);
			msgwin.Add("reads the");
			toString(buf);
			strcat(buf, ".");
			msgwin.Add(buf);
		}
		EFFECT_DATA ed;
		ed.caller	= cr;
		ed.l		= cr->l;
		ed.effect	= scroll_descr[descr].effect;
		ed.power	= 25;
		ed.call_x	= cr->x;
		ed.call_y	= cr->y;
		if (XEffect::GetReq(scroll_descr[descr].effect) == ER_DIRECTION)
		{
			XPoint pt;
			cr->GetTarget(TR_ATTACK_DIRECTION, &pt);
			ed.target_x = pt.x + cr->x;
			ed.target_y = pt.y + cr->y;
		} else if (XEffect::GetReq(scroll_descr[descr].effect) == ER_TARGET)
		{
			XPoint pt;
			cr->GetTarget(TR_ATTACK_TARGET, &pt, XEffect::GetRange(ed.effect, ed.power));
			ed.target_x = pt.x;
			ed.target_y = pt.y;
		}
		flag = XEffect::Make(&ed);
	} else
	{
		flag = 0;
		switch (scroll_descr[descr].scroll_name)
		{
			case SCROLL_RECIPIE:
				if (cr->isHero())
				{
					int val = vRand(XAlchemy::GetReceptionCount());
					XAlchemyRec * pRec = XAlchemy::GetReception(val);
					if (pRec)
						flag = ((XHero *)cr)->LearnReception(pRec->pn1, pRec->pn2, pRec->result);
				}
				break;
		}
	}

	if (!cr->isValid()) return flag; // $$$

	if (flag == 0)
	{
		if (cr->isHero())
		{
			msgwin.Add("You feel nothing special.");
		}
		else if (cr->isVisible())
		{
			msgwin.Add(cr->name);
			msgwin.Add("feels nothing special.");
		}
	} else
	{
		if (!isIdentifed() && cr->isHero())
		{
			Identify(1);
			msgwin.Add("It was");
			toString(buf);
			strcat(buf, ".");
			msgwin.Add(buf);
		}
	}

	return flag;
}

void XScroll::Store(XFile * f)
{
	XItem::Store(f);
	f->Write(&descr, sizeof(int));
	f->Write(&sc_name, sizeof(SCROLL_NAME));
}

void XScroll::Restore(XFile * f)
{
	XItem::Restore(f);
	f->Read(&descr, sizeof(int));
	f->Read(&sc_name, sizeof(SCROLL_NAME));
}

void XScroll::StoreTable(XFile * f)
{
	for (int i = 0; i < SCROLL_RANDOM; i++)
	{
		scroll_descr[i].Store(f);
	}
}

void XScroll::RestoreTable(XFile * f)
{
	for (int i = 0; i < SCROLL_RANDOM; i++)
	{
		scroll_descr[i].Restore(f);
	}
}
