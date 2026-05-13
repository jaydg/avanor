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

#include <algorithm>
#include <cctype>
#include <cmath>
#include <fmt/format.h>

#include "creature/skeep_ai.h"
#include "creature/xhero.h"
#include "engine/xapi.h"
#include "engine/xarchive.h"
#include "game/game.h"
#include "game/quest.h"
#include "game/xtime.h"
#include "helpers/hiscore.h"
#include "helpers/msgwin.h"
#include "helpers/manual.h"
#include "item/item_misc.h"
#include "magic/modifier.h"
#include "map/map_objects.h"

REGISTER_CLASS(XHero);
int _exit_flag = 0;

static std::vector<MELEE_ATTACK> hero_melee;

XHero::XHero(int flag)
{
    melee_attack = &hero_melee;

    creature_person_type = CPT_YOU;
    turn_count = 0;
    last_cast = nullptr;

    x = 5;
    y = 5;
    view = '@';
    color = xWHITE;
    name = "-=RET=-";
    RNG = 5;
    target = nullptr;
    im = IM_HERO;

    auto *d = new XDice("1d3");
    _DV = d->Throw();
    delete d;

    d = new XDice("1d2-1");
    _PV = d->Throw();
    delete d;

    d = new XDice("1d4");
    _HIT = d->Throw();
    delete d;

    dice.Setup("1d2");

    std::string hero_body = "head neck body cloak hand hand ring ring gloves "
                            "boots light_source tool missile_weapon missile";
    XBodyPart::Create(this, hero_body);

    PlayerSetup();

    MAX_HP = stats->Get(S_TOU) / 2 + 3 + (XGame::isGodMode ? 1000 : 0);
    _HP = GetMaxHP();

    MAX_PP = stats->Get(S_MAN) / 2 + 1 + (XGame::isGodMode ? 1000 : 0);
    _PP = GetMaxPP();

    base_exp = static_cast<int>(GetCreatureStrength() * 0.6);

    resistances = std::make_unique<XResistance>();

    isDisturb = 0;
    last_char = '5';
    run_way_count = 0;
    target = nullptr;

    creature_class = CR_HUMAN;

    base_nutrio = 1000;
    nutrio = 10000;
    nutrio_speed = 5;

    XItem * it = new XMoney(15 + vRand(10));
    ContainItem(it);

    it = new XRation(IT_RATION);
    ContainItem(it);
}

void XHero::NewMove()
{
    int moved = 0;
    int w_pressed = 0;

    if (im & IM_HERO) {
        for (int i = -1; i < 2; i++)
            for (int j = -1; j < 2; j++) {
                XMapObject* obj = l->map->GetSpecial(x + i, y + j);

                if (obj && obj->im == IM_TRAP) {
                    if (dynamic_cast<XTrap *>(obj)->Check(this)) {
                        if (isDisturb > 0) {
                            isDisturb = 0;
                        }
                    }
                }
            }
    }

    while (!moved) {
        const int paralyse = md->Get(MOD_PARALYSE);
        moved = 1;

        l->map->Center(x, y);
        l->map->Put(this);
        PutStatus();
        vRefresh();

        if (paralyse) {
            moved = 1;
            continue;
        }

        nx = x;
        ny = y;

        int ch;

        if (--isDisturb <= 0) {
            vXGotoXY(x + SCR_X - l->map->wx, y + SCR_Y - l->map->wy);
            ch = vGetch();
            vHideCursor();
        } else {
            ch = last_char;
        }

        msgwin.ClrMsg();

        int dx = 0;
        int dy = 0;

        if (vCheckForCursorKey(ch, &dx, &dy)) {
            if (GetCarryState() == CSTATE_OVERBURDEN) {
                msgwin.Add("You can't while overburdened.");
                moved = 0;
                continue;
            }

            nx = x + dx;
            ny = y + dy;
        } else
            switch (ch) {
                case 'w' :
                    if (md->Get(MOD_POISON)) {
                        msgwin.Add("You can't while poisoned.");
                        moved = 0;
                        continue;
                    }

                    if (GetCarryState() == CSTATE_OVERBURDEN) {
                        msgwin.Add("You can't while overburdened.");
                        moved = 0;
                        continue;
                    }

                    isDisturb = 30;
                    last_char = vGetch();

                    if (!vCheckForCursorKey(last_char, nullptr, nullptr)) {
                        isDisturb = 0;
                    }

                    w_pressed = 1;

                    moved = 0;
                    continue;

                case 'Q' :
                    msgwin.Add("Are you sure you want to QUIT the game ");

                    if (GetTarget(TR_YES_NO)) {
                        _exit_flag = 1;
                    } else {
                        moved = 0;
                    }

                    break;

                case '<' : {
                    XMapObject* spec = l->map->GetSpecial(x, y);

                    if (spec && spec->im & IM_WAY && ((XStairWay*)spec)->view == '<') {
                        for (int q = -10; q < 10; q++)
                            for (int w = -10; w < 10; w++) {
                                l->map->ResVisible(x + w, y + q);
                            }

                        MoveStairWay();
                    } else {
                        msgwin.Add("There is no stair up here.");
                        moved = 0;
                    }

                    break;
                }

                case '>' : {
                    XMapObject* spec = l->map->GetSpecial(x, y);

                    if (spec && spec->im & IM_WAY && ((XStairWay*)spec)->view == '>') {
                        for (int q = -10; q < 10; q++)
                            for (int w = -10; w < 10; w++) {
                                l->map->ResVisible(x + w, y + q);
                            }

                        MoveStairWay();
                    } else {
                        msgwin.Add("There is no stair down here.");
                        moved = 0;
                    }

                    break;
                }

                case '~' :
                    isDisturb = 500;
                    last_char = '5';
                    w_pressed = 1;
                    break;

                case 'i' :
                    moved = 0;
                    Inventory(&contain, IM_ALL, IF_VIEW_ONLY);
                    break;

                case 'e' :
                    moved = 0;
                    Equipment();
                    break;

                case 'x' :
                    moved = 0;
                    ExpList();
                    break;

                case '@' :
                    moved = 0;
                    InfoList();
                    break;

                case '[' :
                    moved = 0;
                    CreateScreenShot();
                    break;

                case 'D' :
                    DrinkPotion();
                    break;

                case ',' :
                    PickItem();
                    break;

                case 'd' :
                    DropItem();
                    break;

                case 'o' :
                    OpenDoor();
                    break;

                case '_' :
                case 's' :
                    doSacrifice();
                    break;

                case 'O' :
                    OpenChest();
                    break;

                case 'c' :
                    CloseDoor();
                    break;

                case 'l' :
                    LookAt();
                    moved = 0;
                    break;

                case 'E' :
                    Eat();
                    break;

                case 't' :
                    moved = XShoot();
                    break;

                case 'Z' :
                    moved = XCast();
                    break;

                case KEY_CTRL_Z:
                    moved = RepeatCast();
                    break;

                case '#' :
                    MagicLevelList();
                    moved = 0;
                    break;

                case 'A' :
                    SkillsList(SKF_LIST_SKILL);
                    moved = 0;
                    break;

                case 'a' :
                    moved = UseSkill();
                    break;

                case 'W' :
                    WarSkillsList();
                    moved = 0;
                    break;

                case 'q' :
                    XQuest::quest.ShowQuests();
                    moved = 0;
                    break;

                case '?' :
                    XManual::Run();
                    moved = 0;
                    break;

                case 'r' :
                    ReadAll();
                    break;

                case 'R' :
                    ShowReception();
                    moved = 0;
                    break;

                case '!' :
                    MixPotions();
                    break;

                case 'C' :
                    ChatWithMonster();
                    break;

                case 'g' :
                    GiveItem();
                    break;

                case 'T' :
                    SetTactics();
                    moved = 0;
                    break;

                case 'S' :
                    SaveGame();
                    moved = 0;
                    break;

                case 'u' :
                    moved = UseTool();
                    break;

                case 'U' :
                    moved = UseOuterObject();
                    break;

                case 'P' :
                    QuickPay();
                    break;

                case 'p' :
                    Pray();
                    break;

                case 'M' :
                    msgwin.ShowHistory();
                    moved = 0;
                    break;

                case '0' :
                    l->map->ForceRecenter(x, y);
                    moved = 0;
                    break;

                case KEY_CTRL_T:
                    ActivateTrap();
                    break;

                case KEY_CTRL_O:
                    moved = OrderCompanion();
                    break;

                case KEY_CTRL_D:
                    moved = 0;
                    ExecuteScript();
                    break;

                default :
                    moved = 0;
                    break;
            }

        if (w_pressed) {
            run_way_count = PossibleWayCount(nx, ny);
            w_pressed = 0;
        } else if (isDisturb > 0 && run_way_count != PossibleWayCount(x, y)) {
            isDisturb = 0;
            moved = 0;
            continue;
        }

        if (isDisturb > 0 && moved && l->map->XGetMovability(nx, ny) == 1) {
            int ax = abs(nx - x);
            int ay = abs(ny - y);

            if (ax == 0 && ay == 0) {
                isDisturb = 0;
                break;
            }

            if (ax == 1 && ay == 0) {
                if (l->map->XGetMovability(x, y + 1) == 0 && l->map->XGetMovability(x, y - 1) == 1
                    && PossibleWayCount(x, y) == 2) {
                    last_char = '2';
                    continue;
                }

                if (l->map->XGetMovability(x, y + 1) == 1 && l->map->XGetMovability(x, y - 1) == 0
                    && PossibleWayCount(x, y) == 2) {
                    last_char = '8';
                    continue;
                }
            } else {
                if (ax == 0 && ay == 1) {
                    if (l->map->XGetMovability(x + 1, y) == 0 && l->map->XGetMovability(x - 1, y) == 1
                        && PossibleWayCount(x, y) == 2) {
                        last_char = '6';
                        continue;
                    }

                    if (l->map->XGetMovability(x + 1, y) == 1 && l->map->XGetMovability(x - 1, y) == 0
                        && PossibleWayCount(x, y) == 2) {
                        last_char = '4';
                        continue;
                    }
                } else {
                    isDisturb = 0;
                    continue;
                }
            }
        }
    }

    if (XQuest::quest.hero_die) {
        return;
    }

    if (l->map->XGetMovability(nx, ny) == 2 && (nx != x || ny != y)) {
        XCreature* cr = l->map->GetMonster(nx, ny);

        if (!cr->xai->isEnemy(this)) {
            msgwin.Add(fmt::format("Are you sure you want to attack {}", cr->name));

            if (!GetTarget(TR_YES_NO)) {
                if ((cr->xai->GetAIFlag() & AIF_NO_SWAP)) { //we can't swap with some guardians
                    nx = x;
                    ny = y;
                } else {
                    XLocation * tl = l;
                    cr->LastStep();
                    LastStep();
                    cr->x = x;
                    cr->y = y;
                    cr->FirstStep(x, y, tl);
                    FirstStep(nx, ny, tl);
                }
            }
        }

        isDisturb = 0;
        action_data.action = A_ATTACK;
    } else {
        XMapObject* spec = l->map->GetSpecial(nx, ny);

        if (spec && spec->im & IM_DOOR && !dynamic_cast<XDoor *>(spec)->isOpened) {
            OpenDoor();
            nx = x;
            ny = y;
        }
    }
}

