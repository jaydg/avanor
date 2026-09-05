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
#include <algorithm>
#include <iostream>
#include <map>
#include <vector>

#include <cereal/types/map.hpp>
#include <cereal/types/string.hpp>

#include <sol/sol.hpp>

#include "helpers/msgwin.h"
#include "engine/xlua.h"
#include "item/item_cereal.h"
#include "item/xpotion.h"
#include "magic/modifier.h"

REGISTER_CLASS(XPotion);
CEREAL_REGISTER_TYPE(XPotion);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XItem, XPotion);

void XPotion::RegisterLua(sol::state_view& lua)
{
    // How a potion looks before anyone knows what it is. Which potions
    // exist is content (world/items/potions.lua); the vocabulary of
    // appearances is not, because every colour has to be one no other
    // potion took this game, and the engine deals them out.
    lua.new_enum("PotionColor",
        "CLEAR", PotionColor::CLEAR,
        "SMOKY", PotionColor::SMOKY,
        "GREEN", PotionColor::GREEN,
        "ORANGE", PotionColor::ORANGE,
        "YELLOW", PotionColor::YELLOW,
        "BLACK", PotionColor::BLACK,
        "BLUE", PotionColor::BLUE,
        "WHITE", PotionColor::WHITE,
        "CYAN", PotionColor::CYAN,
        "PURPLE", PotionColor::PURPLE,
        "HAZE", PotionColor::HAZE,
        "GOLDEN", PotionColor::GOLDEN,
        "SILVER", PotionColor::SILVER,
        "AZURE", PotionColor::AZURE,
        "MURKY", PotionColor::MURKY,
        "RED", PotionColor::RED,
        "GLOWING", PotionColor::GLOWING,
        "MOTTLED", PotionColor::MOTTLED,
        "BLOBBY", PotionColor::BLOBBY,
        "PINK", PotionColor::PINK,
        "MOULDY", PotionColor::MOULDY,
        "GRAY", PotionColor::GRAY,
        "MERCURY", PotionColor::MERCURY,
        "OILY", PotionColor::OILY,
        "VISCOUS", PotionColor::VISCOUS,
        "DARK_RED", PotionColor::DARK_RED,
        "LIGHT_RED", PotionColor::LIGHT_RED,
        "DARK_BLUE", PotionColor::DARK_BLUE,
        "LIGHT_BLUE", PotionColor::LIGHT_BLUE,
        "BROWN", PotionColor::BROWN,
        "LIGHT_GRAY", PotionColor::LIGHT_GRAY,
        "DARK_GRAY", PotionColor::DARK_GRAY,
        "DARK_GREEN", PotionColor::DARK_GREEN,
        "LIGHT_GREEN", PotionColor::LIGHT_GREEN,
        "BEIGE", PotionColor::BEIGE,
        "AQUAMARINE", PotionColor::AQUAMARINE,
        "CORAL", PotionColor::CORAL,
        "IVORY", PotionColor::IVORY,
        "MAROON", PotionColor::MAROON,
        "TAN", PotionColor::TAN,
        "TURQUOISE", PotionColor::TURQUOISE,
        "VIOLET", PotionColor::VIOLET
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

// Filled from world/items/potions.lua as that script loads.
std::vector<PotionDescription> potion_descr;

int PotionDescription::potion_total_value = 0;

void PotionDescription::RunOnce()
{
    potion_total_value = 0;

    for (const auto& row : potion_descr) {
        potion_total_value += row.rarity;
    }
}

PotionBuilder::PotionBuilder(std::string id)
{
    t.pn = std::move(id);
}

PotionBuilder& PotionBuilder::Called(const std::string& n)
{
    t.name = n;
    return *this;
}

PotionBuilder& PotionBuilder::Effect(const XEffect::Id eff)
{
    t.effect = eff;
    return *this;
}

PotionBuilder& PotionBuilder::Chance(const int rarity)
{
    t.rarity = rarity;
    return *this;
}

PotionBuilder& PotionBuilder::Worth(const int value)
{
    t.value = value;
    return *this;
}

PotionBuilder& PotionBuilder::Alchemy(const int power)
{
    t.alchemy_power = power;
    return *this;
}

PotionBuilder& PotionBuilder::Looks(const PotionColor colour)
{
    t.force_color = colour;
    return *this;
}

PotionBuilder& PotionBuilder::OnDrink(const std::string& handler)
{
    t.on_drink = handler;
    return *this;
}

void PotionBuilder::Register()
{
    if (t.pn.empty()) {
        std::cerr << "world: a potion with no id" << std::endl;
        return;
    }

    if (PotionDescription::GetRec(t.pn)) {
        std::cerr << "world: two potions both called '" << t.pn << "'" << std::endl;
        return;
    }

    // Its look for this game: the one content asked for, or one drawn from
    // whatever no other potion has taken.
    t.force_color = PotionDescription::SelectColor(t.force_color);

    potion_descr.push_back(t);
    PotionDescription::potion_total_value += t.rarity;
}

PotionName PotionDescription::GetRandomPotion()
{
    if (potion_descr.empty()) {
        return PN_NONE;
    }

    int val = vRand(potion_total_value);

    for (const auto& row : potion_descr) {
        val -= row.rarity;

        if (val < 0) {
            return row.pn;
        }
    }

    return potion_descr.front().pn;
}

PotionName PotionDescription::GetAnyPotion()
{
    if (potion_descr.empty()) {
        return PN_NONE;
    }

    return potion_descr[vRand(static_cast<int>(potion_descr.size()))].pn;
}

PotionDescription* PotionDescription::GetRec(const PotionName& pn)
{
    for (auto& row : potion_descr) {
        if (row.pn == pn) {
            return &row;
        }
    }

    return nullptr;
}

XPotion::XPotion(const PotionName& _pn)
{
    if (_pn == PN_NONE) {
        pn = PotionDescription::GetRandomPotion();
    } else {
        pn = _pn;
    }

    pdescr = PotionDescription::GetRec(pn);

    // A name nothing defines - a typo in world/, where potion ids are
    // written by hand. Say so and pour something else, rather than
    // dereferencing a row that is not there.
    if (!pdescr) {
        std::cerr << "world: nothing defines a potion '" << pn
                  << "' - substituting another" << std::endl;

        pn = PotionDescription::GetRandomPotion();
        pdescr = PotionDescription::GetRec(pn);
    }

    if (!pdescr) {
        return;
    }

    kind = ItemKind::POTION;
    bp = BP_OTHER;
    it = IT_POTION;
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
    if (isIdentified()) {
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

bool XPotion::isIdentified()
{
    return pdescr->identified;
}

void XPotion::Identify()
{
    pdescr->identified = true;
}

// Hands a potion that names no effect to the Lua function its row named
// in :OnDrink(). Mirrors XLuaTool::onUse and XItem::OnCreated - the
// handler is looked up by name in the live state, and a row that named
// none simply does nothing.
static int RunDrinkHandler(const std::string& handler,
    const PotionName& pn, XCreature* cr)
{
    if (handler.empty()) {
        return 0;
    }

    sol::state_view lua(XLua::State());
    sol::protected_function fn = lua[handler];

    if (!fn.valid()) {
        std::cerr << "world: potion '" << pn << "' wants drinking through '"
                  << handler << "', which is not defined" << std::endl;

        return 0;
    }

    const auto result = fn(pn, (void*)cr);

    if (!result.valid()) {
        const sol::error err = result;
        std::cerr << "world: " << handler << ": " << err.what() << std::endl;

        return 0;
    }

    if (result.get_type() == sol::type::none
        || result.get_type() == sol::type::lua_nil) {
        return 1;
    }

    return result.get<int>() ? 1 : 0;
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
        // No effect of its own: content finishes the job. The handler
        // gets the potion's id and the drinker, and answers whether
        // anything actually happened - a stat already at its ceiling, or a
        // sip of water, are not the same as a potion doing nothing.
        flag = RunDrinkHandler(pdescr->on_drink, pn, cr);
    }

    if (flag == 0 && cr->isVisible()) {
        if (cr->isHero()) {
            msgwin.Add("You feel nothing special!");
        } else {
            msgwin.Add(fmt::format("Nothing special happens to {}.", cr->name));
        }
    } else if (!isIdentified() && cr->isHero()) {
        Identify();
        msgwin.Add(fmt::format("It was {}.", toString()));
    }

    return 0;
}

void XPotion::FixupDescr()
{
    pdescr = PotionDescription::GetRec(pn);

    // A name nothing defines - a typo in world/, where potion ids are
    // written by hand. Say so and pour something else, rather than
    // dereferencing a row that is not there.
    if (!pdescr) {
        std::cerr << "world: nothing defines a potion '" << pn
                  << "' - substituting another" << std::endl;

        pn = PotionDescription::GetRandomPotion();
        pdescr = PotionDescription::GetRec(pn);
    }

    if (!pdescr) {
        return;
    }
}

// What one game came to know about one sort of potion - whether it has
// been recognised, and what colour it turned out to be. Everything else in
// a row is content and comes back from world/items/potions.lua next load.
struct PotionMemory {
    bool identified{false};
    PotionColor colour{PotionColor::RANDOM};

    template<class Archive>
    void serialize(Archive& ar)
    {
        ar(identified, colour);
    }
};

// Keyed by id rather than written in table order, so content may add rows
// or reorder them without spoiling a save.
void XPotion::SaveTable(cereal::JSONOutputArchive& ar)
{
    std::map<std::string, PotionMemory> learned;

    for (const auto& row : potion_descr) {
        learned[row.pn] = PotionMemory{row.identified, row.force_color};
    }

    ar(learned);
}

void XPotion::LoadTable(cereal::JSONInputArchive& ar)
{
    std::map<std::string, PotionMemory> learned;
    ar(learned);

    for (auto& row : potion_descr) {
        if (const auto it = learned.find(row.pn); it != learned.end()) {
            row.identified = it->second.identified;
            row.force_color = it->second.colour;
        }
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
    // How many tiers there are is content: it is however deep the
    // potions' own :Alchemy() levels go. Recipes turn each tier into the
    // one above, so the deepest tier is made but makes nothing.
    int deepest = 0;

    for (const auto& row : potion_descr) {
        deepest = std::max(deepest, row.alchemy_power);
    }

    for (int i = 1; i < deepest; i++) {
        alchemy.BuildRecipes(i);
    }

    std::ofstream file(vMakePath(HOME_DIR, "recipes.txt"));

    for (const auto& it: alchemy.recipes) {
        file <<  fmt::format("{} + {} = {}\n",
            PotionDescription::GetRec(it->pn1)->name,
            PotionDescription::GetRec(it->pn2)->name,
            PotionDescription::GetRec(it->result)->name
        );
    }
}

std::string XAlchemy::GetRecipeName(const PotionName& pn1, const PotionName& pn2,
    const PotionName& pn3)
{
    static const std::string nothing;
    const auto named = [](const PotionName& pn) -> const std::string& {
        const PotionDescription* row = PotionDescription::GetRec(pn);
        return row ? row->name : nothing;
    };

    return fmt::format("potion of {} + potion of {} = potion of {}",
        named(pn1), named(pn2), named(pn3));
}

void XAlchemy::BuildRecipes(int al_lvl)
{
    PotionName * pTableSrc = nullptr;
    int tbl_src = GetPotionCount(al_lvl, &pTableSrc);

    PotionName * pTableDest = nullptr;
    int tbl_dest = GetPotionCount(al_lvl + 1, &pTableDest);

    int* tbl = new int[tbl_src * tbl_src];
    memset(tbl, -1, sizeof(int) * tbl_src * tbl_src);

    // Every potion of the tier above gets one recipe, made of two
    // different potions of this one. There are tbl_src * (tbl_src - 1)
    // ordered pairs to go round; if content declares more potions on the
    // upper tier than that, the search below would never find a free pair
    // and would spin for ever, so say so and leave the rest unmakeable.
    const int pairs = tbl_src * (tbl_src - 1);

    if (tbl_dest > pairs) {
        std::cerr << "world: alchemy level " << al_lvl + 1 << " has " << tbl_dest
                  << " potions but level " << al_lvl << " offers only " << pairs
                  << " pairs to make them from - "
                  << (tbl_dest - pairs) << " will have no recipe" << std::endl;
    }

    for (int j = 0; j < tbl_dest && j < pairs; j++) {
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

    for (const auto& row : potion_descr)
        if (row.alchemy_power == al_lvl) {
            res++;
        }

    *pTable = new PotionName[res];

    int tres = 0;

    for (const auto& row : potion_descr)
        if (row.alchemy_power == al_lvl) {
            (*pTable)[tres] = row.pn;
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

int XAlchemy::isValidRecipe(const PotionName& pn1, const PotionName& pn2, const PotionName& pn3)
{
    for (auto& rec: alchemy.recipes) {
        if (rec->pn1 == pn1 && rec->pn2 == pn2 && rec->result == pn3)
            return 1;
    }

    return 0;
}

PotionName XAlchemy::GetPotionName(const PotionName& pn1, const PotionName& pn2)
{
    for (auto& rec: alchemy.recipes) {
        if ((rec->pn1 == pn1 && rec->pn2 == pn2) || (rec->pn2 == pn1 && rec->pn1 == pn2))
            return rec->result;
    }

    return PN_NONE;
}

