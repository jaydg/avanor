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

#include "other_misc.h"

#include "creature.h"
#include "location.h"
#include "map.h"
#include "game.h"


REGISTER_CLASS(XTrap);

XTrap::XTrap(int _x, int _y, XLocation * _l, TRAP_LEVEL tl, TRAP_TYPE tt, XCreature * _owner)
{
	SetLocation(_l);
	im = IM_TRAP;
	x = _x;
	y = _y;
	
	if (tt == TT_RANDOM)
		tt = (TRAP_TYPE)vRand(TT_RANDOM);
	trap_type = tt;

	if (tl == TL_RANDOM)
		tl = (TRAP_LEVEL)vRand(TL_RANDOM);
	trap_level = tl;

	switch (tt)
	{
		case TT_MAGICARROW:
			color = xBROWN;
			break;

		case TT_FIREBOLT:
			color = xRED;
			break;

		case TT_ACIDBOLT:
			color = xGREEN;
			break;

		case TT_ARROW:
			color = xBROWN;
			break;

		case TT_SPEAR:
			color = xLIGHTGRAY;
			break;

		case TT_TELEPORT:
			color = xLIGHTGREEN;
			break;
	}

	view = '^';
	isVisibleForHero = 0;
	assert(l->map->GetSpecial(x, y) == NULL);
	l->map->SetSpecial(x, y, this);
	strcpy(name, "trap");
}


int XTrap::MoveIn(XCreature * cr)
{
	if (!isVisibleForHero)
		return Activate(cr);
	else
	{
		if (cr->isVisible())
		{
			msgwin.Add(cr->GetNameEx(CRN_T1));
			msgwin.Add(cr->GetVerb("avoid"));
			msgwin.Add("a trap.");
		}
	}
	return 0;
}

int XTrap::Activate(XCreature * cr)
{
	if (cr->isVisible())
	{
		msgwin.Add(cr->GetNameEx(CRN_T1));
		msgwin.Add(cr->GetVerb("activate"));
		msgwin.Add("a trap.");
		isVisibleForHero = 1;
	}

	EFFECT_DATA ed;
	ed.caller	= NULL;
	ed.l		= l;
	ed.power	= 10 * (trap_level + 1);
	ed.call_x	= x;
	ed.call_y	= y;
	ed.target_x = x;
	ed.target_y = y;
	ed.target	= cr;


	XEffect::Make(&ed);

	switch (trap_type)
	{
		case TT_MAGICARROW:
			ed.effect = E_MAGIC_ARROW;
			break;

		case TT_FIREBOLT:
			ed.effect = E_FIRE_BOLT;
			break;

		case TT_ACIDBOLT:
			ed.effect = E_ACID_BOLT;
			break;

		case TT_TELEPORT:
			ed.effect = E_TELEPORT;
			break;

		default:
			ed.effect = E_MAGIC_ARROW;

	}
	XEffect::Make(&ed);

	return 1;
}

int XTrap::Check(XCreature * cr)
{
	if (isVisibleForHero)
		return 0;
	XSkill * skill = cr->sk->GetSkill(SKT_DETECTTRAP);
	int chance = cr->GetStats(S_PER) * 10;
	if (skill)
		chance += skill->GetLevel() * 50;

	chance -= trap_level * 100;

	if (vRand() % 1000 < chance)
	{
		if (cr->isVisible())
		{
			msgwin.Add(cr->GetNameEx(CRN_T1));
			msgwin.Add(cr->GetVerb("found"));
			msgwin.Add("a trap.");
			isVisibleForHero = 1;
			skill->UseSkill();
		}
		return 1;
	}
	return 0;
}

int XTrap::isVisible(XCreature * cr)
{
	return isVisibleForHero;
}

int XTrap::Disarm(XCreature * cr)
{
	XSkill * skill = cr->sk->GetSkill(SKT_DISARMTRAP);
	int chance = 10;
	if (skill)
		chance += skill->GetLevel() * 100;
	chance -= trap_level * 100;
	int val = vRand() % 1000;
	if (val < chance)
	{
		msgwin.Add(cr->GetNameEx(CRN_T1));
		msgwin.Add(cr->GetVerb("disarm"));
		msgwin.Add("a trap successfully.");
		l->map->SetSpecial(x, y, NULL);
		Invalidate();
		return 1;
	} else
	if (val < (int)(chance * 1.3))
	{
		msgwin.Add(cr->GetNameEx(CRN_T1));
		msgwin.Add("doesn't manage to disarm a trap.");
	} else
	{
		Activate(cr);
	}
	return 0;
}


void XTrap::Store(XFile * f)
{
	XMapObject::Store(f);
	f->Write(&trap_type, sizeof(TRAP_TYPE));
	f->Write(&isVisibleForHero, sizeof(int));
}

void XTrap::Restore(XFile * f)
{
	XMapObject::Restore(f);
	f->Read(&trap_type, sizeof(TRAP_TYPE));
	f->Read(&isVisibleForHero, sizeof(int));
	
}









REGISTER_CLASS(XStairWay);