int XHero::PossibleWayCount(const int px, const int py) const
{
    int res = 0;

    if (l->map->XGetMovability(px, py + 1) != 1) {
        res++;
    }

    if (l->map->XGetMovability(px, py - 1) != 1) {
        res++;
    }

    if (l->map->XGetMovability(px + 1, py) != 1) {
        res++;
    }

    if (l->map->XGetMovability(px - 1, py) != 1) {
        res++;
    }

    return res;
}

void XHero::Die(XCreature * killer)
{
    if (XGame::isGodMode) {
        // God mode entails a choice about whether I die.
        msgwin.Add("You died!!!  Continue game?");

        if (GetTarget(TR_NO_YES)) {
            // Don't want to die twice, since we are cheating it!
            _HP = GetMaxHP();
            md->Remove(MOD_WOUND, main_creature);
            md->Remove(MOD_POISON, main_creature);
            md->Remove(MOD_STUN, main_creature);
            md->Remove(MOD_DISEASE, main_creature);
            md->Remove(MOD_PARALYSE, main_creature);
            md->Remove(MOD_WEAK, main_creature);
            md->Remove(MOD_SLOWNESS, main_creature);
            return;
        }
    }

    msgwin.Add("You died!!!");
    _exit_flag = 1;
    PutStatus();
    l->map->Put(this);
    vRefresh();

    vGetch();
    std::string str;

    if (killer == this) {
        str = fmt::format("Killed himself at {}.", l->GetFullName());
    } else if (killer) {
        str = fmt::format("Killed by {} at {}.", killer->name, l->GetFullName());
    } else {
        str = fmt::format("Died at {}.", l->GetFullName());
    }

    XQuest::quest.hero_die = 1;

    EndGame(str.c_str());
    vClrScr();
    XCreature::Die(killer);
}

void XHero::Move()
{
    turn_count++;

    if constexpr (true) {
        if ((l->map->GetItemCount(nx, ny) > 0 || l->map->GetSpecial(nx, ny)) && (nx != x || ny != y)) {
            isDisturb = 0;
        }

        if (l->map->GetSpecial(nx, ny)) {
            XMapObject *spec = l->map->GetSpecial(nx, ny);
            msgwin.Add(fmt::format("There is {} here.", spec->GetName(this)));
        }

        if (l->map->GetItemCount(nx, ny) > 1) {
            msgwin.Add("There is a heap of items here.");
        } else if (l->map->GetItemCount(nx, ny) == 1) {
            XItemList* ilist = (l->map->GetItemList(nx, ny));
            XItem* item = *(ilist->begin());
            XAnyPlace* place = l->map->GetPlace(nx, ny);

            msgwin.Add(fmt::format("There is a {} here.",
                place ? place->onShowItem(item) : item->toString()));
        }
    }

    HideOldView();
    ShowNewView();

    if (action_data.action == A_EAT || action_data.action == A_READ
        || action_data.action == A_USE_TOOL) {
        l->map->Center(x, y);
        l->map->Put(this);
        PutStatus();
        vRefresh();
        msgwin.ClrMsg();
    }

    if (DecNutrio()) {
        XCreature::Move();
    }
}

void XHero::InfoList()
{
    V_BUFFER xbuf;
    vStore(&xbuf);
    vClrScr();

    vGotoXY(0, 0);
    vPutS(MSG_BROWN "--------- " MSG_LIGHTGRAY "Attributes " MSG_BROWN "---------");
    vGotoXY(0, 1);
    vPutS(MSG_LIGHTGRAY "Attribute  Base  Max  Add  Total");
    vGotoXY(0, 2);
    vPutS(MSG_BROWN "--------------------------------");
    int i = 0;

    for (i = 0; i < S_EOF; i++) {
        char stat[256];
        vGotoXY(0, i + 3);
        vSetAttr(xBROWN);
        vPutS(stats->GetFullName(static_cast<STATS>(i)));
        sprintf(stat, "%d", stats->Get(static_cast<STATS>(i)));
        vSetAttr(xLIGHTGRAY);
        vGotoXY(15 - strlen(stat), 3 + i);
        vPutS(stat);
        sprintf(stat, "%d", max_stats.Get(static_cast<STATS>(i)));
        vSetAttr(xBROWN);
        vGotoXY(20 - strlen(stat), 3 + i);
        vPutS(stat);

        int tres = added_stats.Get(static_cast<STATS>(i));

        if (tres != 0) {
            if (tres > 0) {
                sprintf(stat, "%+d", tres);
                vSetAttr(xLIGHTGREEN);
            } else {
                sprintf(stat, "%+d", tres);
                vSetAttr(xRED);
            }

            vGotoXY(26 - strlen(stat), 3 + i);
            vPutS(stat);
        }

        tres = stats->Get(static_cast<STATS>(i)) + added_stats.Get(static_cast<STATS>(i));

        if (tres <= 0) {
            tres = 1;
        }

        sprintf(stat, "%d", tres);
        vSetAttr(xYELLOW);
        vGotoXY(32 - strlen(stat), 3 + i);
        vPutS(stat);
    }

    vGotoXY(40, 0);
    vPutS(MSG_BROWN "--------- " MSG_LIGHTGRAY "Miscellaneous " MSG_BROWN "---------");
    vGotoXY(40, 1);
    vPutS(MSG_BROWN "Name:" MSG_YELLOW);
    vGotoXY(50, 1);
    vPutS(name);
    vGotoXY(40, 2);
    vPutS(MSG_BROWN "Gender:" MSG_YELLOW);
    vGotoXY(50, 2);
    vPutS(GetGenderStr());
    vGotoXY(40, 3);
    vPutS(MSG_BROWN "Race:" MSG_YELLOW);
    vGotoXY(50, 3);
    vPutS(GetRaceStr());
    vGotoXY(40, 4);
    vPutS(MSG_BROWN "Class:" MSG_YELLOW);
    vGotoXY(50, 4);
    vPutS(GetProfessionStr());
    vGotoXY(40, 5);
    vPutS(MSG_BROWN "Gold:");
    char tbuf[256];
    sprintf(tbuf, MSG_YELLOW "%d" MSG_BROWN " gp", MoneyOp(0));
    vGotoXY(50, 5);
    vPutS(tbuf);
    vGotoXY(40, 6);
    vPutS(MSG_BROWN "Time:");
    vGotoXY(50, 6);
    sprintf(tbuf, MSG_YELLOW "%d" MSG_BROWN " : " MSG_YELLOW "%d" MSG_BROWN " : "
        MSG_YELLOW "%d", XTime::GetHour(), XTime::GetMin(), XTime::GetSec());
    vPutS(tbuf);
    vGotoXY(40, 7);
    vPutS(MSG_BROWN "Date:");
    vGotoXY(50, 7);
    sprintf(tbuf, MSG_YELLOW "%s" MSG_BROWN ", " MSG_YELLOW "%d" MSG_BROWN ", "
        MSG_YELLOW "%d", XTime::GetMonthName(), XTime::GetDay(), XTime::GetYear());
    vPutS(tbuf);
    vGotoXY(40, 8);
    vPutS(MSG_BROWN "Day/Week");
    vGotoXY(50, 8);
    sprintf(tbuf, MSG_YELLOW "%s" MSG_BROWN "/" MSG_YELLOW "%s",
        XTime::GetDayName(), XTime::GetWeekName());
    vPutS(tbuf);

    vSetAttr(xBROWN);

    for (i = 0; i < 20; i++) {
        vGotoXY(i * 4, 12);
        vPutS("----");
        vGotoXY(i * 4, 14);
        vPutS("----");
        vGotoXY(i * 4, 20);
        vPutS("----");
    }

    vGotoXY(0, 13);
    sprintf(tbuf, MSG_LIGHTGRAY "Burden" MSG_BROWN ": current " MSG_YELLOW "%d" MSG_BROWN
        ", burdened " MSG_YELLOW "%d" MSG_BROWN ", strained "
        MSG_YELLOW "%d" MSG_BROWN ", overburdened " MSG_YELLOW "%d",
        carried_weight, CarryValue(CSTATE_NORMAL), CarryValue(CSTATE_BURDENED), CarryValue(CSTATE_STRAINED));
    vPutS(tbuf);

    vGotoXY(0, 15);
    vPutS(MSG_LIGHTGRAY "Melee Attack");

    sprintf(tbuf, MSG_BROWN "Unarmed:    (" MSG_YELLOW "%+d" MSG_BROWN ", "
        MSG_YELLOW"%d" MSG_BROWN "d" MSG_YELLOW "%d %+d" MSG_BROWN ")",
        GetHIT() + wsk->GetHIT(XWarSkills::UNARMED),
        dice.X, dice.Y, dice.Z + GetDMG() + wsk->GetDMG(XWarSkills::UNARMED));
    vGotoXY(0, 16);
    vPutS(tbuf);

    const XBodyPart* hand_1 = GetBodyPart(BP_HAND, 0);
    const XBodyPart* hand_2 = GetBodyPart(BP_HAND, 1);

    int free_hand = (hand_1->Item() == nullptr) | (hand_2->Item() == nullptr);

    if (hand_1->Item() && hand_1->Item()->im & IM_WEAPON) {
        sprintf(tbuf, "Left hand:  (" MSG_YELLOW "%+d" MSG_BROWN ", "
            MSG_YELLOW"%d" MSG_BROWN "d" MSG_YELLOW "%d %+d" MSG_BROWN ")",
            GetHIT() + wsk->GetHIT(hand_1->Item()->wt) + GetHITFHBonus(hand_1->Item()),
            hand_1->Item()->dice.X, hand_1->Item()->dice.Y,
            hand_1->Item()->dice.Z + GetDMG() + wsk->GetDMG(hand_1->Item()->wt));
        vGotoXY(0, 17);
        vPutS(tbuf);
    }

    if (hand_2->Item() && hand_2->Item()->im & IM_WEAPON) {
        sprintf(tbuf, "Right hand: (" MSG_YELLOW "%+d" MSG_BROWN ", "
            MSG_YELLOW"%d" MSG_BROWN "d" MSG_YELLOW "%d %+d" MSG_BROWN ")",
            GetHIT() + wsk->GetHIT(hand_2->Item()->wt) + GetHITFHBonus(hand_2->Item()),
            hand_2->Item()->dice.X, hand_2->Item()->dice.Y,
            hand_2->Item()->dice.Z + GetDMG() + wsk->GetDMG(hand_2->Item()->wt));
        vGotoXY(0, 18);
        vPutS(tbuf);
    }

    int hit;
    int range;
    XDice dmg;
    GetRangeAttackInfo(&range, &hit, &dmg);

    if (range > 0) {
        sprintf(tbuf, "Range Attack: <" MSG_YELLOW "%d" MSG_BROWN "> ("
            MSG_YELLOW "%+d" MSG_BROWN ", " MSG_YELLOW "%d" MSG_BROWN "d"
            MSG_YELLOW "%d %+d" MSG_BROWN ")", range, hit, dmg.X, dmg.Y, dmg.Z);
        vGotoXY(0, 19);
        vPutS(tbuf);
    }

    vGotoXY(0, size_y - 1);
    vPutS("Press any key to exit.");
    vRefresh();
    vGetch();
    vRestore(&xbuf);
}


