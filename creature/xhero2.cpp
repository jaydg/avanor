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
#include "itemf.h"
#include "xdebug.h"
#include "xtool.h"

struct CUSTOM_RACE
{
	char * name;
	char * stats;
	char * max_stats;
	char * speed;
	FOOD_FEELING ff;
};

struct CUSTOM_PROF
{
	char * name;
	char * stats;
};

CUSTOM_RACE cust_race[] = {
{"human",
"St:1d4+8 Dx:1d4+8 To:1d4+8 Le:1d4+8 Wi:1d4+8 Ma:1d4+8 Pe:1d4+8 Ch:1d4+8",
"St:1d8+18 Dx:1d8+18 To:1d8+18 Le:1d8+18 Wi:1d8+18 Ma:1d8+18 Pe:1d8+18 Ch:1d8+18",
"0d0+1000",
FF_NORMAL
},

{"half elf",
"St:1d4+6 Dx:1d4+10 To:1d4+6 Le:1d4+10 Wi:1d4+8 Ma:1d4+8 Pe:1d4+8 Ch:1d4+8",
"St:1d8+16 Dx:1d8+20 To:1d8+16 Le:1d8+20 Wi:1d8+18 Ma:1d8+18 Pe:1d8+18 Ch:1d8+18",
"0d0+1000",
FF_NORMAL
},

{"high elf",
"St:1d4+5 Dx:1d4+12 To:1d4+4 Le:1d4+12 Wi:1d4+8 Ma:1d4+8 Pe:1d4+8 Ch:1d4+8",
"St:1d8+15 Dx:1d8+22 To:1d8+14 Le:1d8+22 Wi:1d8+18 Ma:1d8+18 Pe:1d8+18 Ch:1d8+18",
"0d0+1000",
FF_NORMAL
},


{"halfling",
"St:1d4+5 Dx:1d4+12 To:1d4+4 Le:1d4+8 Wi:1d4+8 Ma:1d4+8 Pe:1d4+10 Ch:1d4+10",
"St:1d8+15 Dx:1d8+22 To:1d8+14 Le:1d8+18 Wi:1d8+18 Ma:1d8+18 Pe:1d8+20 Ch:1d8+20",
"0d0+1000",
FF_SENSETIVE
},

{"half orc",
"St:1d4+14 Dx:1d4+10 To:1d4+12 Le:1d4+4 Wi:1d4+4 Ma:1d4+4 Pe:1d4+10 Ch:1d4+6",
"St:1d8+24 Dx:1d8+20 To:1d8+22 Le:1d8+14 Wi:1d4+14 Ma:1d8+14 Pe:1d8+20 Ch:1d8+16",
"0d0+1000",
FF_TOLERANT
},

{"dwarf",
"St:1d4+10 Dx:1d4+4 To:1d4+14 Le:1d4+8 Wi:1d4+10 Ma:1d4+6 Pe:1d4+6 Ch:1d4+6",
"St:1d8+20 Dx:1d8+14 To:1d8+24 Le:1d8+18 Wi:1d8+20 Ma:1d8+16 Pe:1d8+16 Ch:1d8+16",
"0d0+1000",
FF_NORMAL
},

{"gnome",
"St:1d4+5 Dx:1d4+8 To:1d4+5 Le:1d4+12 Wi:1d4+12 Ma:1d4+8 Pe:1d4+8 Ch:1d4+6",
"St:1d8+15 Dx:1d8+18 To:1d8+15 Le:1d8+22 Wi:1d8+22 Ma:1d8+18 Pe:1d8+18 Ch:1d8+16",
"0d0+1000",
FF_NORMAL
}

};