XStairWay::XStairWay(int _x, int _y, XLocation * loc, LOCATION _ln, STAIRWAYTYPE type)
{
	nx = -1;
	ny = -1;
	x = _x;
	y = _y;
	ln = _ln;
	ttm = 0;
	color = xLIGHTGRAY;
	zOrder = 100;
	im = IM_WAY;
   l = loc;

	assert(loc->map->GetSpecial(x, y) == NULL);
	switch (type)
	{
		case STW_UP:
			view = '<';
			loc->map->SetSpecial(x, y, this);
			strcpy(name, "way up");
			break;

		case STW_DOWN:
			view = '>';
			loc->map->SetSpecial(x, y, this);
			strcpy(name, "way down");
			break;
	}

}

void XStairWay::Bind(XStairWay * way)
{
	nx = way->x;
	ny = way->y;
	way->nx = x;
	way->ny = y;

}

void XStairWay::Store(XFile * f)
{
	XMapObject::Store(f);
	f->Write(&ln, sizeof(LOCATION));
}


void XStairWay::Restore(XFile * f)
{
	XMapObject::Restore(f);
	f->Read(&ln, sizeof(LOCATION));
}




REGISTER_CLASS(XTeleport);

XTeleport::XTeleport(int _x, int _y, XLocation * loc, LOCATION _ln, int _nx, int _ny)
{
	nx = _nx;
	ny = _ny;
	x = _x;
	y = _y;
	ln = _ln;
	ttm = 0;
	color = xWHITE;
	zOrder = 100;
	im = IM_TELEPORT;
	l = loc;
	assert(loc->map->GetSpecial(x, y) == NULL);

	view = '0';
	loc->map->SetSpecial(x, y, this);
	strcpy(name, "magic circle");

}

void XTeleport::Store(XFile * f)
{
	XMapObject::Store(f);
	f->Write(&ln, sizeof(LOCATION));
}


void XTeleport::Restore(XFile * f)
{
	XMapObject::Restore(f);
	f->Read(&ln, sizeof(LOCATION));
}

int XTeleport::MoveIn(XCreature * cr)
{
	if (Game.locations[ln]->map->XGetMovability(nx, ny) == 0)
	{
		cr->LastStep();
		cr->FirstStep(nx, ny, Game.locations[ln]);
	}
	return 1;
}










REGISTER_CLASS(XDoor);

XDoor::XDoor(int _x, int _y, int flg, XLocation * _l)
{
	SetLocation(_l);
	im = IM_DOOR;
	x = _x;
	y = _y;
	isOpened = !flg;
	Switch();
	color = xBROWN;
	
	assert(l->map->GetSpecial(x, y) == NULL);
	l->map->SetSpecial(x, y, this);
	strcpy(name, "door");
}

void XDoor::Switch()
{
	if (isOpened)
	{
		isOpened = 0;
		view = '+';
	} else
	{
		isOpened = 1;
		view = '/';
	}
}

void XDoor::Store(XFile * f)
{
	XMapObject::Store(f);
	f->Write(&isOpened, sizeof(int));
	
}

void XDoor::Restore(XFile * f)
{
	XMapObject::Restore(f);
	f->Read(&isOpened, sizeof(int));
}


REGISTER_CLASS(XAltar);

XAltar::XAltar(int _x, int _y, DEITY deity, XLocation * _l)
{
	SetLocation(_l);
	im = IM_ALTAR;
	x = _x;
	y = _y;
	
	if (deity == D_LIFE)
		color = xWHITE;
	else
		color = xDARKGRAY;

	view = '_';
	
	assert(l->map->GetSpecial(x, y) == NULL);
	l->map->SetSpecial(x, y, this);
	strcpy(name, "altar");
}


void XAltar::Store(XFile * f)
{
	XMapObject::Store(f);
}

void XAltar::Restore(XFile * f)
{
	XMapObject::Restore(f);
}



REGISTER_CLASS(XGrave);

XGrave::XGrave(int _x, int _y, char * subscr, XLocation * _l)
{
	SetLocation(_l);
	im = IM_MISC;
	x = _x;
	y = _y;
	color = xLIGHTGRAY;
	view = '+';
	assert(l->map->GetSpecial(x, y) == NULL);
	l->map->SetSpecial(x, y, this);
	sprintf(name, "the grave signed '%s'", subscr);
	isOpened = 0;
}

void XGrave::HideItem(XItem * item)
{
	hiden_items.push_back(item);
}

int XGrave::onOuterUse(XCreature * cr)
{
	if (cr->isHero())
	{
		if (isOpened)
			msgwin.Add("This grave has already been looted.");
		else
			msgwin.Add("You move the headstone. Something glimmers in the dark.");
	}
	while (!hiden_items.empty())
	{
		hiden_items.begin()->Drop(l, x, y);
		hiden_items.pop_front();
	}
	isOpened = 1;
	return 1;
}


void XGrave::Store(XFile * f)
{
	XMapObject::Store(f);
	f->Write(&isOpened);
	hiden_items.StoreList(f);
}

void XGrave::Restore(XFile * f)
{
	XMapObject::Restore(f);
	f->Read(&isOpened);
	hiden_items.RestoreList(f);
}



REGISTER_CLASS(XThrone);

XThrone::XThrone(int _x, int _y, char * subscr, XLocation * _l)
{
	SetLocation(_l);
	im = IM_MISC;
	x = _x;
	y = _y;
	color = xYELLOW;
	view = '~';
	assert(l->map->GetSpecial(x, y) == NULL);
	l->map->SetSpecial(x, y, this);
	sprintf(name, "the throne of %s", subscr);
}
