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

#include <fmt/format.h>

#include "item/item_cereal.h"
#include "item/xbook.h"
#include "helpers/msgwin.h"

REGISTER_CLASS(XBook);
CEREAL_REGISTER_TYPE(XBook);
CEREAL_REGISTER_POLYMORPHIC_RELATION(XItem, XBook);

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

const char* books_descr[] = {
    "old tome", "small tome", "ancient tome", "dirty tome", "heavy tome",
    "old book", "small book", "ancient book", "dirty book", "heavy book",
    "wrapped tome", "pocket book", "leather-bound tome", "parchment book",
    "thin book", "gold decorated", "silver decorated"
};

#define BOOKS_DESCR_SZ	ARRAY_SIZE(books_descr)

int BOOK_REC::current_descr = 0;
int BOOK_REC::total_value = 0;

BOOK_REC::BOOK_REC(int _rarity, BOOK_NAME bn, SPELL_NAME sn)
    : book_name(bn), spell_name(sn), identify(0), name_index(current_descr), rarity(_rarity)
{
    current_descr++;
    assert(current_descr <= BOOKS_DESCR_SZ);
    total_value += rarity;
}

BOOK_REC book_descr[] = {
    BOOK_REC(100,	BOOK_BURNING_HANDS, SPELL_BURNING_HANDS),
    BOOK_REC(100,	BOOK_ICE_TOUCH, SPELL_ICE_TOUCH),
    BOOK_REC(150,	BOOK_CURE_LIGHT_WOUNDS, SPELL_CURE_LIGHT_WOUNDS),
    BOOK_REC(70,	BOOK_DRAIN_LIFE, SPELL_DRAIN_LIFE),
    BOOK_REC(20,	BOOK_IDENTIFY, SPELL_IDENTIFY),
    BOOK_REC(200,	BOOK_MAGIC_ARROW, SPELL_MAGIC_ARROW),
    BOOK_REC(50,	BOOK_FIRE_BOLT, SPELL_FIRE_BOLT),
    BOOK_REC(50,	BOOK_ICE_BOLT, SPELL_ICE_BOLT),
    BOOK_REC(20,	BOOK_LIGHTNING_BOLT, SPELL_LIGHTNING_BOLT),
    BOOK_REC(10,	BOOK_ACID_BOLT, SPELL_ACID_BOLT),
    BOOK_REC(60,	BOOK_CURE_DISEASE, SPELL_CURE_DISEASE),
    BOOK_REC(80,	BOOK_CURE_POISON, SPELL_CURE_POISON),
    BOOK_REC(15,	BOOK_BLINK, SPELL_BLINK),
    BOOK_REC(5,	BOOK_SELF_KNOWLEDGE, SPELL_SELF_KNOWLEDGE),
};

int BOOK_REC::GetBook(BOOK_NAME bn)
{
    if (bn != BOOK_RANDOM) {
        for (int i = 0; i < BOOK_RANDOM; i++)
            if (book_descr[i].book_name == bn) {
                return i;
            }
    } else {
        int val = vRand(total_value);
        int pos = -1;

        do {
            pos++;
            val -= book_descr[pos].rarity;
        } while (val >= 0);

        assert(pos < BOOK_RANDOM);
        return pos;
    }

    assert(0);
    return -1;
}

void XBook::SaveTable(cereal::JSONOutputArchive& ar)
{
    for (int i = 0; i < ARRAY_SIZE(book_descr); i++) {
        ar(book_descr[i]);
    }
}

void XBook::LoadTable(cereal::JSONInputArchive& ar)
{
    for (int i = 0; i < ARRAY_SIZE(book_descr); i++) {
        ar(book_descr[i]);
    }
}

XBook::XBook(BOOK_NAME bn)
{
    descr = BOOK_REC::GetBook(bn);
    assert(descr > -1 && descr < BOOK_RANDOM);

    value = 20000 / book_descr[descr].rarity;
    name = XSpell::GetName(book_descr[descr].spell_name);

    kind = ItemKind::BOOK;
    bp = BP_OTHER;
    it = ItemType::BOOK;

    view = '"';
    color =	xBROWN;
    weight = 100;
    dice.Setup("1d3");
    left_to_read = value * 10;
    reader_guid = 0;
}

int XBook::isIdentified()
{
    return book_descr[descr].identify;
}

void XBook::Identify(int level)
{
    book_descr[descr].identify = level;
}

int XBook::Compare(XObject * o)
{
    assert(dynamic_cast<XBook*>(o));
    XBook * tit = (XBook*)o;

    if (descr == tit->descr && x == tit->x && y == tit->y) {
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

    if (!isIdentified()) {
        if (quantity == 1) {
            str = books_descr[book_descr[descr].name_index];
        } else {
            str = fmt::format("heap of ({}) {}s",
                quantity, books_descr[book_descr[descr].name_index]);
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
        reader->m->Learn(book_descr[descr].spell_name);
        skill->UseSkill(10);

        if (reader->isHero()) {
            msgwin.Add(fmt::format("You read the {}.", toString()));

            if (!isIdentified()) {
                Identify(1);
                msgwin.Add(fmt::format("It was {}.", toString()));
            }
        } else if (reader->isVisible()) {
            msgwin.Add(fmt::format("{} {} {}.",
                reader->GetNameEx(CRN_T1),
                reader->GetVerb("read"),
                toString()));

            if (!isIdentified()) {
                Identify(1);
                msgwin.Add(fmt::format("It was {}.", toString()));
            }
        }
    }

    return 1;
}