CUSTOM_PROF cust_profession[] = {
{"warrior",
"St:0d0+4 Dx:0d0+4 To:0d0+4 Le:0d0-3 Wi:0d0-3 Ma:0d0-6 Pe:0d0+0 Ch:0d0+0"},

{"wizard",
"St:0d0-3 Dx:0d0-3 To:0d0-3 Le:0d0+3 Wi:0d0+3 Ma:0d0+3 Pe:0d0+0 Ch:0d0+0"},

{"archer",
"St:0d0+1 Dx:0d0+4 To:0d0+2 Le:0d0-2 Wi:0d0-2 Ma:0d0-4 Pe:0d0+1 Ch:0d0+0"},

{"ranger",
"St:0d0+0 Dx:0d0+2 To:0d0+2 Le:0d0-2 Wi:0d0-1 Ma:0d0-0 Pe:0d0+0 Ch:0d0+0"},

{"priest",
"St:0d0-2 Dx:0d0-2 To:0d0-1 Le:0d0+2 Wi:0d0+2 Ma:0d0-2 Pe:0d0+3 Ch:0d0+0"},

{"paladin",
"St:0d0+2 Dx:0d0+0 To:0d0+2 Le:0d0-2 Wi:0d0+1 Ma:0d0-3 Pe:0d0+0 Ch:0d0+0"},

{"alchemist",
"St:0d0-2 Dx:0d0-2 To:0d0-2 Le:0d0+3 Wi:0d0+1 Ma:0d0-2 Pe:0d0+2 Ch:0d0+0"},

{"bard",
"St:0d0+1 Dx:0d0+1 To:0d0 Le:0d0-2 Wi:0d0 Ma:0d0 Pe:0d0 Ch:0d0+0"},


};


