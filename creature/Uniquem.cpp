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

#include "unique.h"
#include "uniquei.h"
#include "quest.h"
#include "itemf.h"
#include "gmsg.h"
#include "modifer.h"
#include "xhero.h"
#include "xapi.h"
#include "setting.h"
#include "xtool.h"
#include "skeep_ai.h"
#include "game.h"


///////////////////////////////////////////////////////////////////////
// AHK-ULAN
///////////////////////////////////////////////////////////////////////
REGISTER_CLASS(XAhkUlan);

XAhkUlan::XAhkUlan(_CREATURE * cr) : XAnyCreature(cr) { }

int XAhkUlan::Chat(XCreature * chater, char * msg)
{
	if (xai->isEnemy(chater))
	{
		msgwin.Add("It is time to die!");
		return 1;
	}

	if (XQuest::quest.ahk_ulan_quest == 0)
	{
		msgwin.Add("Hello, brave hero.");
		msgwin.Add("Some years ago, some evil wizards destroyed my tower.  Now I wait here gaining strength and planning my revenge. I am searching for 3 parts to an ancient machine. Bring them to me and I will reward you well.");
		XQuest::quest.ahk_ulan_quest = 1;
		return 1;
	}

	if (XQuest::quest.ahk_ulan_quest < 4 || (XQuest::quest.roderick_ordered && !XQuest::quest.roderick_killed))
	{
		msgwin.Add("Don't disturb me before completing my quest, puny mortal!");
		return 1;
	}

	if (XQuest::quest.roderick_killed)
	{
		XQuest::quest.hero_win = 1;
		XHero::EndGame("***WINNER***");
	}

	return 1;
}


void XAhkUlan::Die(XCreature * killer)
{
	XQuest::quest.ahk_ulan_killed = 1;
	XAnyCreature::Die(killer);
}


int XAhkUlan::onGiveItem(XCreature * giver, XItem * item)
{
	if (item->it == IT_ANCIENTMACHINEPART)
	{
		ContainItem(item);
		XQuest::quest.ahk_ulan_quest += item->quantity;
		if (XQuest::quest.ahk_ulan_quest >= 4)
		{
			msgwin.Add("Very nice job, servant!");
			msgwin.Add("And now, my last request: kill Roderick, for he is only one who can stop me now.");
			XQuest::quest.roderick_ordered = 1;
		} else
		{
			msgwin.Add("You are a loyal servant!");
		}
		return 1;
	} else
	{
		msgwin.Add("Are you jeering at me?");
		Sacrifice(item);
		return 1;
	}
	return 0;
}


///////////////////////////////////////////////////////////////////////
// Beelzevile
///////////////////////////////////////////////////////////////////////
REGISTER_CLASS(XBeelzvile);

XBeelzvile::XBeelzvile(_CREATURE * cr) : XAnyCreature(cr)
{
	if (XSettings::isDemo)
	{
		XCreature::main_creature = this;
	}

	XRing * ring = new XGreatElementalRing();
	ContainItem(ring);
}


void XBeelzvile::Die(XCreature * killer)
{
	XQuest::quest.beelzvile_killed = 1;
	XAnyCreature::Die(killer);
}

void XBeelzvile::NewMove()
{
	if (XSettings::isDemo)
	{
		l->map->Center(x, y);
		l->map->Put(this);
		PutStatus();
		vRefresh();
		msgwin.ClrMsg();
	}
	XAnyCreature::NewMove();
}

void XBeelzvile::Move()
{
	if (XSettings::isDemo)
	{
		HideOldView();
		ShowNewView();
	}
	XAnyCreature::Move();
}



///////////////////////////////////////////////////////////////////////
// Elder Gridor
///////////////////////////////////////////////////////////////////////
REGISTER_CLASS(XVillageElder);

XVillageElder::XVillageElder(_CREATURE * cr) : XAnyCreature(cr) { }

