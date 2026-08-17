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

#include <fstream>
#include <fmt/format.h>
#include <sol/sol.hpp>

#include "helpers/msgwin.h"
#include "item/item_cereal.h"
#include "item/xpotion.h"
#include "magic/modifier.h"

REGISTER_CLASS(XPotion);
CEREAL_REGISTER_TYPE(XPotion);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XItem, XPotion);

void XPotion::RegisterLua(sol::state_view& lua)
{
    lua.new_enum("PotionName",
        "WATER", PotionName::WATER,
        "APPLEJUCE", PotionName::APPLEJUCE,
        "ORANGEJUCE", PotionName::ORANGEJUCE,
        "HEALING", PotionName::HEALING,
        "CURE_LIGHT_WOUNDS", PotionName::CURE_LIGHT_WOUNDS,
        "CURE_SERIOUS_WOUNDS", PotionName::CURE_SERIOUS_WOUNDS,
        "CURE_CRITICAL_WOUNDS", PotionName::CURE_CRITICAL_WOUNDS,
        "CURE_MORTAL_WOUNDS", PotionName::CURE_MORTAL_WOUNDS,
        "POWER", PotionName::POWER,
        "RESTORATION", PotionName::RESTORATION,
        "GAIN_STRENGTH", PotionName::GAIN_STRENGTH,
        "GAIN_WILLPOWER", PotionName::GAIN_WILLPOWER,
        "GAIN_MANA", PotionName::GAIN_MANA,
        "GAIN_TOUGHNESS", PotionName::GAIN_TOUGHNESS,
        "GAIN_DEXTERITY", PotionName::GAIN_DEXTERITY,
        "CURE_POISON", PotionName::CURE_POISON,
        "POISON", PotionName::POISON,
        "BLEEDNESS", PotionName::BLEEDNESS,
        "DISEASE", PotionName::DISEASE,
        "CURE_DISEASE", PotionName::CURE_DISEASE,
        "HEROISM", PotionName::HEROISM,
        "SEE_INVISIBLE", PotionName::SEE_INVISIBLE,
        "WEAKNESS", PotionName::WEAKNESS,
        "CLUMSINESS", PotionName::CLUMSINESS,
        "DEATH", PotionName::DEATH,
        "SATIATION", PotionName::SATIATION,
        "STARVATION", PotionName::STARVATION,
        "BOOST_SPEED", PotionName::BOOST_SPEED,
        "SLOWNESS", PotionName::SLOWNESS,
        "ACID_RESISTANCE", PotionName::ACID_RESISTANCE,
        "FIRE_RESISTANCE", PotionName::FIRE_RESISTANCE,
        "COLD_RESISTANCE", PotionName::COLD_RESISTANCE,
        "POISON_RESISTANCE", PotionName::POISON_RESISTANCE,
        "RANDOM", PotionName::RANDOM
    );
}

struct PotionColorEntry {
    const char* name;
    int color;
    int is_used;
};

