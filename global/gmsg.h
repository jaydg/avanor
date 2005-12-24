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

#ifndef __GMSG_H
#define __GMSG_H


#define GMSG_SHOPKEEPER_ATTACK "'You were warned! Prepare to die!'"
#define GMSG_SHOPKEEPER_ATTACK2 "'I'll kill you, you bastard!'"
#define GMSG_SHOPKEEPER_ASK_PRICE "'I can give you " MSG_YELLOW "%d" MSG_YELLOW " gp for %s. Do you agree?'"
#define GMSG_SHOPKEEPER_REJECT_ITEM "'Sorry, I can't buy this!'"
#define GMSG_SHOPKEEPER_REJECT_MONEY "'Thank you, but you don't owe me anything!'"
#define GMSG_SHOPKEEPER_ASK_FOR_PAY "'You must pay me %d gp for %s"
#define GMSG_SHOPKEEPER_ASK_FOR_PAY2 \
		"'You owe me %d gp, " \
		"How much you would like to pay?'"
#define GMSG_SHOPKEEPER_THANKS "'Thank you for paying!'"
#define GMSG_SHOPKEEPER_TO_CUSTOMER0 \
		"%s says 'You owe me %d gp. " \
		"Don't try to take anything without paying!'"
#define GMSG_SHOPKEEPER_TO_CUSTOMER1 "%s says 'Welcome to my shop, %s!'"
#define GMSG_SHOPKEEPER_TO_CUSTOMER2 "%s says 'You forgot to pay me %d gp!'"
#define GMSG_SHOPKEEPER_TO_CUSTOMER3 "%s says 'Have a nice expedition, %s!'"


#endif



