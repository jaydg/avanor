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

#include "dwarfcity.h"
#include "xgen.h"
#include "unique.h"
#include "std_ai.h"
#include "incl_i.h"
#include "other_misc.h"
#include "itemf.h"
#include "game.h"
#include "item_misc.h"

XDwarfCityLocation::XDwarfCityLocation(LOCATION tl) : XLocation(tl)
{
	int i;

	strcpy(brief_name, "DvCty");
	strcpy(full_name, "Dwarven City");

	BuildCave();

	PutPalette(0, 0, PAL_DWARF_CITY, this);
	NewWay(4, 17, L_DWARFCITYCAVE6, STW_UP);
	NewWay(75, 16, L_DWARFTREASURE, STW_DOWN);	
	
	NewWay(74, 10, L_GASMINE1, STW_DOWN);	

	XRect guard_area(1, 16, 10, 19);
	XPoint pt;


	for (i = 0; i < 4; i++)
		NewCreature(CN_DWARF_GUARD, &guard_area, GID_DWARVEN_GUARDIAN, AIF_GUARD_AREA)->xai->SetEnemyClass((CREATURE_CLASS)(CR_ALL ^ (CR_HUMAN | CR_HUMANOID)));

	XRect shop_rect(12, 10, 23, 15);
	CreateShop(IM_ARMOUR | IM_WEAPON | IM_POTION | IM_BOOK | IM_SCROLL | IM_NECK | IM_MISSILE | IM_MISSILEW, &shop_rect, "Toberin, the dwarwen shopkeeper", SHOP_DOOR_DOWN);


	XRect trect(22, 10, 30, 15);
	NewCreature(CN_TODIN, &trect, GID_DWARVEN_GUARDIAN, (AIF_GUARD_AREA | AIF_NO_SWAP));
	{
		XRect gr(61, 15, 62, 16);
		NewCreature(CN_DWARF_GUARD, &gr, GID_DWARVEN_GUARDIAN, AIF_GUARD_AREA)->xai->SetEnemyClass((CREATURE_CLASS)(CR_ALL ^ (CR_HUMAN | CR_HUMANOID)));
	}

	{
		XRect gr(63, 15, 64, 16);
		NewCreature(CN_DWARF_GUARD, &gr, GID_DWARVEN_GUARDIAN, AIF_GUARD_AREA)->xai->SetEnemyClass((CREATURE_CLASS)(CR_ALL ^ (CR_HUMAN | CR_HUMANOID)));	
	}

	{
		XRect gr(66, 15, 67, 16);
		NewCreature(CN_DWARF_GUARD, &gr, GID_DWARVEN_GUARDIAN, AIF_GUARD_AREA)->xai->SetEnemyClass((CREATURE_CLASS)(CR_ALL ^ (CR_HUMAN | CR_HUMANOID)));	
	}

	{
		XRect gr(66, 17, 67, 18);
		NewCreature(CN_DWARF_GUARD, &gr, GID_DWARVEN_GUARDIAN, AIF_GUARD_AREA)->xai->SetEnemyClass((CREATURE_CLASS)(CR_ALL ^ (CR_HUMAN | CR_HUMANOID)));	
	}

	{
		XRect gr(68, 15, 69, 16);
		NewCreature(CN_DWARF_GUARD, &gr, GID_DWARVEN_GUARDIAN, AIF_GUARD_AREA)->xai->SetEnemyClass((CREATURE_CLASS)(CR_ALL ^ (CR_HUMAN | CR_HUMANOID)));	
	}

	{
		XRect gr(68, 17, 69, 18);
		NewCreature(CN_DWARF_GUARD, &gr, GID_DWARVEN_GUARDIAN, AIF_GUARD_AREA)->xai->SetEnemyClass((CREATURE_CLASS)(CR_ALL ^ (CR_HUMAN | CR_HUMANOID)));	
	}

	{
		XRect gr(62, 17, 63, 18);
		NewCreature(CN_TORIN, &gr, GID_DWARVEN_GUARDIAN, (AIF_GUARD_AREA | AIF_NO_SWAP))->xai->SetEnemyClass((CREATURE_CLASS)(CR_ALL ^ (CR_HUMAN | CR_HUMANOID)));	}
		new XThrone(62, 17, "the Dwarven Kingdom", this);

	new XAltar(45, 11, D_LIFE, this);

	XRect plaza_area(31, 2, 57, 18);
	for(i = 0; i < 10; i++)
		NewCreature(CN_DWARF, &plaza_area, GID_DWARVEN_GUARDIAN, AIF_GUARD_AREA)->xai->SetEnemyClass((CREATURE_CLASS)(CR_ALL ^ (CR_HUMAN | CR_HUMANOID)));

	XRect t_area(67, 15, 78, 18);
	XAnyPlace * place = new XDwarvenTresurePlace(&t_area, this);
	AddPlace(place);
}


void XDwarfCityLocation::Init()
{

}