void XHero::PickRace()
{
	vClrScr();
	vGotoXY(7, 4);
	vPutS(MSG_LIGHTGRAY "Choose a race:");
	
	char buf[256];

	
	int i;
	for (i = 0; i < 7; i++)
	{
		vGotoXY(7, 6 + i);
		sprintf(buf, MSG_LIGHTGRAY "[" MSG_YELLOW "%c" MSG_LIGHTGRAY "] %s ", i + 97,  cust_race[i].name);
		vPutS(buf);
	}
	vRefresh();
	
	char race_choise = ' ';
	while (1)
	{


#ifndef __CHOOSE_RACE
		char ch = vGetch();
#else
		char ch = 'c';
#endif
		race_choise = ch;

		if (ch >= 97 && ch < 97 + 7)
		{
			XDice * d = new XDice(cust_race[ch - 97].speed);
			ttmb = d->Throw();
			ttm = ttmb;
			delete d;
			s = new XStats(cust_race[ch - 97].stats);
			max_stats.Set(cust_race[ch - 97].max_stats);
			food_feeling = cust_race[ch - 97].ff;
			strcpy(race_profession, cust_race[ch - 97].name);
			strcat(race_profession, " ");
			vClrScr();
			break;
		}
	}


	switch (race_choise)
	{
		case 'a':
			sk->Learn(SKT_COOKING);
			sk->Learn(SKT_BACKSTABBING);
			break;
		case 'b':
			break;
		case 'c':
			sk->Learn(SKT_COOKING);
			break;
		case 'd':
			break;
		case 'e':
			sk->Learn(SKT_ATHLETICS);
			break;
		case 'f':
			sk->Learn(SKT_MINING);
		case 'g':
			sk->Learn(SKT_MINING);
			sk->Learn(SKT_FINDWEAKNESS);
			break;

	}
	sk->Learn(SKT_FIRST_AID);


	vClrScr();
	vGotoXY(7, 4);
	vPutS(MSG_LIGHTGRAY "Choose a profession:");
	

	
	for (i = 0; i < 8; i++)
	{
		vGotoXY(7, 6 + i);
		sprintf(buf, MSG_LIGHTGRAY "[" MSG_YELLOW "%c" MSG_LIGHTGRAY "] %s ", i + 97,  cust_profession[i].name);
		vPutS(buf);
	}
	vRefresh();
	while (1)
	{


#ifndef __CHOOSE_RACE
		char ch = vGetch();
#else
		char ch = 'b';
#endif

		if (ch >= 97 && ch < 97 + 8)
		{
			XStats * stmp = new XStats(cust_profession[ch - 97].stats);
			s->Add(stmp);
			delete stmp;
			for (int ii = S_STR; ii < S_EOF; ii++)
			{
				if (s->Get((STATS)ii) < 1)
					s->SetStat((STATS)ii, 1);
			}
			vClrScr();
			XBodyPart * pbp;
			XPotion * potion;
			XBook * book;
			XScroll * scroll;
			switch (ch)
			{
//************** warrior
				case 'a' :
					switch (race_choise)
					{
						case 'a':
							pbp = GetBodyPart(BP_HAND, 0);
							pbp->Wear(ICREATEB(IM_WEAPON, IT_LONGSWORD, 1, 100));

							pbp = GetBodyPart(BP_HAND, 1);
							pbp->Wear(ICREATEB(IM_SHIELD, IT_SMALLSHIELD, 1, 100));
							break;

						case 'b':
							pbp = GetBodyPart(BP_HAND, 0);
							pbp->Wear(ICREATEB(IM_WEAPON, IT_RAPIER, 1, 100));
							
							pbp = GetBodyPart(BP_BOOTS, 0);
							pbp->Wear(ICREATEB(IM_BOOTS, IT_SANDALS, 1, 20));
							break;

						case 'c':
							pbp = GetBodyPart(BP_HAND, 0);
							pbp->Wear(ICREATEB(IM_WEAPON, IT_RAPIER, 1, 100));
							
							pbp = GetBodyPart(BP_CLOAK, 0);
							pbp->Wear(ICREATEB(IM_CLOAK, IT_LIGHTCLOAK, 1, 50));

							pbp = GetBodyPart(BP_BOOTS, 0);
							pbp->Wear(ICREATEB(IM_BOOTS, IT_SANDALS, 1, 20));
							break;

						case 'd':
							pbp = GetBodyPart(BP_HAND, 0);
							pbp->Wear(ICREATEB(IM_WEAPON, IT_SHORTSWORD, 1, 100));
							
							pbp = GetBodyPart(BP_CLOAK, 0);
							pbp->Wear(ICREATEB(IM_CLOAK, IT_LIGHTCLOAK, 1, 50));

							pbp = GetBodyPart(BP_BOOTS, 0);
							pbp->Wear(ICREATEB(IM_BOOTS, IT_LIGHTBOOTS, 1, 40));

							sk->Learn(SKT_DISARMTRAP);
							break;

						case 'e':
							pbp = GetBodyPart(BP_HAND, 0);
							pbp->Wear(ICREATEB(IM_WEAPON, IT_ORCISHAXE, 1, 100));

							pbp = GetBodyPart(BP_HAND, 1);
							pbp->Wear(ICREATEB(IM_SHIELD, IT_MEDIUMSHIELD, 1, 100));
							break;

						case 'f':
							pbp = GetBodyPart(BP_HAND, 0);
							pbp->Wear(ICREATEB(IM_WEAPON, IT_BATTLEAXE, 1, 100));

							pbp = GetBodyPart(BP_HAND, 1);
							pbp->Wear(ICREATEB(IM_SHIELD, IT_MEDIUMSHIELD, 1, 100));
							break;

						case 'g':
							pbp = GetBodyPart(BP_HAND, 0);
							pbp->Wear(ICREATEB(IM_WEAPON, IT_WARAXE, 1, 100));

							pbp = GetBodyPart(BP_HAND, 1);
							pbp->Wear(ICREATEB(IM_SHIELD, IT_SMALLSHIELD, 1, 100));
							break;


					}

					potion = new XPotion(PN_CURE_LIGHT_WOUNDS);
					potion->Identify(1);
					ContainItem(potion);

					
					potion = new XPotion(PN_HEROISM);
					potion->Identify(1);
					ContainItem(potion);
					
					sk->Learn(SKT_FINDWEAKNESS);
					sk->Learn(SKT_HEALING);
					sk->Learn(SKT_TACTICS);
					sk->Learn(SKT_ATHLETICS);
		
					strcat(race_profession, "warrior");
					break;

//************** wizard
				case 'b' :
					{
						pbp = GetBodyPart(BP_HAND, 0);
						pbp->Wear(ICREATEB(IM_WEAPON, IT_STAFF, 1, 100));

						potion = new XPotion(PN_POWER);
						potion->Identify(1);
						ContainItem(potion);
						
						for (int t = 1; t < 3; t++)
						{
							if (vRand() % 2 == 1)
								scroll = new XScroll(SCROLL_FIRE_BOLT);
							else
								scroll = new XScroll(SCROLL_ICE_BOLT);
							scroll->Identify(1);
							ContainItem(scroll);
						}
					
						if (vRand() % 2 == 1)
							book = new XBook(BOOK_FIRE_BOLT);
						else
							book = new XBook(BOOK_ICE_BOLT);
						book->Identify(1);
						ContainItem(book);

						book = (XBook *)ICREATEA(IM_BOOK);
						book->Identify(1);
						ContainItem(book);
						

						sk->Learn(SKT_HEALING);
						sk->Learn(SKT_LITERACY);
						sk->Learn(SKT_CONCENTRATION);
						sk->Learn(SKT_HERBALISM);

						strcat(race_profession, "wizard");
					}
					break;

//*************** archer
				case 'c' :
					switch (race_choise)
					{
						case 'a':
							pbp = GetBodyPart(BP_MISSILEWEAPON, 0);
							pbp->Wear(ICREATEB(IM_MISSILEW, IT_CROSSBOW, 1, 100));

							pbp = GetBodyPart(BP_MISSILE, 0);
							pbp->Wear(ICREATEB(IM_MISSILE, IT_QUARREL, 1, 100));
							pbp->Item()->quantity = vRand() % 10 + 10;

							pbp = GetBodyPart(BP_HAND, 0);
							pbp->Wear(ICREATEB(IM_WEAPON, IT_DAGGER, 1, 100));
							break;

						case 'b':
							pbp = GetBodyPart(BP_MISSILEWEAPON, 0);
							pbp->Wear(ICREATEB(IM_MISSILEW, IT_LONGBOW, 1, 100));

							pbp = GetBodyPart(BP_MISSILE, 0);
							pbp->Wear(ICREATEB(IM_MISSILE, IT_ARROW, 1, 100));
							pbp->Item()->quantity = vRand() % 10 + 10;

							pbp = GetBodyPart(BP_HAND, 0);
							pbp->Wear(ICREATEB(IM_WEAPON, IT_DAGGER, 1, 100));
							
							pbp = GetBodyPart(BP_BOOTS, 0);
							pbp->Wear(ICREATEB(IM_BOOTS, IT_SANDALS, 1, 20));
							break;

						case 'c':
							pbp = GetBodyPart(BP_MISSILEWEAPON, 0);
							pbp->Wear(ICREATEB(IM_MISSILEW, IT_LONGBOW, 1, 100));

							pbp = GetBodyPart(BP_MISSILE, 0);
							pbp->Wear(ICREATEB(IM_MISSILE, IT_ARROW, 1, 100));
							pbp->Item()->quantity = vRand() % 10 + 10;

							pbp = GetBodyPart(BP_HAND, 0);
							pbp->Wear(ICREATEB(IM_WEAPON, IT_RAPIER, 1, 100));
							
							pbp = GetBodyPart(BP_CLOAK, 0);
							pbp->Wear(ICREATEB(IM_CLOAK, IT_LIGHTCLOAK, 1, 50));

							pbp = GetBodyPart(BP_BOOTS, 0);
							pbp->Wear(ICREATEB(IM_BOOTS, IT_SANDALS, 1, 20));
							break;


						case 'd':
							pbp = GetBodyPart(BP_MISSILEWEAPON, 0);
							pbp->Wear(ICREATEB(IM_MISSILEW, IT_SLING, 1, 100));

							pbp = GetBodyPart(BP_MISSILE, 0);
							pbp->Wear(ICREATEB(IM_MISSILE, IT_SLINGBULLET, 1, 100));
							pbp->Item()->quantity = vRand() % 10 + 10;

							pbp = GetBodyPart(BP_CLOAK, 0);
							pbp->Wear(ICREATEB(IM_CLOAK, IT_LIGHTCLOAK, 1, 50));

							pbp = GetBodyPart(BP_BOOTS, 0);
							pbp->Wear(ICREATEB(IM_BOOTS, IT_LIGHTBOOTS, 1, 40));
							break;

						case 'e':
							pbp = GetBodyPart(BP_MISSILEWEAPON, 0);
							pbp->Wear(ICREATEB(IM_MISSILEW, IT_HEAVYCROSSBOW, 1, 100));
							
							pbp = GetBodyPart(BP_MISSILE, 0);
							pbp->Wear(ICREATEB(IM_MISSILE, IT_QUARREL, 1, 100));
							pbp->Item()->quantity = vRand() % 10 + 10;

							pbp = GetBodyPart(BP_HAND, 0);
							pbp->Wear(ICREATEB(IM_WEAPON, IT_ORCISHDAGGER, 1, 100));
							break;

						case 'f':
							pbp = GetBodyPart(BP_MISSILEWEAPON, 0);
							pbp->Wear(ICREATEB(IM_MISSILEW, IT_HEAVYCROSSBOW, 1, 100));

							pbp = GetBodyPart(BP_MISSILE, 0);
							pbp->Wear(ICREATEB(IM_MISSILE, IT_QUARREL, 1, 100));
							pbp->Item()->quantity = vRand() % 10 + 10;

							pbp = GetBodyPart(BP_HAND, 0);
							pbp->Wear(ICREATEB(IM_WEAPON, IT_WARAXE, 1, 100));
							break;

						case 'g':
							pbp = GetBodyPart(BP_MISSILEWEAPON, 0);
							pbp->Wear(ICREATEB(IM_MISSILEW, IT_LIGHTCROSSBOW, 1, 100));

							pbp = GetBodyPart(BP_MISSILE, 0);
							pbp->Wear(ICREATEB(IM_MISSILE, IT_QUARREL, 1, 100));
							pbp->Item()->quantity = vRand() % 10 + 10;

							pbp = GetBodyPart(BP_HAND, 0);
							pbp->Wear(ICREATEB(IM_WEAPON, IT_WARAXE, 1, 100));
							break;


					}

					potion = new XPotion(PN_CURE_LIGHT_WOUNDS);
					potion->Identify(1);
					ContainItem(potion);

					sk->Learn(SKT_FINDWEAKNESS);
					sk->Learn(SKT_HEALING);
					sk->Learn(SKT_ARCHERY);
					sk->Learn(SKT_ATHLETICS);

					strcat(race_profession, "archer");
					break;


//*************** ranger
				case 'd' :
					switch (race_choise)
					{
						case 'a':
							pbp = GetBodyPart(BP_MISSILEWEAPON, 0);
							pbp->Wear(ICREATEB(IM_MISSILEW, IT_CROSSBOW, 1, 100));

							pbp = GetBodyPart(BP_MISSILE, 0);
							pbp->Wear(ICREATEB(IM_MISSILE, IT_QUARREL, 1, 100));
							pbp->Item()->quantity = vRand() % 10 + 10;

							pbp = GetBodyPart(BP_HAND, 0);
							pbp->Wear(ICREATEB(IM_WEAPON, IT_DAGGER, 1, 100));
							break;

						case 'b':
							pbp = GetBodyPart(BP_MISSILEWEAPON, 0);
							pbp->Wear(ICREATEB(IM_MISSILEW, IT_LONGBOW, 1, 100));

							pbp = GetBodyPart(BP_MISSILE, 0);
							pbp->Wear(ICREATEB(IM_MISSILE, IT_ARROW, 1, 100));
							pbp->Item()->quantity = vRand() % 10 + 10;

							pbp = GetBodyPart(BP_HAND, 0);
							pbp->Wear(ICREATEB(IM_WEAPON, IT_DAGGER, 1, 100));
							
							pbp = GetBodyPart(BP_BOOTS, 0);
							pbp->Wear(ICREATEB(IM_BOOTS, IT_SANDALS, 1, 20));
							break;

						case 'c':
							pbp = GetBodyPart(BP_MISSILEWEAPON, 0);
							pbp->Wear(ICREATEB(IM_MISSILEW, IT_LONGBOW, 1, 100));

							pbp = GetBodyPart(BP_MISSILE, 0);
							pbp->Wear(ICREATEB(IM_MISSILE, IT_ARROW, 1, 100));
							pbp->Item()->quantity = vRand() % 10 + 10;

							pbp = GetBodyPart(BP_HAND, 0);
							pbp->Wear(ICREATEB(IM_WEAPON, IT_RAPIER, 1, 100));
							
							pbp = GetBodyPart(BP_CLOAK, 0);
							pbp->Wear(ICREATEB(IM_CLOAK, IT_LIGHTCLOAK, 1, 50));

							pbp = GetBodyPart(BP_BOOTS, 0);
							pbp->Wear(ICREATEB(IM_BOOTS, IT_SANDALS, 1, 20));
							break;


						case 'd':
							pbp = GetBodyPart(BP_MISSILEWEAPON, 0);
							pbp->Wear(ICREATEB(IM_MISSILEW, IT_SLING, 1, 100));

							pbp = GetBodyPart(BP_MISSILE, 0);
							pbp->Wear(ICREATEB(IM_MISSILE, IT_SLINGBULLET, 1, 100));
							pbp->Item()->quantity = vRand() % 10 + 10;

							pbp = GetBodyPart(BP_HAND, 0);
							pbp->Wear(ICREATEB(IM_WEAPON, IT_LONGDAGGER, 1, 100));

							pbp = GetBodyPart(BP_CLOAK, 0);
							pbp->Wear(ICREATEB(IM_CLOAK, IT_LIGHTCLOAK, 1, 50));

							pbp = GetBodyPart(BP_BOOTS, 0);
							pbp->Wear(ICREATEB(IM_BOOTS, IT_LIGHTBOOTS, 1, 40));
							break;

						case 'e':
							pbp = GetBodyPart(BP_MISSILEWEAPON, 0);
							pbp->Wear(ICREATEB(IM_MISSILEW, IT_HEAVYCROSSBOW, 1, 100));
							
							pbp = GetBodyPart(BP_MISSILE, 0);
							pbp->Wear(ICREATEB(IM_MISSILE, IT_QUARREL, 1, 100));
							pbp->Item()->quantity = vRand() % 10 + 10;

							pbp = GetBodyPart(BP_HAND, 0);
							pbp->Wear(ICREATEB(IM_WEAPON, IT_ORCISHDAGGER, 1, 100));
							break;

						case 'f':
							pbp = GetBodyPart(BP_MISSILEWEAPON, 0);
							pbp->Wear(ICREATEB(IM_MISSILEW, IT_HEAVYCROSSBOW, 1, 100));

							pbp = GetBodyPart(BP_MISSILE, 0);
							pbp->Wear(ICREATEB(IM_MISSILE, IT_QUARREL, 1, 100));
							pbp->Item()->quantity = vRand() % 10 + 10;

							pbp = GetBodyPart(BP_HAND, 0);
							pbp->Wear(ICREATEB(IM_WEAPON, IT_WARAXE, 1, 100));
							break;

						case 'g':
							pbp = GetBodyPart(BP_MISSILEWEAPON, 0);
							pbp->Wear(ICREATEB(IM_MISSILEW, IT_LIGHTCROSSBOW, 1, 100));

							pbp = GetBodyPart(BP_MISSILE, 0);
							pbp->Wear(ICREATEB(IM_MISSILE, IT_QUARREL, 1, 100));
							pbp->Item()->quantity = vRand() % 10 + 10;

							pbp = GetBodyPart(BP_HAND, 0);
							pbp->Wear(ICREATEB(IM_WEAPON, IT_WARAXE, 1, 100));
							break;

					}

					potion = new XPotion(PN_CURE_LIGHT_WOUNDS);
					potion->Identify(1);
					ContainItem(potion);

					book = new XBook(BOOK_MAGIC_ARROW);
					book->Identify(1);
					ContainItem(book);

					sk->Learn(SKT_FINDWEAKNESS);
					sk->Learn(SKT_ARCHERY);
					sk->Learn(SKT_CONCENTRATION);
					sk->Learn(SKT_LITERACY);
					sk->Learn(SKT_COOKING);
					sk->Learn(SKT_ATHLETICS);

					strcat(race_profession, "ranger");
					break;


//*************** priest
				case 'e' :
					pbp = GetBodyPart(BP_HAND, 0);
					pbp->Wear(ICREATEB(IM_WEAPON, IT_MACE, 10, 150));

					pbp = GetBodyPart(BP_HAND, 1);
					pbp->Wear(ICREATEB(IM_SHIELD, IT_SMALLSHIELD, 10, 150));

					scroll = new XScroll(SCROLL_BLINK);
					scroll->Identify(1);
					ContainItem(scroll);

					scroll = new XScroll(SCROLL_HEROISM);
					scroll->Identify(1);
					ContainItem(scroll);

					potion = new XPotion(PN_CURE_LIGHT_WOUNDS);
					potion->Identify(1);
					ContainItem(potion);

					sk->Learn(SKT_HEALING);
					sk->Learn(SKT_LITERACY);
					sk->Learn(SKT_HERBALISM);
					sk->Learn(SKT_RELIGION);

					strcat(race_profession, "priest");
					switch (race_choise)
					{
						case 'a':
						case 'e':
							religion.death_act = 200;
							break;
						case 'b':
						case 'c':
						case 'd':
						case 'f':
						case 'g':
							religion.life_act = 200;
							break;

					}
					break;

//*************** paladin
				case 'f' :
					pbp = GetBodyPart(BP_HAND, 0);
					pbp->Wear(ICREATEB(IM_WEAPON, IT_MACE, 10, 150));

					pbp = GetBodyPart(BP_HAND, 1);
					pbp->Wear(ICREATEB(IM_SHIELD, IT_LARGESHIELD, 10, 150));

					scroll = new XScroll(SCROLL_BLINK);
					scroll->Identify(1);
					ContainItem(scroll);

					scroll = new XScroll(SCROLL_HEROISM);
					scroll->Identify(1);
					ContainItem(scroll);

					potion = new XPotion(PN_CURE_LIGHT_WOUNDS);
					potion->Identify(1);
					ContainItem(potion);

					sk->Learn(SKT_HEALING);
					sk->Learn(SKT_LITERACY);
					sk->Learn(SKT_RELIGION);
					sk->Learn(SKT_ATHLETICS);

					strcat(race_profession, "paladin");
					switch (race_choise)
					{
						case 'a':
						case 'e':
							religion.death_act = 100;
							break;
						case 'b':
						case 'c':
						case 'd':
						case 'f':
						case 'g':
							religion.life_act = 100;
							break;

					}
					break;


//*************** alchemist
				case 'g' :
					pbp = GetBodyPart(BP_HAND, 0);
					pbp->Wear(ICREATEB(IM_WEAPON, IT_DAGGER, 10, 150));

					scroll = new XScroll(SCROLL_BLINK);
					scroll->Identify(1);
					ContainItem(scroll);

					scroll = new XScroll(SCROLL_RECEPTION);
					scroll->Identify(1);
					ContainItem(scroll);

					scroll = new XScroll(SCROLL_RECEPTION);
					scroll->Identify(1);
					ContainItem(scroll);

					potion = new XPotion(PN_CURE_LIGHT_WOUNDS);
					potion->Identify(1);
					ContainItem(potion);

					potion = new XPotion(PN_ORANGEJUCE);
					potion->Identify(1);
					ContainItem(potion);

					potion = new XPotion(PN_APPLEJUCE);
					potion->Identify(1);
					ContainItem(potion);

					potion = new XPotion(PN_WATER);
					potion->Identify(1);
					ContainItem(potion);

					{
						XItem * it = new XAlchemySet();
						ContainItem(it);
					}

					sk->Learn(SKT_HEALING);
					sk->Learn(SKT_LITERACY);
					sk->Learn(SKT_HERBALISM);
					sk->Learn(SKT_ALCHEMY);

					strcat(race_profession, "alchemist");
					break;

//*************** bard
				case 'h' :
					{
						pbp = GetBodyPart(BP_HAND, 0);
						pbp->Wear(ICREATEB(IM_WEAPON, IT_CLUB, 10, 150));

						potion = new XPotion(PN_CURE_LIGHT_WOUNDS);
						potion->Identify(1);
						ContainItem(potion);

						sk->Learn(SKT_HEALING);
						sk->Learn(SKT_LITERACY);
						sk->Learn(SKT_HERBALISM);
						sk->Learn(SKT_ALCHEMY);

					}
					strcat(race_profession, "bard");
					break;


			}
			break;
		}
	}
	
	XBodyPart * bp = GetBodyPart(BP_HAND, 0);
	if (bp->Item() && (bp->Item()->im & IM_WEAPON))
		wsk->SetLevel(bp->Item()->wt, 2);
	
	sk->Learn(SKT_DETECTTRAP);
	
	bp = GetBodyPart(BP_BODY, 0);
	if (vRand(2) == 0)
		bp->Wear(ICREATEB(IM_BODY, IT_CLOTHES, 1, 100));
	else
		bp->Wear(ICREATEB(IM_BODY, IT_ROBE, 1, 100));

#ifndef __ENTER_NAME
	vClrScr();
	vGotoXY(0, 4);
	vPutS(MSG_LIGHTGRAY "Enter character name (15 letters max.): ");
	vRefresh();
	char char_name[20];
	vGetS(char_name, 15);
	strcpy(name, char_name);
#else
	strcpy(name, "-=RET=-");
#endif



}