int XVillageElder::Chat(XCreature * chater, char * msg)
{
	if (XQuest::quest.beelzvile_ordered == 0)
	{
		XQuest::quest.beelzvile_ordered = 1;
		msgwin.Add(GMSG_ELDER_QUEST_0);
		msgwin.Add(GMSG_ELDER_QUEST_1);
		msgwin.Add(GMSG_ELDER_QUEST_2);
		msgwin.Add(GMSG_ELDER_QUEST_3);
		msgwin.Add(GMSG_ELDER_QUEST_4);
		return 1;
	};

	if (XQuest::quest.beelzvile_ordered && XQuest::quest.beelzvile_killed == 0)
	{
		msgwin.Add(GMSG_ELDER_QUEST_NOTCOMPLETE);
		return 1;
	}

	if (XQuest::quest.beelzvile_killed)
	{
		msgwin.Add(GMSG_ELDER_QUEST_COMPLETE);
		
		XQList<SCRIPT_CMD> script;
		SCRIPT_CMD cmd;

		XPoint pt;

		cmd.cmd = SCC_MOVE_POINT;
		
		cmd.pt_x = ((XStairWay *)(*Game.locations[L_MUSHROOMS_CAVE5]->ways_list.begin()))->x;
		cmd.pt_y = ((XStairWay *)(*Game.locations[L_MUSHROOMS_CAVE5]->ways_list.begin()))->y;
		cmd.ln = L_MUSHROOMS_CAVE5;
		script.push_back(cmd);

		cmd.cmd = SCC_COLLECT_MUSHROOM;
		script.push_back(cmd);

		cmd.cmd = SCC_MOVE_POINT;
		cmd.pt_x = 14;
		cmd.pt_y = 3;
		cmd.ln = L_MAIN;
		script.push_back(cmd);

		cmd.cmd = SCC_DROP_ITEM;
		cmd.im = IM_FOOD;
		script.push_back(cmd);


		//hack!!!
		XObject * o = root;
		while (o)
		{
			if ((o->im & IM_CREATURE) && ((XCreature *)o)->group_id == GID_SMALL_VILLAGE_FARMER)
			{
				((XCreature *)(o))->xai->ExecuteScript(&script);
			}
			o = o->next;
		}


		
		return 1;
	}

	msgwin.Add("Have a nice day,");
	if(XCreature::main_creature->GetGender() == GEN_MALE)
		msgwin.Add("sir!");
	else if(XCreature::main_creature->GetGender() == GEN_FEMALE)
		msgwin.Add("ma'am!");
	return 1;
}


///////////////////////////////////////////////////////////////////////
// Gefeon, great master of Fire
///////////////////////////////////////////////////////////////////////
REGISTER_CLASS(XGefeon);
XGefeon::XGefeon(_CREATURE * cr) : XAnyCreature(cr) { }

int XGefeon::Chat(XCreature * chater, char * msg)
{
	if (XQuest::quest.ahk_ulan_ordered == 0 && XQuest::quest.ahk_ulan_killed == 0)
	{
		msgwin.Add("Ahk-Ulan, the evil wizard and master of black magic, lives in the dungeon beneath the ruins of his magic tower. The ruins are to the south-east of town. He causes great evil, and he should be eliminated.");
		XQuest::quest.ahk_ulan_ordered = 1;
		return 1;
	}
	
	if (XQuest::quest.ahk_ulan_ordered == 1 && XQuest::quest.ahk_ulan_killed == 0)
	{
		msgwin.Add("And how is Ahk-Ulan? Still alive? That is very bad.");
		return 1;
	}

	
	if (XQuest::quest.ahk_ulan_killed == 1)
	{
		msgwin.Add("You did a great thing! You truly are the best!");
		XQuest::quest.hero_win = 1;

		XHero::EndGame("***WINNER***");
	}
	
	return 1;
}


///////////////////////////////////////////////////////////////////////
// Gekta, the sheep dog
///////////////////////////////////////////////////////////////////////
REGISTER_CLASS(XGekta);
XGekta::XGekta(_CREATURE * cr) : XAnyCreature(cr) { }

