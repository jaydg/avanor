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

// The character sheet: everything the player can look up about their own
// hero. All read-only - these draw a screen, wait for a key, and put back
// what was underneath. Each also takes an optional stream, which is how
// the memorial file at the end of a game is written.

#include <fmt/format.h>

#include "creature/xhero.h"
#include "game/game.h"
#include "game/xtime.h"
#include "helpers/manual.h"
#include "helpers/msgwin.h"
#include "item/item_misc.h"
#include "item/xpotion.h"
#include "magic/modifier.h"
#include "map/map_objects.h"

const char* wsk_levels_name[] = {
    "unskilled",
    "basic", "basic", "basic",
    "advanced", "advanced", "advanced", "advanced",
    "expert", "expert", "expert", "expert",
    "master", "master", "master",
    "grand master"
};

void XHero::InfoList()
{
    V_BUFFER xbuf;
    vStore(&xbuf);
    vClrScr();

    vGotoXY(0, 0);
    vPutS("<DECORATION>--------- <TEXT>Attributes <DECORATION>---------");
    vGotoXY(0, 1);
    vPutS("<TEXT>Attribute  Base  Max  Add  Total");
    vGotoXY(0, 2);
    vPutS("<DECORATION>--------------------------------");
    int i = 0;

    for (i = 0; i < XStats::COUNT; i++) {
        vGotoXY(0, i + 3);
        vSetAttr(xBROWN);
        vPutS(stats->GetFullName(static_cast<XStats::Id>(i)));
        auto stat = fmt::format("{}", stats->Get(static_cast<XStats::Id>(i)));
        vSetAttr(xLIGHTGRAY);
        vGotoXY(15 - stat.size(), 3 + i);
        vPutS(stat);
        stat = fmt::format("{}", max_stats.Get(static_cast<XStats::Id>(i)));
        vSetAttr(xBROWN);
        vGotoXY(20 - stat.size(), 3 + i);
        vPutS(stat);

        int tres = added_stats.Get(static_cast<XStats::Id>(i));

        if (tres != 0) {
            stat = fmt::format("{:+}", tres);
            if (tres > 0) {
                vSetAttr(xLIGHTGREEN);
            } else {
                vSetAttr(xRED);
            }

            vGotoXY(26 - stat.size(), 3 + i);
            vPutS(stat);
        }

        tres = stats->Get(static_cast<XStats::Id>(i)) + added_stats.Get(static_cast<XStats::Id>(i));

        if (tres <= 0) {
            tres = 1;
        }

        stat = fmt::format("{}", tres);
        vSetAttr(xYELLOW);
        vGotoXY(32 - stat.size(), 3 + i);
        vPutS(stat);
    }

    vGotoXY(40, 0);
    vPutS("<DECORATION>--------- <TEXT>Miscellaneous <DECORATION>---------");
    vGotoXY(40, 1);
    vPutS("<LABEL>Name:<VALUE>");
    vGotoXY(50, 1);
    vPutS(name);
    vGotoXY(40, 2);
    vPutS("<LABEL>Gender:<VALUE>");
    vGotoXY(50, 2);
    vPutS(GetGenderStr());
    vGotoXY(40, 3);
    vPutS("<LABEL>Race:<VALUE>");
    vGotoXY(50, 3);
    vPutS(GetRaceStr());
    vGotoXY(40, 4);
    vPutS("<LABEL>Class:<VALUE>");
    vGotoXY(50, 4);
    vPutS(GetProfessionStr());
    vGotoXY(40, 5);
    vPutS("<LABEL>Gold:");
    vGotoXY(50, 5);
    vPutS(fmt::format("<VALUE>{}<DECORATION> gp", MoneyOp(0)));
    vGotoXY(40, 6);
    vPutS("<LABEL>Time:");
    vGotoXY(50, 6);
    vPutS(fmt::format("<VALUE>{}<DECORATION> : <VALUE>{}<DECORATION> : "
        "<VALUE>{}", XTime::GetHour(), XTime::GetMin(), XTime::GetSec()));
    vGotoXY(40, 7);
    vPutS("<LABEL>Date:");
    vGotoXY(50, 7);
    vPutS(fmt::format("<VALUE>{}<DECORATION>, <VALUE>{}<DECORATION>, "
        "<VALUE>{}", XTime::GetMonthName(), XTime::GetDay(), XTime::GetYear()));
    vGotoXY(40, 8);
    vPutS("<LABEL>Day/Week");
    vGotoXY(50, 8);
    vPutS(fmt::format("<VALUE>{}<DECORATION>/<VALUE>{}",
        XTime::GetDayName(), XTime::GetWeekName()));

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
    vPutS(fmt::format("<TEXT>Burden<DECORATION>: current <VALUE>{}"
        "<DECORATION>, burdened <VALUE>{}<DECORATION>, strained "
        "<VALUE>{}<DECORATION>, overburdened <VALUE>{}",
        carried_weight,
        CarryValue(CSTATE_NORMAL),
        CarryValue(CSTATE_BURDENED),
        CarryValue(CSTATE_STRAINED)));

    vGotoXY(0, 15);
    vPutS("<TEXT>Melee Attack");

    vGotoXY(0, 16);
    vPutS(fmt::format("<LABEL>Unarmed:    (<VALUE>{:+}<DECORATION>, "
        "<VALUE>{}<DECORATION>d<VALUE>{} {:+}<DECORATION>)",
        GetHIT() + wsk->GetHIT(wsk->Best(CombatRole::UNARMED)),
        dice.GetCount(), dice.GetSides(), dice.GetBonus() + GetDMG() + wsk->GetDMG(wsk->Best(CombatRole::UNARMED))));

    const XBodyPart* hand_1 = GetBodyPart(BP_HAND, 0);
    const XBodyPart* hand_2 = GetBodyPart(BP_HAND, 1);

    if (hand_1->Item() && hand_1->Item()->kind & ItemKind::WEAPON) {
        vGotoXY(0, 17);
        vPutS(fmt::format("Left hand:  (<VALUE>{:+}<DECORATION>, "
            "<VALUE>{}<DECORATION>d<VALUE>{}{:+}<DECORATION>)",
            GetHIT() + wsk->GetHIT(hand_1->Item()->wt) + GetUnwieldyHITPenalty(hand_1->Item()),
            hand_1->Item()->dice.GetCount(), hand_1->Item()->dice.GetSides(),
            hand_1->Item()->dice.GetBonus() + GetDMG() + wsk->GetDMG(hand_1->Item()->wt)
                + GetUnwieldyDMGPenalty(hand_1->Item())));
    }

    if (hand_2->Item() && hand_2->Item()->kind & ItemKind::WEAPON) {
        vGotoXY(0, 18);
        vPutS(fmt::format("Right hand: (<VALUE>{:+}<DECORATION>, "
            "<VALUE>{}<DECORATION>d<VALUE>{} {:+}<DECORATION>)",
            GetHIT() + wsk->GetHIT(hand_2->Item()->wt) + GetUnwieldyHITPenalty(hand_2->Item()),
            hand_2->Item()->dice.GetCount(), hand_2->Item()->dice.GetSides(),
            hand_2->Item()->dice.GetBonus() + GetDMG() + wsk->GetDMG(hand_2->Item()->wt)
                + GetUnwieldyDMGPenalty(hand_2->Item())));
    }

    int hit;
    int range;
    XDice dmg;
    GetRangeAttackInfo(&range, &hit, &dmg);

    if (range > 0) {
        vGotoXY(0, 19);
        vPutS(fmt::format("Range Attack: <<VALUE>{}<DECORATION>> ("
            "<VALUE>{:+}<DECORATION>, <VALUE>{}<DECORATION>d"
            "<VALUE>{} {:+}<DECORATION>)", range, hit, dmg.GetCount(), dmg.GetSides(), dmg.GetBonus()));
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

    if (level >= 50) {
        vGotoXY(0, 0);
        vPutS("\x1F\0FYou need unknown points of experience!");
    } else {
        int i = level;
        int xx = 0;
        int yy = 0;

        while (i < 50) {
            vGotoXY(xx, yy);
            vPutS(fmt::format("\x1F\x06Level(\x1F\x0E{}\x1F\x06): \x1F\x0E{}", i + 1, ExpOfLevel(i)));
            yy++;

            if (yy > 17) {
                xx += 26;
                yy = 0;
            }

            i++;
        }

        vGotoXY(0, 20);
        vPutS(fmt::format("\x1F\x06You need \x1F\x0E{}\x1F\x06 experience to next level.",
            ExpOfLevel(level) - experience));
    }

    vGotoXY(0, 22);
    vPutS("Press any key to exit.");
    vRefresh();
    vGetch();
    vRestore(&xbuf);
}

void XHero::MagicLevelList() const
{
    XGuiList list;
    list.SetCaption("<DECORATION>###<TEXT> Magic School <DECORATION>###");

    list.AddItem(new XGuiItem_Text(
        fmt::format("<LABEL>{:<30} {:<14}  {}", "School", "Rank", "Spells Cast"), 0), 0);

    // In the order world/spells.lua declares them, so a world that adds
    // a school gets it listed here without a line changing.
    for (const auto& row : AllMagicSchools()) {
        const MAGIC_SCHOOL& school = row.id;
        auto s = m->LevelToString(school);

        if (s.empty()) {
            continue;
        }

        const int level = m->GetLevel(school);

        // Rank name is the last thing LevelToString() wrote, colored and
        // unpadded - pad it out here (accounting for its own color tag,
        // which GetLevelNameLength() already skips) so the progress
        // column that follows lines up regardless of which rank it is.
        constexpr int kRankColumnWidth = 14;

        if (const int pad = kRankColumnWidth - XMagic::GetLevelNameLength(level); pad > 0) {
            s += std::string(pad, ' ');
        }

        const std::string progress = level >= XMagic::MAX_LEVEL
            ? "<DECORATION>[<TEXT>mastered<DECORATION>]"
            : fmt::format("<DECORATION>[<TEXT>{:>4}<DECORATION>/<TEXT>{:<4}<DECORATION>]",
                          m->GetCount(school), m->GetNextLevelAt(school) + 1);

        list.AddItem(new XGuiItem_SimpleSelect(s + "  " + progress), 0);
    }

    list.Run();
}

XSkill* XHero::SkillsList(const SKILL_FLAG skill_flag, const int marks_left, std::optional<std::reference_wrapper<std::ofstream>> file) const
{
    while (1) {
        XGuiList list;

        if (skill_flag == SKF_IMPROVE_SKILL) {
            list.SetCaption("<DECORATION>###<TEXT> Improve Skill <DECORATION>###");

            const std::string footer = fmt::format(
                "<TEXT>You have <VALUE>{}<TEXT> {} left.",
                marks_left,
                marks_left > 1 ? "improvements" : "improvement");

            list.SetFooter(footer.c_str());
        } else if (skill_flag == SKF_LIST_SKILL) {
            list.SetCaption("<DECORATION>###<VALUE> Skills List <DECORATION>###");
        } else if (skill_flag == SKF_USE_SKILL) {
            list.SetCaption("<DECORATION>###<VALUE> Use Skill <DECORATION>###");
        }

        for (const auto& [skt, skill] : sk->skills) {
            // Fill name up to 17 characters (excluding ANSI prefix) with '.'.
            // "<TEXT>" is an ANSI escape prefix, and does not add to the visual width.
            const std::string_view raw_name = skill->GetName();
            const size_t visible_len = raw_name.size();
            const size_t pad = visible_len < 17 ? 17 - visible_len : 0;

            const std::string name_col = fmt::format(
                "<TEXT>{}{}", raw_name, std::string(pad, '.'));

            // Skill level column: colorful when still improvable, grey when maxed
            const std::string level_col = skill->GetLevel() < skill->GetMaxLevel()
                ? fmt::format("<DECORATION>[<TEXT> {:2} <DECORATION>from<TEXT> {:2} <DECORATION>] ",
                              skill->GetLevel(), skill->GetMaxLevel())
                : fmt::format("<DECORATION>[ {:2} from {:2} ] ",
                              skill->GetLevel(), skill->GetMaxLevel());

            const std::string buf = name_col + level_col + skill->GetSkillLevel();
            list.AddItem(new XGuiItem_SimpleSelect(buf), 0);
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
                        return skill.get();
                    }
                }
            }
        }

    }
}

