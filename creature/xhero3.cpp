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

#include "xhero.h"
#include "quest.h"
#include "hiscore.h"
#include "game.h"

int XHero::UseTool()
{
	XBodyPart * tbp = GetBodyPart(BP_TOOL);
	XItem * tool = NULL;
	if (tbp)
		tool = tbp->Item();
	
	if (tool)
	{
		UseItem(tool);
		return 1;
	} else
	{
		msgwin.Add("You have no tool.");
		return 0;
	}

	return 1;
}

void XHero::doSacrifice()
{
	while (1)
	{
		XItem * item = NULL;

		XItemList * tmpquae = l->map->GetItemList(x, y);
		if (tmpquae->empty() || l->map->GetPlace(x, y))		
			item = Inventory(&contain);
		else
			item = Inventory(tmpquae);
		XItem * drop_item = item;
		if (item)
		{
			if (item->quantity > 1)
			{
				XPoint pt(0, item->quantity);
 				msgwin.Add("How much?");
				int res = GetTarget(TR_HOW_MUCH, &pt, item->quantity);
				if (res == 0)
				{
					contain.Add(item);
					break;
				}
				if (res != item->quantity)
				{
					drop_item = (XItem *)item->MakeCopy();
					drop_item->quantity = res;
					item->quantity -= res;
					contain.Add(item);
				}
			}
			if (drop_item)
				Sacrifice(drop_item);
		} else
			break;
	}
}

XItem * XHero::SelectItem(ITEM_FILTR * filtr)
{
	return Inventory(&contain, IM_UNKNOWN, IF_NONE, 1, filtr);
}

void XHero::DumpVBuffer(FILE * f)
{
	for (int j = 0; j < size_y; j++)
	{
		for (int k = 0; k < size_x; k++)
		{
			fprintf(f, "%c", vTestCh(k, j));
		}
		fprintf(f, "\n");
	}

}

void XHero::CreateScreenShot()
{
	for (int i = 0; i < 1000; i++)
	{
		char buf[256];
		sprintf(buf, "shot%3d.txt", i);
		for (unsigned int q = 0; q < strlen(buf); q++)
			if (buf[q] == ' ') buf[q] = '_';
		FILE * f = fopen(buf, "rb");
		if (!f)
		{
			f = fopen(buf, "wb");
			DumpVBuffer(f);
			fclose(f);
			char buf2[256];
			sprintf(buf2, "Screenshot '%s' created successfuly.", buf);
			msgwin.Add(buf2);
			return;
		} else
			fclose(f);
	}
}

void XHero::Pray()
{
	XGuiList list;
	char buf[256];
	DEITY_HELP * pLifeHelp;
	DEITY_HELP * pDeathHelp;

	DEITY_RELATION dr = religion.GetRelation(D_LIFE);
	sprintf(buf, MSG_YELLOW "%s " MSG_LIGHTGRAY "(%s" MSG_LIGHTGRAY ")", religion.GetDeityName(D_LIFE), religion.GetRelationName(dr));
	list.AddItem(new XGuiItem_Text(buf, 0));
	int life_count = religion.GetAvailHelp(D_LIFE, &pLifeHelp);
	if (life_count == 0)
		list.AddItem(new XGuiItem_Text("< No help available >", 0));
	else
	{
		for (int i = 0; i < life_count; i++)
		{
			list.AddItem(new XGuiItem_Text(pLifeHelp[i].help_name, 1));
		}
	}
	list.AddItem(new XGuiItem_Text("", 0));

	dr = religion.GetRelation(D_DEATH);
	sprintf(buf, MSG_YELLOW "%s " MSG_LIGHTGRAY "(%s" MSG_LIGHTGRAY ")", religion.GetDeityName(D_DEATH), religion.GetRelationName(dr));
	list.AddItem(new XGuiItem_Text(buf, 0));
	int death_count = religion.GetAvailHelp(D_DEATH, &pDeathHelp);
	if (death_count == 0)
		list.AddItem(new XGuiItem_Text("< No help available >", 0));
	else
	{
		for (int i = 0; i < death_count; i++)
		{
			list.AddItem(new XGuiItem_Text(pDeathHelp[i].help_name, 1));
		}
	}

	int res = list.Run();
	if (res == -1)
		return;
	if (res < life_count)
	{
		religion.Pray(D_LIFE, &pLifeHelp[res], this);
	} else
		if (res < life_count + death_count)
		{
			religion.Pray(D_DEATH, &pDeathHelp[res - life_count], this);
		}

}



