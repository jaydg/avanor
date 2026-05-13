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

#include <filesystem>
#include <memory>

#include "creature/xhero.h"
#include "game/game.h"
#include "game/quest.h"
#include "helpers/hiscore.h"
#include "helpers/msgwin.h"

int XHero::UseTool()
{
    if (const XBodyPart* tbp = GetBodyPart(BP_TOOL))
    {
        auto tool = dynamic_cast<XTool *>(tbp->Item());
        UseItem(tool);

        return 1;
    }

    msgwin.Add("You have no tool.");

    return 0;
}

void XHero::doSacrifice()
{
    while (true) {
        XItem* item = nullptr;

        XItemList* tmpquae = l->map->GetItemList(x, y);

        if (tmpquae->empty() || l->map->GetPlace(x, y)) {
            item = Inventory(&contain);
        } else {
            item = Inventory(tmpquae);
        }

        XItem* drop_item = item;

        if (item) {
            if (item->quantity > 1) {
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

            Sacrifice(drop_item);
        } else {
            break;
        }
    }
}

XItem* XHero::SelectItem(XItemFilter* filter, const bool isGetAll)
{
    return Inventory(&contain, IM_UNKNOWN, IF_NONE, !isGetAll, filter);
}

void XHero::DumpVBuffer(std::ofstream& file)
{
    for (int j = 0; j < size_y; j++) {
        for (int k = 0; k < size_x; k++) {
            file << vTestCh(k, j);
        }

        file << "\n";
    }
}

void XHero::CreateScreenShot()
{
    for (int i = 0; i < 1000; i++) {
        auto filename = fmt::format("shot{:03}.txt", i);

        if (!std::filesystem::exists(filename)) {
            std::ofstream file(filename);
            DumpVBuffer(file);

            msgwin.Add(fmt::format("Screenshot '{}' created successfully.",
                filename));

            return;
        }
    }
}

void XHero::Pray()
{
    XGuiList list;
    char buf[256];
    DEITY_HELP * pLifeHelp;
    DEITY_HELP * pDeathHelp;

    DEITY_RELATION dr = religion.GetRelation(D_LIFE);
    sprintf(buf, MSG_YELLOW "%s " MSG_LIGHTGRAY "(%s" MSG_LIGHTGRAY ")", XReligion::GetDeityName(D_LIFE), XReligion::GetRelationName(dr));
    list.AddItem(new XGuiItem_Text(buf, 0));
    const int life_count = religion.GetAvailHelp(D_LIFE, &pLifeHelp);

    if (life_count == 0) {
        list.AddItem(new XGuiItem_Text("< No help available >", 0));
    } else {
        for (int i = 0; i < life_count; i++) {
            list.AddItem(new XGuiItem_Text(pLifeHelp[i].help_name, 1));
        }
    }

    list.AddItem(new XGuiItem_Text("", 0));

    dr = religion.GetRelation(D_DEATH);
    sprintf(buf, MSG_YELLOW "%s " MSG_LIGHTGRAY "(%s" MSG_LIGHTGRAY ")", XReligion::GetDeityName(D_DEATH), XReligion::GetRelationName(dr));
    list.AddItem(new XGuiItem_Text(buf, 0));
    int death_count = religion.GetAvailHelp(D_DEATH, &pDeathHelp);

    if (death_count == 0) {
        list.AddItem(new XGuiItem_Text("< No help available >", 0));
    } else {
        for (int i = 0; i < death_count; i++) {
            list.AddItem(new XGuiItem_Text(pDeathHelp[i].help_name, 1));
        }
    }

    int res = list.Run();

    if (res == -1) {
        return;
    }

    if (res < life_count) {
        religion.Pray(D_LIFE, &pLifeHelp[res], this);
    } else if (res < life_count + death_count) {
        religion.Pray(D_DEATH, &pDeathHelp[res - life_count], this);
    }
}



void XHero::EndGame(const char* end_msg)
{
    unsigned long score = main_creature->_EXP + main_creature->MoneyOp(0);

    XGuiList list;
    char buf2[256];
    char tbuf[256];

    sprintf(buf2, "%s, %s %s %s (L%d).",
        main_creature->name.c_str(),
        main_creature->GetGenderStr(),
        dynamic_cast<XHero *>(main_creature)->GetRaceStr(),
        dynamic_cast<XHero *>(main_creature)->GetProfessionStr(),
        main_creature->level);

    list.AddItem(new XGuiItem_Text(buf2));

    sprintf(tbuf, "You survived %d turns.", dynamic_cast<XHero *>(main_creature)->turn_count);
    list.AddItem(new XGuiItem_Text(tbuf));

    if (XQuest::quest.hero_win) {
        if (XQuest::quest.ahk_ulan_killed && XQuest::quest.roderick_killed) {
            if (main_creature->GetGender() == GEN_MALE) {
                list.AddItem(new XGuiItem_Text("You killed Ahk-Ulan and the King of Avanor and became the new King of Avanor."));
            } else if (main_creature->GetGender() == GEN_FEMALE) {
                list.AddItem(new XGuiItem_Text("You killed Ahk-Ulan and the King of Avanor and became the new Queen of Avanor."));
            }

            score += 30000;
        } else if (XQuest::quest.ahk_ulan_killed) {
            list.AddItem(new XGuiItem_Text("You killed evil Ahk-Ulan and saved the Kingdom of Avanor from Ahk-Ulan's deadly plans."));
            score += 10000;
        } else {
            list.AddItem(new XGuiItem_Text("You killed the King of Avanor and helped Ahk-Ulan to become Usurper of Avanor."));
            score += 20000;
        }
    } else {
        list.AddItem(new XGuiItem_Text(end_msg));
    }

    int place_count = 0;

    for (const auto & location : Game.locations) {
        if (location && location->visited_by_hero) {
            place_count++;
        }
    }

    score += place_count * 200;
    sprintf(tbuf, "You visited %d places.", place_count);
    list.AddItem(new XGuiItem_Text(tbuf));

    const DEITY_RELATION dr1 = main_creature->religion.GetRelation(D_LIFE);
    const DEITY_RELATION dr2 = main_creature->religion.GetRelation(D_DEATH);
    int flag = 1;

    if (dr1 >= DR_ADEPT) {
        sprintf(tbuf, "You were a %s of %s",
            XReligion::GetRelationName(dr1),
            XReligion::GetDeityName(D_LIFE));

        list.AddItem(new XGuiItem_Text(tbuf));
        flag = 0;
        score += dr1 * 300;
    }

    if (dr2 >= DR_ADEPT) {
        sprintf(tbuf, "You were a %s of %s",
            XReligion::GetRelationName(dr2),
            XReligion::GetDeityName(D_DEATH));

        list.AddItem(new XGuiItem_Text(tbuf));
        score += dr2 * 300;
        flag = 0;
    }

    if (flag) {
        list.AddItem(new XGuiItem_Text("You were not very religious."));
    }

    if (XQuest::quest.beelzvile_killed) {
        list.AddItem(new XGuiItem_Text("You killed an ancient demon."));
    }

    if (XQuest::quest.torin_quest == 2) {
        list.AddItem(new XGuiItem_Text("You helped to pump out gas from the dwarven golden mine."));
        score += 5000;
    }

    if (XQuest::quest.guards_get_orc_slay) {
        list.AddItem(new XGuiItem_Text("You brought a useful thing to Ozorik."));
    }

    if (XQuest::quest.roderick_quest == 2) {
        list.AddItem(new XGuiItem_Text("You returned 'Eye of Raa' to Roderick."));
        score += 10000;
    }

    if (XQuest::quest.roderick_quest2 == 2) {
        list.AddItem(new XGuiItem_Text("You cleansed the tomb of Roderick's ancestors."));
        score += 5000;
    }

    if (XQuest::quest.orcs_killed > 0 && XQuest::quest.total_orcs_killed == 30) {
        list.AddItem(new XGuiItem_Text("You helped to repulse an attack of orcs."));
    } else if (XQuest::quest.orcs_killed > 0) {
        list.AddItem(new XGuiItem_Text("You tried to help to repulse an attack of orcs."));
    }

    sprintf(tbuf, "You scored %lu.", score);
    list.AddItem(new XGuiItem_Text(tbuf));
    list.SetCaption(MSG_BROWN "###" MSG_LIGHTGRAY " Achievements " MSG_BROWN "###");
    list.Run();

    vGotoXY(0, 0);
    msgwin.ClrMsg();
    msgwin.Add("Create Memory File? (Y/" MSG_CYAN "N" MSG_LIGHTGRAY ")");

    vRefresh();

    if (const int tch = vGetch(); tch == 'y' || tch == 'Y') {
        msgwin.ClrMsg();
        msgwin.Add("### Screenshot ###");

        auto filename = main_creature->name.append(".mem");
        std::ofstream file(vMakePath(HOME_DIR, filename));
        DumpVBuffer(file);
        list.Put(file);
        file << "\n";
        dynamic_cast<XHero *>(main_creature)->Equipment(file);
        file << "\n";
        dynamic_cast<XHero *>(main_creature)->WarSkillsList(file);
        file << "\n";
        dynamic_cast<XHero *>(main_creature)->SkillsList(SKF_LIST_SKILL, 0, file);
        file << "\n";
        dynamic_cast<XHero *>(main_creature)->XCast(file);
        file << "\n";
        dynamic_cast<XHero *>(main_creature)->ShowResistance(file);
        file << "\n";

        for (const auto item : main_creature->contain) {
            item->Identify(1);
        }

        dynamic_cast<XHero *>(main_creature)->Inventory(
            &main_creature->contain, IM_ALL, IF_NONE, 0, nullptr, file);

        vClrScr();
    }

    const std::shared_ptr<XHiScoreItem> hii(new XHiScoreItem(0, score, buf2, end_msg, XQuest::quest.hero_win, 1));

    XHiScore hiscore;
    hiscore.AddRecord(hii);
    hiscore.Show();
}

void XHero::ShowResistance(const std::optional<std::reference_wrapper<std::ofstream>> file)
{
    XGuiList list;
    list.SetCaption(MSG_BROWN "###" MSG_LIGHTGRAY " Resistances and Intrinsics " MSG_BROWN "###");
    list.SetFooter("Press any key to exit");

    int flag = 0;
    char buf[256];
    XResistance tr;

    for (int i = 0; i < R_EOF; i++) {
        const auto res = static_cast<RESISTANCE>(i);

        tr.SetResistance(res, GetResistance(res));

        if (tr.GetResistance(res) != 0) {
            sprintf(buf, MSG_LIGHTGRAY "%-15s%s", tr.GetResistanceName(res), tr.GetResistanceLevel(res));
            list.AddItem(new XGuiItem_Text(buf, 0), 0);
            flag = 1;
        }
    }

    if (flag == 0) {
        list.AddItem(new XGuiItem_Text("You have no Resistances and Intrinsics.", 0), 0);
    }

    if (file) {
        list.Put(file);
    } else {
        int ch = list.Run(1);
    }
}


int XHero::LearnReception(const POTION_NAME pn1, const POTION_NAME pn2, const POTION_NAME pn3)
{
    for (const auto it : reception_list)
        if (it->pn1 == pn1 && it->pn2 == pn2) {
            return 0;
        }

    if (XAlchemy::isValidReception(pn1, pn2, pn3)) {
        reception_list.push_back(new XAlchemyRec(pn1, pn2, pn3));
        msgwin.Add("You have learned a new alchemy recipe.");
    }

    return 1;
}

void XHero::ShowReception() const {
    XGuiList list;
    list.SetCaption(MSG_BROWN "###" MSG_LIGHTGRAY " Recipes " MSG_BROWN "###");

    if (reception_list.empty()) {
        list.AddItem(new XGuiItem_Text("You don't know any recipes yet.", 0), 0);
    } else {
        for (auto it: reception_list) {
            char buf[256];
            XAlchemy::GetReceptionName(buf, it->pn1, it->pn2, it->result);
            list.AddItem(new XGuiItem_Text(buf, 0), 0);
        }
    }

    list.Run();
}

void XHero::MixPotions()
{
    auto pot1 = dynamic_cast<XPotion *>(Inventory(&contain, IM_POTION, IF_FIXED_MASK, 1));

    if (pot1) {
        auto pot2 = dynamic_cast<XPotion *>(Inventory(&contain, IM_POTION, IF_FIXED_MASK, 1));

        if (pot2) {
            const POTION_NAME pn = XAlchemy::GetPotionName(pot1->pn, pot2->pn);
            const POTION_REC* pr = POTION_REC::GetRec(pot1->pn);
            int val = sk->GetLevel(XSkill::Skill::ALCHEMY) * 8 + 30 - pr->alchemy_power * 10;

            if (pn != PN_UNKNOWN && vRand(100) < val) {
                const auto pot = new XPotion(pn);
                sk->UseSkill(XSkill::Skill::ALCHEMY, 3);
                msgwin.Add(fmt::format("You have mixed {}.", pot->toString()));
                CarryItem(pot);
                contain.insert(pot);
            } else {
                msgwin.Add("You failed to mix a new potion.");
            }

            pot1->Invalidate();
            pot2->Invalidate();
        }
    }
}
