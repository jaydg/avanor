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

#include "helpers/msgwin.h"
#include "item/item_cereal.h"
#include "item/xpotion.h"
#include "magic/modifier.h"

REGISTER_CLASS(XPotion);
CEREAL_REGISTER_TYPE(XPotion);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XItem, XPotion);

struct PN_COLORTABLE {
    const char* name;
    int color;
    int is_used;
};

PN_COLORTABLE pnc_table[] = {
    /* 0 PNC_CLEAR*/{	"clear",	xLIGHTGRAY	, 0},
    /* 1 PNC_SMOKY*/{	"smoky",	xLIGHTGRAY	, 0},
    /* 2 PNC_GREEN*/{	"green",	xGREEN	, 0},
    /* 3 PNC_ORANGE*/{	"orange",	xYELLOW	, 0},
    /* 4 PNC_YELLOW*/{	"yellow",	xYELLOW	, 0},
    /* 5 PNC_BLACK*/{	"black",	xDARKGRAY	, 0},
    /* 6 PNC_BLUE*/{	"blue",	xBLUE	, 0},
    /* 7 PNC_WHITE*/{	"white",	xWHITE	, 0},
    /* 8 PNC_CYAN*/{	"cyan",	xCYAN	, 0},
    /* 9 PNC_PURPLE*/{	"purple",	xLIGHTMAGENTA	, 0},
    /* 10 PNC_HAZE*/{	"haze",	xLIGHTGRAY	, 0},
    /* 11 PNC_GOLDEN*/{	"golden",	xYELLOW	, 0},
    /* 12 PNC_SILVER*/{	"silver",	xLIGHTGRAY	, 0},
    /* 13 PNC_AZURE*/{	"azure",	xLIGHTCYAN	, 0},
    /* 14 PNC_MURKY*/{	"murky",	xDARKGRAY	, 0},
    /* 15 PNC_RED*/	{	"red",	xRED	, 0},
    /* 16 PNC_GLOWING*/{	"glowing",	xYELLOW	, 0},
    /* 17 PNC_MOTTLED*/{	"mottled",	xLIGHTRED	, 0},
    /* 18 PNC_BLOBBED*/{	"blobby",	xBROWN	, 0},
    /* 19 PNC_PINK*/{	"pink",	xLIGHTMAGENTA	, 0},
    /* 20 PNC_MOULDED*/{	"mouldy",	xLIGHTCYAN	, 0},
    /* 21 PNC_GRAY*/{	"gray",	xLIGHTGRAY	, 0},
    /* 22 PNC_MERCURY*/{	"mercury",	xLIGHTGRAY	, 0},
    /* 23 PNC_OILY*/{	"oily",	xDARKGRAY	, 0},
    /* 24 PNC_VISCOUS*/{	"viscous",	xLIGHTCYAN	, 0},
    /* 25 PNC_DARK_RED*/{	"dark red",	xRED	, 0},
    /* 26 PNC_LIGHT_RED*/{	"light red",	xLIGHTRED	, 0},
    /* 27 PNC_DARK_BLUE*/{	"dark blue",	xBLUE	, 0},
    /* 28 PNC_LIGHT_BLUE*/{	"light blue",	xLIGHTBLUE	, 0},
    /* 29 PNC_BROWN*/{	"brown",	xBROWN	, 0},
    /* 30 PNC_LIGHT_GRAY*/{	"light gray",	xLIGHTGRAY	, 0},
    /* 31 PNC_DARK_GRAY*/{	"dark gray",	xDARKGRAY	, 0},
    /* 32 PNC_DARK_GREEN*/{	"dark green",	xGREEN	, 0},
    /* 33 PNC_LIGHT_GREEN*/{"light green",	xLIGHTGREEN	, 0},
    /* 34 PNC_BEIGE*/	{"beige",	xLIGHTGRAY	, 0},
    /* 35 PNC_AQUAMARINE*/	{"aquamarine",	xCYAN	, 0},
    /* 36 PNC_CORAL*/	{"coral",	xGREEN	, 0},
    /* 37 PNC_IVORY*/	{"ivory",	xYELLOW	, 0},
    /* 38 PNC_MAROON*/	{"maroon",	xRED	, 0},
    /* 39 PNC_TAN*/	{"tan",	xBROWN	, 0},
    /* 40 PNC_TURQUOISE*/	{"turquoise",	xCYAN	, 0},
    /* 41 PNC_VIOLET*/	{"violet",	xMAGENTA	, 0},
};