void XHero::EndGame(char * end_msg)
{
	int score = ((XHero *)main_creature)->_EXP + ((XHero *)main_creature)->MoneyOp(0);

	XGuiList list;

	char buf2[256];
	char tbuf[256];
	sprintf(buf2, "%s, %s %s %s (L%d).", ((XHero *)main_creature)->name, ((XHero *)main_creature)->GetGenderStr(), ((XHero *)main_creature)->GetRaceStr(), ((XHero *)main_creature)->GetProfessionStr(), ((XHero *)main_creature)->level);
	list.AddItem(new XGuiItem_Text(buf2));
	
	sprintf(tbuf, "You survived %d turns.", ((XHero *)main_creature)->turn_count);
	list.AddItem(new XGuiItem_Text(tbuf));
	
	if (XQuest::quest.hero_win)
	{
		if (XQuest::quest.ahk_ulan_killed && XQuest::quest.roderick_killed)
		{
			if(main_creature->GetGender() == GEN_MALE)
			{
				list.AddItem(new XGuiItem_Text("You killed Ahk-Ulan and the King of Avanor and became the new King of Avanor."));
			} else if(main_creature->GetGender() == GEN_FEMALE)
			{
				list.AddItem(new XGuiItem_Text("You killed Ahk-Ulan and the King of Avanor and became the new Queen of Avanor."));
			}
			score += 30000;
		} else if(XQuest::quest.ahk_ulan_killed)
		{
			list.AddItem(new XGuiItem_Text("You killed evil Ahk-Ulan and saved Kingdom of Avanor from Ahk-Ulans deadly plans."));
			score += 10000;
		} else
		{
			list.AddItem(new XGuiItem_Text("You killed the King of Avanor and helped Ahk-Ulan to become Usurper of Avanor."));
			score += 20000;
		}
	} else
	{
		list.AddItem(new XGuiItem_Text(end_msg));
	}

	int place_count = 0;
	for (int i = 0; i < L_EOF; i++)
	{
		if (Game.locations[i] && Game.locations[i]->visited_by_hero)
		{
			place_count++;
		}
	}

	score += place_count * 200;
	sprintf(tbuf, "You visited %d places.", place_count);
	list.AddItem(new XGuiItem_Text(tbuf));
	
	DEITY_RELATION dr1 = ((XHero *)main_creature)->religion.GetRelation(D_LIFE);
	DEITY_RELATION dr2 = ((XHero *)main_creature)->religion.GetRelation(D_DEATH);
	int flag = 1;
	if (dr1 >= DR_ADEPT)
	{
		sprintf(tbuf, "You were a %s of %s", XReligion::GetRelationName(dr1), XReligion::GetDeityName(D_LIFE));
		list.AddItem(new XGuiItem_Text(tbuf));
		flag = 0;
		score += dr1 * 300;
	}
	if (dr2 >= DR_ADEPT)
	{
		sprintf(tbuf, "You were a %s of %s", XReligion::GetRelationName(dr2), XReligion::GetDeityName(D_DEATH));
		list.AddItem(new XGuiItem_Text(tbuf));
		score += dr2 * 300;
		flag = 0;
	}
	if (flag)
	{
		list.AddItem(new XGuiItem_Text("You were not very religious."));
	}

	if (XQuest::quest.beelzvile_killed)
	{
		list.AddItem(new XGuiItem_Text("You killed an ancient demon."));
	}

	if (XQuest::quest.torin_quest == 2)
	{
		list.AddItem(new XGuiItem_Text("You helped to pump out gas from the dwarven golden mine."));
		score += 5000;
	}

	if (XQuest::quest.guards_get_orc_slay)
	{
		list.AddItem(new XGuiItem_Text("You brought a useful thing to Ozorik."));
	}

	if (XQuest::quest.roderick_quest == 2)
	{
		list.AddItem(new XGuiItem_Text("You returned 'Eye of Raa' to Roderick."));
		score += 10000;
	}
	if (XQuest::quest.roderick_quest2 == 2)
	{
		list.AddItem(new XGuiItem_Text("You cleansed the tomb of Roderick's ancestors."));
		score += 5000;
	}



	if (XQuest::quest.orcs_killed > 0 && XQuest::quest.total_orcs_killed == 30)
	{
		list.AddItem(new XGuiItem_Text("You helped to repulse an attack of orcs."));
	} else if (XQuest::quest.orcs_killed > 0)
	{
		list.AddItem(new XGuiItem_Text("You tried to help to repulse an attack of orcs."));
	}
	
	sprintf(tbuf, "You scored %d.", score);
	list.AddItem(new XGuiItem_Text(tbuf));
	list.SetCaption(MSG_BROWN "###" MSG_LIGHTGRAY " Achievements " MSG_BROWN "###");
	list.Run();
	
	vGotoXY(0, 0);
	msgwin.ClrMsg();
	msgwin.Add("Create Memory File? (Y/" MSG_CYAN "N" MSG_LIGHTGRAY ")");
	msgwin.Put();
	vRefresh();
	int tch = vGetch();
	if (tch == 'y' || tch == 'Y')
	{
		msgwin.ClrMsg();
		msgwin.Add("### Screenshot ###");
		msgwin.Put();
		char tname[256];
		strcpy(tname, ((XHero *)main_creature)->name);
		strcat(tname, ".mem");
		FILE * f = fopen(vMakePath(HOME_DIR, tname), "w");
		((XHero *)main_creature)->DumpVBuffer(f);
		list.Put(f);
		fprintf(f, "\n");
		((XHero *)main_creature)->Equipment(f);
		fprintf(f, "\n");
		((XHero *)main_creature)->WarSkillsList(f);
		fprintf(f, "\n");
		((XHero *)main_creature)->SkillsList(SKF_LIST_SKILL, 0, f);
		fprintf(f, "\n");
		((XHero *)main_creature)->XCast(f);
		fprintf(f, "\n");
		((XHero *)main_creature)->ShowResistance(f);
		fprintf(f, "\n");
		it_iterator i;
		for (i = ((XHero *)main_creature)->contain.begin(); i != ((XHero *)main_creature)->contain.end(); i++)
		{
			i->Identify(1);
		}
		((XHero *)main_creature)->Inventory(&((XHero *)main_creature)->contain, IM_ALL, IF_NONE, 0, NULL, f);
		fclose(f);
		vClrScr();
	}

	XHiScoreItem * hii;
	if (XQuest::quest.hero_win)
		hii = new XHiScoreItem(0, score, buf2, end_msg, 1, 1);
	else
		hii = new XHiScoreItem(0, score, buf2, end_msg, 0, 1);
	XHiScore hiscore;
	hiscore.AddRecord(hii);
	hiscore.Show();
}

