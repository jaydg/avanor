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

#include <iostream>
#include <vector>

#include <sol/sol.hpp>

#include "creature/xhero.h"
#include "engine/xlua.h"
#include "item/itemf.h"
#include "item/xtool.h"

namespace {

// Draws a lettered menu and waits for one of its letters to be pressed.
//
// The menu is drawn inside the loop rather than before it, so that a
// terminal which changes shape while somebody is choosing gets the menu
// laid out again for the size it now is. Everything else on screen is
// redrawn from scratch each time round, which costs nothing here and
// means the resize needs no special case beyond falling through to the
// next turn of the loop.
int ChooseFromMenu(const char* title, const std::vector<std::string>& names)
{
    while (true) {
        vClrScr();
        vGotoXY(7, 4);
        vPutS(title);

        for (std::size_t i = 0; i < names.size(); i++) {
            vGotoXY(7, 6 + static_cast<int>(i));
            vPutS(fmt::format("<TEXT>[<SELECTOR>{:c}<TEXT>] {} ",
                              static_cast<int>(i) + 'a', names[i]));
        }

        vRefresh();

        if (const int ch = vGetch();
            ch >= 'a' && ch < 'a' + static_cast<int>(names.size())) {
            vClrScr();

            return ch;
        }
    }
}

} // namespace

void XHero::PlayerSetup()
{
    sol::state_view lua(XLua::State());

    // The whole of the offer in one answer: every race, and for each of
    // them the professions it allows. The engine builds menus from this and
    // decides none of it - world/hero.lua does.
    sol::table races = lua["HeroRaces"]();
    std::vector<std::string> race_labels;

    for (auto& [_, row] : races) {
        race_labels.push_back(row.as<sol::table>()["name"]);
    }

    const int race_choice = ChooseFromMenu("<TEXT>Choose a race:", race_labels);
    sol::table chosen_race = races[race_choice - 'a' + 1];
    race = chosen_race["key"];
    race_name = chosen_race["name"];

    sol::table genders = lua["HeroGenders"]();
    std::vector<std::string> gender_labels;

    for (auto& [_, row] : genders) {
        gender_labels.push_back(row.as<sol::table>()["name"]);
    }

    const int gender_choice = ChooseFromMenu("<TEXT>Choose a gender:", gender_labels);
    sol::table chosen_gender = genders[gender_choice - 'a' + 1];
    const std::string gender_key = chosen_gender["key"];

    // The pronoun comes from the entry rather than from its place in the
    // menu, so a world may offer these in any order or offer more of them.
    // YOU is added here and not there: being addressed in the second person
    // is what makes this creature the player, which is the engine's business
    // and not the world's.
    const int pronoun = chosen_gender["pronoun"];
    creature_person_type = static_cast<XCreature::PersonType>(pronoun | XCreature::YOU);

    // Only what this race may take up.
    sol::table professions = chosen_race["professions"];
    std::vector<std::string> profession_labels;

    for (auto& [_, row] : professions) {
        profession_labels.push_back(row.as<sol::table>()["name"]);
    }

    {
        const int ch = ChooseFromMenu("<TEXT>Choose a profession:", profession_labels);
        sol::table chosen_profession = professions[ch - 'a' + 1];
        profession = chosen_profession["key"];
        profession_name = chosen_profession["name"];

        // Everything those three choices mean - the figures, the pace, the
        // stomach, what the race simply knows - belongs to content.
        if (sol::protected_function init = lua["InitHero"]; init.valid()) {
            if (const auto result = init((void*)this, race, gender_key, profession);
                !result.valid()) {
                const sol::error err = result;
                std::cerr << "world: InitHero: " << err.what() << std::endl;
            }
        }

        vClrScr();
    }

    vClrScr();
    vGotoXY(0, 4);
    vPutS("<TEXT>Enter character name (15 letters max.): ");
    vRefresh();
    char char_name[20];
    vGetS(char_name, 15);
    name = char_name;
}

const char* XHero::GetRaceStr() const
{
    return race_name.c_str();
}

const char* XHero::GetProfessionStr() const
{
    return profession_name.c_str();
}
