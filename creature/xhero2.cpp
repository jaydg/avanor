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
    creature_person_type = gender_choice == 'a' ? XCreature::MALE_YOU : XCreature::FEMALE_YOU;

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

        {
            vClrScr();

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
                            pbp->Wear(ICREATEB(ItemKind::WEAPON, ItemType::LONGSWORD, 1, 100));

                            pbp = GetBodyPart(BP_HAND, 1);
                            pbp->Wear(ICREATEB(ItemKind::SHIELD, ItemType::SMALLSHIELD, 1, 100));
                            break;

                        case 'b':
                            pbp = GetBodyPart(BP_HAND, 0);
                            pbp->Wear(ICREATEB(ItemKind::WEAPON, ItemType::RAPIER, 1, 100));

                            pbp = GetBodyPart(BP_BOOTS, 0);
                            pbp->Wear(ICREATEB(ItemKind::BOOTS, ItemType::SANDALS, 1, 20));
                            break;

                        case 'c':
                            pbp = GetBodyPart(BP_HAND, 0);
                            pbp->Wear(ICREATEB(ItemKind::WEAPON, ItemType::RAPIER, 1, 100));

                            pbp = GetBodyPart(BP_CLOAK, 0);
                            pbp->Wear(ICREATEB(ItemKind::CLOAK, ItemType::LIGHTCLOAK, 1, 50));

                            pbp = GetBodyPart(BP_BOOTS, 0);
                            pbp->Wear(ICREATEB(ItemKind::BOOTS, ItemType::SANDALS, 1, 20));
                            break;

                        case 'd':
                            pbp = GetBodyPart(BP_HAND, 0);
                            pbp->Wear(ICREATEB(ItemKind::WEAPON, ItemType::SHORTSWORD, 1, 100));

                            pbp = GetBodyPart(BP_CLOAK, 0);
                            pbp->Wear(ICREATEB(ItemKind::CLOAK, ItemType::LIGHTCLOAK, 1, 50));

                            pbp = GetBodyPart(BP_BOOTS, 0);
                            pbp->Wear(ICREATEB(ItemKind::BOOTS, ItemType::LIGHTBOOTS, 1, 40));

                            sk->Learn(XSkill::Skill::DISARMTRAP);
                            break;

                        case 'e':
                            pbp = GetBodyPart(BP_HAND, 0);
                            pbp->Wear(ICREATEB(ItemKind::WEAPON, ItemType::ORCISHAXE, 1, 100));

                            pbp = GetBodyPart(BP_HAND, 1);
                            pbp->Wear(ICREATEB(ItemKind::SHIELD, ItemType::MEDIUMSHIELD, 1, 100));
                            break;

                        case 'f':
                            pbp = GetBodyPart(BP_HAND, 0);
                            pbp->Wear(ICREATEB(ItemKind::WEAPON, ItemType::BATTLEAXE, 1, 100));

                            pbp = GetBodyPart(BP_HAND, 1);
                            pbp->Wear(ICREATEB(ItemKind::SHIELD, ItemType::MEDIUMSHIELD, 1, 100));
                            break;

                        case 'g':
                            pbp = GetBodyPart(BP_HAND, 0);
                            pbp->Wear(ICREATEB(ItemKind::WEAPON, ItemType::WARAXE, 1, 100));

                            pbp = GetBodyPart(BP_HAND, 1);
                            pbp->Wear(ICREATEB(ItemKind::SHIELD, ItemType::SMALLSHIELD, 1, 100));
                            break;

                        default:
                            assert(0);
                    }

                    potion = new XPotion(PotionName::CURE_LIGHT_WOUNDS);
                    potion->Identify();
                    ContainItem(potion);

                    potion = new XPotion(PotionName::HEROISM);
                    potion->Identify();
                    ContainItem(potion);

                    sk->Learn(XSkill::Skill::FINDWEAKNESS);
                    sk->Learn(XSkill::Skill::HEALING);
                    sk->Learn(XSkill::Skill::TACTICS);
                    sk->Learn(XSkill::Skill::ATHLETICS);

                    break;

                //************** wizard
                case 'b' : {
                    pbp = GetBodyPart(BP_HAND, 0);
                    pbp->Wear(ICREATEB(ItemKind::WEAPON, ItemType::STAFF, 1, 100));

                    potion = new XPotion(PotionName::POWER);
                    potion->Identify();
                    ContainItem(potion);

                    for (int t = 1; t < 3; t++) {
                        if (vRand() % 2 == 1) {
                            scroll = new XScroll(ScrollName::FIRE_BOLT);
                        } else {
                            scroll = new XScroll(ScrollName::ICE_BOLT);
                        }

                        scroll->Identify();
                        ContainItem(scroll);
                    }

                    if (vRand() % 2 == 1) {
                        book = new XBook(BOOK_FIRE_BOLT);
                    } else {
                        book = new XBook(BOOK_ICE_BOLT);
                    }

                    book->Identify();
                    ContainItem(book);

                    book = dynamic_cast<XBook *>(ICREATEA(ItemKind::BOOK));
                    book->Identify();
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
                            pbp->Wear(ICREATEB(ItemKind::MISSILEW, ItemType::CROSSBOW, 1, 100));

                            pbp = GetBodyPart(BP_MISSILE, 0);
                            pbp->Wear(ICREATEB(ItemKind::MISSILE, ItemType::QUARREL, 1, 100));
                            pbp->Item()->quantity = vRand() % 10 + 10;

                            pbp = GetBodyPart(BP_HAND, 0);
                            pbp->Wear(ICREATEB(ItemKind::WEAPON, ItemType::DAGGER, 1, 100));
                            break;

                        case 'b':
                            pbp = GetBodyPart(BP_MISSILE_WEAPON, 0);
                            pbp->Wear(ICREATEB(ItemKind::MISSILEW, ItemType::LONGBOW, 1, 100));

                            pbp = GetBodyPart(BP_MISSILE, 0);
                            pbp->Wear(ICREATEB(ItemKind::MISSILE, ItemType::ARROW, 1, 100));
                            pbp->Item()->quantity = vRand() % 10 + 10;

                            pbp = GetBodyPart(BP_HAND, 0);
                            pbp->Wear(ICREATEB(ItemKind::WEAPON, ItemType::DAGGER, 1, 100));

                            pbp = GetBodyPart(BP_BOOTS, 0);
                            pbp->Wear(ICREATEB(ItemKind::BOOTS, ItemType::SANDALS, 1, 20));
                            break;

                        case 'c':
                            pbp = GetBodyPart(BP_MISSILE_WEAPON, 0);
                            pbp->Wear(ICREATEB(ItemKind::MISSILEW, ItemType::LONGBOW, 1, 100));

                            pbp = GetBodyPart(BP_MISSILE, 0);
                            pbp->Wear(ICREATEB(ItemKind::MISSILE, ItemType::ARROW, 1, 100));
                            pbp->Item()->quantity = vRand() % 10 + 10;

                            pbp = GetBodyPart(BP_HAND, 0);
                            pbp->Wear(ICREATEB(ItemKind::WEAPON, ItemType::RAPIER, 1, 100));

                            pbp = GetBodyPart(BP_CLOAK, 0);
                            pbp->Wear(ICREATEB(ItemKind::CLOAK, ItemType::LIGHTCLOAK, 1, 50));

                            pbp = GetBodyPart(BP_BOOTS, 0);
                            pbp->Wear(ICREATEB(ItemKind::BOOTS, ItemType::SANDALS, 1, 20));
                            break;

                        case 'd':
                            pbp = GetBodyPart(BP_MISSILE_WEAPON, 0);
                            pbp->Wear(ICREATEB(ItemKind::MISSILEW, ItemType::SLING, 1, 100));

                            pbp = GetBodyPart(BP_MISSILE, 0);
                            pbp->Wear(ICREATEB(ItemKind::MISSILE, ItemType::SLINGBULLET, 1, 100));
                            pbp->Item()->quantity = vRand() % 10 + 10;

                            pbp = GetBodyPart(BP_CLOAK, 0);
                            pbp->Wear(ICREATEB(ItemKind::CLOAK, ItemType::LIGHTCLOAK, 1, 50));

                            pbp = GetBodyPart(BP_BOOTS, 0);
                            pbp->Wear(ICREATEB(ItemKind::BOOTS, ItemType::LIGHTBOOTS, 1, 40));
                            break;

                        case 'e':
                            pbp = GetBodyPart(BP_MISSILE_WEAPON, 0);
                            pbp->Wear(ICREATEB(ItemKind::MISSILEW, ItemType::HEAVYCROSSBOW, 1, 100));

                            pbp = GetBodyPart(BP_MISSILE, 0);
                            pbp->Wear(ICREATEB(ItemKind::MISSILE, ItemType::QUARREL, 1, 100));
                            pbp->Item()->quantity = vRand() % 10 + 10;

                            pbp = GetBodyPart(BP_HAND, 0);
                            pbp->Wear(ICREATEB(ItemKind::WEAPON, ItemType::ORCISHDAGGER, 1, 100));
                            break;

                        case 'f':
                            pbp = GetBodyPart(BP_MISSILE_WEAPON, 0);
                            pbp->Wear(ICREATEB(ItemKind::MISSILEW, ItemType::HEAVYCROSSBOW, 1, 100));

                            pbp = GetBodyPart(BP_MISSILE, 0);
                            pbp->Wear(ICREATEB(ItemKind::MISSILE, ItemType::QUARREL, 1, 100));
                            pbp->Item()->quantity = vRand() % 10 + 10;

                            pbp = GetBodyPart(BP_HAND, 0);
                            pbp->Wear(ICREATEB(ItemKind::WEAPON, ItemType::WARAXE, 1, 100));
                            break;

                        case 'g':
                            pbp = GetBodyPart(BP_MISSILE_WEAPON, 0);
                            pbp->Wear(ICREATEB(ItemKind::MISSILEW, ItemType::LIGHTCROSSBOW, 1, 100));

                            pbp = GetBodyPart(BP_MISSILE, 0);
                            pbp->Wear(ICREATEB(ItemKind::MISSILE, ItemType::QUARREL, 1, 100));
                            pbp->Item()->quantity = vRand() % 10 + 10;

                            pbp = GetBodyPart(BP_HAND, 0);
                            pbp->Wear(ICREATEB(ItemKind::WEAPON, ItemType::WARAXE, 1, 100));
                            break;

                        default:
                            assert(0);
                    }

                    potion = new XPotion(PotionName::CURE_LIGHT_WOUNDS);
                    potion->Identify();
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
                            pbp->Wear(ICREATEB(ItemKind::MISSILEW, ItemType::CROSSBOW, 1, 100));

                            pbp = GetBodyPart(BP_MISSILE, 0);
                            pbp->Wear(ICREATEB(ItemKind::MISSILE, ItemType::QUARREL, 1, 100));
                            pbp->Item()->quantity = vRand() % 10 + 10;

                            pbp = GetBodyPart(BP_HAND, 0);
                            pbp->Wear(ICREATEB(ItemKind::WEAPON, ItemType::DAGGER, 1, 100));
                            break;

                        case 'b':
                            pbp = GetBodyPart(BP_MISSILE_WEAPON, 0);
                            pbp->Wear(ICREATEB(ItemKind::MISSILEW, ItemType::LONGBOW, 1, 100));

                            pbp = GetBodyPart(BP_MISSILE, 0);
                            pbp->Wear(ICREATEB(ItemKind::MISSILE, ItemType::ARROW, 1, 100));
                            pbp->Item()->quantity = vRand() % 10 + 10;

                            pbp = GetBodyPart(BP_HAND, 0);
                            pbp->Wear(ICREATEB(ItemKind::WEAPON, ItemType::DAGGER, 1, 100));

                            pbp = GetBodyPart(BP_BOOTS, 0);
                            pbp->Wear(ICREATEB(ItemKind::BOOTS, ItemType::SANDALS, 1, 20));
                            break;

                        case 'c':
                            pbp = GetBodyPart(BP_MISSILE_WEAPON, 0);
                            pbp->Wear(ICREATEB(ItemKind::MISSILEW, ItemType::LONGBOW, 1, 100));

                            pbp = GetBodyPart(BP_MISSILE, 0);
                            pbp->Wear(ICREATEB(ItemKind::MISSILE, ItemType::ARROW, 1, 100));
                            pbp->Item()->quantity = vRand() % 10 + 10;

                            pbp = GetBodyPart(BP_HAND, 0);
                            pbp->Wear(ICREATEB(ItemKind::WEAPON, ItemType::RAPIER, 1, 100));

                            pbp = GetBodyPart(BP_CLOAK, 0);
                            pbp->Wear(ICREATEB(ItemKind::CLOAK, ItemType::LIGHTCLOAK, 1, 50));

                            pbp = GetBodyPart(BP_BOOTS, 0);
                            pbp->Wear(ICREATEB(ItemKind::BOOTS, ItemType::SANDALS, 1, 20));
                            break;

                        case 'd':
                            pbp = GetBodyPart(BP_MISSILE_WEAPON, 0);
                            pbp->Wear(ICREATEB(ItemKind::MISSILEW, ItemType::SLING, 1, 100));

                            pbp = GetBodyPart(BP_MISSILE, 0);
                            pbp->Wear(ICREATEB(ItemKind::MISSILE, ItemType::SLINGBULLET, 1, 100));
                            pbp->Item()->quantity = vRand() % 10 + 10;

                            pbp = GetBodyPart(BP_HAND, 0);
                            pbp->Wear(ICREATEB(ItemKind::WEAPON, ItemType::LONGDAGGER, 1, 100));

                            pbp = GetBodyPart(BP_CLOAK, 0);
                            pbp->Wear(ICREATEB(ItemKind::CLOAK, ItemType::LIGHTCLOAK, 1, 50));

                            pbp = GetBodyPart(BP_BOOTS, 0);
                            pbp->Wear(ICREATEB(ItemKind::BOOTS, ItemType::LIGHTBOOTS, 1, 40));
                            break;

                        case 'e':
                            pbp = GetBodyPart(BP_MISSILE_WEAPON, 0);
                            pbp->Wear(ICREATEB(ItemKind::MISSILEW, ItemType::HEAVYCROSSBOW, 1, 100));

                            pbp = GetBodyPart(BP_MISSILE, 0);
                            pbp->Wear(ICREATEB(ItemKind::MISSILE, ItemType::QUARREL, 1, 100));
                            pbp->Item()->quantity = vRand() % 10 + 10;

                            pbp = GetBodyPart(BP_HAND, 0);
                            pbp->Wear(ICREATEB(ItemKind::WEAPON, ItemType::ORCISHDAGGER, 1, 100));
                            break;

                        case 'f':
                            pbp = GetBodyPart(BP_MISSILE_WEAPON, 0);
                            pbp->Wear(ICREATEB(ItemKind::MISSILEW, ItemType::HEAVYCROSSBOW, 1, 100));

                            pbp = GetBodyPart(BP_MISSILE, 0);
                            pbp->Wear(ICREATEB(ItemKind::MISSILE, ItemType::QUARREL, 1, 100));
                            pbp->Item()->quantity = vRand() % 10 + 10;

                            pbp = GetBodyPart(BP_HAND, 0);
                            pbp->Wear(ICREATEB(ItemKind::WEAPON, ItemType::WARAXE, 1, 100));
                            break;

                        case 'g':
                            pbp = GetBodyPart(BP_MISSILE_WEAPON, 0);
                            pbp->Wear(ICREATEB(ItemKind::MISSILEW, ItemType::LIGHTCROSSBOW, 1, 100));

                            pbp = GetBodyPart(BP_MISSILE, 0);
                            pbp->Wear(ICREATEB(ItemKind::MISSILE, ItemType::QUARREL, 1, 100));
                            pbp->Item()->quantity = vRand() % 10 + 10;

                            pbp = GetBodyPart(BP_HAND, 0);
                            pbp->Wear(ICREATEB(ItemKind::WEAPON, ItemType::WARAXE, 1, 100));
                            break;
                    }

                    potion = new XPotion(PotionName::CURE_LIGHT_WOUNDS);
                    potion->Identify();
                    ContainItem(potion);

                    book = new XBook(BOOK_MAGIC_ARROW);
                    book->Identify();
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
                    pbp->Wear(ICREATEB(ItemKind::WEAPON, ItemType::MACE, 10, 150));

                    pbp = GetBodyPart(BP_HAND, 1);
                    pbp->Wear(ICREATEB(ItemKind::SHIELD, ItemType::SMALLSHIELD, 10, 150));

                    scroll = new XScroll(ScrollName::BLINK);
                    scroll->Identify();
                    ContainItem(scroll);

                    scroll = new XScroll(ScrollName::HEROISM);
                    scroll->Identify();
                    ContainItem(scroll);

                    potion = new XPotion(PotionName::CURE_LIGHT_WOUNDS);
                    potion->Identify();
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
                    pbp->Wear(ICREATEB(ItemKind::WEAPON, ItemType::MACE, 10, 150));

                    pbp = GetBodyPart(BP_HAND, 1);
                    pbp->Wear(ICREATEB(ItemKind::SHIELD, ItemType::LARGESHIELD, 10, 150));

                    scroll = new XScroll(ScrollName::BLINK);
                    scroll->Identify();
                    ContainItem(scroll);

                    scroll = new XScroll(ScrollName::HEROISM);
                    scroll->Identify();
                    ContainItem(scroll);

                    potion = new XPotion(PotionName::CURE_LIGHT_WOUNDS);
                    potion->Identify();
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
                    pbp->Wear(ICREATEB(ItemKind::WEAPON, ItemType::DAGGER, 10, 150));

                    scroll = new XScroll(ScrollName::BLINK);
                    scroll->Identify();
                    ContainItem(scroll);

                    scroll = new XScroll(ScrollName::RECIPE);
                    scroll->Identify();
                    ContainItem(scroll);

                    scroll = new XScroll(ScrollName::RECIPE);
                    scroll->Identify();
                    ContainItem(scroll);

                    potion = new XPotion(PotionName::CURE_LIGHT_WOUNDS);
                    potion->Identify();
                    ContainItem(potion);

                    potion = new XPotion(PotionName::ORANGEJUCE);
                    potion->Identify();
                    ContainItem(potion);

                    potion = new XPotion(PotionName::APPLEJUCE);
                    potion->Identify();
                    ContainItem(potion);

                    potion = new XPotion(PotionName::WATER);
                    potion->Identify();
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
                    pbp->Wear(ICREATEB(ItemKind::WEAPON, ItemType::CLUB, 10, 150));

                    potion = new XPotion(PotionName::CURE_LIGHT_WOUNDS);
                    potion->Identify();
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
        }
    }

    XBodyPart * bp = GetBodyPart(BP_HAND, 0);

    if (bp->Item() && (bp->Item()->kind & ItemKind::WEAPON)) {
        wsk->SetLevel(bp->Item()->wt, 2);
    }

    sk->Learn(XSkill::Skill::DETECTTRAP);
    sk->Learn(XSkill::Skill::CREATETRAP);
    sk->Learn(XSkill::Skill::CLIMBING);

    bp = GetBodyPart(BP_BODY, 0);

    if (vRand(2) == 0) {
        bp->Wear(ICREATEB(ItemKind::BODY, ItemType::CLOTHES, 1, 100));
    } else {
        bp->Wear(ICREATEB(ItemKind::BODY, ItemType::ROBE, 1, 100));
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