int XGekta::onGiveItem(XCreature * giver, XItem * item)
{
	if (item->im & IM_FOOD)
	{
		if (item->it == IT_BONE)
		{
			for (int i = 0; i < item->quantity; i++)
			{
				 if (vRand() % 7 == 0)
				 {
					msgwin.Add(GMSG_GEKTA_FIND_ITEM);
					XItem * it = ICREATE((ITEM_MASK)(IM_ITEM ^ IM_FOOD), 20, 300);
					DropItem(it);
				 } else
				 {
					 msgwin.Add("Gekta eats the bone.");
				 }
			}
		} else
		{
			char buf[256];
			sprintf(buf, "Gekta eats the %s.", item->name);
			msgwin.Add(buf);
		}
		item->Invalidate();
		if (vRand(1) == 0)
		{
			msgwin.Add("Gekta looks at you faithfully!");
			xai->companion = giver;
			xai->companion_command = CC_FOLLOW;
		}
		return 1;
	} else
	{
		return 0;
	}
}



///////////////////////////////////////////////////////////////////////
// Jorgus, the master thief
///////////////////////////////////////////////////////////////////////
REGISTER_CLASS(XJorgus);
XJorgus::XJorgus(_CREATURE * cr) : XAnyCreature(cr) 
{ 
	XBodyPart * cloak = GetBodyPart(BP_CLOAK, 0);
	if (cloak->Item())
		cloak->UnWear()->Invalidate();
	cloak->Wear(new XForestBrotherCloak());
}

int XJorgus::Chat(XCreature * chater, char * msg)
{
	int steal_level = chater->sk->GetLevel(SKT_STEALING);
	char buf[256];

	if (steal_level == 0)
	{
		msgwin.Add(GMSG_JORGUS_SUGGEST);
		int res = chater->GetTarget(TR_YES_NO);
		if (res)
		{
			if (chater->MoneyOp(0) >= 1000)
			{
				chater->MoneyOp(-1000);
				chater->sk->Learn(SKT_STEALING, 1);
				if(main_creature->GetGender() == GEN_MALE)
					sprintf(buf, GMSG_JORGUS_WELCOME, "brother");
				else if(main_creature->GetGender() == GEN_FEMALE)
					sprintf(buf, GMSG_JORGUS_WELCOME, "sister");
				msgwin.Add(buf);
			} else
			{
				msgwin.Add(GMSG_JORGUS_NO_ENOUGH_MONEY);
			}
		} else
		{
			msgwin.Add(GMSG_JORGUS_CANCEL);
		}
	} else
	{
		if(main_creature->GetGender() == GEN_MALE)
			sprintf(buf, GMSG_JORGUS_HELLO, "brother");
		else if(main_creature->GetGender() == GEN_FEMALE)
			sprintf(buf, GMSG_JORGUS_HELLO, "sister");
		msgwin.Add(buf);
	}
	return 1;
}


///////////////////////////////////////////////////////////////////////
// Ozorik, the royal guard captain
///////////////////////////////////////////////////////////////////////
REGISTER_CLASS(XOzorik);

XOzorik::XOzorik(_CREATURE * cr) : XAnyCreature(cr)
{
	XItem * item = new XGlamdring();
	ContainItem(item);
	
	item = new XDeathHack();
	ContainItem(item);
	deathhack_guid = item->xguid;
}

int XOzorik::Chat(XCreature * chater, char * msg)
{
	int flag = 0;
	if ((XQuest::quest.orcs_killed > 0 && Game.locations[L_MAIN]->GetCreatureCount(CR_ORC) == 0)
		|| (XQuest::quest.guards_get_orc_slay > 0))
	{
		flag = 1;
		msgwin.Add(GMSG_OZORIK_QUEST_COMPLETE1);
		XItem * it = NULL;

		XBodyPart * hand = GetBodyPart(BP_HAND, 0);
		if (hand->Item() && hand->Item()->xguid == deathhack_guid)
		{
			it = hand->UnWear();
		}
		hand = GetBodyPart(BP_HAND, 1);
		if (!it && hand->Item() && hand->Item()->xguid == deathhack_guid)
		{
			it = hand->UnWear();
		}

		if (!it)
			it = (XItem *)contain.Find(deathhack_guid);

		if (it)
		{
			contain.Remove(it->xguid);
			msgwin.Add(GMSG_OZORIK_QUEST_COMPLETE2);
			UnCarryItem(it);
			if (chater->CarryItem(it))
				chater->contain.Add(it);
			else
				DropItem(it);
		}
	}
	
	if (XQuest::quest.beelzvile_ordered == 1 && !flag)
	{
		msgwin.Add(GMSG_OZORIK_DEMONS);
	} else if(!flag)
	{
		msgwin.Add(GMSG_OZORIK_BUSY);
	}

	return 1;
}