void XHero::ExpList() const
{
    V_BUFFER xbuf;
    vStore(&xbuf);
    vClrScr();

    char bufx[250];

    if (level >= 50) {
        vGotoXY(0, 0);
        vPutS("\x1F\0FYou need unknown points of experience!");
    } else {
        int i = level;
        int xx = 0;
        int yy = 0;

        while (i < 50) {
            sprintf(bufx, "\x1F\x06Level(\x1F\x0E%d\x1F\x06): \x1F\x0E%lu", i + 1, ExpOfLevel(i));
            vGotoXY(xx, yy);
            vPutS(bufx);
            yy++;

            if (yy > 17) {
                xx += 26;
                yy = 0;
            }

            i++;
        }

        vGotoXY(0, 20);
        sprintf(bufx, "\x1F\x06You need \x1F\x0E%lu\x1F\x06 experience to next level.", ExpOfLevel(level) - _EXP);
        vPutS(bufx);
    }

    vGotoXY(0, 22);
    vPutS("Press any key to exit.");
    vRefresh();
    vGetch();
    vRestore(&xbuf);
}


auto empty = "                                                                 ";
auto smask = "[|{}'=!?\"\\%]]$X";
ITEM_MASK imask[] = {
    static_cast<ITEM_MASK>(IM_HAT | IM_BODY | IM_BOOTS | IM_GLOVES | IM_CLOAK | IM_SHIELD),
    IM_WEAPON, IM_MISSILEW, IM_MISSILE,
    IM_NECK, IM_RING, IM_POTION, IM_SCROLL, IM_BOOK,
    IM_WAND, IM_FOOD, IM_LIGHTSOURCE,
    IM_TOOL, IM_MONEY, IM_ALL
};

ITEM_MASK output_items_mask[] = {
    IM_HAT, IM_BODY, IM_CLOAK, IM_GLOVES, IM_BOOTS, IM_SHIELD,
    IM_WEAPON, IM_NECK, IM_RING, IM_MISSILEW, IM_MISSILE, IM_POTION,
    IM_SCROLL, IM_BOOK, IM_WAND, IM_FOOD, IM_HERB, IM_LIGHTSOURCE, IM_TOOL, IM_MONEY
};

auto output_items_ext = "[[[[[[|'={}!?\"\\%%]]$";
const char* output_items_name[] = {
    "Helmets", "Armours", "Cloaks", "Gloves", "Boots", "Shields",
    "Weapon", "Necklaces", "Rings", "Missile weapon", "Missiles", "Potions",
    "Scrolls", "Books", "Wands", "Food", "Herbs", "Light sources", "Tools", "Money"
};


// first_item must be 0 if need to start from first item
static int first_item = 0;
static XItemList* pLastList = nullptr;

XItem* XHero::Inventory(XItemList* item_list, ITEM_MASK mask, const INVENTORY_FLAG flag, const int ret_item_count,
    XItemFilter* ifiltr, const std::optional<std::reference_wrapper<std::ofstream>> file) const
{
    while (true) {
        XGuiList list;

        if (&contain == item_list) {
            list.SetCaption(MSG_BROWN "###" MSG_LIGHTGRAY " Inventory " MSG_BROWN "###");
        } else {
            list.SetCaption(MSG_BROWN "###" MSG_LIGHTGRAY " Items " MSG_BROWN "###");
        }

        list.SetFooter(MSG_LIGHTGRAY "filtr: " MSG_BROWN "[" MSG_YELLOW "[|{}'=!?\"\\%]$X" MSG_BROWN "]");

        //count items for show
        int all_item_count = 0;

        for (const auto it : *item_list) {
            if ((ifiltr && ifiltr(it)) || it->im & mask) {
                all_item_count++;
            }
        }

        char buf[256];

        if (all_item_count == 0) {
            if ((mask == IM_ALL) || (mask == IM_UNKNOWN))
                if (&contain == item_list) {
                    list.AddItem(new XGuiItem_Text(MSG_LIGHTGRAY "You have no such items."), 0);
                } else {
                    list.AddItem(new XGuiItem_Text(MSG_LIGHTGRAY "There are no such items."), 0);
                } else {
                for (int oi = 0; oi < std::size(output_items_name); oi++) {
                    if (output_items_mask[oi] & mask) {
                        if (&contain == item_list) {
                            sprintf(buf, MSG_LIGHTGRAY "You have no %s.", output_items_name[oi]);
                        } else {
                            sprintf(buf, MSG_LIGHTGRAY "There are no %s.", output_items_name[oi]);
                        }

                        list.AddItem(new XGuiItem_Text(buf), 0);
                    }
                }
            }
        } else {
            ITEM_MASK last_mask = IM_UNKNOWN;

            for (const auto item: *item_list) {
                if ((ifiltr && ifiltr(item)) || (item->im & mask)) {
                    // we need to show item group name (e.g. boots, weapons etc.)
                    if (item->im != last_mask) {
                        // skip output empty string for first item in inventory
                        if (last_mask != IM_UNKNOWN) {
                            list.AddItem(new XGuiItem_Text(""), 0);
                        }

                        last_mask = item->im;

                        for (int oi = 0; oi < std::size(output_items_name); oi++) {
                            if (output_items_mask[oi] & last_mask) {
                                sprintf(buf, MSG_YELLOW "%s " MSG_BROWN "('" MSG_YELLOW "%c" MSG_BROWN "')", output_items_name[oi], output_items_ext[oi]);
                                list.AddItem(new XGuiItem_Text(buf), 0);
                            }
                        }
                    }

                    //output item
                    list.AddItem(new XGuiItem_Inventory(item), 0);
                }
            }
        }

        //output list
        if (pLastList != item_list) {
            pLastList = item_list;
            first_item = 0;
        }

        if (file) {
            list.Put(file);
            return nullptr;
        }

        int item_number = list.Run(1, first_item);
        first_item = list.GetTopItemIndex();

        if (item_number == -1 || (flag & IF_VIEW_ONLY)) { //there was no item selected
            const int ch = list.GetLastKey();

            if (!(flag & IF_FIXED_MASK)) {
                for (unsigned int i = 0; i < strlen(smask); i++)
                    if (ch == smask[i]) {
                        mask = imask[i];
                    }
            }

            if (ch == 0 || ch == KEY_ESC || ch == 'z' || ch == 'v' || ch == 'V' || ch == 'Z' || ch == ' ') {
                break;
            }
        } else {
            auto selected_it = item_list->begin();
            int stop_flag = -1;

            while (true) {
                if ((ifiltr && ifiltr(*selected_it)) || ((*selected_it)->im & mask)) {
                    stop_flag++;
                }

                if (stop_flag == item_number) {
                    break;
                }

                ++selected_it;
            }

            assert(stop_flag == item_number);

            XItem* ritem = *selected_it;
            item_list->erase(selected_it);

            if (ret_item_count <= 0) {
                return ritem;
            }

            if (ritem->quantity <= ret_item_count) {
                return ritem;
            }

            auto sitem = dynamic_cast<XItem *>(ritem->MakeCopy());
            sitem->quantity = ret_item_count;
            ritem->quantity -= ret_item_count;
            item_list->insert(ritem);

            return sitem;

        }
    }

    return nullptr;
}