PotionColorEntry pnc_table[] = {
    /* 0 PotionColor::CLEAR*/{	"clear",	xLIGHTGRAY	, 0},
    /* 1 PotionColor::SMOKY*/{	"smoky",	xLIGHTGRAY	, 0},
    /* 2 PotionColor::GREEN*/{	"green",	xGREEN	, 0},
    /* 3 PotionColor::ORANGE*/{	"orange",	xYELLOW	, 0},
    /* 4 PotionColor::YELLOW*/{	"yellow",	xYELLOW	, 0},
    /* 5 PotionColor::BLACK*/{	"black",	xDARKGRAY	, 0},
    /* 6 PotionColor::BLUE*/{	"blue",	xBLUE	, 0},
    /* 7 PotionColor::WHITE*/{	"white",	xWHITE	, 0},
    /* 8 PotionColor::CYAN*/{	"cyan",	xCYAN	, 0},
    /* 9 PotionColor::PURPLE*/{	"purple",	xLIGHTMAGENTA	, 0},
    /* 10 PotionColor::HAZE*/{	"haze",	xLIGHTGRAY	, 0},
    /* 11 PotionColor::GOLDEN*/{	"golden",	xYELLOW	, 0},
    /* 12 PotionColor::SILVER*/{	"silver",	xLIGHTGRAY	, 0},
    /* 13 PotionColor::AZURE*/{	"azure",	xLIGHTCYAN	, 0},
    /* 14 PotionColor::MURKY*/{	"murky",	xDARKGRAY	, 0},
    /* 15 PotionColor::RED*/	{	"red",	xRED	, 0},
    /* 16 PotionColor::GLOWING*/{	"glowing",	xYELLOW	, 0},
    /* 17 PotionColor::MOTTLED*/{	"mottled",	xLIGHTRED	, 0},
    /* 18 PotionColor::BLOBBED*/{	"blobby",	xBROWN	, 0},
    /* 19 PotionColor::PINK*/{	"pink",	xLIGHTMAGENTA	, 0},
    /* 20 PotionColor::MOULDED*/{	"mouldy",	xLIGHTCYAN	, 0},
    /* 21 PotionColor::GRAY*/{	"gray",	xLIGHTGRAY	, 0},
    /* 22 PotionColor::MERCURY*/{	"mercury",	xLIGHTGRAY	, 0},
    /* 23 PotionColor::OILY*/{	"oily",	xDARKGRAY	, 0},
    /* 24 PotionColor::VISCOUS*/{	"viscous",	xLIGHTCYAN	, 0},
    /* 25 PotionColor::DARK_RED*/{	"dark red",	xRED	, 0},
    /* 26 PotionColor::LIGHT_RED*/{	"light red",	xLIGHTRED	, 0},
    /* 27 PotionColor::DARK_BLUE*/{	"dark blue",	xBLUE	, 0},
    /* 28 PotionColor::LIGHT_BLUE*/{	"light blue",	xLIGHTBLUE	, 0},
    /* 29 PotionColor::BROWN*/{	"brown",	xBROWN	, 0},
    /* 30 PotionColor::LIGHT_GRAY*/{	"light gray",	xLIGHTGRAY	, 0},
    /* 31 PotionColor::DARK_GRAY*/{	"dark gray",	xDARKGRAY	, 0},
    /* 32 PotionColor::DARK_GREEN*/{	"dark green",	xGREEN	, 0},
    /* 33 PotionColor::LIGHT_GREEN*/{"light green",	xLIGHTGREEN	, 0},
    /* 34 PotionColor::BEIGE*/	{"beige",	xLIGHTGRAY	, 0},
    /* 35 PotionColor::AQUAMARINE*/	{"aquamarine",	xCYAN	, 0},
    /* 36 PotionColor::CORAL*/	{"coral",	xGREEN	, 0},
    /* 37 PotionColor::IVORY*/	{"ivory",	xYELLOW	, 0},
    /* 38 PotionColor::MAROON*/	{"maroon",	xRED	, 0},
    /* 39 PotionColor::TAN*/	{"tan",	xBROWN	, 0},
    /* 40 PotionColor::TURQUOISE*/	{"turquoise",	xCYAN	, 0},
    /* 41 PotionColor::VIOLET*/	{"violet",	xMAGENTA	, 0},
};

PotionColor PotionDescription::SelectColor(PotionColor pnc)
{
    if (pnc == PotionColor::RANDOM) {
        int count = 1000;

        while (count-- > 0) {
            int rp = vRand() % static_cast<int>(PotionColor::RANDOM);

            if (pnc_table[rp].is_used == 0) {
                pnc_table[rp].is_used = 1;

                return static_cast<PotionColor>(rp);
            }
        }

        assert(0);
        return PotionColor::CLEAR;
    }

    assert(pnc_table[static_cast<int>(pnc)].is_used == 0);
    pnc_table[static_cast<int>(pnc)].is_used = 1;

    return pnc;
}