void XHero::ShowResistance(FILE * f)
{
	XGuiList list;
	list.SetCaption(MSG_BROWN "###" MSG_LIGHTGRAY " Resistances and Intrinsics " MSG_BROWN "###");
	list.SetFooter("Press any key to exit");
   
	int flag = 0;
	char buf[256];
	XResistance tr;
	for (int i = 0; i < R_EOF; i++)
	{
		tr.SetResistance((RESISTANCE)i, GetResistance((RESISTANCE)i));
		if (tr.GetResistance((RESISTANCE)i) != 0)
		{
			sprintf(buf, MSG_LIGHTGRAY "%-15s%s", tr.GetResistanceName((RESISTANCE)i), tr.GetResistanceLevel((RESISTANCE)i));
			list.AddItem(new XGuiItem_Text(buf, 0), 0);
			flag = 1;
		}
	}
	
	if (flag == 0)
	{
		list.AddItem(new XGuiItem_Text("You have no Resistances and Intrinsics.", 0), 0);
	}

	if (f)
	{
		list.Put(f);
	} else
	{
		int ch = list.Run(1);
	}
}


int XHero::LearnReception(POTION_NAME pn1, POTION_NAME pn2, POTION_NAME pn3)
{
	XList<XAlchemyRec *>::iterator it;
	for (it = reception_list.begin(); it != reception_list.end(); it++)
		if (it->pn1 == pn1 && it->pn2 == pn2)
			return 0;

	if (XAlchemy::isValidReception(pn1, pn2, pn3))
	{
		reception_list.push_back(new XAlchemyRec(pn1, pn2, pn3));
		msgwin.Add("You have learned a new alchemy recipe.");
	}
	return 1;
}

void XHero::ShowReception()
{
	XGuiList list;
	list.SetCaption(MSG_BROWN "###" MSG_LIGHTGRAY " Recipes " MSG_BROWN "###");
	if (reception_list.empty())
	{
		list.AddItem(new XGuiItem_Text("You don't know any recipes yet.", 0), 0);
	} else
	{
		XList<XAlchemyRec *>::iterator it;
		for (it = reception_list.begin(); it != reception_list.end(); it++)
		{
			char buf[256];
			XAlchemy::GetReceptionName(buf, it->pn1, it->pn2, it->result);
			list.AddItem(new XGuiItem_Text(buf, 0), 0);
		}
	}
	list.Run();
}

void XHero::MixPotions()
{
    XPotion * pot1 = (XPotion *)Inventory(&contain, IM_POTION, IF_FIXED_MASK, 1);
    if (pot1)
    {
		XPotion * pot2 = (XPotion *)Inventory(&contain, IM_POTION, IF_FIXED_MASK, 1);
		if (pot2)
		{
			POTION_NAME pn = XAlchemy::GetPotionName(pot1->pn, pot2->pn);
			POTION_REC * pr = POTION_REC::GetRec(pot1->pn);
			int val = sk->GetLevel(SKT_ALCHEMY) * 8 + 30 - pr->alchemy_power * 10;
			if (pn != PN_UNKNOWN && vRand(100) < val)
			{
				XPotion * pot = new XPotion(pn);
				sk->UseSkill(SKT_ALCHEMY, 3);
				char buf[256];
				char buf1[256];
				pot->toString(buf);
				sprintf(buf1, "You have mixed %s.", buf);
				msgwin.Add(buf1);
				CarryItem(pot);
				contain.Add(pot);
			} else
			{
				msgwin.Add("You failed to mix a new potion.");
			}
			pot1->Invalidate();
			pot2->Invalidate();
		}
    }
}



