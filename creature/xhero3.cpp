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
        if (auto tool = dynamic_cast<XTool *>(tbp->Item())) {
            UseItem(tool);
            return 1;
        }
    }

    msgwin.Add("You have no tool.");

    return 0;
}

void XHero::doSacrifice()
{
    while (true) {
        std::shared_ptr<XItem> item;

        XItemList* tmpquae = l->map->GetItemList(x, y);

        if (tmpquae->empty() || l->map->GetPlace(x, y)) {
            item = Inventory(&contain, ItemKind::ALL, IF_HIDE_WORN);
        } else {
            item = Inventory(tmpquae, ItemKind::ALL, IF_HIDE_WORN);
        }

        std::shared_ptr<XItem> drop_item = item;

        if (item) {
            // One offering's worth of messages at a time. Cleared here,
            // after the choosing rather than before it, so the last
            // offering still reads while this one is picked - the same
            // place XHero::DropItem() clears. Without it the prompts and
            // the answers pile up across a whole armful until the two
            // message lines overflow into "(more)".
            msgwin.ClrMsg();

            if (item->quantity > 1) {
                XPoint pt(0, item->quantity);
                msgwin.Add("How much?");
                const int res = GetTarget(TR_HOW_MUCH, &pt, item->quantity);

                if (res == 0) {
                    contain.insert(item);
                    break;
                }

                if (res != item->quantity) {
                    drop_item = XItem::Own(item->MakeCopy());
                    drop_item->quantity = res;
                    item->quantity -= res;
                    contain.insert(item);
                }
            }

            Sacrifice(drop_item.get());
        } else {
            break;
        }
    }
}

std::shared_ptr<XItem> XHero::SelectItem(XItemFilter* filter, const bool isGetAll)
{
    return Inventory(&contain, ItemKind::UNKNOWN, IF_NONE, !isGetAll, filter);
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

    // One section per god a world declares, in the order it declared
    // them. What is on offer, and how much of it a follower of this
    // standing may ask for, comes from the god's own rows.
    struct Offer {
        DEITY deity;
        const DeityHelp* help;
    };

    std::vector<Offer> offers;

    for (const auto& row : deities_db) {
        const DeityRank* rank = religion.GetRank(row.id);
        const std::string rank_name = rank ? rank->name : std::string("unknown");

        list.AddItem(new XGuiItem_Text(fmt::format("<VALUE>{} <TEXT>({}<TEXT>)",
            row.name, rank_name), 0));

        const auto available = religion.AvailableHelp(row.id);

        if (available.empty()) {
            list.AddItem(new XGuiItem_Text("< No help available >", 0));
        } else {
            for (const DeityHelp* help : available) {
                list.AddItem(new XGuiItem_Text(help->name, 1));
                offers.push_back({row.id, help});
            }
        }

        list.AddItem(new XGuiItem_Text("", 0));
    }

    const int res = list.Run();

    if (res < 0 || static_cast<size_t>(res) >= offers.size()) {
        return;
    }

    religion.Pray(offers[res].deity, *offers[res].help, this);
}




void XHero::EndGame(const char* end_msg)
{
    // Static, so there is no `this` to write the tombstone for - it goes
    // through main_creature, whoever is standing in for the hero. In
    // "-demo" mode that is an ordinary creature (see SetMainCreature()),
    // and then there is no hero's game to end. Asked once here, so that
    // everything below is a hero by type rather than by assumption.
    XHero* hero = dynamic_cast<XHero*>(main_creature);

    if (!hero) {
        return;
    }

    unsigned long score = hero->experience + hero->MoneyOp(0);

    XGuiList list;

    const std::string title = fmt::format("{}, {} {} {} (L{}).",
        hero->name,
        hero->GetGenderStr(),
        hero->GetRaceStr(),
        hero->GetProfessionStr(),
        hero->level);

    list.AddItem(new XGuiItem_Text(title));

    list.AddItem(new XGuiItem_Text(fmt::format("You survived {} turns.", hero->turn_count)));

    // How the story ended, in one line.
    list.AddItem(new XGuiItem_Text(end_msg));

    int place_count = 0;

    for (const auto& [key, location] : Game.locations) {
        if (location && location->visited_by_hero) {
            place_count++;
        }
    }

    score += place_count * 200;
    list.AddItem(new XGuiItem_Text(fmt::format("You visited {} places.", place_count)));

    // Standing with each god a world declares, counted only where it
    // amounts to something - which is to say where the god has become
    // willing to do anything for them.
    int flag = 1;

    for (const auto& row : deities_db) {
        const DeityRank* rank = hero->religion.GetRank(row.id);

        if (!rank || hero->religion.AvailableHelp(row.id).empty()) {
            continue;
        }

        list.AddItem(new XGuiItem_Text(fmt::format("You were a {} of {}",
            rank->name, row.name)));

        score += rank->score;
        flag = 0;
    }

    if (flag) {
        list.AddItem(new XGuiItem_Text("You were not very religious."));
    }

    // Quests are defined world/quests.lua, with all messages and the score.
    // COMPLETE means the deed was done, CLOSED that it was done and reported;
    // a quest that has nothing to say about one of those leaves that string
    // empty, and the score is paid either way so that never going back to be
    // thanked does not cost the credit.
    for (const auto& quest: XQuest::quest.quests) {
        if (quest->status != XQuest::COMPLETE && quest->status != XQuest::CLOSED) {
            continue;
        }

        const std::string& line = quest->status == XQuest::CLOSED
            ? quest->closed
            : quest->complete;

        if (!line.empty()) {
            list.AddItem(new XGuiItem_Text(line));
        }

        score += quest->score;
    }

    list.AddItem(new XGuiItem_Text(fmt::format("You scored %{}.", score)));
    list.SetCaption("<DECORATION>###<TEXT> Achievements <DECORATION>###");
    list.Run();

    vGotoXY(0, 0);
    msgwin.ClrMsg();
    msgwin.Add("Create Memory File? (Y/<KEY>N<TEXT>)");

    vRefresh();

    if (const int tch = vGetch(); tch == 'y' || tch == 'Y') {
        msgwin.ClrMsg();
        msgwin.Add("### Screenshot ###");

        // Not name.append(): that would rename the hero as a side effect.
        const std::string filename = hero->name + ".mem";
        std::ofstream file(vMakePath(HOME_DIR, filename));
        DumpVBuffer(file);
        list.Put(file);
        file << "\n";
        hero->Equipment(file);
        file << "\n";
        hero->WarSkillsList(file);
        file << "\n";
        hero->SkillsList(SKF_LIST_SKILL, 0, file);
        file << "\n";
        hero->XCast(file);
        file << "\n";
        hero->ShowResistance(file);
        file << "\n";

        for (const auto& item : hero->contain) {
            item->Identify();
        }

        hero->Inventory(
            &hero->contain, ItemKind::ALL, IF_NONE, 0, nullptr, file);

        vClrScr();
    }

    const std::shared_ptr<XHiScoreItem> hii(new XHiScoreItem(0, score, title, end_msg, XQuest::quest.hero_win, 1));

    XHiScore hiscore;
    hiscore.AddRecord(hii);
    hiscore.Show();
}

