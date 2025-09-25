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

#include "creature/xhero.h"
#include "item/itemf.h"
#include "item/xtool.h"

struct CUSTOM_RACE {
    const char* name;
    const char* stats;
    const char* max_stats;
    const char* speed;
    FOOD_FEELING ff;
};

struct CUSTOM_PROF {
    const char* name;
    const char* stats;
};

struct CUSTOM_GEND {
    const char* name;
    const char* stats;
};

CUSTOM_RACE cust_race[] = {
    {
        "human",
        "St:1d4+8 Dx:1d4+8 To:1d4+8 Le:1d4+8 Wi:1d4+8 Ma:1d4+8 Pe:1d4+8 Ch:1d4+8",
        "St:1d8+18 Dx:1d8+18 To:1d8+18 Le:1d8+18 Wi:1d8+18 Ma:1d8+18 Pe:1d8+18 Ch:1d8+18",
        "0d0+1000",
        FF_NORMAL
    },
    {
        "half elf",
        "St:1d4+6 Dx:1d4+10 To:1d4+6 Le:1d4+10 Wi:1d4+8 Ma:1d4+8 Pe:1d4+8 Ch:1d4+8",
        "St:1d8+16 Dx:1d8+20 To:1d8+16 Le:1d8+20 Wi:1d8+18 Ma:1d8+18 Pe:1d8+18 Ch:1d8+18",
        "0d0+1000",
        FF_NORMAL
    },
    {
        "high elf",
        "St:1d4+5 Dx:1d4+12 To:1d4+4 Le:1d4+12 Wi:1d4+8 Ma:1d4+8 Pe:1d4+8 Ch:1d4+8",
        "St:1d8+15 Dx:1d8+22 To:1d8+14 Le:1d8+22 Wi:1d8+18 Ma:1d8+18 Pe:1d8+18 Ch:1d8+18",
        "0d0+1000",
        FF_NORMAL
    },
    {
        "halfling",
        "St:1d4+5 Dx:1d4+12 To:1d4+4 Le:1d4+8 Wi:1d4+8 Ma:1d4+8 Pe:1d4+10 Ch:1d4+10",
        "St:1d8+15 Dx:1d8+22 To:1d8+14 Le:1d8+18 Wi:1d8+18 Ma:1d8+18 Pe:1d8+20 Ch:1d8+20",
        "0d0+1000",
        FF_SENSITIVE
    },
    {
        "half orc",
        "St:1d4+14 Dx:1d4+10 To:1d4+12 Le:1d4+4 Wi:1d4+4 Ma:1d4+4 Pe:1d4+10 Ch:1d4+6",
        "St:1d8+24 Dx:1d8+20 To:1d8+22 Le:1d8+14 Wi:1d4+14 Ma:1d8+14 Pe:1d8+20 Ch:1d8+16",
        "0d0+1000",
        FF_TOLERANT
    },
    {
        "dwarf",
        "St:1d4+10 Dx:1d4+4 To:1d4+14 Le:1d4+8 Wi:1d4+10 Ma:1d4+6 Pe:1d4+6 Ch:1d4+6",
        "St:1d8+20 Dx:1d8+14 To:1d8+24 Le:1d8+18 Wi:1d8+20 Ma:1d8+16 Pe:1d8+16 Ch:1d8+16",
        "0d0+1000",
        FF_NORMAL
    },
    {
        "gnome",
        "St:1d4+5 Dx:1d4+8 To:1d4+5 Le:1d4+12 Wi:1d4+12 Ma:1d4+8 Pe:1d4+8 Ch:1d4+6",
        "St:1d8+15 Dx:1d8+18 To:1d8+15 Le:1d8+22 Wi:1d8+22 Ma:1d8+18 Pe:1d8+18 Ch:1d8+16",
        "0d0+1000",
        FF_NORMAL
    }
};