int XOzorik::onGiveItem(XCreature * giver, XItem * item)
{
	if (item->im & IM_WEAPON && item->brt & BR_ORCSLAYER 
		&& (item->wt == WSK_SWORD))
	{
		msgwin.Add(GMSG_OZORIK_QUEST_COMPLETE0);
	} else
	{
		msgwin.Add(GMSG_OZORIK_REJECT_ITEM);
	}
	return 0;
}

void XOzorik::Store(XFile * f)
{
	XAnyCreature::Store(f);
	f->Write(&deathhack_guid, sizeof(XGUID));
}

void XOzorik::Restore(XFile * f)
{
	XAnyCreature::Restore(f);
	f->Read(&deathhack_guid, sizeof(XGUID));
}


///////////////////////////////////////////////////////////////////////
// Todin, dwarven weaponsmith
///////////////////////////////////////////////////////////////////////
REGISTER_CLASS(XTodin);
XTodin::XTodin(_CREATURE * cr) : XAnyCreature(cr) { }

int XTodin::Chat(XCreature * chater, char * msg)
{
	msgwin.Add(GMSG_TODIN_STD_ANSWER);
	return 1;
}


int XTodin::onGiveItem(XCreature * giver, XItem * item)
{
	if (item->im & IM_WEAPON)
	{
		int money_need = 300;
		int mask = BR_FIRE | BR_COLD | BR_ORCSLAYER;

		if (item->brt & BR_ANY)
			money_need += 300 * vBitsCount(item->brt);
		if (item->brt & BR_FIRE)
		{
			mask ^= BR_FIRE;
		}
		if (item->brt & BR_COLD)
		{
			mask ^= BR_COLD;
		}
		if (item->brt & BR_ORCSLAYER)
		{
			mask ^= BR_ORCSLAYER;
		}

		if (!mask)
		{
			msgwin.Add(GMSG_TODIN_GOOD_WEAPON);
			return 0;
		}

		money_need *= item->quantity;
		
		int target_ench = vGetRandomBit(mask);
		char buf[256];
		sprintf(buf, GMSG_TODIN_ASK_MONEY, money_need);
		msgwin.Add(buf);
		if (giver->MoneyOp(0) >= money_need)
		{
			msgwin.Add(GMSG_TODIN_ASK);
			if (giver->GetTarget(TR_YES_NO))
			{
				item->brt = (BRAND_TYPE)(item->brt | target_ench);
				msgwin.Add(GMSG_TODIN_THANKS);
				giver->MoneyOp(-money_need);
				return 0;
			} else
			{
				msgwin.Add(GMSG_TODIN_CANCEL);
				return 0;
			}
		} else
		{
			msgwin.Add(GMSG_TODIN_NO_ENOUGH_MONEY);
			return 0;
		}
	} else
	{
		msgwin.Add(GMSG_TODIN_REJECT_ITEM);
		return 0;
	}
}


///////////////////////////////////////////////////////////////////////
// Yohjishiro, the elven wizard
///////////////////////////////////////////////////////////////////////
REGISTER_CLASS(XYohjishiro);

XYohjishiro::XYohjishiro(_CREATURE * cr) : XAnyCreature(cr) { }