const char* part_names[] = {"",
        "Head", "Necklace", "Body", "Cloak",
        "Left hand", "Left ring", "Gloves",
        "Boots", "Light source", "Tool", "Missile weapon", "Missile", "eof"
    };

void XHero::Equipment(const std::optional<std::reference_wrapper<std::ofstream>> file)
{
    XBodyPart* xqsa[30]; //this array save us from typing hard algorithm
    first_item = 0;

    while (true) {
        int was_hand = 0;
        int was_ring = 0;
        int counter = 0;

        XGuiList list;

        list.SetCaption(MSG_BROWN "###" MSG_LIGHTGRAY " Equipment " MSG_BROWN "###");
        list.SetFooter(MSG_BROWN "[" MSG_YELLOW "V" MSG_BROWN "]" MSG_LIGHTGRAY " - show inventory.");

        for (auto xbp: components) {
            xqsa[counter] = xbp;

            // Determine name of body part
            std::string_view part_name;
            if (xbp->Fit(BP_HAND) || xbp->Fit(BP_RING)) {
                if (xbp->Fit(BP_HAND)) {
                    part_name = (was_hand == 0) ? part_names[xbp->bp_uin] : "Right hand";
                    was_hand++;
                }
                if (xbp->Fit(BP_RING)) {
                    part_name = (was_ring == 0) ? part_names[xbp->bp_uin] : "Right ring";
                    was_ring++;
                }
            } else {
                part_name = part_names[xbp->bp_uin];
            }

            // Left column
            std::string left = fmt::format(MSG_LIGHTGRAY "{}", part_name);
            const auto visible = static_cast<size_t>(x_strlen(left.c_str()));
            if (visible < 20)
                left.append(20 - visible, ' ');

            // Right column
            std::string right;
            if (xbp->Item()) {
                right = fmt::format(MSG_BROWN ": " MSG_LIGHTGRAY "{}",
                                    xbp->Item()->toString());
            } else {
                right = MSG_BROWN ": " MSG_BROWN "-";
            }

            const std::string buf = left + right;
            list.AddItem(new XGuiItem_SimpleSelect(buf.c_str()));
            counter++;
            ++xbp;
        }

        if (file) {
            list.Put(file);
            return;
        }

        if (int ch = list.Run(1); ch == -1) {
            ch = list.GetLastKey();

            if (ch == 0 || ch == KEY_ESC || ch == ' ' || ch == 'z' || ch == 'Z') {
                break;
            }

            if (ch == 'v' || ch == 'V') {
                Inventory(&contain, IM_ALL, IF_VIEW_ONLY);
            }
        } else {
            const int n = ch;
            XItem* witem = xqsa[n]->Item();

            if (witem != nullptr) {
                xqsa[n]->UnWear();
                contain.insert(witem);
            } else {
                if (xqsa[n]->GetProperIM() == IM_MISSILE) {
                    witem = Inventory(&contain, xqsa[n]->GetProperIM(), IF_FIXED_MASK);
                } else {
                    witem = Inventory(&contain, xqsa[n]->GetProperIM(), IF_FIXED_MASK, 1);
                }

                if (witem) {
                    if (xqsa[n]->bp_uin == BP_HAND) {
                        xqsa[n]->Wear(witem);
                    } else {
                        if (xqsa[n]->bp_uin == witem->bp) {
                            xqsa[n]->Wear(witem);
                        } else {
                            contain.insert(witem);
                        }
                    }
                }
            }
        }
    }
}

void XHero::Eat()
{
    if (nutrio > base_nutrio * 16) {
        msgwin.Add("You can't eat any more!");
    } else {
        first_item = 0;

        XItem* food = nullptr;
        XItemList* tmpquae = l->map->GetItemList(x, y);

        if (!tmpquae->empty()) {
            food = Inventory(tmpquae, IM_FOOD, IF_FIXED_MASK, 1);

            XAnyPlace* place = l->map->GetPlace(x, y);

            if (place && food && !place->onCreaturePickItem(this, food)) {
                vRefresh();
                return;
            }

        }

        if (!food) {
            food = Inventory(&contain, IM_FOOD, IF_FIXED_MASK, 1);
        }

        vRefresh();

        if (food) {
            XCreature::Eat(dynamic_cast<XAnyFood *>(food));
        }
    }
}

int XHero::stopAction()
{
    return XCreature::stopAction();
}

void XHero::ReadAll()
{
    first_item = 0;
    XItem* item = Inventory(&contain, static_cast<ITEM_MASK>(IM_BOOK | IM_SCROLL), IF_FIXED_MASK, 1);

    if (item) {
        if (item->im & IM_SCROLL) {
            if (!XCreature::Read(item)) {
                contain.insert(item);
            }
        } else if (item->im & IM_BOOK) {
            if (!XCreature::Read(item)) {
                contain.insert(item);
            }
        }
    }

    vRefresh();
}

void XHero::DrinkPotion()
{
    first_item = 0;
    auto pot = dynamic_cast<XPotion *>(Inventory(&contain, IM_POTION, IF_FIXED_MASK, 1));

    if (pot) {
        if (pot->onDrink(this)) {
            pot->Identify(1);
        }

        pot->UnCarry();
        pot->Invalidate();
    }
}

void XHero::DropItem()
{
    XItem * item;
    first_item = 0;

    while (contain.begin() != contain.end() && (item = Inventory(&contain))) {
        XItem* drop_item = item;

        if (item->quantity > 1) {
            msgwin.ClrMsg();
            XPoint pt(0, item->quantity);
            msgwin.Add("How much?");
            const int res = GetTarget(TR_HOW_MUCH, &pt, item->quantity);

            if (res == 0) {
                contain.insert(item);
                break;
            }

            if (res != item->quantity) {
                drop_item = dynamic_cast<XItem *>(item->MakeCopy());
                drop_item->quantity = res;
                item->quantity -= res;
                contain.insert(item);
            }
        }

        if (!XCreature::DropItem(drop_item)) {
            contain.insert(drop_item);
            return;
        }

        msgwin.Add(fmt::format("{] drops {}.", name, drop_item->toString()));
    }
}

void XHero::PickItem()
{
    char bufx[256];

    XItemList* tmpquae = l->map->GetItemList(x, y);

    if (tmpquae->empty()) {
        XMapObject* obj = l->map->GetSpecial(x, y);

        if (obj == nullptr || !obj->isValid() || obj->im != IM_OTHER) {
            msgwin.Add("There is nothing to pick up here.");
        } else {
            const auto tit = dynamic_cast<XItem *>(obj->Pick(this));

            auto desc = tit->toString();
            if (PickUpItem(tit)) {
                msgwin.Add(fmt::format("You pick a {}.", desc));
            } else {
                tit->Invalidate();
            }
        }
    } else if (tmpquae->size() == 1) {
        XItem* tit = *(tmpquae->begin());
        tmpquae->erase(tit);

        if (PickUpItem(tit)) {
            msgwin.Add(fmt::format("You pick up a {}.", tit->toString()));
        } else {
            tmpquae->insert(tit);
        }
    } else {
        XItem* tit;
        int nitem = 0;

        while (!tmpquae->empty() && (tit = Inventory(tmpquae))) {
            if (PickUpItem(tit)) {
                nitem++;
            } else {
                // we can't pick item, so return it back
                tmpquae->insert(tit);
                vRefresh();
                vGetch();
            }
        }

        if (nitem == 1) {
            msgwin.Add(fmt::format("You pick up a {}.", tit->toString()));
        } else if (nitem > 1) {
            msgwin.Add("You pick up a heap of items.");
        }
    }
}

void XHero::OpenChest()
{
    XItemList* tq = l->map->GetItemList(x, y);
    int chest_count = 0;
    XChest* last_chest = nullptr;

    for (const auto it : *tq) {
        if (it->im == IM_CHEST) {
            last_chest = dynamic_cast<XChest *>(it);
            chest_count++;
        }
    }

    if (chest_count > 1) {
        last_chest = dynamic_cast<XChest *>(Inventory(tq, IM_CHEST, IF_NONE, 1));

        if (!last_chest) {
            return;
        }

        XItem* it = nullptr;

        do {
            it = Inventory(&last_chest->contain);

            if (it) {
                last_chest->weight -= it->weight;

                if (!ContainItem(it)) {
                    it->Drop(l, x, y);
                }
            }
        } while (it);

        tq->insert(last_chest);
    } else if (chest_count == 1) {
        msgwin.Add("Do you wish to open the chest?");

        if (GetTarget(TR_NO_YES)) {
            XItem* it = nullptr;

            do {
                it = Inventory(&last_chest->contain);

                if (it) {
                    last_chest->weight -= it->weight;

                    if (!ContainItem(it)) {
                        it->Drop(l, x, y);
                    }
                }
            } while (it);

        }
    } else {
        msgwin.Add("There is no chest here.");
    }
}