PotionDescription potion_descr[] = {
    {PotionName::WATER,	"water",	XEffect::NONE,	100,	1,	1,	PotionDescription::SelectColor(PotionColor::CLEAR),	0},
    {PotionName::APPLEJUCE,	"apple juice",	XEffect::NONE,	95,	1,	2,	PotionDescription::SelectColor(PotionColor::YELLOW),	0},
    {PotionName::ORANGEJUCE,	"orange juice",	XEffect::NONE,	95,	1,	3,	PotionDescription::SelectColor(PotionColor::ORANGE),	0},
    {PotionName::HEALING,	"healing",	XEffect::HEAL,	10,	4,	200, PotionDescription::SelectColor(PotionColor::WHITE),	0},
    {PotionName::CURE_LIGHT_WOUNDS,	"cure light wounds",	XEffect::CURE_LIGHT_WOUNDS,	80,	2,	15,	PotionDescription::SelectColor(),	0},
    {PotionName::CURE_SERIOUS_WOUNDS, "cure serious wounds",	XEffect::CURE_SERIOUS_WOUNDS,	70,	2,	40,	PotionDescription::SelectColor(),	0},
    {PotionName::CURE_CRITICAL_WOUNDS, "cure critical wounds", XEffect::CURE_CRITICAL_WOUNDS,	40,	3,	60,	PotionDescription::SelectColor(),	0},
    {PotionName::CURE_MORTAL_WOUNDS,	"cure mortal wounds",	XEffect::CURE_MORTAL_WOUNDS,	20,	3,	100, PotionDescription::SelectColor(),	0},
    {PotionName::POWER,	"power",	XEffect::POWER,	80,	2,	15,	PotionDescription::SelectColor(),	0},
    {PotionName::RESTORATION,	"restoration",	XEffect::RESTORATION,	5,	5,	400, PotionDescription::SelectColor(),	0},
    {PotionName::GAIN_STRENGTH,	"strength",	XEffect::NONE,	10,	4,	1000, PotionDescription::SelectColor(),	0},
    {PotionName::GAIN_WILLPOWER,	"willpower",	XEffect::NONE,	10,	4,	1000, PotionDescription::SelectColor(),	0},
    {PotionName::GAIN_MANA,	"mana",	XEffect::NONE,	10,	4,	1000, PotionDescription::SelectColor(),	0},
    {PotionName::GAIN_TOUGHNESS,	"toughness",	XEffect::NONE,	10,	4,	1000, PotionDescription::SelectColor(),	0},
    {PotionName::GAIN_DEXTERITY,	"swiftness",	XEffect::NONE,	10,	4,	1000, PotionDescription::SelectColor(),	0},
    {PotionName::POISON,	"poison",	XEffect::NONE,	150,	1,	5,	PotionDescription::SelectColor(),	0},
    {PotionName::CURE_POISON,	"cure poison",	XEffect::CURE_POISON,	80,	3,	25, PotionDescription::SelectColor(),	0},
    {PotionName::BLEEDNESS,	"bleeding",	XEffect::NONE,	300,	1,	1, PotionDescription::SelectColor(),	0},
    {PotionName::DISEASE,	"disease",	XEffect::NONE,	200,	1,	1, PotionDescription::SelectColor(),	0},
    {PotionName::CURE_DISEASE,	"cure disease",	XEffect::CURE_DISEASE,	50,	3,	50, PotionDescription::SelectColor(),	0},
    {PotionName::HEROISM,	"heroism",	XEffect::HEROISM,	75,	2,	20, PotionDescription::SelectColor(),	0},
    {PotionName::SEE_INVISIBLE,	"see invisible",	XEffect::SEE_INVISIBLE,	30,	3,	30,	PotionDescription::SelectColor(),	0},
    {PotionName::WEAKNESS,	"weakness",	XEffect::NONE,	70,	1,	5,	PotionDescription::SelectColor(),	0},
    {PotionName::CLUMSINESS,	"clumsiness",	XEffect::NONE,	70,	1,	5,	PotionDescription::SelectColor(),	0},
    {PotionName::DEATH,	"death",	XEffect::NONE,	1,	2,	5,	PotionDescription::SelectColor(),	0},
    {PotionName::SATIATION,	"satiation",	XEffect::NONE,	50,	2,	15,	PotionDescription::SelectColor(),	0},
    {PotionName::STARVATION,	"starvation",	XEffect::NONE,	40,	3,	15,	PotionDescription::SelectColor(),	0},
    {PotionName::BOOST_SPEED,	"boost speed",	XEffect::NONE,	30,	3,	100, PotionDescription::SelectColor(),	0},
    {PotionName::SLOWNESS,	"slowness",	XEffect::NONE,	150,	1,	2,	PotionDescription::SelectColor(),	0},
    {PotionName::ACID_RESISTANCE,	"acid resistance",	XEffect::ACID_RESISTANCE,	35,	3,	70,	PotionDescription::SelectColor(),	0},
    {PotionName::FIRE_RESISTANCE,	"fire resistance",	XEffect::FIRE_RESISTANCE,	45,	2,	50,	PotionDescription::SelectColor(),	0},
    {PotionName::COLD_RESISTANCE,	"cold resistance",	XEffect::COLD_RESISTANCE,	45,	2,	50,	PotionDescription::SelectColor(),	0},
    {PotionName::POISON_RESISTANCE,	"poison resistance",	XEffect::POISON_RESISTANCE,	40,	3,	50,	PotionDescription::SelectColor(),	0},
};

int PotionDescription::potion_total_value = 0;
void PotionDescription::RunOnce()
{
    for (int i = 0; i < static_cast<int>(PotionName::RANDOM); i++) {
        potion_total_value += potion_descr[i].rarity;
    }
}