POTION_COLOR POTION_REC::SelectColor(POTION_COLOR pnc)
{
    if (pnc == PNC_RANDOM) {
        int count = 1000;

        while (count-- > 0) {
            int rp = vRand() % PNC_RANDOM;

            if (pnc_table[rp].is_used == 0) {
                pnc_table[rp].is_used = 1;

                return static_cast<POTION_COLOR>(rp);
            }
        }

        assert(0);
        return PNC_CLEAR;
    }

    assert(pnc_table[pnc].is_used == 0);
    pnc_table[pnc].is_used = 1;

    return pnc;
}

POTION_REC potion_descr[] = {
    {PN_WATER,	"water",	XEffect::NONE,	100,	1,	1,	POTION_REC::SelectColor(PNC_CLEAR),	0},
    {PN_APPLEJUCE,	"apple juice",	XEffect::NONE,	95,	1,	2,	POTION_REC::SelectColor(PNC_YELLOW),	0},
    {PN_ORANGEJUCE,	"orange juice",	XEffect::NONE,	95,	1,	3,	POTION_REC::SelectColor(PNC_ORANGE),	0},
    {PN_HEALING,	"healing",	XEffect::HEAL,	10,	4,	200, POTION_REC::SelectColor(PNC_WHITE),	0},
    {PN_CURE_LIGHT_WOUNDS,	"cure light wounds",	XEffect::CURE_LIGHT_WOUNDS,	80,	2,	15,	POTION_REC::SelectColor(),	0},
    {PN_CURE_SERIOUS_WOUNDS, "cure serious wounds",	XEffect::CURE_SERIOUS_WOUNDS,	70,	2,	40,	POTION_REC::SelectColor(),	0},
    {PN_CURE_CRITICAL_WOUNDS, "cure critical wounds", XEffect::CURE_CRITICAL_WOUNDS,	40,	3,	60,	POTION_REC::SelectColor(),	0},
    {PN_CURE_MORTAL_WOUNDS,	"cure mortal wounds",	XEffect::CURE_MORTAL_WOUNDS,	20,	3,	100, POTION_REC::SelectColor(),	0},
    {PN_POWER,	"power",	XEffect::POWER,	80,	2,	15,	POTION_REC::SelectColor(),	0},
    {PN_RESTORATION,	"restoration",	XEffect::RESTORATION,	5,	5,	400, POTION_REC::SelectColor(),	0},
    {PN_GAIN_STRENGTH,	"strength",	XEffect::NONE,	10,	4,	1000, POTION_REC::SelectColor(),	0},
    {PN_GAIN_WILLPOWER,	"willpower",	XEffect::NONE,	10,	4,	1000, POTION_REC::SelectColor(),	0},
    {PN_GAIN_MANA,	"mana",	XEffect::NONE,	10,	4,	1000, POTION_REC::SelectColor(),	0},
    {PN_GAIN_TOUGHNESS,	"toughness",	XEffect::NONE,	10,	4,	1000, POTION_REC::SelectColor(),	0},
    {PN_GAIN_DEXTERITY,	"swiftness",	XEffect::NONE,	10,	4,	1000, POTION_REC::SelectColor(),	0},
    {PN_POISON,	"poison",	XEffect::NONE,	150,	1,	5,	POTION_REC::SelectColor(),	0},
    {PN_CURE_POISON,	"cure poison",	XEffect::CURE_POISON,	80,	3,	25, POTION_REC::SelectColor(),	0},
    {PN_BLEEDNESS,	"bleeding",	XEffect::NONE,	300,	1,	1, POTION_REC::SelectColor(),	0},
    {PN_DISEASE,	"disease",	XEffect::NONE,	200,	1,	1, POTION_REC::SelectColor(),	0},
    {PN_CURE_DISEASE,	"cure disease",	XEffect::CURE_DISEASE,	50,	3,	50, POTION_REC::SelectColor(),	0},
    {PN_HEROISM,	"heroism",	XEffect::HEROISM,	75,	2,	20, POTION_REC::SelectColor(),	0},
    {PN_SEE_INVISIBLE,	"see invisible",	XEffect::SEE_INVISIBLE,	30,	3,	30,	POTION_REC::SelectColor(),	0},
    {PN_WEAKNESS,	"weakness",	XEffect::NONE,	70,	1,	5,	POTION_REC::SelectColor(),	0},
    {PN_CLUMSINESS,	"clumsiness",	XEffect::NONE,	70,	1,	5,	POTION_REC::SelectColor(),	0},
    {PN_DEATH,	"death",	XEffect::NONE,	1,	2,	5,	POTION_REC::SelectColor(),	0},
    {PN_SATIATION,	"satiation",	XEffect::NONE,	50,	2,	15,	POTION_REC::SelectColor(),	0},
    {PN_STARVATION,	"starvation",	XEffect::NONE,	40,	3,	15,	POTION_REC::SelectColor(),	0},
    {PN_BOOST_SPEED,	"boost speed",	XEffect::NONE,	30,	3,	100, POTION_REC::SelectColor(),	0},
    {PN_SLOWNESS,	"slowness",	XEffect::NONE,	150,	1,	2,	POTION_REC::SelectColor(),	0},
    {PN_ACID_RESISTANCE,	"acid resistance",	XEffect::ACID_RESISTANCE,	35,	3,	70,	POTION_REC::SelectColor(),	0},
    {PN_FIRE_RESISTANCE,	"fire resistance",	XEffect::FIRE_RESISTANCE,	45,	2,	50,	POTION_REC::SelectColor(),	0},
    {PN_COLD_RESISTANCE,	"cold resistance",	XEffect::COLD_RESISTANCE,	45,	2,	50,	POTION_REC::SelectColor(),	0},
    {PN_POISON_RESISTANCE,	"poison resistance",	XEffect::POISON_RESISTANCE,	40,	3,	50,	POTION_REC::SelectColor(),	0},
};