int XYohjishiro::Chat(XCreature * chater, char * msg)
{
	XSkill * skill = chater->sk->GetSkill(SKT_LITERACY);
	
	msgwin.Add(GMSG_YOHJISHIRO_ASK0);
	char askbuf[10] = "q";
	char buf[256];
	strcpy(buf, "[q] quest"); 
	if (!skill)
	{
		strcat (buf, ", [l] learn");
		strcat(askbuf, "l");
	}
	
	strcat(buf, "?");
	msgwin.Add(buf);

	vRefresh();
	char ch = vXGetch(askbuf);

	if (ch == 0)
	{
		msgwin.Add(GMSG_YOHJISHIRO_CANCEL);
	} else
	{
		if (ch == 'l')
		{
			msgwin.Add(GMSG_YOHJISHIRO_ASK1);
			if (chater->GetTarget(TR_YES_NO))
			{
				if (chater->MoneyOp(-500) >= 0)
				{
					chater->sk->Learn(SKT_LITERACY);
				} else
				{
					msgwin.Add(GMSG_YOHJISHIRO_NO_ENOUGH_MONEY);
				}
			} else
			{
				msgwin.Add(GMSG_YOHJISHIRO_CANCEL);
			}
		}

		if (ch == 'q')
		{
			if (XQuest::quest.yohjishiro_it_quest != IT_UNKNOWN)
			{
				msgwin.Add(GMSG_YOHJISHIRO_NOT_COMPLETE);
			} else
			{
				msgwin.Add(GMSG_YOHJISHIRO_BRING_ME);
				int tr = vRand() % 2;
				if (tr)
				{
					XQuest::quest.yohjishiro_it_quest = IT_RATTAIL;
					msgwin.Add("rat tail.");
				} else
				{
					XQuest::quest.yohjishiro_it_quest = IT_BATWING;
					msgwin.Add("bat wing.");
				}
			}
		}
	}

	return 1;
}

int XYohjishiro::onGiveItem(XCreature * giver, XItem * item)
{
	if (item->im & IM_FOOD && (item->it == IT_RATTAIL || item->it == IT_BATWING))
	{
		if (item->it == XQuest::quest.yohjishiro_it_quest)
		{
			msgwin.Add(GMSG_YOHJISHIRO_COMPLETE);
			XQuest::quest.yohjishiro_it_quest = IT_UNKNOWN;
			for (it_iterator i = giver->contain.begin(); i != giver->contain.end(); ++i)
			{
				i->Identify(1);
			}
		} else
		{
			char tbuf[256];
			sprintf(tbuf, GMSG_YOHJISHIRO_REWARD, 100 * item->quantity);
			msgwin.Add(tbuf);
			giver->MoneyOp(100 * item->quantity);
			item->Invalidate();
		}
		return 1;
	} else
	{
		msgwin.Add(GMSG_YOHJISHIRO_NO_INTEREST);
		return 0;
	}
}



///////////////////////////////////////////////////////////////////////
// RODERICK, King of Avanor
///////////////////////////////////////////////////////////////////////

REGISTER_CLASS(XRoderick);
XRoderick::XRoderick(_CREATURE * cr) : XAnyCreature(cr) 
{ 
	XPotion * pt = new XPotion(PN_HEALING);
	ContainItem(pt);

	pt = new XPotion(PN_HEALING);
	ContainItem(pt);

	pt = new XPotion(PN_HEALING);
	ContainItem(pt);

	pt = new XPotion(PN_HEALING);
	ContainItem(pt);

	XItem * it = new XAvanorCrown();
	ContainItem(it);

	it = new XAvanorScepter();
	ContainItem(it);
}

int XRoderick::Chat(XCreature * chater, char * msg)
{
	XItem * it1 = chater->GetBodyPart(BP_HAND, 0)->Item();
	XItem * it2 = chater->GetBodyPart(BP_HAND, 1)->Item();

	if (xai->isEnemy(chater))
	{
		msgwin.Add("No mercy!");	
	} else if ((it1 && it1->xguid == XAvanorDefender::avanordefender_guid) || (it2 && it2->xguid == XAvanorDefender::avanordefender_guid))
	{
		msgwin.Add(
			"I recognize that sword in your hand. "
			"You have looted the tomb of my ancestors! "
			"Guards! Seize the traitor!");
		xai->AddPersonalEnemy(chater);
		xai->SetGroupEnemy(chater);
	} else 
	{
		msgwin.Add("Hello, brave hero.");
		if (XQuest::quest.roderick_quest2 == 0)
		{
			msgwin.Add(
				"I have heard that my family crypt has been occupied by a group of undead. "
				"Clear the crypt and I will reward you. It lies to the south-west of the city.");
			XQuest::quest.roderick_quest2 = 1;
		} else if (XQuest::quest.roderick_quest2 == 1)
		{
			if (Game.locations[L_UNDEADS_TOMB1]->GetCreatureCount(CR_UNDEAD) == 0)
			{
				msgwin.Add(
					"Thank you for destroying the evil in our crypt. "
					"Please accept these coins and my gratitude for a job well done.");
				XQuest::quest.roderick_quest2 = 2;
				chater->MoneyOp(1000);
			} else
			{
				msgwin.Add("You still have not cleansed my ancestor's crypt.");
			}
		} else if (XQuest::quest.roderick_quest == 0)
		{
			msgwin.Add("Some years ago one of my trusted servants stole a powerful artifact, the 'Eye of Raa' from me. He tryed to hide it from me in one of the caves far south from here, but people say that he was unsuccessful.  Could you return this artifact to me?");
			XQuest::quest.roderick_quest = 1;
		} else if (XQuest::quest.roderick_quest == 1)
		{
			msgwin.Add("Please, return the 'Eye of Raa' to me.");
		}
	}
	return 1;
}