struct PotionRunOnce {
    PotionRunOnce()
    {
        PotionDescription::RunOnce();
    }
} potion_run_once;

PotionName PotionDescription::GetRandomPotion()
{
    int val = vRand(potion_total_value);
    int pos = -1;

    do {
        pos++;
        val -= potion_descr[pos].rarity;
    } while (val >= 0);

    return static_cast<PotionName>(pos);
}

PotionDescription* PotionDescription::GetRec(const PotionName pn)
{
    for (int i = 0; i < static_cast<int>(PotionName::RANDOM); i++)
        if (potion_descr[i].pn == pn) {
            return &potion_descr[i];
        }

    return nullptr;
}

XPotion::XPotion(const PotionName _pn)
{
    if (_pn == PotionName::RANDOM) {
        pn = PotionDescription::GetRandomPotion();
    } else {
        pn = _pn;
    }

    pdescr = nullptr;

    for (int i = 0; i < static_cast<int>(PotionName::RANDOM); i++)
        if (potion_descr[i].pn == pn) {
            pdescr = &potion_descr[i];
            break;
        }

    assert(pdescr);

    kind = ItemKind::POTION;
    bp = BP_OTHER;
    it = ItemType::POTION;
    view = '!';
    color =	pnc_table[static_cast<int>(pdescr->force_color)].color;

    name = pdescr->name;

    weight = 3;
    value = pdescr->value;
    dice.Setup("1d2");
}

XPotion::XPotion(XPotion * copy) : XItem(static_cast<XItem *>(copy))
{
    pn = copy->pn;
    pdescr = copy->pdescr;
}

int XPotion::Compare(XObject * o)
{
    assert(dynamic_cast<XPotion*>(o));
    auto pot = static_cast<XPotion *>(o);

    if (pot->x == x && pot->y == y && pot->pn == pn) {
        return 0;
    } else {
        if (pn > pot->pn) {
            return -1;
        } else {
            return 1;
        }
    }
}

std::string XPotion::toString()
{
    if (isIdentifed()) {
        if (quantity == 1) {
            return fmt::format("potion of {}", name);
        }

        return fmt::format("heap of {} potions of {}", quantity, name);
    }

    if (quantity == 1) {
        return fmt::format("{} potion", pnc_table[static_cast<int>(pdescr->force_color)].name);
    }

    return fmt::format("heap of {} {} potions", quantity, pnc_table[static_cast<int>(pdescr->force_color)].name);
}

int XPotion::isIdentifed()
{
    return pdescr->identify;
}

void XPotion::Identify(int level)
{
    pdescr->identify = level;
}