int POTION_REC::potion_total_value = 0;
void POTION_REC::RunOnce()
{
    for (int i = 0; i < PN_RANDOM; i++) {
        potion_total_value += potion_descr[i].rarity;
    }
}

struct POTION_RUN_ONCE {
    POTION_RUN_ONCE()
    {
        POTION_REC::RunOnce();
    }
} _POTION_RUN_ONCE;

POTION_NAME POTION_REC::GetRandomPotion()
{
    int val = vRand(potion_total_value);
    int pos = -1;

    do {
        pos++;
        val -= potion_descr[pos].rarity;
    } while (val >= 0);

    return static_cast<POTION_NAME>(pos);
}

POTION_REC* POTION_REC::GetRec(const POTION_NAME pn)
{
    for (int i = 0; i < PN_RANDOM; i++)
        if (potion_descr[i].pn == pn) {
            return &potion_descr[i];
        }

    return nullptr;
}

XPotion::XPotion(const POTION_NAME _pn)
{
    if (_pn == PN_RANDOM) {
        pn = POTION_REC::GetRandomPotion();
    } else {
        pn = _pn;
    }

    pdescr = nullptr;

    for (int i = 0; i < PN_RANDOM; i++)
        if (potion_descr[i].pn == pn) {
            pdescr = &potion_descr[i];
            break;
        }

    assert(pdescr);

    im = IM_POTION;
    bp = BP_OTHER;
    it = IT_POTION;
    view = '!';
    color =	pnc_table[pdescr->force_color].color;

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
        return fmt::format("{} potion", pnc_table[pdescr->force_color].name);
    }

    return fmt::format("heap of {} {} potions", quantity, pnc_table[pdescr->force_color].name);
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
            case PN_WATER:
            case PN_APPLEJUCE:
            case PN_ORANGEJUCE:
                if (cr->isHero()) {
                    msgwin.Add("You feel less thirsty.");
                } else if (cr->isVisible()) {
                    msgwin.Add(cr->name);
                    msgwin.Add("looks less thirsty.");
                }

                flag = 1;
                break;

            case PN_GAIN_STRENGTH:
                cr->GainAttr(XStats::STR, 1);
                break;

            case PN_GAIN_TOUGHNESS:
                cr->GainAttr(XStats::TOU, 1);
                break;

            case PN_GAIN_WILLPOWER:
                cr->GainAttr(XStats::WIL, 1);
                break;

            case PN_GAIN_DEXTERITY:
                cr->GainAttr(XStats::DEX, 1);
                break;

            case PN_GAIN_MANA:
                cr->GainAttr(XStats::MAN, 1);
                break;

            case PN_WEAKNESS:
                cr->GainAttr(XStats::STR, -1);
                break;

            case PN_CLUMSINESS:
                cr->GainAttr(XStats::DEX, -1);
                break;

            case PN_DEATH:
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

            case PN_SATIATION:
                cr->nutrio += cr->base_nutrio * 7;

                if (cr->isHero()) {
                    msgwin.Add("You feel much fuller!");
                } else if (cr->isVisible()) {
                    msgwin.Add(cr->name);
                    msgwin.Add("looks full!");
                }

                flag = 1;
                break;

            case PN_STARVATION:
                cr->nutrio = cr->base_nutrio * 3;

                if (cr->isHero()) {
                    msgwin.Add("You feel hungrier!");
                } else if (cr->isVisible()) {
                    msgwin.Add(cr->name);
                    msgwin.Add("looks very hungry!");
                }

                flag = 1;
                break;

            case PN_BOOST_SPEED:
                cr->md->Add(MOD_BOOST_SPEED, 100, cr);

                if (cr->isVisible() && !cr->isHero()) {
                    msgwin.Add("moves more quickly!");
                }

                flag = 1;
                break;

            case PN_SLOWNESS:
                cr->md->Add(MOD_SLOWNESS, 100, cr);

                if (cr->isVisible() && !cr->isHero()) {
                    msgwin.Add(cr->name);
                    msgwin.Add("moves slowly!");
                }

                flag = 1;
                break;

            case PN_BLEEDNESS:
                cr->md->Add(MOD_WOUND, 30, cr);

                if (cr->isHero()) {
                    msgwin.Add("You begin to bleed.");
                } else if (cr->isVisible()) {
                    msgwin.Add(cr->name);
                    msgwin.Add("starts to bleed.");
                }

                flag = 1;
                break;

            case PN_DISEASE:
                cr->md->Add(MOD_DISEASE, 25, cr);

                if (cr->isVisible() && !cr->isHero()) {
                    msgwin.Add(cr->name);
                    msgwin.Add("looks ill.");
                }

                flag = 1;
                break;

            case PN_POISON:
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

    for (int i = 0; i < PN_RANDOM; i++)
        if (potion_descr[i].pn == pn) {
            pdescr = &potion_descr[i];
            break;
        }

    assert(pdescr);
}