void XHero::OpenDoor()
{
    XMapObject* spec = l->map->GetSpecial(x, y);

    if (spec && spec->im & IM_MISC) {
        spec->onOuterUse(this);
        return;
    }

    int o_door = 0;
    int c_door = 0;
    int cd_x = -1;
    int cd_y = -1;

    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            if (!(i == 0 && j == 0)) {
                XMapObject* door = l->map->GetSpecial(x + i, y + j);

                if (door && door->im & IM_DOOR && dynamic_cast<XDoor *>(door)->isOpened == 0) {
                    c_door++;
                    cd_x = x + i;
                    cd_y = y + j;
                } else if (door && door->im & IM_DOOR && dynamic_cast<XDoor *>(door)->isOpened == 1) {
                    o_door++;
                }
            }
        }
    }

    if (c_door == 0 && o_door == 0) {
        msgwin.Add("There is no door here.");
    }

    if (c_door == 0 && o_door == 1) {
        msgwin.Add("The door is already opened.");
    }

    if (c_door == 0 && o_door > 1) {
        msgwin.Add("The doors are already opened.");
    }

    if (c_door == 1) {
        msgwin.Add("You have opened the door.");
        XMapObject* door = l->map->GetSpecial(cd_x, cd_y);
        dynamic_cast<XDoor *>(door)->Switch();
        return;
    }

    if (c_door > 1) {
        XPoint pt;
        XMapObject* door;

        if (x == nx && y == ny) {
            if (!WhichDirection(&pt)) {
                return;
            }

            door = l->map->GetSpecial(x + pt.x, y + pt.y);
        } else {
            door = l->map->GetSpecial(nx, ny);
        }

        if (door && door->im & IM_DOOR && dynamic_cast<XDoor *>(door)->isOpened) {
            msgwin.Add("The door is already opened.");
        } else if (door && door->im & IM_DOOR && dynamic_cast<XDoor *>(door)->isOpened == 0) {
            msgwin.Add("You have opened the door.");
            dynamic_cast<XDoor *>(door)->Switch();
        } else {
            msgwin.Add("There is no door here.");
        }
    }
}

void XHero::CloseDoor()
{
    int o_door = 0;
    int c_door = 0;
    int od_x = -1;
    int od_y = -1;

    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            if (!(i == 0 && j == 0)) {
                XMapObject* spec = l->map->GetSpecial(x + i, y + j);

                if (spec && spec->im & IM_DOOR && dynamic_cast<XDoor *>(spec)->isOpened) {
                    o_door++;
                    od_x = x + i;
                    od_y = y + j;
                } else if (spec && spec->im & IM_DOOR && dynamic_cast<XDoor *>(spec)->isOpened == 0) {
                    c_door++;
                }
            }
        }
    }

    if (c_door == 0 && o_door == 0) {
        msgwin.Add("There is no door here.");
    }

    if (o_door == 0 && c_door == 1) {
        msgwin.Add("The door is already closed.");
    }

    if (o_door == 0 && c_door > 1) {
        msgwin.Add("The doors are already closed.");
    }

    if (o_door == 1) {
        msgwin.Add("You have closed the door.");
        XMapObject * spec = l->map->GetSpecial(od_x, od_y);
        LastStep();
        dynamic_cast<XDoor *>(spec)->Switch();
        FirstStep(x, y, l.get());
        return;
    }

    if (o_door > 1) {
        XPoint pt;

        if (!WhichDirection(&pt, 0)) {
            return;
        }

        XMapObject* spec = l->map->GetSpecial(x + pt.x, y + pt.y);

        if (spec && spec->im & IM_DOOR && dynamic_cast<XDoor *>(spec)->isOpened == 0) {
            msgwin.Add("The door is already closed.");
        } else if (spec && spec->im & IM_DOOR && dynamic_cast<XDoor *>(spec)->isOpened) {
            msgwin.Add("You have closed the door.");
            LastStep();
            dynamic_cast<XDoor *>(spec)->Switch();
            FirstStep(x, y, l.get());
        } else {
            msgwin.Add("There is no door here.");
        }
    }
}

int XHero::WhichDirection(XPoint* pt, const int flag)
{
    msgwin.Add("Which direction [123456789, z]?");
    vRefresh();

    while (true) {
        const int ch = vGetch();

        if (vCheckForCursorKey(ch, &pt->x, &pt->y)) {
            if (!flag && pt->x == 0 && pt->y == 0) {
                continue;
            }

            return 1;
        }

        if (ch == 'z' || ch == KEY_ESC) {
            vRefresh();
            msgwin.ClrMsg();
            return 0;
        }
    }
}

int XHero::XShoot()
{
    XItem* missile = GetItem(BP_MISSILE);
    const auto missile_w = dynamic_cast<XMissileWeapon *>(GetItem(BP_MISSILE_WEAPON));

    // if no missile, try to load them
    if (!missile) {
        for (auto it: contain) {
            if (it->im & IM_MISSILE && XMissile::isProperWeapon(it, missile_w)) {
                msgwin.ClrMsg();
                msgwin.Add(fmt::format("Load {}", it->toString()));
                msgwin.Add("["
                    MSG_CYAN "Y" MSG_LIGHTGRAY ", "
                    MSG_CYAN "N" MSG_LIGHTGRAY ", "
                    MSG_CYAN "Esc" MSG_LIGHTGRAY "]?");
                vRefresh();
                const int ch = vGetch();

                if (ch == 'y' || ch == 'Y' || ch == ' ' || ch == KEY_ENTER) {
                    msgwin.ClrMsg();
                    vRefresh();
                    XBodyPart * bp = GetBodyPart(BP_MISSILE);

                    if (bp->Item()) {
                        return 0;
                    }

                    XItem * tmp = it;
                    contain.erase(it);
                    bp->Wear(tmp);
                    break;
                }

                if (ch == KEY_ESC) {
                    msgwin.ClrMsg();
                    vRefresh();
                    return 0;
                }
            }
        }

        msgwin.ClrMsg();
        vRefresh();
    }

    missile = GetItem(BP_MISSILE);

    if (!missile || !XMissile::isProperWeapon(missile, missile_w)) {
        if (missile_w) {
            msgwin.Add(fmt::format("You need a proper ammo to shoot from {}.",
                missile_w->name));
        } else {
            msgwin.Add("You need something to throw.");
        }

        return 0;
    }

    XPoint pt;
    int hit = 0;
    int range = 0;
    XDice dmg;
    GetRangeAttackInfo(&range, &hit, &dmg);

    if (Targeting(range, &pt) != ABORT) {
        Shoot(pt.x, pt.y);

        if (l->map->GetMonster(pt.x, pt.y) && !(pt.x == x && pt.y == y)) {
            target = l->map->GetMonster(pt.x, pt.y);
        } else {
            target = nullptr;
        }

        return 1;
    }

    return 0;
}

int XHero::Targeting(int range, XPoint * pt)
{
    if (target && !target->isValid()) {
        target = nullptr;
    }

    const XCreature* tgt = nullptr;
    int dist = 10000;

    for (int i = -7; i <= 7; i++)
        for (int j = -7; j <= 7; j++)
            if (l->map->GetMonster(x + j, y + i) && l->map->GetMonster(x + j, y + i)->isVisible()
                && l->map->GetMonster(x + j, y + i)->xai->isEnemy(this)
                && std::max(abs(i), abs(j)) < dist && !(j == 0 && i == 0)) {
                dist = std::max(abs(i), abs(j));
                tgt = l->map->GetMonster(x + j, y + i);
            }

    int tx = x;
    int ty = y;

    if (tgt && !target) {
        tx = tgt->x;
        ty = tgt->y;
    } else if (target && target->isVisible()) {
        tx = target->x;
        ty = target->y;
    }

    while (true) {
        auto xrng = std::sqrt(static_cast<float>((tx - x) * (tx - x) + (ty - y) * (ty - y)));
        float cos_alpha;
        float sin_alpha;

        if (xrng > 0) {
            cos_alpha = static_cast<float>(tx - x) / xrng;
            sin_alpha = static_cast<float>(ty - y) / xrng;
        } else {
            cos_alpha = 0;
            sin_alpha = 0;
        }

        auto mx = static_cast<float>(x);
        auto my = static_cast<float>(y);

        l->map->Center(tx, ty);
        l->map->Put(this);

        int flag = 1;
        int trange = range;

        while (sqrt((tx - mx) * (tx - mx) + (ty - my) * (ty - my)) >= 0.5f) {
            trange--;
            mx += cos_alpha;
            my += sin_alpha;

            if ((std::lround(mx) != x || std::lround(my) != y) && flag) {
                if (trange >= 0) {
                    l->map->PutChar(std::lround(mx), std::lround(my), '*', xYELLOW);
                } else {
                    l->map->PutChar(std::lround(mx), std::lround(my), '*', xRED);
                }
            }

            if ((l->map->GetMovability(std::lround(mx), std::lround(my)) >= MO_WALL
                || !l->map->GetVisible(std::lround(mx), std::lround(my)))
                && flag) {
                if (trange >= 0) {
                    l->map->PutChar(std::lround(mx), std::lround(my), 'X', xYELLOW);
                } else {
                    l->map->PutChar(std::lround(mx), std::lround(my), 'X', xRED);
                }

                flag = 0;
            }

            xrng -= 1;
        }

        if (trange >= 0 && flag) {
            l->map->PutChar(tx, ty, 'X', xYELLOW);
        } else {
            l->map->PutChar(tx, ty, 'X', xRED);
        }

        vRefresh();

        int ch = vGetch();

        if (ch == ' ' || ch == 't') {
            pt->x = tx;
            pt->y = ty;
            return 1;
        }

        if (ch == KEY_ESC) {
            return ABORT;
        }

        int dx = 0;
        int dy = 0;

        if (vCheckForCursorKey(ch, &dx, &dy)) {
            tx = tx + dx;

            if (tx < 0) {
                tx = 0;
            }

            if (tx >= l->map->len) {
                tx = l->map->len - 1;
            }

            ty = ty + dy;

            if (ty < 0) {
                ty = 0;
            }

            if (ty >= l->map->hgt) {
                ty = l->map->hgt - 1;
            }
        }
    }
}

