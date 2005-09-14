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

#include "skeep_ai.h"
#include "gmsg.h"
#include "itemf.h"
#include "xapi.h"

REGISTER_CLASS(XShopKeeperAI);

XShopKeeperAI::XShopKeeperAI(XCreature * shopkeeper, XShop * _shop)
	 : XStandardAI(shopkeeper)
{
	SetShop(_shop);
	_shop->SetShopkeeper(shopkeeper);
	
	debt.debtor = NULL;
	debt.debtor_sum = 0;
	debt.turn_count = 0;
	debt.debtor_leave_shop = 0;

	SetArea(shop->GetArea(), shop->location->ln);
	SetAIFlag(AIF_GUARD_AREA);
	SetAIFlag(AIF_GUARD_AREA);
	SetAIFlag(AIF_COWARD);
	SetAIFlag(AIF_RANDOM_MOVE);
	SetEnemyClass(CR_NONE);

	ResAIFlag(AIF_ALLOW_PICK_UP);
}

void XShopKeeperAI::Invalidate()
{
	shop        = NULL;
	debt.debtor = NULL;
	debt.item_list.KillAll();
	XStandardAI::Invalidate();
}

void XShopKeeperAI::Move()
{
	if (ai_owner->l->map->GetItemCount(ai_owner->x, ai_owner->y) == 0 && vRand(50) == 0 && shop->GetArea()->PointIn(ai_owner->x, ai_owner->y))
	{
		XItem * item = ICREATEA(shop->shop_mask);
		item->Identify(1);
		item->Drop(ai_owner->l, ai_owner->x, ai_owner->y);
	}
	
	XStandardAI::Move();

	if (debt.debtor)
	{
		debt.debtor_sum += debt.debtor_add_value;
	}
}


int XShopKeeperAI::onAnyonePickItem(XCreature * customer, XItem * item)
{
	if (!(customer->im & IM_HERO)) return 0;
	if (!(shop->shop_mask & item->im))
		return 1;

	if (debt.item_list.size() > 0)
	{
		msgwin.Add("Pay for what you've picked up already before you take anything else!");
		return 0;
	}
	//hack!!!
	XItem * titem = (XItem *)(item->MakeCopy());
	titem->x = -1;
	titem->y = -1;

	debt.item_list.Add(titem);
	if (debt.debtor_leave_shop == 0) return 1;

	if (!isEnemy(customer))
		msgwin.Add(GMSG_SHOPKEEPER_ATTACK);
	else
		msgwin.Add(GMSG_SHOPKEEPER_ATTACK2);

	AddPersonalEnemy(customer);

	return 1;
}

int XShopKeeperAI::onAnyoneDropItem(XCreature * customer, XItem * item)
{
//	Shopkeeper takes only items of appropriate type
	if (!(item->im & shop->shop_mask))
	{
		msgwin.Add(GMSG_SHOPKEEPER_REJECT_ITEM);
		return 0;
	}

//	Return taken and unpaid items back to the shop (only for HERO)
	if ((customer->im & IM_HERO))
	{
		XList<XItem *>::iterator it = debt.item_list.begin();
		while (it != debt.item_list.end())
		{
			if (item->im != it->im || item->Compare(it) != 0) { it++; continue; }
			if (it->quantity > item->quantity)
			{
				it->quantity -= item->quantity;
				return 1;
			} 

			if (it->quantity == item->quantity)
			{
				debt.item_list.Remove(it);//->Invalidate();
				return 1;
			}

			item->quantity -= it->quantity;
			XItem * t = it;
			it = debt.item_list.erase(it);
			t->Invalidate();
		}
	}
//	Sell the remaining items to the shopkeeper
	char buf[256];
	char buf1[256];
	item->toString(buf);
	int price = (item->GetValue() / 4 + 1) * item->quantity;

	if ((customer->im & IM_HERO))
	{
		sprintf(buf1, GMSG_SHOPKEEPER_ASK_PRICE, price, buf);
		msgwin.Add(buf1);
	}

	//if it is NPC or Hero asked YES
	if (!(customer->im & IM_HERO) || customer->GetTarget(TR_NO_YES))
	{
		int money_to_add = price;
		if (debt.debtor == customer)
		{
			if ((int)debt.debtor_sum > price)
			{
				debt.debtor_sum -= price;
				return 1;
			} else
			{
				money_to_add = price - (int)debt.debtor_sum;
 				RemovePersonalEnemy(debt.debtor);
				debt.debtor_sum = 0;
				debt.debtor = NULL;
				debt.debtor_leave_shop = 0;
				if (money_to_add == 0)
					return 1;
			}
		}
		assert(money_to_add > 0);
		customer->MoneyOp(money_to_add);
		return 1;
	}
	return 0;
}