void XRoderick::Die(XCreature * killer)
{
	XQuest::quest.roderick_killed = 1;
	XAnyCreature::Die(killer);
}


int XRoderick::onGiveItem(XCreature * giver, XItem * item)
{
	if (item->it == IT_EYEOFRAA)
	{
		msgwin.Add("Thank you for your great help. The citizens of Avanor never forget your exploits!");
		XQuest::quest.roderick_quest = 2;
		ContainItem(item);
		return 1;
	} else
	{
		msgwin.Add("I don't need this");
	}
	return 0;
}



///////////////////////////////////////////////////////////////////////
// TORIN
///////////////////////////////////////////////////////////////////////
REGISTER_CLASS(XTorin);

XTorin::XTorin(_CREATURE * cr) : XAnyCreature(cr) 
{ 
	XItem * it = new XTorinShield();
	ContainItem(it);

	it = new XTorinAxe();
	ContainItem(it);

	it = new XPickAxe();
	showel_guid = it->xguid;
	ContainItem(it);
}


int XTorin::Chat(XCreature * chater, char * msg)
{
	if (xai->isEnemy(chater))
	{
		msgwin.Add("It is time to die!");
		return 1;
	} else
	{
		if (XQuest::quest.torin_quest == 1)
		{
			msgwin.Add("You haven't complete my previous request? Hmm... GET OUT OF HERE!");
		} else if (XQuest::quest.torin_quest == 0)
		{
			msgwin.Add("Hello, brave hero. As you know, we dwarves mine our treasures deep from the ground. Some time ago, one of our mine was filled by a mysterious gas, which slowly kills all living things. It is oozing from the rocks. We have gas pump there, but there is no one who can switch this pump on. Please solve this problem.");
			XQuest::quest.torin_quest = 1;
		} else if (XQuest::quest.torin_quest == 2)
		{
			msgwin.Add("Thank you for your great help.");

			XItem * it = NULL;
			XBodyPart * tool = GetBodyPart(BP_TOOL, 0);
			if (tool->Item() && tool->Item()->xguid == showel_guid)
			{
				it = tool->UnWear();
			}
			if (!it)
				it = (XItem *)contain.Find(showel_guid);
			if (it)
			{
				contain.Remove(it->xguid);
				msgwin.Add("Take this tool as a reward.");
				UnCarryItem(it);
				if (chater->CarryItem(it))
					chater->contain.Add(it);
				else
					DropItem(it);
			}
			XQuest::quest.torin_quest = 3;
		} else
		{
			msgwin.Add("Thank you for your help.");
		}

	}

	return 1;
}


int XTorin::onGiveItem(XCreature * giver, XItem * item)
{
	return 0;
}


void XTorin::Store(XFile * f)
{
	XAnyCreature::Store(f);
	f->Write(&showel_guid, sizeof(XGUID));	
}

void XTorin::Restore(XFile * f)
{
	XAnyCreature::Restore(f);
	f->Read(&showel_guid, sizeof(XGUID));	
}


///////////////////////////////////////////////////////////////////////
// XSHEE-VOO
///////////////////////////////////////////////////////////////////////
REGISTER_CLASS(XXSheeVoo);

XXSheeVoo::XXSheeVoo(_CREATURE * cr) : XAnyCreature(cr)
{
	XItem * it = new XBlackClub();
	ContainItem(it);
}

