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

#ifndef __XHERO_H
#define __XHERO_H

#include <math.h>
#include "global.h"
#include "creature.h"
#include "incl_i.h"
#include "anycr.h"
#include "xgui.h"

extern int _exit_flag;


enum INVENTORY_FLAG 
{
	IF_NONE			= 0x0000,
	IF_FIXED_MASK	= 0x0001,
	IF_VIEW_ONLY	= 0x0002,
};

enum SKILL_FLAG
{
	SKF_IMPROVE_SKILL,
	SKF_LIST_SKILL,
	SKF_USE_SKILL,
};


class XHero : public XCreature
{
protected:
	XHero() {last_char = '5'; isDisturb = 0; run_way_count = 0; target = NULL;}
	int last_char;
	int run_way_count;
	XPtr<XCreature> target; //for convenient user intrface
	int PossibleWayCount(int px, int py);
public:
	DECLARE_CREATOR(XHero, XCreature);
	XHero(int flag);
	void PlayerSetup();
	void NewMove();
	void Move();
	XItem * Inventory(XItemList * quae, ITEM_MASK mask = IM_ALL, INVENTORY_FLAG flag = IF_NONE, int ret_item_cout = 0, ITEM_FILTR * ifiltr = NULL, FILE * f = NULL);
	void Equipment(FILE * f = NULL);
	void PickItem();
	void DropItem();
	void LookAt();
	void CreateScreenShot();
	static void DumpVBuffer(FILE * f);
	void ReadAll();
	int Compare (XObject * o) {return -1;}
	void ExpList();
	void InfoList();
	void Eat();
	int XShoot();
	int Targeting(int range, XPoint * pt);
	virtual int GetTarget(TARGET_REASON tr, XPoint * pt = NULL, int max_range = 0, XObject ** back = NULL); //Get target for a spell
	virtual XItem * SelectItem(ITEM_FILTR * filtr, bool isGetAll = false);

	int SelectPosition(XPoint * pt, int flag = 0);
	unsigned int turn_count;
	void OpenDoor();
	void OpenChest();
	void CloseDoor();
	virtual void Die(XCreature * killer);
	int XCast(FILE * f = NULL);
	XList<XSpell *>::iterator last_cast;
	int RepeatCast();

	void MagicLevelList();
	XSkill * SkillsList(SKILL_FLAG skill_flag, int marks_left = 0, FILE * f = NULL);
	int UseSkill();
	void IncLevel();
	void WarSkillsList(FILE * f = NULL);
	void HelpScreen();
	void DrinkPotion();
	void SetTactics();
	void SaveGame();
	int UseTool();
	int UseOuterObject();
	void QuickPay();
	void Pray();
	int WhichDirection(XPoint * pt, int flag = 1);//flag == 1 - allow 0,0 coords (self)
	XItem * onIdentifyItem();
	void ShowResistance(FILE * f = NULL);

	void ActivateTrap();
	void GiveItem();
	void ChatWithMonster();
	virtual int Chat(XCreature * chatter, char * msg);

	void FirstStep(int _x, int _y, XLocation * _l);
	void LastStep();

	virtual int stopAction();

	virtual void Store(XFile * f);
	virtual void Restore(XFile * f);

	void doSacrifice();
	int OrderCompanion();
	int ExecuteScript();

	int race;
	int profession;	

	char * GetRaceStr();
	char * GetProfessionStr();

	static void EndGame(char * end_msg);

	//ALCHEMY
	int LearnReception(POTION_NAME pn1, POTION_NAME pn2, POTION_NAME pn3);
	void ShowReception();
	void MixPotions();
	XList<XAlchemyRec *> reception_list;

};


class  XGuiItem_Inventory : public XGuiItem
{
	XItem * pItem;
	char buf[256];
public:
	XGuiItem_Inventory(XItem * item) : pItem(item), XGuiItem() 
	{ 
		char tbuf[256];
		strcpy(buf, MSG_LIGHTGRAY);
		item->toString(buf + strlen(buf));
		for (int i = strlen(buf); i < size_x; i++)
			buf[i] = ' ';
		
		sprintf(tbuf, MSG_BROWN "[" MSG_LIGHTGRAY "%d" MSG_BROWN "]", item->weight * item->quantity);
		strcpy(buf + size_x - 5 - x_strlen(tbuf), tbuf);
	}

   virtual int isSelectable() {return 1;}
   virtual int isTitle() {return 0;}

   virtual bool SetWidth(int new_width) { return true; }
   virtual int GetHeight() { return 1; }
   virtual const char * operator[](int index) { return buf; }
};



#endif