void XHero::ShowResistance(const std::optional<std::reference_wrapper<std::ofstream>> file)
{
    XGuiList list;
    list.SetCaption("<DECORATION>###<TEXT> Resistances and Intrinsics <DECORATION>###");
    list.SetFooter("Press any key to exit");

    int flag = 0;
    XResistance tr;

    // Walked in the order world/resistances.lua declares them, so the list
    // reads the same way every time rather than in whatever order a map
    // happens to hold.
    for (const auto& row : resistances_db) {
        tr.SetResistance(row.id, GetResistance(row.id));

        if (tr.GetResistance(row.id) != 0) {
            auto res_str = fmt::format("<TEXT>{:<15}{}",
                XResistance::GetResistanceName(row.id), tr.GetResistanceLevel(row.id));
            list.AddItem(new XGuiItem_Text(res_str, 0), 0);
            flag = 1;
        }
    }

    if (flag == 0) {
        list.AddItem(new XGuiItem_Text("You have no Resistances and Intrinsics.", 0), 0);
    }

    if (file) {
        list.Put(file);
    } else {
        list.Run(1);
    }
}


int XHero::LearnRecipe(const PotionName pn1, const PotionName pn2, const PotionName pn3)
{
    for (const auto& it : recipe_list)
        if (it->pn1 == pn1 && it->pn2 == pn2) {
            return 0;
        }

    if (XAlchemy::isValidRecipe(pn1, pn2, pn3)) {
        recipe_list.push_back(std::make_unique<XAlchemyRecipe>(pn1, pn2, pn3));
        msgwin.Add("You have learned a new alchemy recipe.");
    }

    return 1;
}

void XHero::ShowRecipes() const {
    XGuiList list;
    list.SetCaption("<DECORATION>###<TEXT> Recipes <DECORATION>###");

    if (recipe_list.empty()) {
        list.AddItem(new XGuiItem_Text("You don't know any recipes yet.", 0), 0);
    } else {
        for (auto& it: recipe_list) {
            auto recipe = XAlchemy::GetRecipeName(it->pn1, it->pn2, it->result);
            list.AddItem(new XGuiItem_Text(recipe, 0), 0);
        }
    }

    list.Run();
}

void XHero::MixPotions()
{
    auto item1 = Inventory(&contain, ItemKind::POTION, IF_FIXED_MASK, 1);
    auto pot1 = dynamic_cast<XPotion *>(item1.get());

    if (pot1) {
        auto item2 = Inventory(&contain, ItemKind::POTION, IF_FIXED_MASK, 1);
        auto pot2 = dynamic_cast<XPotion *>(item2.get());

        if (pot2) {
            const PotionName pn = XAlchemy::GetPotionName(pot1->pn, pot2->pn);
            const PotionDescription* pr = PotionDescription::GetRec(pot1->pn);
            int val = sk->GetLevel(XSkill::Skill::ALCHEMY) * 8 + 30 - pr->alchemy_power * 10;

            if (pn != PN_NONE && vRand(100) < val) {
                const auto pot = new XPotion(pn);
                sk->UseSkill(XSkill::Skill::ALCHEMY, 3);
                msgwin.Add(fmt::format("You have mixed {}.", pot->toString()));
                CarryItem(pot);
                contain.insert(XItem::Own(pot));
            } else {
                msgwin.Add("You failed to mix a new potion.");
            }

            pot1->Invalidate();
            pot2->Invalidate();
        }
    }
}