int XHero::SelectPosition(XPoint * pt, int flag)
{
    int tx = x;
    int ty = y;

    msgwin.ClrMsg(1);
    vGotoXY(0, 0);
    vPutS("press [" MSG_CYAN "1" MSG_LIGHTGRAY ".." MSG_CYAN "9"
        MSG_LIGHTGRAY "] - for move cursor, ["
        MSG_CYAN "z" MSG_LIGHTGRAY ", " MSG_CYAN "esc" MSG_LIGHTGRAY "] - for exit");

    while (1) {
        l->map->Center(tx, ty);
        l->map->Put(this);
        l->map->PutChar(tx, ty, 'X', xYELLOW);

        int more_info_flag = 0;
        vGotoXY(0, 1);

        if (flag) {
            if (l->map->GetVisible(tx, ty)) {
                XCreature * cr = l->map->GetMonster(tx, ty);
                XMapObject * m_obj = l->map->GetSpecial(tx, ty);
                const unsigned int item_count = l->map->GetItemCount(tx, ty);

                if (cr && cr->isVisible()) {
                    more_info_flag = 1;
                    vPutS(fmt::format("{} ({}) [" MSG_CYAN "space" MSG_LIGHTGRAY "] - more.",
                        cr->name, cr->GetWoundMsg()));
                } else if (item_count == 1) {
                    vPutS("item");
                } else if (item_count > 1) {
                    vPutS("some items");
                } else if (m_obj) {
                    vPutS(m_obj->GetName(this));
                } else {
                    vPutS(l->map->GetDescription(tx, ty));
                }
            } else {
                vPutS("invisible at this time.");
            }

            vClrEol();
        }

        vRefresh();
        const int ch = vGetch();

        if (!flag || more_info_flag) {
            if (ch == ' ' || ch == 't') {
                pt->x = tx;
                pt->y = ty;
                return 1;
            }
        }

        if (ch == KEY_ESC || ch == 'z' || ch == 'Z') {
            return 0;
        }

        int dx = 0;
        int dy = 0;

        if (vCheckForCursorKey(ch, &dx, &dy)) {
            tx = tx + dx;
            ty = ty + dy;

            if (tx < 0) {
                tx = 0;
            }

            if (tx >= l->map->len) {
                tx = l->map->len - 1;
            }

            if (ty < 0) {
                ty = 0;
            }

            if (ty >= l->map->hgt) {
                ty = l->map->hgt - 1;
            }
        }
    }
}

int XHero::XCast(std::optional<std::reference_wrapper<std::ofstream>> file)
{
    int ch = '!';

    while (true) {
        XGuiList list;
        list.SetCaption(MSG_BROWN "###" MSG_LIGHTGRAY " Cast Spell " MSG_BROWN "###");
        if (m->spells.empty()) {
            list.AddItem(new XGuiItem_Text("You do not know any spells", 0));
        } else {
            for (auto spell : m->spells) {
                list.AddItem(new XGuiItem_SimpleSelect(spell->toString().c_str()), 0);
            }
        }

        if (file) {
            list.Put(file);
            return 0;
        }

        ch = list.Run(1);

        if (ch == -1) {
            ch = list.GetLastKey();

            if (ch == 'z' || ch == KEY_ESC || ch == 'Z' || ch == ' ') {
                return 0;
            }
        } else {
            for(auto spell : m->spells) {
                if (ch == 0) {
                    if (m->Cast(spell, this) == CONTINUE) {
                        return 0;
                    }

                    last_cast = spell;
                    break;
                }

                ch--;
            }

            return 1;
        }
    }

    return 0;
}

int XHero::RepeatCast()
{
    if (last_cast) {
        if (m->Cast(last_cast, this) == CONTINUE) {
            return 0;
        }

        return 1;
    }

    return 0;
}

int XHero::GetTarget(const TARGET_REASON tr, XPoint* pt, int max_range, XObject** back)
{
    int flag = 0;
    char buf[256];
    char ch;
    int cx;
    int cy;
    char in_buf[256];
    int value;
    int gets_flag = 0;

    switch (tr) {
        case TR_ATTACK_DIRECTION:
            flag = WhichDirection(pt);
            break;

        case TR_ATTACK_TARGET:
            return Targeting(max_range, pt);
            break;

        case TR_YES_NO:
            msgwin.Add("(Yes/" MSG_CYAN "N" MSG_LIGHTGRAY "o)?");
            vRefresh();
            ch = vGetch();
            msgwin.ClrMsg();

            if (ch == 'Y' || ch == 'y') {
                return 1;
            }

            return 0;
            break;

        case TR_NO_YES:
            msgwin.Add("(" MSG_CYAN "Y" MSG_LIGHTGRAY "es/No)?");
            vRefresh();
            ch = vGetch();
            msgwin.ClrMsg();

            if (ch == 'N' || ch == 'n') {
                return 0;
            }

            return 1;
            break;

        case TR_HOW_MUCH:
            if (max_range < pt->x) {
                max_range = pt->x;
            }

            if (max_range > pt->y) {
                max_range = pt->y;
            }

            vGetCursorPos(&cx, &cy);

            if (cx > 60) {
                cy++;
                cx = 0;
            }

            if (cy > 2) {
                msgwin.ClrMsg();
                cx = 0;
                cy = 0;
            }

            sprintf(buf, "(Enter = %d)? ", max_range);
            msgwin.Add(buf);
            vPutS(" ");
            vRefresh();
            gets_flag = vGetS(in_buf, 9);

            if (gets_flag == 1 && strlen(in_buf) == 0) {
                return max_range;
            }

            if (gets_flag == 0) {
                return 0;
            }

            value = atoi(in_buf);

            if (value < pt->x) {
                return pt->x;
            }

            if (value > pt->y) {
                return pt->y;
            }

            return value;
            break;

        case TR_STEAL_ITEM:
            assert(back);

            if (GetTarget(TR_ATTACK_DIRECTION, pt)) {
                XItem* item;
                XCreature *cr = l->map->GetMonster(x + pt->x, y + pt->y);

                if (cr) {
                    if (cr->xai->isEnemy(this)) {
                        msgwin.Add("You couldn't steal anything from the enemy.");
                        return 0;
                    }

                    if ((item = Inventory(&cr->contain))) {
                        *back = item;
                        return 1;
                    }

                    return 0;
                }

                XAnyPlace* pl = l->map->GetPlace(x + pt->x, y + pt->y);

                if (pl && pl->GetOwner() && l->map->GetItemCount(x + pt->x, y + pt->y) > 0) {
                    if ((pl->GetOwner())->xai->isEnemy(this)) {
                        msgwin.Add("You can't.");
                        return 0;
                    }

                    item = Inventory(l->map->GetItemList(x + pt->x, y + pt->y));

                    if (item) {
                        *back = item;
                        return 1;
                    }

                    return 0;
                }
            } else {
                return 0;
            }

            break;
        default: ;
    }

    return flag;
}

void XHero::MagicLevelList() const
{
    XGuiList list;
    list.SetCaption(MSG_BROWN "###" MSG_LIGHTGRAY " Magic School " MSG_BROWN "###");

    for (int i = 0; i < MS_EOF; i++) {
        char buf[256];

        if (m->LevelToString(static_cast<MAGIC_SCHOOL>(i), buf)) {
            list.AddItem(new XGuiItem_SimpleSelect(buf), 0);
        }
    }

    list.Run();
}

XSkill* XHero::SkillsList(const SKILL_FLAG skill_flag, const int marks_left, std::optional<std::reference_wrapper<std::ofstream>> file) const
{
    while (1) {
        XGuiList list;

        if (skill_flag == SKF_IMPROVE_SKILL) {
            list.SetCaption(MSG_BROWN "###" MSG_LIGHTGRAY " Improve Skill " MSG_BROWN "###");

            const std::string footer = fmt::format(
                MSG_LIGHTGRAY "You have " MSG_YELLOW "{}" MSG_LIGHTGRAY " {} left.",
                marks_left,
                marks_left > 1 ? "improvements" : "improvement");

            list.SetFooter(footer.c_str());
        } else if (skill_flag == SKF_LIST_SKILL) {
            list.SetCaption(MSG_BROWN "###" MSG_YELLOW " Skills List " MSG_BROWN "###");
        } else if (skill_flag == SKF_USE_SKILL) {
            list.SetCaption(MSG_BROWN "###" MSG_YELLOW " Use Skill " MSG_BROWN "###");
        }

        for (const auto& [skt, skill] : sk->skills) {
            // Fill name up to 17 characters (excluding ANSI prefix) with '.'.
            // MSG_LIGHTGRAY is an ANSI escape prefix, and does not add to the visual width.
            const std::string_view raw_name = skill->GetName();
            const size_t visible_len = raw_name.size();
            const size_t pad = visible_len < 17 ? 17 - visible_len : 0;

            const std::string name_col = fmt::format(
                MSG_LIGHTGRAY "{}{}", raw_name, std::string(pad, '.'));

            // Skill level column: colorful when still improvable, grey when maxed
            const std::string level_col = skill->GetLevel() < skill->GetMaxLevel()
                ? fmt::format(MSG_BROWN "[" MSG_LIGHTGRAY " {:2} " MSG_BROWN "from" MSG_LIGHTGRAY " {:2} " MSG_BROWN "] ",
                              skill->GetLevel(), skill->GetMaxLevel())
                : fmt::format(MSG_BROWN "[ {:2} from {:2} ] ",
                              skill->GetLevel(), skill->GetMaxLevel());

            const std::string buf = name_col + level_col + skill->GetSkillLevel();
            list.AddItem(new XGuiItem_SimpleSelect(buf.c_str()), 0);
        }

        if (file) {
            list.Put(file);
            return nullptr;
        }

        if (int ch = list.Run(1); ch == -1) {
            ch = list.GetLastKey();

            if (ch == 'z' || ch == 'Z' || ch == KEY_ESC || ch == ' ') {
                return nullptr;
            }
        } else {
            if (skill_flag == SKF_IMPROVE_SKILL || skill_flag == SKF_USE_SKILL) {
                for (const auto& [skt, skill] : sk->skills) {
                    if (ch > 0) {
                        ch--;
                    } else {
                        return skill;
                    }
                }
            }
        }

    }
}