CUSTOM_PROF cust_profession[] = {
    {
        "warrior",
        "St:0d0+4 Dx:0d0+4 To:0d0+4 Le:0d0-3 Wi:0d0-3 Ma:0d0-6 Pe:0d0+0 Ch:0d0+0"
    },
    {
        "wizard",
        "St:0d0-3 Dx:0d0-3 To:0d0-3 Le:0d0+3 Wi:0d0+3 Ma:0d0+3 Pe:0d0+0 Ch:0d0+0"
    },
    {
        "archer",
        "St:0d0+1 Dx:0d0+4 To:0d0+2 Le:0d0-2 Wi:0d0-2 Ma:0d0-4 Pe:0d0+1 Ch:0d0+0"
    },
    {
        "ranger",
        "St:0d0+0 Dx:0d0+2 To:0d0+2 Le:0d0-2 Wi:0d0-1 Ma:0d0-0 Pe:0d0+0 Ch:0d0+0"
    },
    {
        "cleric",
        "St:0d0-2 Dx:0d0-2 To:0d0-1 Le:0d0+2 Wi:0d0+2 Ma:0d0-2 Pe:0d0+3 Ch:0d0+0"
    },
    {
        "paladin",
        "St:0d0+2 Dx:0d0+0 To:0d0+2 Le:0d0-2 Wi:0d0+1 Ma:0d0-3 Pe:0d0+0 Ch:0d0+0"
    },
    {
        "alchemist",
        "St:0d0-2 Dx:0d0-2 To:0d0-2 Le:0d0+3 Wi:0d0+1 Ma:0d0-2 Pe:0d0+2 Ch:0d0+0"
    },
    {
        "bard",
        "St:0d0+1 Dx:0d0+1 To:0d0 Le:0d0-2 Wi:0d0 Ma:0d0 Pe:0d0 Ch:0d0+0"
    },
};

CUSTOM_GEND cust_gender[] = {
    {
        "male",
        "St:0d0+1 Dx:0d0+0 To:0d0+0 Le:0d0+0 Wi:0d0+0 Ma:0d0+0 Pe:0d0+0 Ch:0d0+0"
    },
    {
        "female",
        "St:0d0+0 Dx:0d0+1 To:0d0+0 Le:0d0+0 Wi:0d0+0 Ma:0d0+0 Pe:0d0+0 Ch:0d0+0"
    },
};