int XShopKeeperAI::onGiveItem(XCreature * giver, XItem * item)
{
//	Attempt to give item to the shopkeeper results in selling
//	of that item (by dropping it to the ground)
	if (!(item->im & IM_MONEY))
	{
		int res = onAnyoneDropItem(giver, item);
		if (res)
		{
			ai_owner->ContainItem(item);
		}
		return res;
	} 

//	Giving the money to the shopkeeper results in reducing or
//	completely clearing off a debt
 	if (debt.debtor_sum == 0 && debt.item_list.empty())
 	{
 		msgwin.Add(GMSG_SHOPKEEPER_REJECT_MONEY);
 		return 0;
 	}
	
	item->AddRef(); // prevent money object from destroying

	if (!debt.item_list.empty())
	{

		XList<XItem *>::iterator it = debt.item_list.begin();
		while (it != debt.item_list.end() && item->quantity > 0)
		{
			char buf[256];
			char buf1[256];
			XItem * titem = it;

			titem->toString(buf1);
			sprintf(buf, GMSG_SHOPKEEPER_ASK_FOR_PAY, titem->GetValue() * titem->quantity, buf1);
			msgwin.Add(buf);
			XPoint pt(0, vMin(titem->GetValue() * titem->quantity, item->quantity));
			int res = giver->GetTarget(TR_HOW_MUCH, &pt, titem->GetValue() * titem->quantity);
			if (res <= 0) break;

			giver->MoneyOp(-res);
			debt.debtor_sum += titem->GetValue() * titem->quantity - res;
			XItem * t = it; 
			it = debt.item_list.erase(it);
			t->Invalidate();
			msgwin.Add(GMSG_SHOPKEEPER_THANKS);
		}
	}
    		
 	if (debt.debtor_sum > 0 && item->isValid() && item->quantity > 0)
 	{
 		char buf[256];
 		sprintf(buf, GMSG_SHOPKEEPER_ASK_FOR_PAY2, (int)debt.debtor_sum);
 		msgwin.Add(buf);
 		XPoint pt(0, item->quantity);
 		int res = giver->GetTarget(TR_HOW_MUCH, &pt, (int)debt.debtor_sum);
 		if (res > 0)
 		{
 			debt.debtor_sum -= res;
 			if (debt.debtor_sum < 1)
 			{
 				RemovePersonalEnemy(debt.debtor);
 				debt.debtor            = NULL;
				debt.debtor_sum        = 0;
 				debt.debtor_leave_shop = 0;
 				debt.debtor_add_value  = 0;
 			}
 			giver->MoneyOp(-res);
 			msgwin.Add(GMSG_SHOPKEEPER_THANKS);
 		}
 	}
	item->Release();
	return 0;
}

void XShopKeeperAI::onCreatureEnterShop(XCreature * customer)
{
	if (customer->im & IM_HERO)
	{
		if (isEnemy(customer))
		{
			msgwin.Add("Prepare to die!");
		} else
		{
			char buf[256];
			if (debt.debtor_sum > 0)
				sprintf(buf, GMSG_SHOPKEEPER_TO_CUSTOMER0, ai_owner->name, (int)debt.debtor_sum);
			else
				sprintf(buf, GMSG_SHOPKEEPER_TO_CUSTOMER1, ai_owner->name, customer->name);
			msgwin.Add(buf);
		}
	}
}

void XShopKeeperAI::onCreatureLeaveShop(XCreature * customer)
{
	if (!(customer->im & IM_HERO)) return;

	XItem * item;
	while ((item = static_cast<XItem *>(debt.item_list.RemoveFirst())) != NULL)
	{
		debt.debtor = customer;
		debt.debtor_sum += item->GetValue() * item->quantity;
		item->Invalidate();
	}
	debt.debtor_add_value = debt.debtor_sum * 0.001;
	
	if (isEnemy(customer))
	{
		msgwin.Add("You will be dead soon!");
	} else
	{
		char buf[256];
		if (debt.debtor_sum > 0)
		{
			sprintf(buf, GMSG_SHOPKEEPER_TO_CUSTOMER2, ai_owner->name, (int)debt.debtor_sum);
			debt.debtor_leave_shop = 1;
			debt.debtor = customer;
			if (debt.debtor_sum > 150)
				AddPersonalEnemy(customer);
		}
		else
		{
			sprintf(buf, GMSG_SHOPKEEPER_TO_CUSTOMER3, ai_owner->name, customer->name);
		}

		msgwin.Add(buf);
	}
}

void XShopKeeperAI::Store(XFile * f)
{
	XStandardAI::Store(f);
	f->Write(&debt.debtor_add_value, sizeof(double));
	debt.debtor.Store(f);
	f->Write(&debt.debtor_leave_shop, sizeof(int));
	f->Write(&debt.debtor_sum, sizeof(double));
	debt.item_list.StoreList(f);
	f->Write(&debt.turn_count, sizeof(int));
	shop.Store(f);
}

void XShopKeeperAI::Restore(XFile * f)
{
	XStandardAI::Restore(f);
	f->Read(&debt.debtor_add_value, sizeof(double));
	debt.debtor.Restore(f);
	f->Read(&debt.debtor_leave_shop, sizeof(int));
	f->Read(&debt.debtor_sum, sizeof(double));
	debt.item_list.RestoreList(f);
	f->Read(&debt.turn_count, sizeof(int));
	shop.Restore(f);
}