int XPotion::onDrink(XCreature * cr)
{
    if (cr->isHero()) {
        msgwin.Add(fmt::format("You drink a {}.", toString()));
    } else if (cr->isVisible()) {
        msgwin.Add(fmt::format("{} drinks a {}.", cr->name, toString()));
    }

    int flag{};

    if (pdescr->effect > XEffect::NONE) {
        flag = XEffect::Make(cr, pdescr->effect, 30);
    } else {
        switch (pn) {
            case PotionName::WATER:
            case PotionName::APPLEJUCE:
            case PotionName::ORANGEJUCE:
                if (cr->isHero()) {
                    msgwin.Add("You feel less thirsty.");
                } else if (cr->isVisible()) {
                    msgwin.Add(cr->name);
                    msgwin.Add("looks less thirsty.");
                }

                flag = 1;
                break;

            case PotionName::GAIN_STRENGTH:
                cr->GainAttr(XStats::STR, 1);
                break;

            case PotionName::GAIN_TOUGHNESS:
                cr->GainAttr(XStats::TOU, 1);
                break;

            case PotionName::GAIN_WILLPOWER:
                cr->GainAttr(XStats::WIL, 1);
                break;

            case PotionName::GAIN_DEXTERITY:
                cr->GainAttr(XStats::DEX, 1);
                break;

            case PotionName::GAIN_MANA:
                cr->GainAttr(XStats::MAN, 1);
                break;

            case PotionName::WEAKNESS:
                cr->GainAttr(XStats::STR, -1);
                break;

            case PotionName::CLUMSINESS:
                cr->GainAttr(XStats::DEX, -1);
                break;

            case PotionName::DEATH:
                // A potion of death should hurt really badly!
                // As potion of lifelessness, it was just another
                // clumsiness potion, but now it is a potion of death.
                // Moral: Watch what you drink...  This will kill *anyone*.

                // Note, this potion is going to be extremely rare as well.
                if (cr->isHero()) {
                    // Inform the hero of their fate.
                    msgwin.Add("You feel your life draining away very rapidly!");
                } else if (cr->isVisible()) {
                    if (cr->unique) {
                        // Uniques are too smart to be fooled by such petty implements...
                        msgwin.Add(cr->name);
                        msgwin.Add("seems to change");

                        switch (cr->creature_person_type) {
                            case XCreature::NAMED_HE:
                            case XCreature::HE:
                                msgwin.Add("his");

                            case XCreature::NAMED_SHE:
                            case XCreature::SHE:
                                msgwin.Add("her");

                            case XCreature::NAMED_IT:
                            case XCreature::IT:
                                msgwin.Add("its");
                        }

                        msgwin.Add("mind and throws the potion away!");
                        flag = 1;
                        break;
                    } else {
                        // Ha ha! A stupid monster drank the potion of death!
                        msgwin.Add(cr->name);
                        msgwin.Add("seems to be dying!");
                    }
                }

                flag = 1;
                cr->GainAttr(XStats::STR, -1); // Weakness
                cr->GainAttr(XStats::DEX, -1); // Damage
                cr->GainAttr(XStats::TOU, -1); // Fatigue
                cr->GainAttr(XStats::LEN, -1); // Can't learn if you're dead
                cr->GainAttr(XStats::WIL, -1); // Lost the will to live
                cr->GainAttr(XStats::MAN, -1); // Out of touch with nature
                cr->GainAttr(XStats::PER, -1); // Senses are useless when dead
                cr->GainAttr(XStats::CHR, -1); // Rotting is ugly
                cr->md->Add(MOD_WOUND, 100, cr); // Ensure death
                break;

            case PotionName::SATIATION:
                cr->nutrio += cr->base_nutrio * 7;

                if (cr->isHero()) {
                    msgwin.Add("You feel much fuller!");
                } else if (cr->isVisible()) {
                    msgwin.Add(cr->name);
                    msgwin.Add("looks full!");
                }

                flag = 1;
                break;

            case PotionName::STARVATION:
                cr->nutrio = cr->base_nutrio * 3;

                if (cr->isHero()) {
                    msgwin.Add("You feel hungrier!");
                } else if (cr->isVisible()) {
                    msgwin.Add(cr->name);
                    msgwin.Add("looks very hungry!");
                }

                flag = 1;
                break;

            case PotionName::BOOST_SPEED:
                cr->md->Add(MOD_BOOST_SPEED, 100, cr);

                if (cr->isVisible() && !cr->isHero()) {
                    msgwin.Add("moves more quickly!");
                }

                flag = 1;
                break;

            case PotionName::SLOWNESS:
                cr->md->Add(MOD_SLOWNESS, 100, cr);

                if (cr->isVisible() && !cr->isHero()) {
                    msgwin.Add(cr->name);
                    msgwin.Add("moves slowly!");
                }

                flag = 1;
                break;

            case PotionName::BLEEDNESS:
                cr->md->Add(MOD_WOUND, 30, cr);

                if (cr->isHero()) {
                    msgwin.Add("You begin to bleed.");
                } else if (cr->isVisible()) {
                    msgwin.Add(cr->name);
                    msgwin.Add("starts to bleed.");
                }

                flag = 1;
                break;

            case PotionName::DISEASE:
                cr->md->Add(MOD_DISEASE, 25, cr);

                if (cr->isVisible() && !cr->isHero()) {
                    msgwin.Add(cr->name);
                    msgwin.Add("looks ill.");
                }

                flag = 1;
                break;

            case PotionName::POISON:
                cr->md->Add(MOD_POISON, 10, cr);

                if (cr->isVisible() && !cr->isHero()) {
                    msgwin.Add(cr->name);
                    msgwin.Add("is poisoned.");
                }

                flag = 1;
                break;

            default:
                assert(0);
        }
    }

    if (flag == 0 && cr->isVisible()) {
        if (cr->isHero()) {
            msgwin.Add("You feel nothing special!");
        } else {
            msgwin.Add(fmt::format("Nothing special happens to {}.", cr->name));
        }
    } else if (!isIdentifed() && cr->isHero()) {
        Identify(1);
        msgwin.Add(fmt::format("It was {}.", toString()));
    }

    return 0;
}