void XHero::PlayerSetup()
{
    XStats * stmp;

    vClrScr();
    vGotoXY(7, 4);
    vPutS(MSG_LIGHTGRAY "Choose a race:");

    char buf[256];
    int i;

    for (i = 0; i < 7; i++) {
        vGotoXY(7, 6 + i);
        sprintf(buf, MSG_LIGHTGRAY "[" MSG_YELLOW "%c" MSG_LIGHTGRAY "] %s ", i + 97, cust_race[i].name);
        vPutS(buf);
    }

    vRefresh();

    int race_choice = ' ';

    while (true) {
#ifndef __CHOOSE_RACE
        int ch = vGetch();
#else
        int ch = 'c';
#endif
        race_choice = ch;

        if (ch >= 97 && ch < 97 + 7) {
            XDice d(cust_race[ch - 97].speed);
            ttmb = d.Throw();
            ttm = ttmb;

            s = new XStats(cust_race[ch - 97].stats);
            max_stats.Set(cust_race[ch - 97].max_stats);
            food_feeling = cust_race[ch - 97].ff;

            race = ch - 97;
            vClrScr();
            break;
        }
    }

    switch (race_choice) {
        case 'a':
            sk->Learn(XSkill::Skill::COOKING);
            sk->Learn(XSkill::Skill::BACKSTABBING);
            break;

        case 'b':
            break;

        case 'c':
            sk->Learn(XSkill::Skill::COOKING);
            break;

        case 'd':
            break;

        case 'e':
            sk->Learn(XSkill::Skill::ATHLETICS);
            break;

        case 'f':
            sk->Learn(XSkill::Skill::MINING);

        case 'g':
            sk->Learn(XSkill::Skill::MINING);
            sk->Learn(XSkill::Skill::FINDWEAKNESS);
            break;

        default:
            assert(0);
    }

    sk->Learn(XSkill::Skill::FIRST_AID);

    vClrScr();
    vGotoXY(7, 4);
    vPutS(MSG_LIGHTGRAY "Choose a gender:");

    for (i = 0; i < 2; i++) {
        vGotoXY(7, 6 + i);
        sprintf(buf, MSG_LIGHTGRAY "[" MSG_YELLOW "%c" MSG_LIGHTGRAY "] %s ", i + 97, cust_gender[i].name);
        vPutS(buf);
    }

    vRefresh();

    while (true) {
#ifndef __CHOOSE_RACE
        int ch = vGetch();
#else
        int ch = 'a';
#endif
        if (ch >= 97 && ch < 97 + 2) {
            stmp = new XStats(cust_gender[ch - 97].stats);
            s->Add(stmp);
            delete stmp;

            if (ch == 'a') {
                creature_person_type = CPT_MALE_YOU;
            } else if (ch == 'b') {
                creature_person_type = CPT_FEMALE_YOU;
            }

            vClrScr();
            break;
        }
    }

    vClrScr();
    vGotoXY(7, 4);
    vPutS(MSG_LIGHTGRAY "Choose a profession:");

    for (i = 0; i < 8; i++) {
        vGotoXY(7, 6 + i);
        sprintf(buf, MSG_LIGHTGRAY "[" MSG_YELLOW "%c" MSG_LIGHTGRAY "] %s ", i + 97, cust_profession[i].name);
        vPutS(buf);
    }

    vRefresh();

    while (true) {
#ifndef __CHOOSE_RACE
        int ch = vGetch();
#else
        char ch = 'a';
#endif

        if (ch >= 97 && ch < 97 + 8) {
            stmp = new XStats(cust_profession[ch - 97].stats);
            s->Add(stmp);
            delete stmp;

            for (int ii = S_STR; ii < S_EOF; ii++) {
                if (s->Get(static_cast<STATS>(ii)) < 1) {
                    s->SetStat(static_cast<STATS>(ii), 1);
                }
            }

            vClrScr();

            profession = ch - 97;

            XBodyPart * pbp;
            XPotion * potion;
            XBook * book;
            XScroll * scroll;

            switch (ch) {
                //************** warrior
                case 'a' :
                    switch (race_choice) {
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

                            sk->Learn(XSkill::Skill::DISARMTRAP);
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

                        default:
                            assert(0);
                    }

                    potion = new XPotion(PN_CURE_LIGHT_WOUNDS);
                    potion->Identify(1);
                    ContainItem(potion);

                    potion = new XPotion(PN_HEROISM);
                    potion->Identify(1);
                    ContainItem(potion);

                    sk->Learn(XSkill::Skill::FINDWEAKNESS);
                    sk->Learn(XSkill::Skill::HEALING);
                    sk->Learn(XSkill::Skill::TACTICS);
                    sk->Learn(XSkill::Skill::ATHLETICS);

                    break;

                //************** wizard
                case 'b' : {
                    pbp = GetBodyPart(BP_HAND, 0);
                    pbp->Wear(ICREATEB(IM_WEAPON, IT_STAFF, 1, 100));

                    potion = new XPotion(PN_POWER);
                    potion->Identify(1);
                    ContainItem(potion);

                    for (int t = 1; t < 3; t++) {
                        if (vRand() % 2 == 1) {
                            scroll = new XScroll(SCROLL_FIRE_BOLT);
                        } else {
                            scroll = new XScroll(SCROLL_ICE_BOLT);
                        }

                        scroll->Identify(1);
                        ContainItem(scroll);
                    }

                    if (vRand() % 2 == 1) {
                        book = new XBook(BOOK_FIRE_BOLT);
                    } else {
                        book = new XBook(BOOK_ICE_BOLT);
                    }

                    book->Identify(1);
                    ContainItem(book);

                    book = dynamic_cast<XBook *>(ICREATEA(IM_BOOK));
                    book->Identify(1);
                    ContainItem(book);

                    sk->Learn(XSkill::Skill::HEALING);
                    sk->Learn(XSkill::Skill::LITERACY);
                    sk->Learn(XSkill::Skill::CONCENTRATION);
                    sk->Learn(XSkill::Skill::HERBALISM);
                }
                break;

                //*************** archer
                case 'c' :
                    switch (race_choice) {
                        case 'a':
                            pbp = GetBodyPart(BP_MISSILE_WEAPON, 0);
                            pbp->Wear(ICREATEB(IM_MISSILEW, IT_CROSSBOW, 1, 100));

                            pbp = GetBodyPart(BP_MISSILE, 0);
                            pbp->Wear(ICREATEB(IM_MISSILE, IT_QUARREL, 1, 100));
                            pbp->Item()->quantity = vRand() % 10 + 10;

                            pbp = GetBodyPart(BP_HAND, 0);
                            pbp->Wear(ICREATEB(IM_WEAPON, IT_DAGGER, 1, 100));
                            break;

                        case 'b':
                            pbp = GetBodyPart(BP_MISSILE_WEAPON, 0);
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
                            pbp = GetBodyPart(BP_MISSILE_WEAPON, 0);
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
                            pbp = GetBodyPart(BP_MISSILE_WEAPON, 0);
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
                            pbp = GetBodyPart(BP_MISSILE_WEAPON, 0);
                            pbp->Wear(ICREATEB(IM_MISSILEW, IT_HEAVYCROSSBOW, 1, 100));

                            pbp = GetBodyPart(BP_MISSILE, 0);
                            pbp->Wear(ICREATEB(IM_MISSILE, IT_QUARREL, 1, 100));
                            pbp->Item()->quantity = vRand() % 10 + 10;

                            pbp = GetBodyPart(BP_HAND, 0);
                            pbp->Wear(ICREATEB(IM_WEAPON, IT_ORCISHDAGGER, 1, 100));
                            break;

                        case 'f':
                            pbp = GetBodyPart(BP_MISSILE_WEAPON, 0);
                            pbp->Wear(ICREATEB(IM_MISSILEW, IT_HEAVYCROSSBOW, 1, 100));

                            pbp = GetBodyPart(BP_MISSILE, 0);
                            pbp->Wear(ICREATEB(IM_MISSILE, IT_QUARREL, 1, 100));
                            pbp->Item()->quantity = vRand() % 10 + 10;

                            pbp = GetBodyPart(BP_HAND, 0);
                            pbp->Wear(ICREATEB(IM_WEAPON, IT_WARAXE, 1, 100));
                            break;

                        case 'g':
                            pbp = GetBodyPart(BP_MISSILE_WEAPON, 0);
                            pbp->Wear(ICREATEB(IM_MISSILEW, IT_LIGHTCROSSBOW, 1, 100));

                            pbp = GetBodyPart(BP_MISSILE, 0);
                            pbp->Wear(ICREATEB(IM_MISSILE, IT_QUARREL, 1, 100));
                            pbp->Item()->quantity = vRand() % 10 + 10;

                            pbp = GetBodyPart(BP_HAND, 0);
                            pbp->Wear(ICREATEB(IM_WEAPON, IT_WARAXE, 1, 100));
                            break;

                        default:
                            assert(0);
                    }

                    potion = new XPotion(PN_CURE_LIGHT_WOUNDS);
                    potion->Identify(1);
                    ContainItem(potion);

                    sk->Learn(XSkill::Skill::FINDWEAKNESS);
                    sk->Learn(XSkill::Skill::HEALING);
                    sk->Learn(XSkill::Skill::ARCHERY);
                    sk->Learn(XSkill::Skill::ATHLETICS);

                    break;

                //*************** ranger
                case 'd' :
                    switch (race_choice) {
                        case 'a':
                            pbp = GetBodyPart(BP_MISSILE_WEAPON, 0);
                            pbp->Wear(ICREATEB(IM_MISSILEW, IT_CROSSBOW, 1, 100));

                            pbp = GetBodyPart(BP_MISSILE, 0);
                            pbp->Wear(ICREATEB(IM_MISSILE, IT_QUARREL, 1, 100));
                            pbp->Item()->quantity = vRand() % 10 + 10;

                            pbp = GetBodyPart(BP_HAND, 0);
                            pbp->Wear(ICREATEB(IM_WEAPON, IT_DAGGER, 1, 100));
                            break;

                        case 'b':
                            pbp = GetBodyPart(BP_MISSILE_WEAPON, 0);
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
                            pbp = GetBodyPart(BP_MISSILE_WEAPON, 0);
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
                            pbp = GetBodyPart(BP_MISSILE_WEAPON, 0);
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
                            pbp = GetBodyPart(BP_MISSILE_WEAPON, 0);
                            pbp->Wear(ICREATEB(IM_MISSILEW, IT_HEAVYCROSSBOW, 1, 100));

                            pbp = GetBodyPart(BP_MISSILE, 0);
                            pbp->Wear(ICREATEB(IM_MISSILE, IT_QUARREL, 1, 100));
                            pbp->Item()->quantity = vRand() % 10 + 10;

                            pbp = GetBodyPart(BP_HAND, 0);
                            pbp->Wear(ICREATEB(IM_WEAPON, IT_ORCISHDAGGER, 1, 100));
                            break;

                        case 'f':
                            pbp = GetBodyPart(BP_MISSILE_WEAPON, 0);
                            pbp->Wear(ICREATEB(IM_MISSILEW, IT_HEAVYCROSSBOW, 1, 100));

                            pbp = GetBodyPart(BP_MISSILE, 0);
                            pbp->Wear(ICREATEB(IM_MISSILE, IT_QUARREL, 1, 100));
                            pbp->Item()->quantity = vRand() % 10 + 10;

                            pbp = GetBodyPart(BP_HAND, 0);
                            pbp->Wear(ICREATEB(IM_WEAPON, IT_WARAXE, 1, 100));
                            break;

                        case 'g':
                            pbp = GetBodyPart(BP_MISSILE_WEAPON, 0);
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

                    sk->Learn(XSkill::Skill::FINDWEAKNESS);
                    sk->Learn(XSkill::Skill::ARCHERY);
                    sk->Learn(XSkill::Skill::CONCENTRATION);
                    sk->Learn(XSkill::Skill::LITERACY);
                    sk->Learn(XSkill::Skill::COOKING);
                    sk->Learn(XSkill::Skill::ATHLETICS);

                    break;


                //*************** cleric
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

                    sk->Learn(XSkill::Skill::HEALING);
                    sk->Learn(XSkill::Skill::LITERACY);
                    sk->Learn(XSkill::Skill::HERBALISM);
                    sk->Learn(XSkill::Skill::RELIGION);

                    switch (race_choice) {
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

                        default:
                            assert(0);
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

                    sk->Learn(XSkill::Skill::HEALING);
                    sk->Learn(XSkill::Skill::LITERACY);
                    sk->Learn(XSkill::Skill::RELIGION);
                    sk->Learn(XSkill::Skill::ATHLETICS);

                    switch (race_choice) {
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

                        default:
                            assert(0);
                    }

                    break;

                //*************** alchemist
                case 'g' :
                    pbp = GetBodyPart(BP_HAND, 0);
                    pbp->Wear(ICREATEB(IM_WEAPON, IT_DAGGER, 10, 150));

                    scroll = new XScroll(SCROLL_BLINK);
                    scroll->Identify(1);
                    ContainItem(scroll);

                    scroll = new XScroll(SCROLL_RECIPIE);
                    scroll->Identify(1);
                    ContainItem(scroll);

                    scroll = new XScroll(SCROLL_RECIPIE);
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

                    sk->Learn(XSkill::Skill::HEALING);
                    sk->Learn(XSkill::Skill::LITERACY);
                    sk->Learn(XSkill::Skill::HERBALISM);
                    sk->Learn(XSkill::Skill::ALCHEMY);

                    break;

                //*************** bard
                case 'h' : {
                    pbp = GetBodyPart(BP_HAND, 0);
                    pbp->Wear(ICREATEB(IM_WEAPON, IT_CLUB, 10, 150));

                    potion = new XPotion(PN_CURE_LIGHT_WOUNDS);
                    potion->Identify(1);
                    ContainItem(potion);

                    sk->Learn(XSkill::Skill::HEALING);
                    sk->Learn(XSkill::Skill::LITERACY);
                    sk->Learn(XSkill::Skill::HERBALISM);
                    sk->Learn(XSkill::Skill::ALCHEMY);

                }
                break;

                default:
                    assert(0);
            }

            break;
        }
    }

    XBodyPart * bp = GetBodyPart(BP_HAND, 0);

    if (bp->Item() && (bp->Item()->im & IM_WEAPON)) {
        wsk->SetLevel(bp->Item()->wt, 2);
    }

    sk->Learn(XSkill::Skill::DETECTTRAP);
    sk->Learn(XSkill::Skill::CREATETRAP);
    sk->Learn(XSkill::Skill::CLIMBING);

    bp = GetBodyPart(BP_BODY, 0);

    if (vRand(2) == 0) {
        bp->Wear(ICREATEB(IM_BODY, IT_CLOTHES, 1, 100));
    } else {
        bp->Wear(ICREATEB(IM_BODY, IT_ROBE, 1, 100));
    }

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

const char* XHero::GetRaceStr() const
{
    return cust_race[race].name;
}

const char* XHero::GetProfessionStr() const
{
    return cust_profession[profession].name;
}