void XHero::IncLevel()
{
    msgwin.Add("Congratulations! You have advanced to a new level. Press any key.");
    l->map->Put(this);
    vRefresh();
    vGetch();
    msgwin.ClrMsg();

    int counter = stats->Get(S_LEN) / 5 + 1;

    if (counter < 3) {
        counter = 3;
    }

    while (counter > 0) {
        int flag = 0;

        for (const auto& [skt, skill] : sk->skills) {
            if (skill->GetLevel() < skill->GetMaxLevel()) {
                flag = 1;
                break;
            }
        }

        if (flag) {
            XSkill* tskill = SkillsList(SKF_IMPROVE_SKILL, counter);

            if (tskill && tskill->GetLevel() < tskill->GetMaxLevel()) {
                tskill->IncLevel();
                counter--;

                if (counter == 0) {
                    SkillsList(SKF_LIST_SKILL);
                }
            }
        } else {
            SkillsList(SKF_LIST_SKILL);
            break;
        }
    }

    XCreature::IncLevel();
}

int XHero::UseSkill()
{
    XSkill* skill = nullptr;
    skill = SkillsList(SKF_USE_SKILL);

    if (skill) {
        skill->Use(this);
        return 1;
    } else {
        return 0;
    }
}

const char* wsk_levels_name[] = {
    "unskilled",
    "basic", "basic", "basic",
    "advanced", "advanced", "advanced", "advanced",
    "expert", "expert", "expert", "expert",
    "master", "master", "master",
    "grand master"
};

void XHero::WarSkillsList(std::optional<std::reference_wrapper<std::ofstream>> file) const
{
    XGuiList list;
    list.SetCaption(MSG_BROWN "###" MSG_LIGHTGRAY " Weapon Skills " MSG_BROWN "###");

    list.AddItem(new XGuiItem_Text(MSG_BROWN "Melee Weapon         DV  HIT  DMG      Level              required marks", 0), 0);
    char buf[256];

    for (int i = 0; i < XWarSkills::ALL; i++) {
        const auto w_skill = static_cast<XWarSkills::Type>(i);

        if (w_skill == XWarSkills::BOW) {
            list.AddItem(new XGuiItem_Text("", 0), 0);
            list.AddItem(new XGuiItem_Text(MSG_BROWN "Missile Weapon       RNG HIT  DMG      Level              required marks", 0), 0);
        }

        if (i == XWarSkills::SHIELD) {
            list.AddItem(new XGuiItem_Text("", 0), 0);
            list.AddItem(new XGuiItem_Text(MSG_BROWN "Shields              DV                Level              required marks", 0), 0);
        }

        char tbuf[256];

        if (wsk->GetLevel(w_skill) < 15) {
            sprintf(tbuf, "%d", wsk->GetMarks(w_skill));
        } else {
            sprintf(tbuf, "NaN");
        }

        sprintf(buf, MSG_YELLOW "%-18s " MSG_LIGHTGRAY "%+4d %+4d %+4d      " MSG_BROWN "[" MSG_LIGHTGRAY "%d" MSG_BROWN "]" MSG_LIGHTGRAY " %-10s " MSG_LIGHTGRAY "%8s",
            wsk->GetName(w_skill), wsk->GetDV(w_skill),
            wsk->GetHIT(w_skill), wsk->GetDMG(w_skill),
            wsk->GetLevel(w_skill), wsk_levels_name[wsk->GetLevel(w_skill)], tbuf);

        list.AddItem(new XGuiItem_Text(buf, 0), 0);
    }

    if (file) {
        list.Put(file);
    } else {
        list.Run();
    }
}

//////////////////////////////////////////////////////////////////////////////

void XHero::FirstStep(int _x, int _y, XLocation * _l)
{
    XCreature::FirstStep(_x, _y, _l);
    ShowNewView();
}

void XHero::LastStep()
{
    HideOldView();
    XCreature::LastStep();
}

void XHero::LookAt()
{
    XPoint pt;

    if (!SelectPosition(&pt, 1)) {
        return;
    }

    XCreature* xcr = l->map->GetMonster(pt.x, pt.y);

    XGuiList list;

    if (xcr) {
        std::string str;
        if (xcr->isHero()) {
            str = fmt::format(
                MSG_BROWN "### " MSG_LIGHTGRAY "'{}{}" MSG_LIGHTGRAY "' {}, the {} {} {}" MSG_BROWN " ###",
                SCOLOR(xcr->color), xcr->view, xcr->name, GetGenderStr(), GetRaceStr(), GetProfessionStr());
        } else {
            str = fmt::format(
                MSG_BROWN "### " MSG_LIGHTGRAY "'{}{}" MSG_LIGHTGRAY "' {}" MSG_BROWN " ###",
                SCOLOR(xcr->color), xcr->view, xcr->GetNameEx(CRN_T1));
        }

        list.SetCaption(str.c_str());

        if (xcr->isHero()) {
            str = fmt::format(MSG_YELLOW "You are {}.",
                xcr->GetWoundMsg());
        } else {
            str = fmt::format(MSG_YELLOW "{} is {}.",
                xcr->GetNameEx(CRN_T2),
                xcr->GetWoundMsg());

            str[2] = toupper(str[2]);
        }

        list.AddItem(new XGuiItem_Text(str, 0), 0);

        bool iflag = false;

        for (auto xbp: xcr->components) {
            if (xbp->Item()) {
                if (!iflag) {
                    if (xcr->isHero()) {
                        str = MSG_YELLOW "You are wearing the following items:";
                    } else {
                        str = fmt::format(MSG_YELLOW "{} is wearing the following items:", xcr->GetNameEx(CRN_T2));
                        str[2] = toupper(str[2]);
                    }

                    list.AddItem(new XGuiItem_Text(str, 0), 0);
                }

                list.AddItem(new XGuiItem_Text(xbp->Item()->toString(), 0), 0);
                iflag = true;
            }

            ++xbp;
        }

        list.AddItem(new XGuiItem_Text("", 0), 0);

        if (!xcr->isHero()) {
            list.AddItem(new XGuiItem_Text(xcr->creature_description, 0), 0);
        }

#ifdef __XDEBUG_01
        list.AddItem(new XGuiItem_Text("", 0), 0);

        sprintf(static_buffer, "St:%d Dx:%d To:%d Le:%d Wi:%d Ma:%d Pe:%d Ch:%d Sp:%d    ", xcr->GetStats(S_STR), xcr->GetStats(S_DEX), xcr->GetStats(S_TOU),
            xcr->GetStats(S_LEN), xcr->GetStats(S_WIL), xcr->GetStats(S_MAN), xcr->GetStats(S_PER), xcr->GetStats(S_CHR), 100000 / xcr->GetSpeed());
        list.AddItem(new XGuiItem_Text(static_buffer, 0), 0);

        sprintf(static_buffer, "HP:%d(%d)  PP:%d(%d)", xcr->_HP, xcr->GetMaxHP(), xcr->_PP, xcr->GetMaxPP());
        list.AddItem(new XGuiItem_Text(static_buffer, 0), 0);

        sprintf(static_buffer, "Exp(%d)%lu", xcr->level, xcr->_EXP);
        list.AddItem(new XGuiItem_Text(static_buffer, 0), 0);

        sprintf(static_buffer, "_DV - %d, _PV - %d, hit, xdy+z[%d, %dd%d + %d] _EXP(EXP_ADD) %d(%d)"
            , xcr->GetDV(), xcr->GetPV()
            , xcr->GetHIT(), xcr->dice.X, xcr->dice.Y, xcr->dice.Z
            , xcr->_EXP, xcr->GetExp());
        list.AddItem(new XGuiItem_Text(static_buffer, 0), 0);
#endif
    } else {
        list.AddItem(new XGuiItem_Text(l->map->GetDescription(pt.x, pt.y), 0), 0);
    }

    list.Run();
}

XItem* XHero::onIdentifyItem()
{
    XItem * it = Inventory(&contain);

    if (it) {
        contain.insert(it);
    }

    return it;
}


void XHero::ChatWithMonster()
{
    int creature_count = 0;
    const XCreature* last_creature = nullptr;

    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            if (!(i == 0 && j == 0) && l->map->GetMonster(x + i, y + j)) {
                creature_count++;
                last_creature = l->map->GetMonster(x + i, y + j);
            }
        }
    }

    if (creature_count == 0) {
        msgwin.Add("There was no reply.");
    } else if (creature_count == 1) {
        last_creature->xai->Chat(this, "");
    } else {
        XPoint pt;
        WhichDirection(&pt);

        if (l->map->GetMonster(x + pt.x, y + pt.y)) {
            (l->map->GetMonster(x + pt.x, y + pt.y))->xai->Chat(this, "");
        }
    }
}

int XHero::Chat(XCreature * chatter, const char* msg)
{
    msgwin.Add("You don't like to speak yourself");
    return 1;
}

