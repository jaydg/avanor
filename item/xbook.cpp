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
#include <map>
#include <vector>

#include <cereal/types/map.hpp>
#include <cereal/types/string.hpp>
#include <sol/sol.hpp>

#include <fmt/format.h>

#include "item/item_cereal.h"
#include "item/xbook.h"
#include "helpers/msgwin.h"

REGISTER_CLASS(XBook);
CEREAL_REGISTER_TYPE(XBook);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XItem, XBook);

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

// The looks an unread book can have. Content's list, not the engine's -
// filled by SetBookAppearances() from world/items/books.lua.
static std::vector<std::string> books_descr;

std::vector<BOOK_REC> book_descr;
int BOOK_REC::total_value = 0;

void SetBookAppearances(const sol::table& looks)
{
    books_descr.clear();

    for (size_t i = 1; i <= looks.size(); i++) {
        books_descr.push_back(looks[i]);
    }
}

BOOK_REC* BOOK_REC::Find(const SPELL_NAME& spell)
{
    for (auto& row : book_descr) {
        if (row.spell_name == spell) {
            return &row;
        }
    }

    return nullptr;
}

SPELL_NAME BOOK_REC::GetRandomBook()
{
    if (book_descr.empty()) {
        return SP_NONE;
    }

    int val = vRand(total_value);

    for (const auto& row : book_descr) {
        val -= row.rarity;

        if (val < 0) {
            return row.spell_name;
        }
    }

    return book_descr.front().spell_name;
}

// A look no other book has taken this game. Was an index handed out in
// declaration order, so the "old tome" was the same book in every game a
// player ever started - which is no disguise at all.
static std::string TakeBookLook()
{
    std::vector<const std::string*> free_looks;

    for (const auto& look : books_descr) {
        bool taken = false;

        for (const auto& row : book_descr) {
            if (row.look == look) {
                taken = true;
                break;
            }
        }

        if (!taken) {
            free_looks.push_back(&look);
        }
    }

    if (free_looks.empty()) {
        std::cerr << "world: more books than looks to disguise them as"
                  << std::endl;

        return books_descr.empty() ? std::string("book") : books_descr.front();
    }

    return *free_looks[vRand(static_cast<int>(free_looks.size()))];
}

BookBuilder::BookBuilder(std::string spell) : spell(std::move(spell)) {}

BookBuilder& BookBuilder::Chance(const int rar)
{
    rarity = rar;
    return *this;
}

void BookBuilder::Register()
{
    if (spell.empty()) {
        std::cerr << "world: a book teaching no spell" << std::endl;
        return;
    }

    if (BOOK_REC::Find(spell)) {
        std::cerr << "world: two books both teaching '" << spell << "'"
                  << std::endl;
        return;
    }

    BOOK_REC row;
    row.spell_name = spell;
    row.rarity = rarity;
    book_descr.push_back(row);
    BOOK_REC::total_value += rarity;

    // Taken after the row is in the table, so it counts itself out.
    book_descr.back().look = TakeBookLook();
}

// What one game came to know about one sort of book - what it looks like
// and whether anyone has read it. Keyed by the spell it teaches, so
// content may add or reorder books without spoiling a save.
struct BookMemory {
    bool identified{false};
    std::string look;

    template<class Archive>
    void serialize(Archive& ar)
    {
        ar(identified, look);
    }
};

void XBook::SaveTable(cereal::JSONOutputArchive& ar)
{
    std::map<std::string, BookMemory> learned;

    for (const auto& row : book_descr) {
        learned[row.spell_name] = BookMemory{row.identified, row.look};
    }

    ar(learned);
}

void XBook::LoadTable(cereal::JSONInputArchive& ar)
{
    std::map<std::string, BookMemory> learned;
    ar(learned);

    for (auto& row : book_descr) {
        if (const auto it = learned.find(row.spell_name); it != learned.end()) {
            row.identified = it->second.identified;
            row.look = it->second.look;
        }
    }
}

XBook::XBook(const SPELL_NAME& spell)
{
    spell_name = spell.empty() ? BOOK_REC::GetRandomBook() : spell;

    const BOOK_REC* row = BOOK_REC::Find(spell_name);

    if (!row) {
        std::cerr << "world: no book teaches '" << spell_name
                  << "' - substituting another" << std::endl;

        spell_name = BOOK_REC::GetRandomBook();
        row = BOOK_REC::Find(spell_name);
    }

    if (row) {
        value = row->rarity > 0 ? 20000 / row->rarity : 20000;
        name = XSpell::GetName(row->spell_name);
    }

    kind = ItemKind::BOOK;
    bp = BP_OTHER;
    it = IT_BOOK;

    view = '"';
    color =	xBROWN;
    weight = 100;
    dice.Setup("1d3");
    left_to_read = value * 10;
    reader_guid = 0;
}

bool XBook::isIdentified()
{
    const BOOK_REC* row = BOOK_REC::Find(spell_name);
    return row && row->identified;
}

void XBook::Identify()
{
    if (BOOK_REC* row = BOOK_REC::Find(spell_name)) {
        row->identified = true;
    }
}

int XBook::Compare(XObject * o)
{
    assert(dynamic_cast<XBook*>(o));
    XBook * tit = (XBook*)o;

    if (spell_name == tit->spell_name && x == tit->x && y == tit->y) {
        return 0;
    } else {
        if (quantity > tit->quantity) {
            return -1;
        } else {
            return 1;
        }
    }
}

std::string XBook::toString()
{
    std::string str;

    const BOOK_REC* row = BOOK_REC::Find(spell_name);
    const std::string look = row ? row->look : spell_name;

    if (!isIdentified()) {
        if (quantity == 1) {
            str = look;
        } else {
            str = fmt::format("heap of ({}) {}s", quantity, look);
        }
    } else {
        if (quantity == 1) {
            str = fmt::format("book of {}", name);
        } else {
            str = fmt::format("heap of ({}) books of {}",
                quantity, name);
        }
    }

    return str;
}

int XBook::onRead(XCreature * reader)
{
    if (reader->guid() != reader_guid) {
        left_to_read = value * 20;
        reader_guid = reader->guid();
    }

    XSkill * skill = reader->sk->GetSkill(XSkill::Skill::LITERACY);
    left_to_read -= (skill->GetLevel() + reader->GetStats(XStats::LEN));

    if (left_to_read <= 0) {
        reader->m->Learn(spell_name);
        skill->UseSkill(10);

        if (reader->isHero()) {
            msgwin.Add(fmt::format("You read the {}.", toString()));

            if (!isIdentified()) {
                Identify();
                msgwin.Add(fmt::format("It was {}.", toString()));
            }
        } else if (reader->isVisible()) {
            msgwin.Add(fmt::format("{} {} {}.",
                reader->GetNameEx(CRN_T1),
                reader->GetVerb("read"),
                toString()));

            if (!isIdentified()) {
                Identify();
                msgwin.Add(fmt::format("It was {}.", toString()));
            }
        }
    }

    return 1;
}