void XPotion::FixupDescr()
{
    pdescr = nullptr;

    for (int i = 0; i < static_cast<int>(PotionName::RANDOM); i++)
        if (potion_descr[i].pn == pn) {
            pdescr = &potion_descr[i];
            break;
        }

    assert(pdescr);
}

void XPotion::SaveTable(cereal::JSONOutputArchive& ar)
{
    for (int i = 0; i < static_cast<int>(PotionName::RANDOM); i++) {
        ar(potion_descr[i]);
    }
}

void XPotion::LoadTable(cereal::JSONInputArchive& ar)
{
    for (int i = 0; i < static_cast<int>(PotionName::RANDOM); i++) {
        ar(potion_descr[i]);
    }
}

XAlchemy alchemy;

XAlchemy::XAlchemy()
{
}

XAlchemy::~XAlchemy()
{
}

void XAlchemy::Init()
{
    for (int i = 1; i < 5; i++) {
        alchemy.BuildRecipes(i);
    }

    std::ofstream file(vMakePath(HOME_DIR, "recipes.txt"));

    for (const auto& it: alchemy.recipes) {
        file <<  fmt::format("{} + {} = {}\n",
            potion_descr[static_cast<int>(it->pn1)].name,
            potion_descr[static_cast<int>(it->pn2)].name,
            potion_descr[static_cast<int>(it->result)].name
        );
    }
}

std::string XAlchemy::GetRecipeName(const PotionName pn1, const PotionName pn2, const PotionName pn3)
{
    const char* c1 = potion_descr[static_cast<int>(pn1)].name;
    const char* c2 = potion_descr[static_cast<int>(pn2)].name;
    const char* c3 = potion_descr[static_cast<int>(pn3)].name;
    return fmt::format("potion of {} + potion of {} = potion of {}", c1, c2, c3);
}

void XAlchemy::BuildRecipes(int al_lvl)
{
    PotionName * pTableSrc = nullptr;
    int tbl_src = GetPotionCount(al_lvl, &pTableSrc);

    PotionName * pTableDest = nullptr;
    int tbl_dest = GetPotionCount(al_lvl + 1, &pTableDest);

    int* tbl = new int[tbl_src * tbl_src];
    memset(tbl, -1, sizeof(int) * tbl_src * tbl_src);

    for (int j = 0; j < tbl_dest; j++) {
        while (true) {
            const int pos1 = vRand(tbl_src);
            int pos2 = vRand(tbl_src);

            if (pos1 != pos2 && tbl[tbl_src * pos1 + pos2] == -1) {
                tbl[tbl_src * pos1 + pos2] = j;
                recipes.push_back(std::make_unique<XAlchemyRecipe>(pTableSrc[pos1], pTableSrc[pos2], pTableDest[j]));
                break;
            }
        }
    }

    delete[] tbl;
    delete[] pTableSrc;
    delete[] pTableDest;
}

int XAlchemy::GetPotionCount(const int al_lvl, PotionName** pTable)
{
    int res = 0;
    int i;

    for (i = 0; i < static_cast<int>(PotionName::RANDOM); i++)
        if (potion_descr[i].alchemy_power == al_lvl) {
            res++;
        }

    *pTable = new PotionName[res];

    int tres = 0;

    for (i = 0; i < static_cast<int>(PotionName::RANDOM); i++)
        if (potion_descr[i].alchemy_power == al_lvl) {
            (*pTable)[tres] = potion_descr[i].pn;
            tres++;
        }

    return res;
}

int XAlchemy::GetRecipeCount()
{
    return alchemy.recipes.size();
}

// Zero-based, matching its only caller (XScroll::onRead picks
// vRand(GetRecipeCount()), which yields 0..count-1).
XAlchemyRecipe* XAlchemy::GetRecipe(int num)
{
    if (num < 0 || static_cast<size_t>(num) >= alchemy.recipes.size()) {
        return nullptr;
    }

    return alchemy.recipes[num].get();
}

int XAlchemy::isValidRecipe(PotionName pn1, PotionName pn2, PotionName pn3)
{
    for (auto& rec: alchemy.recipes) {
        if (rec->pn1 == pn1 && rec->pn2 == pn2 && rec->result == pn3)
            return 1;
    }

    return 0;
}

PotionName XAlchemy::GetPotionName(PotionName pn1, PotionName pn2)
{
    for (auto& rec: alchemy.recipes) {
        if (rec->pn1 == pn1 && rec->pn2 == pn2 || rec->pn2 == pn1 && rec->pn1 == pn2)
            return rec->result;
    }

    return PotionName::UNKNOWN;
}

