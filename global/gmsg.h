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

#define GMSG_ELDER_QUEST_0 "Good day, friend! All that we have are our mushrooms." 
#define GMSG_ELDER_QUEST_1 "We collect them in the cave to the west of the village."
#define GMSG_ELDER_QUEST_2 "But a short time ago, an evil monster occupied the cave."
#define GMSG_ELDER_QUEST_3 "It looks like a demon and is very dangerous."
#define GMSG_ELDER_QUEST_4 "Maybe the people who live to the south of the lake can help."
	

#define GMSG_ELDER_QUEST_NOTCOMPLETE "The evil monster is still there."
#define GMSG_ELDER_QUEST_COMPLETE "Thank you for your great help! Now, our farmers can collect mushrooms."


#define GMSG_FARMER_ANSWER \
			"Please speak with our elder. " \
			"He lives in the stone house."

#define GMSG_JORGUS_SUGGEST \
	"I can teach you the great art of theft for 1000gp. " \
	"Would you like to learn?"

#define GMSG_JORGUS_WELCOME	"You're welcome,"
#define GMSG_JORGUS_NO_ENOUGH_MONEY	"You haven't enough money!"
#define GMSG_JORGUS_CANCEL "Don't waste my time!"
#define GMSG_JORGUS_HELLO "Good day,"


#define GMSG_TODIN_STD_ANSWER "'Give me your weapon, and I'll make it the best!'"
#define GMSG_TODIN_ASK "'Do you agree?'"
#define GMSG_TODIN_THANKS "Thank you!"
#define GMSG_TODIN_CANCEL "'Don't waste my time!'"
#define GMSG_TODIN_NO_ENOUGH_MONEY "'But you haven't enough money!'"
#define GMSG_TODIN_REJECT_ITEM "'Sorry, I don't need this.'"
#define GMSG_TODIN_ASK_MONEY "'I need %d gp to improve this weapon.'"
#define GMSG_TODIN_GOOD_WEAPON "'This weapon's good enough!'"


#define GMSG_OZORIK_QUEST_COMPLETE1 "'You gained us victory!'"
#define GMSG_OZORIK_QUEST_COMPLETE2 "'Take this dagger as a reward!'"
#define GMSG_OZORIK_DEMONS \
	"'Demons? We are mighty enough to slay them, " \
	"but now another problem approaches from the south - an orc war-party!'"

#define GMSG_OZORIK_BUSY \
		"'Sorry, but I'm really busy now. " \
		"The orc war-party will be here soon!'"
#define GMSG_OZORIK_QUEST_COMPLETE0 \
	"'Wow, you've probably saved our souls!" \
	"Please, take this weapon to one " \
	"of my guardians, than return to me!"
#define GMSG_OZORIK_REJECT_ITEM "'We are not looking for this.'"



#define GMSG_YOHJISHIRO_HELLO "'Have a nice day!'"
#define GMSG_YOHJISHIRO_NO_ENOUGH_MONEY "'You haven't enough money!'"
#define GMSG_YOHJISHIRO_ASK1 "'Do you want to learn literacy for 500gp?'"
#define GMSG_YOHJISHIRO_CANCEL "'As you wish.'"

//new
#define GMSG_YOHJISHIRO_ASK0 "'What do you wish to speak about'"
#define GMSG_YOHJISHIRO_NOT_COMPLETE "'Please, complete my last request first'"
#define GMSG_YOHJISHIRO_BRING_ME \
	"I can identify all your inventory, " \
	"if you bring me a"
#define GMSG_YOHJISHIRO_REWARD "'I hope %d gp will be enough for this.'"
#define GMSG_YOHJISHIRO_COMPLETE \
	"'Oh, thank you!' Yohjishiro touches you. " \
	"Suddenly you know more about your inventory."
#define GMSG_YOHJISHIRO_NO_INTEREST "It is of no interest to me."
//end new




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


//new
#define GMSG_GEKTA_FIND_ITEM \
		"Gekta suddenly start to dig in the ground. " \
		"She digs a pit. Gekta digs something up from the ground. " \
		"After this, she puts a bone in the pit and buries it."
//end new


#endif