void XHero::QuickPay()
{

    XCreature* shopkeeper = nullptr;
    XAnyPlace* pl = l->map->GetPlace(x, y);

    //hack!!!
    if (pl == nullptr || (pl->GetClassName() != "XShop")) {
        msgwin.Add("You can pay only in shops.");
        return;
    }

    shopkeeper = pl->GetOwner();

    if (shopkeeper) {
        const auto pai = dynamic_cast<XShopKeeperAI *>(shopkeeper->xai.get());
        int val = 0;

        for (const auto item: pai->debt.unpaid_items)
            val += item->GetValue() * item->quantity;

        val += static_cast<int>(pai->debt.debtor_sum);

        if (val > 0) {
            if (MoneyOp(0) >= val) {
                const auto money = new XMoney(val);
                pai->onGiveItem(this, money);
            } else {
                msgwin.Add("You don't have enough money!");
            }
        } else {
            msgwin.Add(fmt::format("You owe nothing to {}.", shopkeeper->name));
        }
    }
}

void XHero::GiveItem()
{
    int creature_count = 0;
    const XCreature* last_creature = nullptr;

    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            if (!(i == 0 && j == 0) && l->map->GetMonster(x + i, y + j)) {
                creature_count++;
                last_creature = l->map->GetMonster(x + i, y + j);
            }
        }
    }

    if (creature_count == 0) {
        msgwin.Add("There is no creature to give anything to.");
        return;
    }

    if (creature_count > 1) {
        XPoint pt;
        WhichDirection(&pt);

        if (l->map->GetMonster(x + pt.x, y + pt.y)) {
            last_creature = l->map->GetMonster(x + pt.x, y + pt.y);
        }
    }

    if (XItem* item = Inventory(&contain)) {
        if (item->im & IM_MONEY) {
            contain.insert(item);
            last_creature->xai->onGiveItem(this, item);
        } else {
            int res = item->quantity;

            if (res > 1) {
                XPoint pt(1, res);
                res = GetTarget(TR_HOW_MUCH, &pt, res);
            }

            int flag = 1;
            XItem * gitem;

            if (res < item->quantity && res > 0) {
                gitem = dynamic_cast<XItem *>(item->MakeCopy());
                gitem->quantity = res;
                item->quantity -= res;
                contain.insert(item);
            } else {
                gitem = item;
            }

            if (res > 0) {
                carried_weight -= item->weight * res;
                flag = last_creature->xai->onGiveItem(this, gitem);
            }

            if (!flag || res == 0) {
                carried_weight += gitem->weight;
                contain.insert(gitem);
            }
        }
    }
}

void XHero::SetTactics()
{
    while (true) {
        msgwin.ClrMsg();

        char buf[256] = "Change tactics: ";

        switch (tactics) {
            case TS_COWARD	:
                strcat(buf, "Coward ");
                break;

            case TS_DEFENSIVE	:
                strcat(buf, "Defensive ");
                break;

            case TS_NORMAL	:
                strcat(buf, "Normal ");
                break;

            case TS_AGGRESSIVE	:
                strcat(buf, "Aggressive ");
                break;

            case TS_BERSERKER	:
                strcat(buf, "Berserker ");
                break;

            default :
                assert(0);
        }

        vGotoXY(0, 0);
        vPutS(buf);

        sprintf(buf, " (DV:%+d, HIT:%+d, DMG:%+d)      ", GetTacticsDVBonus(), GetTacticsHITBonus(), GetTacticsDMGBonus());
        vPutS(buf);

        vGotoXY(0, 1);

        vPutS(MSG_LIGHTGRAY "[" MSG_CYAN "+" MSG_LIGHTGRAY "," MSG_CYAN "-"
            MSG_LIGHTGRAY "] - change, [" MSG_CYAN "Z" MSG_LIGHTGRAY "] - exit");

        vRefresh();

        const int ch = vGetch();

        if (ch == 'Z' || ch == 'z' || ch == KEY_ESC) {
            msgwin.ClrMsg();
            break;
        }

        if (ch == '+' && tactics < TS_BERSERKER) {
            ChangeTactics(static_cast<TACTICS_STATE>(tactics + 1));
        }

        if (ch == '-' && tactics > TS_COWARD) {
            ChangeTactics(static_cast<TACTICS_STATE>(tactics - 1));
        }
    }
}

void XHero::SaveGame()
{
    V_BUFFER xyzbuf;
    vStore(&xyzbuf);
    vClrScr();
    vGotoXY(0, 0);
    vPutS(MSG_LIGHTGRAY "Storing the game:");
    vRefresh();
    XGame::hero_guid = guid();
    XArchive::StoreGame();
    vRestore(&xyzbuf);
    vRefresh();
};

void XHero::Store(XFile * f)
{
    XCreature::Store(f);
    f->Write(&race, sizeof(int), 1);
    f->Write(&profession, sizeof(int), 1);
    f->Write(&turn_count);

    // FIXME: Implement when porting saving/restoring to Cereal
    // reception_list.StoreList(f);
}

void XHero::Restore(XFile * f)
{
    XCreature::Restore(f);
    f->Read(&race, sizeof(int), 1);
    f->Read(&profession, sizeof(int), 1);
    f->Read(&turn_count);

    // FIXME: Implement when porting saving/restoring to Cereal
    // reception_list.RestoreList(f);

    isDisturb = 0;
    last_char = '5';
    run_way_count = 0;
    target = nullptr;
    last_cast = nullptr;
    melee_attack = &hero_melee;
}

int XHero::UseOuterObject()
{
    XMapObject* spec = l->map->GetSpecial(nx, ny);

    if (spec) {
        if (!spec->onOuterUse(this)) {
            msgwin.Add("You can't use this.");
            return 0;
        }

        return 1;
    }

    msgwin.Add("Nothing to use.");

    return 0;
}

void XHero::ActivateTrap()
{
    XMapObject* obj = l->map->GetSpecial(x, y);

    if (obj && obj->im == IM_TRAP) {
        dynamic_cast<XTrap *>(obj)->Activate(this);
    }
}

int XHero::OrderCompanion()
{
    std::vector<XCreature*> companions_list;

    for (int i = -10; i < 11; i++)
        for (int j = -10; j < 11; j++) {
            if (isVisibleArea(x + i, y + j)) {
                XCreature* cr = l->map->GetMonster(x + i, y + j);

                if (cr && cr->xai->companion && cr->xai->companion->isHero()) {
                    companions_list.push_back(cr);
                }
            }
        }

    if (companions_list.empty()) {
        msgwin.Add("You don't have a companion to give orders to.");
        return 0;
    }

    XCreature* slave = nullptr;

    if (companions_list.size() > 1) {
        std::string list;
        int index = 0;

        for (auto it: companions_list) {
            list.append(fmt::format("[{c}] {}\n",
                64 + index, it->GetNameEx(CRN_T1)));
            index++;
        }

        msgwin.Add(list);
        vRefresh();
        const int ch = vGetch();

        if (ch == KEY_ESC || ch == ' ') {
            return 0;
        }

        index = 96;

        for (auto it: companions_list) {
            if (ch == index || ch == (index - 32)) {
                slave = it;
            }

            index++;
        }
    } else {
        slave = *companions_list.begin();
    }

    if (!slave) {
        return 0;
    }

    msgwin.ClrMsg();
    msgwin.Add(
        "[" MSG_CYAN "a" MSG_LIGHTGRAY "] - attack, "
        "[" MSG_CYAN "f" MSG_LIGHTGRAY "] - follow me, "
        "[" MSG_CYAN "w" MSG_LIGHTGRAY "] - wait");

    vRefresh();
    const int ch = vGetch();
    msgwin.ClrMsg();
    vRefresh();

    if (ch == 'a' || ch == 'A') {
        XPoint pt;

        if (!Targeting(GetVisibleRadius(), &pt)) {
            return 0;
        }

        XCreature * tgt = l->map->GetMonster(pt.x, pt.y);

        if (!tgt) {
            return 0;
        }

        if (tgt == slave) {
            return 0;
        }

        msgwin.Add("You command");
        msgwin.Add(slave->GetNameEx(CRN_T1));
        msgwin.Add("to attack");
        msgwin.Add(tgt->GetNameEx(CRN_T1));
        slave->xai->ordered_enemy = tgt;
        slave->xai->companion_command = CC_ATTACK;
        slave->xai->ResAIFlag(AIF_GUARD_AREA);
        return 1;
    } else if (ch == 'f' || ch == 'F') {
        msgwin.Add("You command");
        msgwin.Add(slave->GetNameEx(CRN_T1));
        msgwin.Add("to come to you.");
        slave->xai->ordered_enemy = nullptr;
        slave->xai->companion_command = CC_FOLLOW;
        slave->xai->ResAIFlag(AIF_GUARD_AREA);
        return 1;
    } else if (ch == 'w' || ch == 'W') {
        msgwin.Add("You command");
        msgwin.Add(slave->GetNameEx(CRN_T1));
        msgwin.Add("to wait.");
        slave->xai->ordered_enemy = nullptr;
        slave->xai->companion_command = CC_WAIT;
        XRect tr(slave->x, slave->y, slave->x + 1, slave->y + 1);
        slave->xai->SetArea(tr, slave->l->ln);
        slave->xai->SetAIFlag(AIF_GUARD_AREA);
        return 1;
    }

    return 0;

}

//Location Script Support
extern "C"
{
#include "lauxlib.h"
#include "lualib.h"
}

int XHero::ExecuteScript()
{
    msgwin.Add("===DEBUG CMD===: ");
    vRefresh();
    char buf[255];
    vGetS(buf, 250);
    luaL_dostring(XLocation::L, buf);
    return 0;
}
