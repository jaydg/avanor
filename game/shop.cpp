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

#include "shop.h"
#include "itemf.h"
#include "skeep_ai.h"
#include "xarchive.h"

REGISTER_CLASS(XShop);

XShop::XShop(XRect * _area, ITEM_MASK _im, XLocation * _loc, SHOP_DOOR sd)
	:XAnyPlace(_area, _loc)
{
	shop_mask = _im;

	int dx = 0;
	int dy = 0;

	switch (sd)
	{
		case SHOP_DOOR_DOWN : 
			dx = (area.left + area.right) / 2;
			dy = area.bottom - 1;
			break;

		case SHOP_DOOR_UP : 
			dx = (area.left + area.right) / 2;
			dy = area.top;
			break;

		case SHOP_DOOR_LEFT:
			dx = area.left;
			dy = (area.top + area.bottom) / 2;
			break;

		case SHOP_DOOR_RIGHT:
			dx = area.right - 1;
			dy = (area.top + area.bottom) / 2;
			break;

		default: assert(0);

	}

	location->map->CreateRoom(area.left, area.top, area.Width(), area.Hight(), 
		dx, dy, M_STONEFLOOR, M_STONEWALL);
	
	for (int i = area.left + 1; i < area.right - 1; i++)
		for (int j = area.top + 1; j < area.bottom - 1; j++)
		{
			XItem * item = ICREATEA(shop_mask);
			item->Drop(location.get(), i, j);
		}

	hero_in = 0;
}

int XShop::onCreaturePickItem(XCreature * cr, XItem * item)
{
	if (owner)
	{
		return ((XShopKeeperAI *)(owner->xai))->onAnyonePickItem(cr, item);
	} 
	return 1;
}


int XShop::onCreatureEnter(XCreature * cr)
{
	if (cr->im & IM_HERO)
	{
		for (int i = area.left + 1; i < area.right - 1; i++)
			for (int j = area.top + 1; j < area.bottom - 1; j++)
			{
				XItemList * ilist = location->map->GetItemList(i, j);
				it_iterator it;
				for(it = ilist->begin(); it != ilist->end(); it++)
				{
					XItem * tit = static_cast<XItem *>(static_cast<XObject *>(it));
					tit->Identify(1);
				}
			}
	}

	if (owner)
	{
		((XShopKeeperAI *)(owner->xai))->onCreatureEnterShop(cr);
	}
	return 1;
};

int XShop::onCreatureLeave(XCreature * cr)
{
	if (owner)
	{
		((XShopKeeperAI *)(owner->xai))->onCreatureLeaveShop(cr);
	}
	return 1;
};

int XShop::onCreatureDropItem(XCreature * cr, XItem * item)
{
//	cr->contain->Add(new XMoney(item->GetValue() * item->quantity));
	if (owner)
	{
		return ((XShopKeeperAI *)(owner->xai))->onAnyoneDropItem(cr, item);
	} 
	return 1;
}


int XShop::onCreatureMove(XCreature * cr)
{
	return 1;
}

void XShop::Store(XFile * f)
{
	XAnyPlace::Store(f);
	f->Write(&hero_in);
	f->Write(&shop_mask, sizeof(ITEM_MASK));
}

void XShop::Restore(XFile * f)
{
	XAnyPlace::Restore(f);
	f->Read(&hero_in);
	f->Read(&shop_mask, sizeof(ITEM_MASK));
}

void XShop::onShowItem(XItem * item, char * buf)
{
	item->toString(buf);
	if (owner)
	{
		char buf2[256];
		sprintf(buf2, "{%dgp}", item->quantity * item->GetValue());
		strcat(buf, buf2);
	}
}