XDwarfTreasureLocation::XDwarfTreasureLocation(LOCATION tl) : XLocation(tl)
{
	strcpy(brief_name, "DvTr");
	strcpy(full_name, "Dwarven Treasure");

	BuildCave();

	PutPalette(0, 0, PAL_DWARF_TREASURE, this);
	NewWay(10, 5, L_DWARFCITY, STW_UP);	

	int i;
	for (i = 5; i < 16; i++)
	{
		XMoney * money = new XMoney(vRand() % 300 + 200);
		money->Drop(this, i, 1);
		if (vRand() % 3 == 0)
			new XTrap(i, 1, this);
		money = new XMoney(vRand() % 300 + 200);
		money->Drop(this, i, 9);
		if (vRand() % 3 == 0)
			new XTrap(i, 9, this);
	}

	for (i = 3; i < 8; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (vRand(3) == 0)
			{
				XChest * tchest = new XChest(5, IM_ITEM, 200, 15000);
				tchest->Drop(this, 1 + j, i);
			}
			if (vRand(3) == 0)
			{
				XChest * tchest = new XChest(5, IM_ITEM, 200, 15000);
				tchest->Drop(this, 17 + j, i);
			}
		}
	}

	XItem * it = new XAncientMachinePart();
	it->Drop(this, 2, 4);
}


void XDwarfTreasureLocation::Init()
{

}

REGISTER_CLASS(XDwarvenTresurePlace);
int XDwarvenTresurePlace::onCreatureEnter(XCreature * cr)
{
	if (cr->isHero())
	{
		int flag = 1;
		for (int i = 59; i < 69 && flag; i++)
			for (int j = 15; j < 18 && flag; j++)
			{
				XCreature * grd = location->map->GetMonster(i, j);
				if (grd && grd->group_id == GID_DWARVEN_GUARDIAN
					&& !grd->xai->isEnemy(cr))
				{
					msgwin.Add("'Halt! You aren't allowed to enter.'");
					flag = 0;
					move_count = 0;
				}
			}
	}
	return 1;
}

int XDwarvenTresurePlace::onCreatureMove(XCreature * cr)
{
	int flg = 1;
	if (cr->isHero())
	{
		move_count++;
		if (move_count > 3)
		{
			for (int i = 59; i < 78 && flg; i++)
				for (int j = 15; j < 18 && flg; j++)
				{
					XCreature * grd = location->map->GetMonster(i, j);
					if (grd && grd->group_id == GID_DWARVEN_GUARDIAN)
					{
						grd->xai->SetGroupEnemy(cr);
						flg = 0;
					}
				}
		} else
		{
			for (int i = 59; i < 78 && flg; i++)
				for (int j = 15; j < 18 && flg; j++)
				{
					XCreature * grd = location->map->GetMonster(i, j);
					if (grd && grd->group_id == GID_DWARVEN_GUARDIAN && !grd->xai->isEnemy(cr))
					{
						if (move_count < 3)
							msgwin.Add("'Leave here immediately!'");
						else
							msgwin.Add("'This is your last chance to leave!'");
						flg = 0;
					}
				}
		}
	}
	return 1;
}


void XDwarvenTresurePlace::Store(XFile * f)
{
	XAnyPlace::Store(f);
	f->Write(&move_count);
}

void XDwarvenTresurePlace::Restore(XFile * f)
{
	XAnyPlace::Restore(f);
	f->Read(&move_count);
}



XGasMineLocation::XGasMineLocation(LOCATION loc) : XLocation(loc)
{
	map = new XMap(80, 20);
	BuildLabirint(0);
	XPoint pt;
	switch (loc)
	{
		case L_GASMINE1:
		{
			strcpy(brief_name, "GM1");
			strcpy(full_name, "Gassed Mine level 1");
			NewWay(L_DWARFCITY, STW_UP);
			NewWay(L_GASMINE2, STW_DOWN);

			XRect t_area(0, 0, 80, 20);
			XAnyPlace * place = new XGasPlace(&t_area, this);
			AddPlace(place);
		}
			break;

		case L_GASMINE2:
		{
			strcpy(brief_name, "GM2");
			strcpy(full_name, "Gassed Mine level 2");
			NewWay(L_GASMINE1, STW_UP);
			NewWay(L_GASMINE3, STW_DOWN);
			XRect t_area(0, 0, 80, 20);
			XAnyPlace * place = new XGasPlace(&t_area, this);
			AddPlace(place);

		}
			break;
		
		case L_GASMINE3:
		{
			strcpy(brief_name, "GM3");
			strcpy(full_name, "Gassed Mine level 3");
			NewWay(L_GASMINE2, STW_UP);
			XRect t_area(0, 0, 80, 20);
			XAnyPlace * place = new XGasPlace(&t_area, this);
			AddPlace(place);
			GetFreeXY(&pt);
			new XGasPump(pt.x, pt.y, this);

		}
			break;
	}
}


REGISTER_CLASS(XGasPlace);
int XGasPlace::onCreatureMove(XCreature * mover)
{
	if (XQuest::quest.torin_quest < 2 && vRand(3) == 0)
	{
		mover->_HP -= vRand(5) + 2;
		if (mover->_HP < 0)
		{
			XFakeCreature * tcr = new XFakeCreature("mysterious gas");
			mover->Die(tcr);
			tcr->Invalidate();
		}
	}
	return 1;
}


REGISTER_CLASS(XGasPump);

XGasPump::XGasPump(int _x, int _y, XLocation * _l)
{
	SetLocation(_l);
	im = IM_MISC;
	x = _x;
	y = _y;
	
	color = xBLUE;
	view = '#';
	
	assert(l->map->GetSpecial(x, y) == NULL);
	l->map->SetSpecial(x, y, this);
	strcpy(name, "gas pump");
}


int XGasPump::onOuterUse(XCreature * cr)
{
	if (XQuest::quest.torin_quest < 2)
	{
		XQuest::quest.torin_quest = 2;
		msgwin.Add("You hear hollow rumble as the pump starts.");
	} else
	{
		msgwin.Add("You can't seem to stop the pump.");
	}
	return 1;
}