int XXSheeVoo::Chat(XCreature * chater, char * msg)
{
	if (xai->isEnemy(chater))
	{
		msgwin.Add("It is time to die!");
		return 1;
	}

	return 1;
}


void XXSheeVoo::Die(XCreature * killer)
{
	XAnyCreature::Die(killer);
}



///////////////////////////////////////////////////////////////////////
// MAGNUSH
///////////////////////////////////////////////////////////////////////
REGISTER_CLASS(XMagnush);

XMagnush::XMagnush(_CREATURE * cr) : XAnyCreature(cr)
{
	XItem * it = new XBlackClub();
	ContainItem(it);
}

void XMagnush::Die(XCreature * killer)
{
	XAnyCreature::Die(killer);
}

///////////////////////////////////////////////////////////////////////
// BANDIT
///////////////////////////////////////////////////////////////////////
REGISTER_CLASS(XBandit);
REGISTER_CLASS(XBanditAI);

XBandit::XBandit(_CREATURE * cr) : XAnyCreature(cr)
{
	xai->Invalidate();
	xai = new XBanditAI(this);
	xai->SetAIFlag(AIF_GUARD_AREA);
	xai->SetAIFlag(AIF_PROTECT_AREA);
	xai->SetEnemyClass(CR_NONE);
	XBodyPart * cloak = GetBodyPart(BP_CLOAK, 0);
	if (cloak->Item())
		cloak->UnWear()->Invalidate();
	cloak->Wear(new XForestBrotherCloak());
}

int XBanditAI::isEnemy(XCreature * cr)
{
	if (isPersonalEnemy(cr)) 
		return 1;
	XBodyPart * bp = cr->GetBodyPart(BP_CLOAK);
	if (bp && bp->Item() && bp->Item()->it == IT_FORESTBROTHERCLOAK) 
		return 0;
	return XStandardAI::isEnemy(cr);
}




///////////////////////////////////////////////////////////////////////
// SHOPKEEPER
///////////////////////////////////////////////////////////////////////

REGISTER_CLASS(XShopkeeper);

XShopkeeper::XShopkeeper(_CREATURE * cr) : XAnyCreature(cr)
{
	XAmulet * am = new XAmulet(ENH_SEEINVISIBLE);
	CarryItem(am);
	XBodyPart * bp = GetBodyPart(BP_NECK);
	bp->Wear(am);

	XRing * rn = new XRing(ENH_ACIDRESIST);
	CarryItem(rn);
	bp = GetBodyPart(BP_RING, 0);
	bp->Wear(rn);

	rn = new XRing(ENH_SLAYING);
	CarryItem(rn);
	bp = GetBodyPart(BP_RING, 1);
	bp->Wear(rn);
}

void XShopkeeper::SetShop(char * _name, XShop * shop)
{
	strcpy(name, _name);
	xai->Invalidate();
	xai = new XShopKeeperAI(this, shop);
}

void XShopkeeper::Die(XCreature * killer)
{
	(((XShopKeeperAI *)xai)->GetShop())->SetShopkeeper(NULL);
	XAnyCreature::Die(killer);
}

char * XShopkeeper::StdAnswer() 
{
	assert(xai && dynamic_cast<XShopKeeperAI *>(xai));
	XShopKeeperAI * ai = static_cast<XShopKeeperAI *>(xai);
	if (!ai->debt.item_list.empty())
	{
		sprintf(static_buffer, "Don't forget to pay for the items you have taken!", ai->debt.debtor_sum);
		return static_buffer;
	}
	if (ai->debt.debtor_sum > 0)
	{
		sprintf(static_buffer, "Remember that you owe me money. Don't touch anything else before you pay me %d gp!", (int)ai->debt.debtor_sum);
		return static_buffer;
	}
	else
	{
		switch(vRand(5))
		{
			case 0 : return "I have excellent quality goods for sale!";
			case 1 : return "I'm sure you will find everything you need here in my shop.";
			case 2 : return "You'd better look at the goods instead of talking.";
			case 3 : return "Don't even try to steal anything.";
			default: return "Please buy an item!";
		}
    }
}