void XHero::WarSkillsList(std::optional<std::reference_wrapper<std::ofstream>> file) const
{
    XGuiList list;
    list.SetCaption("<DECORATION>###<TEXT> Weapon Skills <DECORATION>###");

    // Walked group by group, in the order the headings read, rather than
    // by position in a fixed list: which skills exist and which group each
    // belongs to are both content's word now (world/combat_skills.lua).
    const struct {
        CombatSkillStats::Group group;
        const char* heading;
    } sections[] = {
        {CombatSkillStats::Group::MELEE,
         "<LABEL>Melee Weapon         DV  HIT  DMG      Level          required marks"},
        {CombatSkillStats::Group::MISSILE,
         "<LABEL>Missile Weapon       RNG HIT  DMG      Level          required marks"},
        {CombatSkillStats::Group::SHIELD,
         "<LABEL>Shields              DV                Level          required marks"},
    };

    bool first_section = true;

    for (const auto& section : sections) {
        bool printed_heading = false;

        for (const auto& row : combat_skills) {
            if (row.group != section.group) {
                continue;
            }

            // The heading waits until something is actually under it, so a
            // world with no shields grows no empty "Shields" section.
            if (!printed_heading) {
                if (!first_section) {
                    list.AddItem(new XGuiItem_Text("", 0), 0);
                }

                list.AddItem(new XGuiItem_Text(section.heading, 0), 0);
                printed_heading = true;
                first_section = false;
            }

            const COMBAT_SKILL& w_skill = row.id;
            const int level = wsk->GetLevel(w_skill);

            auto str = fmt::format(
                "<VALUE>{:<18} <TEXT>{:+4} {:+4} {:+4}      "
                "<DECORATION>[<TEXT>{}<DECORATION>]"
                "<TEXT> {:<10} <TEXT>{:8}",
                wsk->GetName(w_skill),
                wsk->GetDV(w_skill), wsk->GetHIT(w_skill), wsk->GetDMG(w_skill),
                level,
                wsk_levels_name[level],
                (level < XCombatSkills::MAX_LEVEL)
                    ? fmt::format("{}", wsk->GetMarks(w_skill)) : "NaN"
            );

            list.AddItem(new XGuiItem_Text(str, 0), 0);
        }
    }

    if (file) {
        list.Put(file);
    } else {
        list.Run();
    }
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
