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

#include "xmoney.h"

REGISTER_CLASS(XMoney);

XMoney::XMoney(int _quantity)
{
	assert(_quantity > 0);
	quantity = _quantity;

	im = IM_MONEY;
	view = '$';
	color = xYELLOW;
	it = IT_MONEY;
	value = 1;
}

void XMoney::toString(char * buf)
{

	if (quantity == 1)
		sprintf(buf, "one golden coin");
	else
		sprintf(buf, "%d golden coins", quantity, name);
}

int XMoney::Compare(XObject * o)
{
	XMoney * tit;
 	if (!(tit = dynamic_cast<XMoney *>(o)))
 	{
 		return XItem::Compare(o);
	} else
	{
		return 0;
	}

}