void XPotion::SaveTable(cereal::JSONOutputArchive& ar)
{
    for (int i = 0; i < PN_RANDOM; i++) {
        ar(potion_descr[i]);
    }
}

void XPotion::LoadTable(cereal::JSONInputArchive& ar)
{
    for (int i = 0; i < PN_RANDOM; i++) {
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
        alchemy.BuildReception(i);
    }

    std::fstream file(vMakePath(HOME_DIR, "recipies.txt"));

    for (const auto& it: alchemy.reception) {
        file <<  fmt::format("{} + {} = {}\n",
            potion_descr[it->pn1].name,
            potion_descr[it->pn2].name,
            potion_descr[it->result].name
        );
    }
}

std::string XAlchemy::GetReceptionName(const POTION_NAME pn1, const POTION_NAME pn2, const POTION_NAME pn3)
{
    const char* c1 = potion_descr[pn1].name;
    const char* c2 = potion_descr[pn2].name;
    const char* c3 = potion_descr[pn3].name;
    return fmt::format("potion of {} + potion of {} = potion of {}", c1, c2, c3);
}

void XAlchemy::BuildReception(int al_lvl)
{
    POTION_NAME * pTableSrc = nullptr;
    int tbl_src = GetPotionCount(al_lvl, &pTableSrc);

    POTION_NAME * pTableDest = nullptr;
    int tbl_dest = GetPotionCount(al_lvl + 1, &pTableDest);

    int* tbl = new int[tbl_src * tbl_src];
    memset(tbl, -1, sizeof(int) * tbl_src * tbl_src);

    for (int j = 0; j < tbl_dest; j++) {
        while (true) {
            const int pos1 = vRand(tbl_src);
            int pos2 = vRand(tbl_src);

            if (pos1 != pos2 && tbl[tbl_src * pos1 + pos2] == -1) {
                tbl[tbl_src * pos1 + pos2] = j;
                reception.push_back(std::make_unique<XAlchemyRec>(pTableSrc[pos1], pTableSrc[pos2], pTableDest[j]));
                break;
            }
        }
    }

    delete[] tbl;
    delete[] pTableSrc;
    delete[] pTableDest;
}

int XAlchemy::GetPotionCount(const int al_lvl, POTION_NAME** pTable)
{
    int res = 0;
    int i;

    for (i = 0; i < PN_RANDOM; i++)
        if (potion_descr[i].alchemy_power == al_lvl) {
            res++;
        }

    *pTable = new POTION_NAME[res];

    int tres = 0;

    for (i = 0; i < PN_RANDOM; i++)
        if (potion_descr[i].alchemy_power == al_lvl) {
            (*pTable)[tres] = potion_descr[i].pn;
            tres++;
        }

    return res;
}

int XAlchemy::GetReceptionCount()
{
    return alchemy.reception.size();
}

XAlchemyRec* XAlchemy::GetReception(int num)
{
    if (num > alchemy.reception.size())
        return nullptr;

    return alchemy.reception[num - 1].get();
}

int XAlchemy::isValidReception(POTION_NAME pn1, POTION_NAME pn2, POTION_NAME pn3)
{
    for (auto& rec: alchemy.reception) {
        if (rec->pn1 == pn1 && rec->pn2 == pn2 && rec->result == pn3)
            return 1;
    }

    return 0;
}

POTION_NAME XAlchemy::GetPotionName(POTION_NAME pn1, POTION_NAME pn2)
{
    for (auto& rec: alchemy.reception) {
        if (rec->pn1 == pn1 && rec->pn2 == pn2 || rec->pn2 == pn1 && rec->pn1 == pn2)
            return rec->result;
    }

    return PN_UNKNOWN;
}

